////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2011-2014 Krell Institute. All Rights Reserved.
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

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <unistd.h>

#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string>
#include <iostream>
#include <sstream>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
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
//#include "KrellInstitute/Core/SymtabAPISymbols.hpp"
#include "KrellInstitute/Core/Time.hpp"
#include "KrellInstitute/Core/CBTFTopology.hpp"

using namespace boost;
using namespace KrellInstitute::CBTF;
using namespace KrellInstitute::Core;

enum exe_class_types { MPI_exe_type, SEQ_RunAs_MPI_exe_type, SEQ_exe_type, Unknown_exe_type };

namespace {
    /** suspends one half second */
    void suspend()
    {
        struct timespec wait;
        wait.tv_sec = 0;
        wait.tv_nsec = 500 * 1000 * 1000;
        while(nanosleep(&wait, &wait));
    }

    bool is_debug_timing_enabled = (getenv("CBTF_TIME_CLIENT_EVENTS") != NULL);
    bool is_debug_client_enabled = (getenv("CBTF_DEBUG_CLIENT") != NULL);
    bool defer_view = (getenv("OPENSS_DEFER_VIEW") != NULL);
}

// Client Utilities.

// Function that returns the number of BE processes that are required for LW MRNet BEs.
// The function tokenizes the program command and searches for -np or -n.
static int getBEcountFromCommand(std::string command) {

    int retval = 1;

    boost::char_separator<char> sep(" ");
    boost::tokenizer<boost::char_separator<char> > btokens(command, sep);
    std::string S = "";
    std::string n_token = "-n";
    std::string np_token = "-np";
    std::string ntasks_token = "--ntasks=";
    std::string dashdashn_token = "--n";


    bool found_be_count = false;

    BOOST_FOREACH (const std::string& t, btokens) {
	S = t;

	// For openmpi, -n2 or -np2 is in fact rejected by the openmpi mpirun command.
	// If we find the -n case with no space, get the value just after -n and
	// return it as an int while terminating loop.

	std::string::size_type ntasks_token_pos = S.find(ntasks_token);
	std::string::size_type n_token_pos = S.find(n_token);
	std::string::size_type np_token_pos = S.find(np_token);
        std::string::size_type dashdashn_token_pos = S.find(dashdashn_token);

	if (S.size() > 2 && S[n_token_pos-1] == '-' && (S[n_token_pos] == '-' || S[ntasks_token_pos+1] == '-')  ) {

	// this is a -- command, so skip unless it is --ntasks=nn.
	// We want to get the number of ranks from the --ntasks= phrase

	    if (S.size() == 9 && S[ntasks_token_pos+2] == 'n' && S[n_token_pos+3] == 't' && S[n_token_pos+4] == 'a' ) {
		if (ntasks_token_pos != std::string::npos) {
		    std::string::size_type token_size = ntasks_token.length();
		    if (S.substr( ntasks_token_pos+token_size, std::string::npos ).size() > 0) {
			retval = boost::lexical_cast<int>(S.substr( ntasks_token_pos+token_size, std::string::npos ));
			break;
		    }
		}
            } else if (S.size() == 3 && S[dashdashn_token_pos+1] == '-' && S[dashdashn_token_pos+2] == 'n' ) {
                if (dashdashn_token_pos != std::string::npos) {
                    std::string::size_type token_size = dashdashn_token.length();
                    if (S.substr( dashdashn_token_pos+token_size, std::string::npos ).size() > 0) {
                        retval = boost::lexical_cast<int>(S.substr( dashdashn_token_pos+token_size, std::string::npos ));
                        break;
                    }
                }
	    } else {
		// SKIP -- token phrases
		continue;
	    }
	}
 
	if (np_token_pos == std::string::npos && n_token_pos != std::string::npos) {
	    std::string::size_type token_size = n_token.length();
	    if (S.substr( n_token_pos+token_size, std::string::npos ).size() > 0) {
		retval = boost::lexical_cast<int>(S.substr( n_token_pos+token_size, std::string::npos ));
		break;
	    }
	}

	// This handles the cases where there is a space after the -n or -np.
	// In fact, openmpi's mpirun requires the space.
	if (found_be_count) {
	    S = t;
	    retval = boost::lexical_cast<int>(S);
	    break;
	} else if (!strcmp(S.c_str(), std::string("--ntasks").c_str())) {
	    found_be_count = true;
	} else if (!strcmp(S.c_str(), std::string("--n").c_str())) {
	    found_be_count = true;
	} else if (!strcmp( S.c_str(), std::string("-np").c_str())) {
	    found_be_count = true;
	} else if (!strcmp(S.c_str(), std::string("-n").c_str())) {
	    found_be_count = true;
	}
    } // end foreach

    return retval;
}


