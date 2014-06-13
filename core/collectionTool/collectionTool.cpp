////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2011-2013 Krell Institute. All Rights Reserved.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License as published by the Free
// Software Foundation; either version 2.1 of the License, or (at your option)
// any later version.
//
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
// details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, write to the Free Software Foundation, Inc.,
// 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
////////////////////////////////////////////////////////////////////////////////

/** @file simple collection tool frontend. */

#include <sys/param.h>
#include <errno.h>
#include <sys/wait.h>
#include <stdio.h>
#include <sys/stat.h>
#include <iostream>
#include <sstream>
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/tokenizer.hpp>
#include <boost/program_options.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <KrellInstitute/CBTF/BoostExts.hpp>
#include <KrellInstitute/CBTF/Component.hpp>
#include <KrellInstitute/CBTF/Type.hpp>
#include <KrellInstitute/CBTF/ValueSink.hpp>
#include <KrellInstitute/CBTF/ValueSource.hpp>
#include <KrellInstitute/CBTF/XML.hpp>
#include "KrellInstitute/Core/SymtabAPISymbols.hpp"
#include "KrellInstitute/Core/CBTFTopology.hpp"

using namespace boost;
using namespace KrellInstitute::CBTF;
using namespace KrellInstitute::Core;

enum exe_class_types { MPI_exe_type, SEQ_RunAs_MPI_exe_type, SEQ_exe_type };

// Experiment Utilities.

// Function that returns the number of BE processes that are required for LW MRNet BEs.
// The function tokenizes the program command and searches for -np or -n.
static int getBEcountFromCommand(std::string command) {

    int retval = 1;

    boost::char_separator<char> sep(" ");
    boost::tokenizer<boost::char_separator<char> > btokens(command, sep);
    std::string S = "";

    bool found_be_count = false;

    BOOST_FOREACH (const std::string& t, btokens) {
	S = t;
	if (found_be_count) {
	    S = t;
	    retval = boost::lexical_cast<int>(S);
	    break;
	} else if (!strcmp( S.c_str(), std::string("-np").c_str())) {
	    found_be_count = true;
	} else if (!strcmp(S.c_str(), std::string("-n").c_str())) {
	    found_be_count = true;
	}
    } // end foreach

    return retval;
}

// 
// Determine if libmpi is present in this executable.
//
static bool isMpiExe(const std::string exe) {
    SymtabAPISymbols stapi_symbols;
    bool found_libmpi = stapi_symbols.foundLibrary(exe,"libmpi");
    return found_libmpi;
}

//
// Determine what type of executable situation we have for running with cbtfrun.
// Is this a pure MPI executable or are we running a sequential executable with a mpi driver?
// We catagorize these into three types: mpi, seq runing under mpi driver, and sequential
//
static exe_class_types typeOfExecutable ( std::string program, const std::string exe ) {

   exe_class_types tmp_exe_type;

   if ( isMpiExe(exe) ) { 
          tmp_exe_type = MPI_exe_type;
   } else {

    if ( std::string::npos != program.find("aprun")) {
        tmp_exe_type = SEQ_RunAs_MPI_exe_type;
    } else {
        tmp_exe_type = SEQ_exe_type;
    }

  }
 
  return tmp_exe_type;

}

// Function that returns whether the filename is an executable file.
// Uses stat to obtain the mode of the filename and if it executable returns true.
static bool is_executable(std::string file)
{
    struct stat status_buffer;

    // Call stat with filename which will fill status_buffer
    if (stat(file.c_str(), &status_buffer) < 0)
        return false;

    // Examine for executable status
    if ((status_buffer.st_mode & S_IEXEC) != 0)
        return true;

    return false;
}

// Function that returns the filename of the executable file found in the "command".
// It tokenizes the command and runs through it backwards looking for the first file that is executable.
// That might not be sufficient in all cases.
static std::string getMPIExecutableFromCommand(std::string command) {

    std::string retval = "";

    boost::char_separator<char> sep(" ");
    boost::tokenizer<boost::char_separator<char> > btokens(command, sep);

    BOOST_FOREACH (const std::string& t, btokens) {
      if (is_executable( t )) {
         exe_class_types local_exe_type = typeOfExecutable(command, t);
         if (local_exe_type == MPI_exe_type || local_exe_type == SEQ_RunAs_MPI_exe_type ) {
           return t;
         }
      }
    } // end foreach

  return retval;
}


/**
 * main thread for the example collection tool.
 */

class FEThread
{
  public:

  FEThread()
  {
  }

  void start(const std::string& topology, const std::string& connections,
	     const std::string& collector, const unsigned int& numBE,
	     bool& finished)
  {
    dm_thread = boost::thread(&FEThread::run, this, topology, connections,
			      collector, numBE, finished);
  }

  void join()
  {
    dm_thread.join();
  }

