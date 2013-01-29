////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2012-2013 Krell Institute. All Rights Reserved.
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

#include <iostream>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <KrellInstitute/CBTF/BoostExts.hpp>
#include <KrellInstitute/CBTF/Component.hpp>
#include <KrellInstitute/CBTF/Type.hpp>
#include <KrellInstitute/CBTF/ValueSink.hpp>
#include <KrellInstitute/CBTF/ValueSource.hpp>
#include <KrellInstitute/CBTF/XML.hpp>
#include "KrellInstitute/Core/CBTFTopology.hpp"
#include <string>

using namespace boost;
using namespace KrellInstitute::CBTF;
using namespace KrellInstitute::Core;
using namespace std;



/**
 * Class wrapping the main implementation of the daemon tool.
 */
class DaemonTool
{
    
  public:
    
    DaemonTool()
    {
    }
    
    void start(const string& topology,
               const string& tool,
               const string& toolargs,
               const unsigned int& numBE)
    {
        dm_thread = thread(&DaemonTool::run, this, topology, tool, toolargs, numBE);
    }

    void join()
    {
        dm_thread.join();
    }


    // The tool argument specifies an xml file where at minimum a Frontend
    // and Backend network must be defined. (expand on this).
    void run(const string& topology,
             const string& tool,
             const string& toolargs,
             const unsigned int& numBE)
    {
        //
        // The tool argument is a string that represents a CBTF distributed
        // component network tool specified in an xml file.  The xml must
        // specify at a minumum a <Frontend> and <Backend> component network
        // internally.  It is optional to specify <Filter> component networks
        // for placement on nodes where commuication processes may exist in
        // the topology (CP's are mrnet_commnodes where filter plugins may
        // be loaded). The xml file is responsible to set up all needed
        // component plugin paths and load needed components.
        //
        // In order to make use of the CBTF distrbuted (via MRNet) component
        // network defined in the tool .xml passed by the tool argument we
        // must first register that XML file with CBTF. The string defined
        // by the tool argument may potentially specify the xml filename
        // explicitly. Since we also use that name later below to identify
        // the network, we must extract the real tool name in that case.
        // i.e. MyTool names the tool definition as MyTool.xml and the
        // network instance as MyTool (which is encoded in the xml and
        // referenced below where the network is instantiated).

	std::string realtoolname;
	filesystem::path toolpath(tool);
	if (toolpath.is_complete()) {
	    registerXML(toolpath);
	    realtoolname = toolpath.stem();
	} else {
	    // search default path for xml defined by XMLDIR.
	    // TODO: provide a CBTF_XML_PATH environment variable
	    // for cases where xml tool specifications are installed
	    // elsewhere?
	    std::string xmlfile(tool);
	    xmlfile += ".xml";
	    registerXML(filesystem::path(XMLDIR) / xmlfile);
	    realtoolname = tool;
	}


        //
        // The plugin BasicMRNetLaunchers contains, you guessed it, two basic
        // MRNet launchers - one that uses MRNet's backend create mode, and one
        // that uses MRNet's backend attach mode. Since we want to utilize the
        // distributed component network "tool" exclusive of the type
        // of launcher employed, that network does not specify a launcher. We
        // will create one explicitly in just a moment. That means, however,
        // that "tool" doesn't list BasicMRNetLaunchers in its list
        // of plugins. Thus we must register it explicitly here.
        //

        Component::registerPlugin(
            filesystem::path(LIBDIR) / "KrellInstitute/CBTF/BasicMRNetLaunchers.so"
            );

        //
        // Create an instance of the "tool" distributed component
        // network. This constructs the component network that resides on the
        // frontend, but does not yet attempt to construct the filter/backend
        // component networks. This full network is not instantiated until a
        // MRNet Network object is received on the Network input. This input
        // is not defined in "tool".xml - it is automagically added
        // by CBTF to ALL MRNet-based CBTF distributed component networks.
        //
        
        Component::Instance network = Component::instantiate(
            Type(realtoolname)
            );

        //
        // Create an instance of the basic MRNet launcher that uses the backend
        // create mode. This component takes a MRNet topology file as input and
        // has a Network output that is of the MRNet Network type.
        // 

        Component::Instance launcher = Component::instantiate(
            Type("BasicMRNetLauncherUsingBackendCreate")
            );

        //
        // Instruct CBTF to connect the Network output of the launcher
        // component to the Network input of our "tool" instance.
        // 

        Component::connect(launcher, "Network", network, "Network");

        //
        // In order to pass values between "raw" C++ code and a CBTF component
        // network, bridge objects are used. The CBTF ValueSource and ValueSink
        // template classes function as these bridges. In this client we have
        // two inputs to and no outputs from, the "tool"+launcher
        // combination:
        //
        //     topology_file: The name of the topology file describing the
        //                    MRNet network to be constructed. This will be
        //                    passed into the launcher.
        //
        //           toolargs: Any arguments to the tool to be executed.
        //
        // Note that unlike most other CBTF components, the Value[Source|Sink]
        // components are explictly instantiated via their instantiate() method.
        // In order for CBTF to connect these bridge components into our network
        // we must cast them to a Component::Instance as well.
        //

        shared_ptr<ValueSource<filesystem::path> > topology_file =
            ValueSource<filesystem::path>::instantiate();

        Component::Instance topology_file_component = 
            reinterpret_pointer_cast<Component>(topology_file);

	shared_ptr<ValueSource<string> > args =
            ValueSource<string>::instantiate();

	Component::Instance args_component =
	    reinterpret_pointer_cast<Component>(args);

        //
        // Connect the bridges to the appropriate inputs and output of the
        // launcher and "tool" components.
        //

        Component::connect(
            topology_file_component, "value", launcher, "TopologyFile"
            );

	Component::connect(args_component, "value", network, "ToolArgs");

        //
        // Now that all the components have been hooked up, we are finally
        // ready to get things going. The BasicMRNetLauncherUsingBackendCreate
        // launcher needs a single input - the path of the MRNet topology file.
        // Once this value is passed in, the launcher calls MRNet to construct
        // the MRNet network and then emits the Network object on its output,
        // where it travels to the "tool" component and causes the
        // complete CBTF distrubted (via MRNet) component network to be created.
        // 

	// start the network.
        *topology_file = topology;
	// send any tool arguments.
	*args = toolargs;

    } // run
    