//
// This routine runs the ldd command on the executable represented by the passed in executable name
// and looks to see if the passed in library name (libname) is found in the output.
//
static bool foundLibraryFromLdd(const std::string& exename, const std::string& libname)
{
    // is there any chance that ldd is not installed or in the default path?
    std::string command = "ldd ";

    // create our ldd command string with passed libname.
    command.append(exename.c_str());

    // popen ldd so we can process output.
    // make sure popen succeeds.? error checking?
    FILE *lddOutFile = popen(command.c_str(), "r");

    // now find is libname is anywhere in ldd.  More precise find
    // would use strings like "/libmpi.so".  However, there are versions
    // of libmpi.so, like libmpi_dbg.so that we would miss.
    if (lddOutFile != NULL) {
	char buffer[BUFSIZ];
	memset(&buffer, 0, sizeof(buffer));

	while (fgets(buffer, sizeof(buffer), lddOutFile)) {
	    std::string line(buffer);

	    if (!line.empty()) {
		if (line.find(libname) != std::string::npos) {
		    pclose(lddOutFile);  //  don't forget to close before return.
		    return true;
		}
	    }
	}
	pclose(lddOutFile);
	return false;
    } else {
	std::cerr << "WARNING: ldd on " << exename << " failed. Looking for " << libname  << std::endl;
	return false;
    }
}

//
// Determine if libmpi is present in this executable.
// libmpi should appear as a substring "/libmpi.so" in ldd output.
// We would like use strings like "/libmpi.so" for the search. However,
// there are versions of libmpi.so, like libmpi_dbg.so that we would miss.
// So, we are using "/libmpi" instead.
//
static bool isMpiExe(const std::string exe) {
    bool found_libmpi = foundLibraryFromLdd(exe,"/libmpi");
    return found_libmpi;
}

//
// Determine if openMP runtime library is present in this executable.
//
static bool isOpenMPExe(const std::string exe) {
    bool found_openmp = foundLibraryFromLdd(exe,"/libiomp5");
    if (!found_openmp) {
        found_openmp = foundLibraryFromLdd(exe,"/libgomp");
    }
    return found_openmp;
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
    if ((status_buffer.st_mode & S_IEXEC) != 0 && S_ISREG(status_buffer.st_mode))
	return true;

    return false;
}



// Function that returns the filename of the executable file found in the "command".
// It tokenizes the command and runs through it backwards looking for the first file that is executable.
// That might not be sufficient in all cases.
// Set type as needed to one of MPI_exe_type,SEQ_RunAs_MPI_exe_type,SEQ_exe_type.
static std::string getExecutableExeTypeFromCommand(std::string command, exe_class_types *type) {

    std::string retval = "";
    exe_class_types local_exe_type;

    boost::char_separator<char> sep(" ");
    boost::tokenizer<boost::char_separator<char> > btokens(command, sep);

    BOOST_FOREACH (const std::string& t, btokens) {
	if (is_executable( t )) {
	    local_exe_type = typeOfExecutable(command, t);
	    if (local_exe_type == MPI_exe_type ||
		local_exe_type == SEQ_RunAs_MPI_exe_type ||
		local_exe_type == SEQ_exe_type) {
		*type = local_exe_type;
		retval = t;
		break;
	    }
	}
    } // end foreach

    return retval;
}

/**
 * Creates a full directory tree for csv data files.
 *
 */
