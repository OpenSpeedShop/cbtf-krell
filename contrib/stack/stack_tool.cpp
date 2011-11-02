////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2010 Krell Institute. All Rights Reserved.
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

/** @file Unit tests for the CBTF XML library. */


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

using namespace KrellInstitute::CBTF;

/**
 * Unit test for the MonolithicTool class.
 */
int main(int argc,  char *argv[ ])
{
  if(argc != 2)
  {
    std::cout << "Error argc = " << argc 
      << " stack must be run with the name of an application\n";
    std::cout << "  example >./stack mpi_hang\n";
    return 0; 
  }

  // XML
  registerXML(boost::filesystem::path(XMLDIR) / "stack.xml");
  //registerXML("stack.xml");

  // Setup MRNet
  Component::registerPlugin(
            boost::filesystem::path(LIBDIR) / "KrellInstitute/CBTF/BasicMRNetLaunchers.so"
            );

  Component::Instance launcher = Component::instantiate(
    Type("BasicMRNetLauncherUsingBackendCreate")
  );

  // Setup the Network
  Component::Instance network;
  network = Component::instantiate(Type("stackNetwork"));

  Component::connect(launcher, "Network", network, "Network");

  // Setup Topology file
  boost::shared_ptr<ValueSource<boost::filesystem::path> > topology_file =
        ValueSource<boost::filesystem::path>::instantiate();

 Component::Instance topology_file_component = boost::reinterpret_pointer_cast<Component>(topology_file);

  Component::connect(
        topology_file_component, "value", launcher, "TopologyFile"
        );

  // create the input
  boost::shared_ptr<ValueSource<std::string> > input_value = ValueSource<std::string>::instantiate();
  Component::Instance input_value_component = boost::reinterpret_pointer_cast<Component>(input_value);
  Component::connect(input_value_component, "value", network, "in");

  // create the output
  boost::shared_ptr<ValueSink<std::vector<std::string> > > output_value = ValueSink<std::vector<std::string> >::instantiate();
  Component::Instance output_value_component = boost::reinterpret_pointer_cast<Component>(output_value);
  Component::connect(network, "out", output_value_component, "value");

  char const* home = getenv("HOME");
  std::string default_topology(home);
  default_topology += "/.cbtf/cbtf_topology";

  // start mrnet network
  *topology_file = default_topology;

  // send the app name down the mrnet tree
  *input_value = argv[1];

  std::vector<std::string> output = *output_value;

  for(std::vector<std::string>::const_iterator i = output.begin(); 
        i != output.end(); ++i)
  {
    std::cout << *i;
  }

  return 0;
}

