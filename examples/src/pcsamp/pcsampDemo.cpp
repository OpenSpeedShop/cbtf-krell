////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2011 Krell Institute. All Rights Reserved.
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

/** @file Example PC sampling tool. */

#include <iostream>
#include <sstream>
#include <boost/program_options.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <KrellInstitute/CBTF/BoostExts.hpp>
#include <KrellInstitute/CBTF/Component.hpp>
#include <KrellInstitute/CBTF/Type.hpp>
#include <KrellInstitute/CBTF/ValueSource.hpp>
#include <KrellInstitute/CBTF/XML.hpp>

using namespace boost;
using namespace KrellInstitute::CBTF;


/**
 * Main function for the example PC sampling tool.
 */

class PCSampDemo
{
  public:

  PCSampDemo()
  {
  }

  void start(const std::string& topology, const unsigned int& numBE)
  {
    dm_thread = boost::thread(&PCSampDemo::run, this, topology, numBE);
  }

  void join()
  {
    dm_thread.join();
  }

  void run(const std::string& topology, const unsigned int& numBE)
  {
    // FIXME: hardcoded path
    registerXML(filesystem::path(BUILDDIR) / "pcsampDemo.xml");
    

    Component::registerPlugin(
        filesystem::path(LIBDIR) / "KrellInstitute/CBTF/BasicMRNetLaunchers");
    
    Component::Instance network = Component::instantiate(
        Type("PC_Sampling_Demo")
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


// Offline/libmonitor Lightweight MRNet instrumentation:
// The issue with specifying a connections file here is the we
// need the lightweight mrnet instrumentation to be in sync
// with any file specified here.  Currently both default to
// users $HOME/.cbtf/attachBEconnection.  It is likely easier
// to just leave it alone and standardize this aspect.
// For a future dyninst mode of instrumenting lightweight mrnet
// into a mutatee using a connections type file, we can
// possibly specify a connections file.  But that may not
// be needed since the dyninst backend daemon could like
// just pass the needed connection onformaion directly.
#if 0
    shared_ptr<ValueSource<filesystem::path> > backend_attach_file =
        ValueSource<filesystem::path>::instantiate();
    Component::Instance backend_attach_file_component = 
        reinterpret_pointer_cast<Component>(backend_attach_file);
    Component::connect(
        backend_attach_file_component, "value", launcher, "BackendAttachFile"
        );    

    // FIXME: hardcoded path. this does not seem to work anywys. :(
    *backend_attach_file = filesystem::path(BUILDDIR) / connection;
#endif

    shared_ptr<ValueSource<filesystem::path> > topology_file =
        ValueSource<filesystem::path>::instantiate();
    Component::Instance topology_file_component = 
        reinterpret_pointer_cast<Component>(topology_file);
    Component::connect(
        topology_file_component, "value", launcher, "TopologyFile"
        );

    Component::connect(launcher, "Network", network, "Network");

    *backend_attach_count = numBE;
    *topology_file = topology;

    // FIXME: signal that we are done (from pcsampDemoPlugin Display component)
    // Do proper shutown of mrnet here?
    while (true);
  }

  private:
	boost::thread dm_thread;
};

int main(int argc, char** argv)
{
    unsigned int numBE;
    bool isMPI;
    std::string topology;
    std::string collector;
    std::string program;
    std::string mpiexecutable;

    // create a default for topology file.
    char const* home = getenv("HOME");
    std::string default_topology(home);
    default_topology += "/.cbtf/cbtf_topology";
    std::string default_collector("pcsamp");

    boost::program_options::options_description desc("pcsampDemo options");
    desc.add_options()
        ("help,h", "Produce this help message.")
        ("numBE", boost::program_options::value<unsigned int>(&numBE)->default_value(1),
	    "Number of lightweight mrnet backends. Default is 1, For an mpi job this must match the number of mpi ranks specififed in the mpi launcher arguments.")
        ("topology",
	    boost::program_options::value<std::string>(&topology)->default_value(default_topology),
	    "Path name to a valid mrnet topology file. (i.e. from mrnet_topgen),")
        ("collector",
	    boost::program_options::value<std::string>(&collector)->default_value(default_collector),
	    "Name of collector to use. Default is pcsamp.")
        ("program",
	    boost::program_options::value<std::string>(&program)->default_value(""),
	    "Program to collect data from, Program with arguments needs double quotes.")
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

    if (vm.count("help")) {
        std::cout << desc << std::endl;
        return 1;
    }

    if (program == "") {
        std::cout << desc << std::endl;
        return 1;
    }

    // verify valid numBE.
    if (numBE == 0) {
        std::cout << desc << std::endl;
        return 1;
    } else {
        std::cout << "Running " << collector << " collector."
	  << "\nProgram: " << program
	  << "\nNumber of mrnet backends: "  << numBE
          << "\nTopology file used: " << topology << std::endl;
    }

    // TODO: need to cleanly terminate mrnet.
    PCSampDemo cbtfdemo;
    cbtfdemo.start(topology,numBE);
    sleep(3);

    // simple fork of process to run the program with collector.
    pid_t child;
    child = fork();
    if(child < 0){
        std::cout << "fork failed";
    } else if(child == 0){
        if (!mpiexecutable.empty()) {
	    size_t pos = program.find(mpiexecutable);
	    program.insert(pos, " cbtfrun --mrnet --mpi -c pcsamp \"");
	    program.append("\"");
	    std::cerr << "execucuting mpi program: " << program << std::endl;
	    
	    // FIXME: non-optimal.
            ::system(program.c_str());
	} else {
    	    const char * command = "cbtfrun";
            std::cerr << "executing sequential program: "
		<< command << " -m -c " << collector << " " << program << std::endl;
            execlp(command,"-m", "-c", collector.c_str(), program.c_str(), NULL);
	}
    } else {
        // FIXME: signal that we are done (from pcsampDemoPlugin Display component)
        // Do proper shutown of mrnet here?
        cbtfdemo.join();
    }

}
