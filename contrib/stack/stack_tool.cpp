////////////////////////////////////////////////////////////////////////////////
// stack_tool.cpp tool file for the stack tool
// LACC #:  LA-CC-13-051
// Copyright (c) 2013 Michael Mason; HPC-3, LANL
// Copyright (c) 2013 Krell Institute. All Rights Reserved.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
////////////////////////////////////////////////////////////////////////////////

/** @file Unit tests for the CBTF XML library. */


// This is the include section where we include libraries from
// boost, MRNet and the CBTF libraries from the Krell Institute.
// Below we also include generic C++ libraries for some manipulation 
// of the data returned by the CBTF network.

#include <boost/shared_ptr.hpp>
#include <boost/program_options.hpp>

#include <sys/param.h>
#include <mrnet/MRNet.h>
#include <typeinfo>

#include <KrellInstitute/CBTF/BoostExts.hpp>
#include <KrellInstitute/CBTF/Component.hpp>
#include <KrellInstitute/CBTF/Type.hpp>
#include <KrellInstitute/CBTF/ValueSink.hpp>
#include <KrellInstitute/CBTF/ValueSource.hpp>
#include <KrellInstitute/CBTF/Version.hpp>
#include <KrellInstitute/CBTF/XML.hpp>

#include <iostream>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <stdio.h>
#include <sstream>
#include <unistd.h>
#include <vector>

// we work in the KrellInstitute::CBTF name space
using namespace KrellInstitute::CBTF;

// This is the main function for the Tool, it will take the name of the 
// MPI application as a command line argument.
int main(int argc,  char *argv[ ])
{
  // just checking to make sure the users gave the name of an   
  // application on the command line.
  if(argc != 2)
  {
    std::cout << "Error argc = " << argc 
      << " stack must be run with the name of an application\n";
    std::cout << "  example >./stack mpi_hang\n";
    return 0; 
  }

  // Register the XML file for the Tool that defines the connections
  // between the components.  In the build tree boost::filesystem::path(XMLDIR) 
  // will be substituted for the XML directory path, if you are building 
  // outside of the build tree you can specify the full path or find a way to make it more flexible.
  registerXML(boost::filesystem::path(XMLDIR) / "stack.xml");

  // Setup MRNet in BackendCreate mode using the premade Krell components.
  Component::registerPlugin(
      boost::filesystem::path(LIBDIR) / "KrellInstitute/CBTF/BasicMRNetLaunchers.so");
  Component::Instance launcher = Component::instantiate(
      Type("BasicMRNetLauncherUsingBackendCreate") );

  // Start the CBTF Network defined in the Tools XML file which we registered above.
  Component::Instance network;
  network = Component::instantiate(Type("stackNetwork"));
  Component::connect(launcher, "Network", network, "Network");

  // Setup the Topology file as a boost shared pointer, then create a component 
  // out of it and create a connection.  These are the few connections we define 
  // in C++, they are simpler in the XML files.
  boost::shared_ptr<ValueSource<boost::filesystem::path> > topology_file =
      ValueSource<boost::filesystem::path>::instantiate();
  Component::Instance topology_file_component = boost::reinterpret_pointer_cast<Component>(topology_file);
  Component::connect(
      topology_file_component, "value", launcher, "TopologyFile");

  // Create the initial input to the CBTF network, which will be the name 
  // of the MPI application.  Again we start by making a boost shared 
  // pointer, turn it into a CBTF component then create a connection. 
  // We will see in the XML file where the input value "in" is defined in the network.
  boost::shared_ptr<ValueSource<std::string> > input_value = ValueSource<std::string>::instantiate();
  Component::Instance input_value_component = boost::reinterpret_pointer_cast<Component>(input_value);
  Component::connect(input_value_component, "value", network, "in");

  // Create the final output from the CBTF network, which will be the 
  // group of stack traces from the MPI application.  Again we start by 
  // making a boost shared pointer, turn it into a CBTF component then 
  // create a connection.  We will see in the XML file where the output 
  // value "out" is defined in the network.
  boost::shared_ptr<ValueSink<std::vector<std::string> > > output_value = ValueSink<std::vector<std::string> >::instantiate();
  Component::Instance output_value_component = boost::reinterpret_pointer_cast<Component>(output_value);
  Component::connect(network, "out", output_value_component, "value");

  // To start MRNet we must give it a topology file.  The Tool assumes 
  // that the topology file will exist in the users home directory 
  // under ~/.cbtf/cbtf_topology.  We set the Boost pointer to this and 
  // that information is transferred to the component to start MRNet.
  char const* home = getenv("HOME");
  std::string default_topology(home);
  default_topology += "/.cbtf/cbtf_topology";
  *topology_file = default_topology;

  // Send the application name that was passed as a command line 
  // argument down the mrnet tree, again by setting the boost variable
  *input_value = argv[1];

  // Because output_value is a Boost variable this call is blocking 
  // until a value is sent up the MRNet tree to the output "out" from 
  // the network.  The stack traces are returned as a vector of strings.
  std::vector<std::string> output = *output_value;


  // Now the vector of strings output which contains all the stack 
  // trace information is simply shown to the user.
  for(std::vector<std::string>::const_iterator i = output.begin(); 
        i != output.end(); ++i)
  {
    std::cout << *i;
  }

  return 0;
}

