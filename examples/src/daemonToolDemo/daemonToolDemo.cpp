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

#include <iostream>
#include <boost/program_options.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <KrellInstitute/CBTF/BoostExts.hpp>
#include <KrellInstitute/CBTF/Component.hpp>
#include <KrellInstitute/CBTF/Type.hpp>
#include <KrellInstitute/CBTF/ValueSink.hpp>
#include <KrellInstitute/CBTF/ValueSource.hpp>
#include <KrellInstitute/CBTF/XML.hpp>
#include <string>

using namespace boost;
using namespace KrellInstitute::CBTF;
using namespace std;



/**
 * Class wrapping the main implementation of the example daemon tool.
 */
class DaemonToolDemo
{
    
public:
    
    DaemonToolDemo()
    {
    }
    
    void start(const string& topology,
               const string& cmd,
               const unsigned int& numBE)
    {
        dm_thread = thread(&DaemonToolDemo::run, this, topology, cmd, numBE);
    }

    void join()
    {
        dm_thread.join();
    }

    void run(const string& topology,
             const string& cmd,
             const unsigned int& numBE)
    {
        //
        // In order to make use of the CBTF distrbuted (via MRNet) component
        // network Daemon_Tool_Demo defined in daemonToolDemo.xml we must first
        // register that XML file with CBTF.
        //

        registerXML(filesystem::path(XMLDIR) / "daemonToolDemo.xml");

        //
        // The plugin BasicMRNetLaunchers contains, you guessed it, two basic
        // MRNet launchers - one that uses MRNet's backend create mode, and one
        // that uses MRNet's backend attach mode. Since we want to utilize the
        // distributed component network Daemon_Tool_Demo exclusive of the type
        // of launcher employed, that network does not specify a launcher. We
        // will create one explicitly in just a moment. That means, however,
        // that Daemon_Tool_Demo doesn't list BasicMRNetLaunchers in its list
        // of plugins. Thus we must register it explicitly here.
        //

        Component::registerPlugin(
            filesystem::path(LIBDIR) / "KrellInstitute/CBTF/BasicMRNetLaunchers"
            );

        //
        // Create an instance of the Daemon_Tool_Demo distributed component
        // network. This constructs the component network that resides on the
        // frontend, but does not yet attempt to construct the filter/backend
        // component networks. This full network is not instantiated until a
        // MRNet Network object is received on the Network input. This input
        // is not defined in daemonToolDemo.xml - it is automagically added
        // by CBTF to ALL MRNet-based CBTF distributed component networks.
        //
        
        Component::Instance network = Component::instantiate(
            Type("Daemon_Tool_Demo")
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
        // component to the Network input of our Daemon_Tool_Demo instance.
        // 

        Component::connect(launcher, "Network", network, "Network");

        //
        // In order to pass values between "raw" C++ code and a CBTF component
        // network, bridge objects are used. The CBTF ValueSource and ValueSink
        // template classes function as these bridges. In this example we have
        // two inputs to, and one output from, the Daemon_Tool_Demo+launcher
        // combination:
        //
        //     topology_file: The name of the topology file describing the
        //                    MRNet network to be constructed. This will be
        //                    passed into the launcher.
        //
        //           command: The command to be executed. This is passed into
        //                    the Daemon_Tool_Demo component.
        //
        //           outputs: The outputs of the coomand. This is generated
        //                    by the Daemon_Tool_Demo component.
        //
        // Note that unlike most other CBTF components, the Value[Source|Sink]
        // components are explictly instantiated via their instantiate() method.
        // In order for CBTF to connect these bridge components into our network
        // we must cast them to a Component::Instance as well.
        //

        shared_ptr<ValueSource<filesystem::path> > topology_file =
            ValueSource<filesystem::path>::instantiate();

        shared_ptr<ValueSource<string> > command =
            ValueSource<string>::instantiate();

        shared_ptr<ValueSink<vector<string> > > outputs =
            ValueSink<vector<string> >::instantiate();
        
        Component::Instance topology_file_component = 
            reinterpret_pointer_cast<Component>(topology_file);
        
        Component::Instance command_component =
            reinterpret_pointer_cast<Component>(command);
        
        Component::Instance outputs_component =
            reinterpret_pointer_cast<Component>(outputs);

        //
        // Connect the bridges to the appropriate inputs and output of the
        // launcher and Daemon_Tool_Demo components.
        //

        Component::connect(
            topology_file_component, "value", launcher, "TopologyFile"
            );

        Component::connect(command_component, "value", network, "command");

        Component::connect(network, "output", outputs_component, "value");

        //
        // Now that all the components have been hooked up, we are finally
        // ready to get things going. The BasicMRNetLauncherUsingBackendCreate
        // launcher needs a single input - the path of the MRNet topology file.
        // Once this value is passed in, the launcher calls MRNet to construct
        // the MRNet network and then emits the Network object on its output,
        // where it travels to the Daemon_Tool_Demo component and causes the
        // complete CBTF distrubted (via MRNet) component network to be created.
        // 

        *topology_file = topology;

        //
        // Send a command down the MRNet network to the backends for execution.
        // 
        
        *command = cmd;

        //
        // Loop until command outputs have been received from all backends.
        // 

        for (int num_received = 0; num_received < numBE; ++num_received)
        {
            //
            // The following line, which calls ValueSink<...>::operator(), will
            // block until a value is available on the component output to which
            // it is attached.
            //

            vector<string> output = *outputs;

            //
            // Display the received command output on the stdout stream.
            //

            for (vector<string>::const_iterator
                     i = output.begin(); i != output.end(); ++i)
            {
                cout << *i << endl;
            }
        }
    } // run
    
private:
	thread dm_thread;
};



/**
 * Main function for the example daemon tool.
 */
int main(int argc, char** argv)
{
    //
    // Create a default topology file location that is used if one isn't
    // specified as a command-line argument.
    //

    char const* home = getenv("HOME");
    string default_topology(home);
    default_topology += "/.cbtf/cbtf_topology";
    string default_cmd = "ps -ef";

    //
    // Parse the command-line arguments using Boost.Program_options.
    //

    unsigned int numBE;
    string topology;
    string cmd;
    
    program_options::options_description desc("daemonToolDemo options");

    desc.add_options()
        ("help,h", "Produce this help message.")

        ("numBE",
         program_options::value<unsigned int>(&numBE)->default_value(1),
         "Number of expected mrnet backends. Default is 1, This should "
         "match the number of nodes in your topology and job allocation.")
        
        ("topology",
         program_options::value<string>(&topology)->
         default_value(default_topology),
         "Path name to a valid mrnet topology file. (i.e. from mrnet_topgen),")

        ("cmd",
         program_options::value<string>(&cmd)->default_value(default_cmd),
         "Command to execute on backend tool daemon. Must be in quotes "
         "if command has arguments.")
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
    
    cout << "Running command " << cmd 
         << "\nNumber of mrnet backends: "  << numBE
         << endl;
    
    //
    // Construct an instance of DaemonToolDemo, initiate a separate thread
    // to run the command, and wait for that thread to complete.
    //

    DaemonToolDemo dtdemo;
    dtdemo.start(topology, cmd, numBE);
    dtdemo.join();
}
