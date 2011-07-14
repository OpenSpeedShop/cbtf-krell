////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2010,2011 Krell Institute. All Rights Reserved.
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
#include <sstream>
#include <boost/program_options.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <KrellInstitute/CBTF/BoostExts.hpp>
#include <KrellInstitute/CBTF/Component.hpp>
#include <KrellInstitute/CBTF/Type.hpp>
#include <KrellInstitute/CBTF/ValueSink.hpp>
#include <KrellInstitute/CBTF/ValueSource.hpp>
#include <KrellInstitute/CBTF/XML.hpp>
#include <map>
#include <set>
#include <stdexcept>
#include <string>


using namespace boost;
using namespace KrellInstitute::CBTF;



namespace std {

    /**
     * Redirect a std::map<std:string, Type> const iterator to an output stream.
     * Defined in order to allow the Boost.Test macros to work properly.
     *
     * @param stream      Target output stream.
     * @param iterator    Const iterator to redirect.
     * @return            Target output stream.
     */
    std::ostream& operator<<(
        std::ostream& stream,
        const std::map<std::string, Type>::const_iterator& iterator
        )
    {
        stream << "std::map<std::string, Type>::const_iterator";
        return stream;
    }

    /**
     * Redirect a std::set<Type> const iterator to an output stream. Defined
     * in order to allow the Boost.Test macros to work properly.
     *
     * @param stream      Target output stream.
     * @param iterator    Const iterator to redirect.
     * @return            Target output stream.
     */
    std::ostream& operator<<(std::ostream& stream,
                             const std::set<Type>::const_iterator& iterator)
    {
        stream << "std::set<Type>::const_iterator";
        return stream;
    }
    
} // namespace std


/**
 * Main function for the example PC sampling tool.
 */

class DaemonToolDemo
{
  public:

  DaemonToolDemo()
  {
  }

  void start(const std::string& topology, const std::string& cmd, const unsigned int& numBE)
  {
    dm_thread = boost::thread(&DaemonToolDemo::run, this, topology, cmd, numBE);
  }

  void join()
  {
    dm_thread.join();
  }

  void run(const std::string& topology, const std::string& cmd, const unsigned int& numBE)
  {

    registerXML(filesystem::path(XMLDIR) / "daemonToolDemo.xml");

    std::set<Type> available_types = Component::getAvailableTypes();


    Component::registerPlugin(
        filesystem::path(LIBDIR) / "KrellInstitute/CBTF/BasicMRNetLaunchers");
    
    Component::Instance network = Component::instantiate(
        Type("Daemon_Tool_Demo")
        );

    Component::Instance launcher = Component::instantiate(
        Type("BasicMRNetLauncherUsingBackendCreate")
        );

    boost::shared_ptr<ValueSource<filesystem::path> > topology_file =
        ValueSource<filesystem::path>::instantiate();
    boost::shared_ptr<ValueSource<std::string> > input_value =
        ValueSource<std::string>::instantiate();
    boost::shared_ptr<ValueSink<std::vector<std::string> > > output_value =
        ValueSink<std::vector<std::string> >::instantiate();

    Component::Instance topology_file_component = 
        reinterpret_pointer_cast<Component>(topology_file);
    Component::Instance input_value_component =
        boost::reinterpret_pointer_cast<Component>(input_value);
    Component::Instance output_value_component =
        boost::reinterpret_pointer_cast<Component>(output_value);

    Component::connect(
        topology_file_component, "value", launcher, "TopologyFile"
        );

    Component::connect(launcher, "Network", network, "Network");
    Component::connect(input_value_component, "value", network, "in");
    Component::connect(network, "out", output_value_component, "value");

    // start mrnet network
    *topology_file = topology;
    // send the command to run on backend (on backend filter)
    *input_value = cmd;

    int num_done = 0;
    // display the results of the command...
    while (num_done < numBE) {
        std::vector<std::string> outval = *output_value;
        std::vector<std::string>::const_iterator ci;
        for(ci = outval.begin(); ci != outval.end(); ci++) {
            std::cout << *ci << std::endl;
        }
	num_done++;
    }

  }

  private:
	boost::thread dm_thread;
};

int main(int argc, char** argv)
{
    unsigned int numBE;
    std::string topology;
    std::string cmd;

    // create a default for topology file.
    char const* home = getenv("HOME");
    std::string default_topology(home);
    default_topology += "/.cbtf/cbtf_topology";
    std::string default_cmd = "ps -ef";

    boost::program_options::options_description desc("pcsampDemo options");
    desc.add_options()
        ("help,h", "Produce this help message.")
        ("numBE", boost::program_options::value<unsigned int>(&numBE)->default_value(1),
            "Number of expected mrnet backends. Default is 1, This should match the number of nodes in your topology and job allocation.")
        ("topology",
	    boost::program_options::value<std::string>(&topology)->default_value(default_topology),
	    "Path name to a valid mrnet topology file. (i.e. from mrnet_topgen),")
        ("cmd",
	    boost::program_options::value<std::string>(&cmd)->default_value(default_cmd),
	    "Command to execute on backend tool daemon. Must be in quotes if command has arguments.")
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

    DaemonToolDemo dtdemo;
    std::cout << "Running command " << cmd 
          << "\nNumber of mrnet backends: "  << numBE
	  << std::endl;
    dtdemo.start(topology,cmd,numBE);

    dtdemo.join();
}