static std::string createCSVdir(std::string prefix)
{
    std::string csvDirName;

    char *csv_directory = getenv("CBTF_CSVDATA_DIR");
    if (csv_directory) {
	csvDirName = csv_directory;
    } else {
	boost::filesystem::path full_path( boost::filesystem::current_path() );
	csvDirName = full_path.c_str();
    }

    int cnt = 0;
    for (cnt = 0; cnt < 1000; ++cnt) {
	std::stringstream tmpName;
	tmpName << csvDirName << "/" << prefix << "-csvdata-" << cnt;
	Assert(tmpName.str().c_str() != NULL);

	if (boost::filesystem::exists(tmpName.str())) {
	    continue;
	} else {
	    boost::filesystem::create_directories(tmpName.str());
	    csvDirName = tmpName.str();
	    break;
	}
    }

    return csvDirName;
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

#ifndef NDEBUG
    if (is_debug_timing_enabled) {
	std::cerr << Time::Now()
	<< " collectionTool FE thread starts cbtf network."
	<< std::endl;
    }
#endif

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

    shared_ptr<ValueSink<bool> > threads_finished =
		ValueSink<bool>::instantiate();
    Component::Instance threads_finished_output_component =
		reinterpret_pointer_cast<Component>(threads_finished);
    Component::connect(network, "threads_finished",
		       threads_finished_output_component, "value");


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

#ifndef NDEBUG
    if (is_debug_timing_enabled) {
	std::cerr << Time::Now() << " collectionTool FE cbtf network running."
	<< std::endl;
    }
#endif


    // component network emit a threads_finished signal wheh
    // all ltwt BE's and all threads attached to said backends
    // have terminated.  We can fall out of this thread and
    // join in main to finish up and terminate.
    bool threads_done = false;
    while (true) {
        threads_done = *threads_finished;
        suspend();
        if (threads_done) {
            finished = true;
            break;
	}
    }        
#ifndef NDEBUG
    if (is_debug_timing_enabled) {
	std::cerr << Time::Now() << " collectionTool FE cbtf network finished."
	<< std::endl;
    }
#endif

  }

  private:
	boost::thread dm_thread;
};

int main(int argc, char** argv)
{
#ifndef NDEBUG
    if (is_debug_timing_enabled) {
	std::cerr << Time::Now() << " collectionTool started." << std::endl;
    }
#endif

    unsigned int numBE;
    std::string topology, arch, connections, collector, program, mpiexecutable,
		cbtfrunpath, seqexecutable;

#if defined(CBTF_CN_RUNTIME_DIR)
    cbtfrunpath = CBTF_CN_RUNTIME_DIR + "/bin/cbtfrun";
#else
    // assumes that cbtfrun is in PATH.
    // could use CMAKE_INSTALL_PREFIX/bin/cbtfrun if only building via cmake.
    cbtfrunpath = "cbtfrun";
#endif

    // create a default for topology file.
    char const* curr_dir = getenv("PWD");

    std::string cbtf_path(curr_dir);


    std::string default_topology(curr_dir);
    default_topology += "/cbtfAutoTopology";

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
	    "Number of lightweight mrnet backends. Default is 1, For an mpi job, the number of ranks specified to the launcher will be used.")
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
            boost::program_options::value<std::string>(&cbtfrunpath)->default_value("cbtfrun"),
            "Path to cbtfrun to collect data from, If target is cray or bluegene, use this to point to the targeted client.")
        ("mpiexecutable",
	    boost::program_options::value<std::string>(&mpiexecutable)->default_value(""),
	    "Name of the mpi executable. This must match the name of the mpi exectuable used in the program argument and implies the collection is being done on an mpi job if it is set.")
        ("offline", boost::program_options::bool_switch()->default_value(false),
	    "Use offline mode. Default is false.")
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

    bool use_offline_mode = vm["offline"].as<bool>();
    bool exe_has_openmp = false;
    exe_class_types theExecutableType = Unknown_exe_type;

    std::string theExecutable = getExecutableExeTypeFromCommand(program, &theExecutableType);

    if (theExecutable == "") {
        std::cerr << "Could not find executable to run from the specified input run command:" << std::endl;
        std::cerr << program << std::endl;
        return 1;
    }

    if (theExecutableType == MPI_exe_type || theExecutableType == SEQ_RunAs_MPI_exe_type ) {
	mpiexecutable = theExecutable;
	numBE = getBEcountFromCommand(program);
    } else if (theExecutableType == SEQ_exe_type ) {
	//seqexecutable = theExecutable;
    }

    // determine if this executable has openMP.
    exe_has_openmp = isOpenMPExe(theExecutable);