  void run(const std::string& topology, const std::string& connections,
	   const std::string& collector, const unsigned int& numBE,
	   bool& finished)
  {
    std::string xmlfile(collector);
    xmlfile += ".xml";
    registerXML(filesystem::path(XMLDIR) / xmlfile);


    Component::registerPlugin(
        filesystem::path(LIBDIR) / "KrellInstitute/CBTF/BasicMRNetLaunchers.so");
    
    Component::Instance network = Component::instantiate(
        Type(collector)
        );
    
    Component::Instance launcher = Component::instantiate(
        Type("BasicMRNetLauncherUsingBackendAttach")
        );

    shared_ptr<ValueSource<unsigned int> > backend_attach_count =
        ValueSource<unsigned int>::instantiate();
    Component::Instance backend_attach_count_component = 
        reinterpret_pointer_cast<Component>(backend_attach_count);
    Component::connect(
        backend_attach_count_component, "value", launcher, "BackendAttachCount"
        );


    shared_ptr<ValueSource<filesystem::path> > backend_attach_file =
        ValueSource<filesystem::path>::instantiate();
    Component::Instance backend_attach_file_component = 
        reinterpret_pointer_cast<Component>(backend_attach_file);
    Component::connect(
        backend_attach_file_component, "value", launcher, "BackendAttachFile"
        );    


    shared_ptr<ValueSource<filesystem::path> > topology_file =
        ValueSource<filesystem::path>::instantiate();
    Component::Instance topology_file_component = 
        reinterpret_pointer_cast<Component>(topology_file);
    Component::connect(
        topology_file_component, "value", launcher, "TopologyFile"
        );

    //shared_ptr<ValueSink<bool> > done = ValueSink<bool>::instantiate();
    //Component::Instance outputs_component =
     //       reinterpret_pointer_cast<Component>(done);
    //Component::connect(network, "output", outputs_component, "value");
    shared_ptr<ValueSink<bool> > threads_finished = ValueSink<bool>::instantiate();
    Component::Instance threads_finished_output_component =
            reinterpret_pointer_cast<Component>(threads_finished);
    Component::connect(network, "threads_finished", threads_finished_output_component, "value");


    Component::connect(launcher, "Network", network, "Network");

    *backend_attach_count = numBE;
    *backend_attach_file = connections;
    *topology_file = topology;

    std::map<std::string, Type> inputs = network->getInputs();

    if (inputs.find("numBE") != inputs.end()) {
    boost::shared_ptr<ValueSource<int> > numberBackends =
        ValueSource<int>::instantiate();
    Component::Instance numberBackends_component =
        boost::reinterpret_pointer_cast<Component>(numberBackends);
    Component::connect(numberBackends_component, "value", network, "numBE");
    *numberBackends = numBE;
    }

    bool threads_done = false;
    while (true) {
        threads_done = *threads_finished;
        nanosleep((struct timespec[]){{0, 500000000}}, NULL);
        if (threads_done) {
            finished = true;
            break;
	}
    }        

  }

  private:
	boost::thread dm_thread;
};

