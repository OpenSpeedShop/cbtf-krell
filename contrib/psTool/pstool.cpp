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
 * 
 */
int main(int argc, char *argv[])
{
  // XML
  registerXML(boost::filesystem::path(XMLDIR) / "ps.xml");
  //registerXML("ps.xml");

  // Setup MRNet
  Component::registerPlugin(
            boost::filesystem::path(LIBDIR) / "KrellInstitute/CBTF/BasicMRNetLaunchers.so"
            );

  Component::Instance launcher = Component::instantiate(
    Type("BasicMRNetLauncherUsingBackendCreate")  );

  // Setup the Network
  Component::Instance network;
  network = Component::instantiate(Type("psNetwork"));

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
  boost::shared_ptr<ValueSink<std::vector<std::string> > > outSame_value = ValueSink<std::vector<std::string> >::instantiate();
  Component::Instance outSame_value_component = boost::reinterpret_pointer_cast<Component>(outSame_value);
  Component::connect(network, "outSame", outSame_value_component, "value");

boost::shared_ptr<ValueSink<std::vector<std::string> > > outDiff_value = ValueSink<std::vector<std::string> >::instantiate();
  Component::Instance outDiff_value_component = boost::reinterpret_pointer_cast<Component>(outDiff_value);
  Component::connect(network, "outDiff", outDiff_value_component, "value");

  char const* home = getenv("HOME");
  std::string default_topology(home);
  default_topology += "/.cbtf/cbtf_topology";

  // start mrnet network
  *topology_file = default_topology;

  // start chain
  *input_value = "start";
  
  std::vector<std::string> outSame = *outSame_value;
  std::vector<std::string> outDiff = *outDiff_value;

  // output same
  std::cout << "---Same Proc's---\n";
  for(std::vector<std::string>::const_iterator i = outSame.begin(); 
        i != outSame.end(); ++i)
  {
    std::cout << *i;
  }
  std::cout << "---End Same Proc's---\n";

  // output diff
  std::cout << "---Diff Proc's---\n";
  for(std::vector<std::string>::const_iterator i = outDiff.begin(); 
        i != outDiff.end(); ++i)
  {
    std::cout << *i;
  }
  std::cout << "---End Diff Proc's---\n";

  return 0;
}