// CSVDIR
    if (collector == "overview")
    {

    boost::filesystem::path prg(theExecutable.c_str());
    if (prg.empty()) {
        prg += program;
    }

    // find just the real application name.
    std::vector<std::string> strs;
    std::string realname;
    boost::split(strs, prg.stem().string(), boost::is_any_of("\t "));
    realname = strs[0];
    std::cerr << "realname:" << realname << std::endl;

    // TODO: currently all of this csv directory work is intended for
    // the overview/summary experiment.  Maybe make it a command line
    // option as well.
    // create a csv directory based on application name and collector.
    std::string csvdirprefix(realname);
    csvdirprefix += "-";
    csvdirprefix += collector;
    // create a csvdir that does not conflict with an existing directory
    // using the naming scheme for the csvdata directory.
    std::string csvdirname = createCSVdir(csvdirprefix);
    //std::cerr << "csvdirname:" << csvdirname << std::endl;

    // now set the environment up for the collectors that may use
    // this.
    setenv("CBTF_CSVDATA_DIR", csvdirname.c_str(), true);

    }
// CSVDIR

    if (vm.count("help")) {
	std::cout << desc << std::endl;
	return 1;
    }

    bool finished = false;
    std::string aprunLlist = "";

    if (numBE == 0) {
	std::cout << desc << std::endl;
	return 1;
    }

    // start with a fresh connections file.
    // FIXME: this likely would remove any connections file passed
    // on the command line. Should we allow that any more...
    bool connections_exists = boost::filesystem::exists(connections);
    if (connections_exists) {
	boost::filesystem::remove(connections);
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

    FEThread fethread;

    // verify valid numBE.
    if (numBE == 0 && !use_offline_mode) {
	std::cout << desc << std::endl;
	return 1;
    } else if (!use_offline_mode && program == "" && numBE > 0) {
	// this allows us to start the mrnet client FE
	// and then start the program with collector in
	// a separate window using cbtfrun.
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
	if (use_offline_mode) {
	    std::cout << "Running offline " << collector << " collector."
	    << "\nProgram: " << program << std::endl;
	} else {
	    std::cout << "Running " << collector << " collector."
	    << "\nProgram: " << program
	    << "\nNumber of mrnet backends: "  << numBE
            << "\nTopology file used: " << topology << std::endl;

	    // TODO: need to cleanly terminate mrnet.
	    fethread.start(topology,connections,collector,numBE,finished);

	    // sleep was not sufficient to ensure we have a connections file
	    // written by the fethread.  Without the connections file the
	    // ltwt mrnet BE's cannot connect to the netowrk.
	    // Wait for the connections file to be written before proceeding
	    // to stat the mpi job and allowing the ltwt BEs to connect to
	    // the component network instantiated by the fethread.
	    bool connections_written = boost::filesystem::exists(connections);
	    while (!connections_written) {
		connections_written = boost::filesystem::exists(connections);
	    }

	}


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

	    // build the needed options for cbtfrun.
	    std::string cbtfrun_opts;
 
	    // is this an MPI program?
	    if (theExecutableType == MPI_exe_type) {
	        cbtfrun_opts.append(" --mpi ");
	    }

	    // does the program use OpenMP?
	    if (exe_has_openmp) {
		cbtfrun_opts.append(" --openmp");
	    }

	    // Does the user wish to run the offline version?
	    if (use_offline_mode) {
	        cbtfrun_opts.append(" --fileio ");
	    } else {
	        cbtfrun_opts.append(" --mrnet ");
	    }

	    // add the collector as last option.
	    cbtfrun_opts.append(" -c " + collector);
	    cbtfrun_opts.append(" ");

	    // now insert the cbtfrun command and it's options before the
	    // mpi executable.
	    pos = program.find(mpiexecutable);
	    program.insert(pos, " " + cbtfrunpath + " " + cbtfrun_opts);
	    
	    std::cout << "executing mpi program: " << program << std::endl;

	    ::system(program.c_str());

	} else {

	    std::string cmdtorun;
	    cmdtorun.append(cbtfrunpath + " -c " + collector);

	    // Does the user wish to run the offline version?
	    if (use_offline_mode) {
	        cmdtorun.append(" --fileio ");
	    } else {
	        cmdtorun.append(" --mrnet ");
	    }

	    if (exe_has_openmp) {
		cmdtorun.append(" --openmp ");
	    } else {
		cmdtorun.append(" ");

	    } 


	    cmdtorun.append(program);
	    std::cerr << "executing sequential program: " << cmdtorun << std::endl;
	    ::system(cmdtorun.c_str());
	}

	if (!use_offline_mode) {
	    fethread.join();
	}
    }

    if (use_offline_mode) {
    }

#ifndef NDEBUG
    if (is_debug_timing_enabled) {
	std::cerr << Time::Now() << " collectionTool exits." << std::endl;
    }
#endif
}