int main(int argc, char** argv)
{
    unsigned int numBE;
    bool isMPI;
    std::string topology, arch, connections, collector, program, mpiexecutable, cbtfrunpath;


    // create a default for topology file.
    char const* curr_dir = getenv("PWD");

    std::string cbtf_path(curr_dir);


    std::string default_topology(curr_dir);
    default_topology += "/cbtf_topology";

    // create a default for connections file.
    std::string default_connections(curr_dir);
    default_connections += "/attachBE_connections";

    std::string default_arch("");

    // create a default for the collection type.
    std::string default_collector("pcsamp");

    boost::program_options::options_description desc("collectionTool options");
    desc.add_options()
        ("help,h", "Produce this help message.")
        ("numBE", boost::program_options::value<unsigned int>(&numBE)->default_value(1),
	    "Number of lightweight mrnet backends. Default is 1, For an mpi job this must match the number of mpi ranks specififed in the mpi launcher arguments.")
        ("arch",
	    boost::program_options::value<std::string>(&arch)->default_value(""),
	    "automatic topology type defaults to a standard cluster.  These options are specific to a Cray or BlueGene. [cray | bluegene]")
        ("topology",
	    boost::program_options::value<std::string>(&topology)->default_value(""),
	    "By default the tool will create a topology for you.  Use this option to pass a path name to a valid mrnet topology file. (i.e. from mrnet_topgen). Use this options with care.")
        ("connections",
	    boost::program_options::value<std::string>(&connections)->default_value(default_connections),
	    "Path name to a valid backend connections file. The connections file is created by the mrnet backends based on the mrnet topology file. The default is sufficient for most cases.")
        ("collector",
	    boost::program_options::value<std::string>(&collector)->default_value(default_collector),
	    "Name of collector to use [pcsamp | usertime | hwc]. Default is pcsamp.")
        ("program",
	    boost::program_options::value<std::string>(&program)->default_value(""),
	    "Program to collect data from, Program with arguments needs double quotes.  If program is not specified this client will start the mrnet tree and wait for the user to manually attach backends in another window via cbtfrun.")
        ("cbtfrunpath",
            boost::program_options::value<std::string>(&cbtfrunpath)->default_value(""),
            "Path to cbtfrun to collect data from, If target is cray or bluegene, use this to point to the targeted client.")
        ("mpiexecutable",
	    boost::program_options::value<std::string>(&mpiexecutable)->default_value(""),
	    "Name of the mpi executable. This must match the name of the mpi exectuable used in the program argument and implies the collection is being done on an mpi job if it is set.")
        ;

    boost::program_options::variables_map vm;

    // handle any regular options
    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
    boost::program_options::notify(vm);    

    // handle any positional options
    boost::program_options::positional_options_description p;
    boost::program_options::store(boost::program_options::command_line_parser(argc, argv).
				  options(desc).positional(p).run(), vm);
    boost::program_options::notify(vm);


    // Generate the --mpiexecutable argument value if it is not set
      
    if (program != "" && mpiexecutable == "") {

      // Find out if there is an mpi driver to key off of
      // Then match the mpiexecutable value to the program name
      mpiexecutable = getMPIExecutableFromCommand(program);

    }

    if (mpiexecutable != "") {
         numBE = getBEcountFromCommand(program);
    }

    if (vm.count("help")) {
        std::cout << desc << std::endl;
        return 1;
    }

    bool finished = false;
    std::string aprunLlist ="";

    if (numBE == 0) {
        std::cout << desc << std::endl;
        return 1;
    }

    // TODO: pass numBE to CBTFTopology and record as the number
    // of application processes.
    CBTFTopology cbtftopology;
    if (topology.empty()) {
      if (arch == "cray") {
          cbtftopology.autoCreateTopology(BE_CRAY_ATTACH,numBE);
      } else {
          cbtftopology.autoCreateTopology(BE_ATTACH,numBE);
      }
      topology = cbtftopology.getTopologyFileName();
      std::cerr << "Generated topology file: " << topology << std::endl;
    }

    // verify valid numBE.
    if (numBE == 0) {
        std::cout << desc << std::endl;
        return 1;
    } else if (program == "" && numBE > 0) {
	// this allows us to start the mrnet client FE
	// and then start the program with collector in
	// a separate window using cbtfrun.
	FEThread fethread;
	fethread.start(topology,connections,collector,numBE,finished);
	std::cout << "Running Frontend for " << collector << " collector."
	  << "\nNumber of mrnet backends: "  << numBE
          << "\nTopology file used: " << topology << std::endl;
	std::cout << "Start mrnet backends now..." << std::endl;
	// ctrl-c to exit.  need cbtfdemo to notify us when all threads
	// have finised.
	while(true);
        fethread.join();
	exit(0);
    } else {
        std::cout << "Running " << collector << " collector."
	  << "\nProgram: " << program
	  << "\nNumber of mrnet backends: "  << numBE
          << "\nTopology file used: " << topology << std::endl;
    }

    // TODO: need to cleanly terminate mrnet.
    FEThread fethread;
    fethread.start(topology,connections,collector,numBE,finished);
    sleep(3);

    // simple fork of process to run the program with collector.
    pid_t child,w;
    int status;

    child = fork();
    if(child < 0){
        std::cout << "fork failed";
    } else if(child == 0){
        if (!mpiexecutable.empty()) {

	    size_t pos;
	    if (cbtftopology.getIsCray()) {
		if (std::string::npos != program.find("aprun")) {
		    // Add in the -L list of nodes if aprun is present 
		    // and we are not co-locating
		    std::list<std::string> nodes = cbtftopology.getAppNodeList();
		    std::string appNodesForAprun = "-L " + cbtftopology.createRangeCSVstring(nodes) + " "; 
		    pos = program.find("aprun ") + 6;
		    program.insert(pos, appNodesForAprun);
		}
	    }

	    pos = program.find(mpiexecutable);

            exe_class_types appl_type =  typeOfExecutable(program, mpiexecutable);

            if (appl_type == MPI_exe_type) {

              if (!cbtfrunpath.empty()) {
                program.insert(pos, " " + cbtfrunpath + " --mrnet --mpi -c " + collector + " \"");
              } else {
                program.insert(pos, " cbtfrun --mrnet --mpi -c " + collector + " \"");
              }

            } else {

              if (!cbtfrunpath.empty()) {
                program.insert(pos, " " + cbtfrunpath + " --mrnet -c " + collector + " \"");
              } else {
                program.insert(pos, " cbtfrun --mrnet -c " + collector + " \"");
              }

            }
            program.append("\"");
            std::cerr << "executing mpi program: " << program << std::endl;
	    
            ::system(program.c_str());

	} else {
    	    const char * command = "cbtfrun";
            if (!cbtfrunpath.empty()) {
                command = cbtfrunpath.c_str() ;
            } 

            std::cerr << "executing sequential program: "
		<< command << " -m -c " << collector << " " << program << std::endl;

            execlp(command,"-m", "-c", collector.c_str(), program.c_str(), NULL);
	}
    } else {

	do {
	    w = waitpid(child, &status, WUNTRACED | WCONTINUED);
	    if (w == -1) {
		perror("waitpid");
		exit(EXIT_FAILURE);
	    }
	} while (!WIFEXITED(status) && !WIFSIGNALED(status));

        fethread.join();
    }

}