  private:
	thread dm_thread;
};



/**
 * Main function for the daemon tool.
 */
int main(int argc, char** argv)
{
    //
    // Create a default topology file location that is used if one isn't
    // specified as a command-line argument.
    //
    string default_topology("");

    //
    // Parse the command-line arguments using Boost.Program_options.
    //

    unsigned int numBE;
    string topology;
    string tool;
    string toolargs;
    
    program_options::options_description desc("daemonTool options");

    desc.add_options()
        ("help,h", "Produce this help message.")

        ("tool",
         program_options::value<string>(&tool)->default_value(""),
         "Name specifying tool to execute. This is the name of the tool xml specification. e.g. If the tool is defined by mytool.xml, use --tool mytool. In this case the default search path for cbtf XML files will be used. You can specify the full pathname. e.g. /foo/bar/abc.xml and the tool abc will be launched.")

        ("toolargs",
         program_options::value<string>(&toolargs)->default_value(""),
         "arguments to tool.  If more than one argument, this value must be in double quotes. e.g. --toolargs \"arg1 arg2\".")
        
        ("topology",
         program_options::value<string>(&topology)->
         default_value(default_topology),
         "Path name to a valid mrnet topology file. (i.e. from mrnet_topgen). If no topology is specified one will be created. Default is to auto create a topology and choose the full number of nodes available to you for the numBE option.")

        ("numBE",
         program_options::value<unsigned int>(&numBE)->default_value(1),
         "Number of desired mrnet backends. Default is 1, This typically should "
         "match the number of nodes in your job allocation.")
        ;
    
    program_options::variables_map vm;

    // Handle any regular options
    program_options::store(
        program_options::parse_command_line(argc, argv, desc), vm
        );
    program_options::notify(vm);    

    // Handle any positional options
    program_options::positional_options_description p;
    program_options::store(
        program_options::command_line_parser(argc, argv).
        options(desc).positional(p).run(), vm
        );
    program_options::notify(vm);

    if (vm.count("help"))
    {
        cout << desc << endl;
        return 1;
    }
    
    std::cout << "Running tool " << tool 
         << "\nNumber of mrnet backends: "  << numBE
         << std::endl;
    
    //
    // Construct an instance of DaemonTool, initiate a separate thread
    // to run the command, and wait for that thread to complete.
    //

    if (tool.empty()) {
	std::cerr << "No tool specified! Please specify a valid tool using --tool <tool name>" << std::endl;
    } else {

	if (topology.empty()) {
	    // For a daemon tool we typically wish to run one instance
	    // per node in a cluster.  So numBE should be set to the
	    // number of nodes you wish to monitor on.  Can be a
	    // subset of the cluster. TODO: Have the slurm parser
	    // autocreate this if numBE was not set on the command line.
	    CBTFTopology cbtftopology;
	    cbtftopology.autoCreateTopology(BE_START);
	    topology = cbtftopology.getTopologyFileName();
	    std::cerr << "Generated topology file: " << topology << std::endl;
	}

	// kick of the boost thread that runs the tool...
	DaemonTool dt;
	dt.start(topology, tool, toolargs, numBE);
	dt.join();
    } 
}
