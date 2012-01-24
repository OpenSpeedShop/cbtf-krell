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

using namespace boost;
using namespace KrellInstitute::CBTF;

/**
 * 
 */
int main(int argc,  char *argv[ ])
{
  if(argc != 4)
  {
    std::cout << "Error argc = " << argc 
      << " memtool must be run with the number of backends and the name of an application\n";
    std::cout << "  example >./memtool -be 2 mpi_hang\n";
    return 0; 
  }

  // parse argv
  std::string be = "";
  int numBE = 0;
  std::string cmd = "";
  std::string tmparg = "";
  tmparg += argv[1];
  tmparg += " ";
  tmparg += argv[2];
  tmparg += " ";
  tmparg += argv[3];
  std::stringstream argstream(tmparg);
  
  argstream >> be;
  argstream >> numBE;
  argstream >> cmd;

  if( be != "-be" )
  {
    std::cout << "Error must specify number of backends with -be" << std::endl;
    std::cout << "  example >./memtool -be 2 mpi_hang" << std::endl;
  }
  else if( numBE <= 0 )
  {
    std::cout << "Error number of backends must be greater then 0" << std::endl;
    std::cout << "  example >./memtool -be 2 mpi_hang" << std::endl;
  }
  else if( cmd == "" )
  {
    std::cout << "Error must include vaild proc name" << std::endl;
    std::cout << "  example >./memtool -be 2 mpi_hang" << std::endl;
  }

  char const* home = getenv("HOME");
  std::string default_topology(home);
  default_topology += "/.cbtf/cbtf_topology";

  // XML
  registerXML(filesystem::path(XMLDIR) / "mem.xml");

  // Setup MRNet
  Component::registerPlugin(
            filesystem::path(LIBDIR) / "KrellInstitute/CBTF/BasicMRNetLaunchers.so"
            );
  
  Component::Instance launcher = Component::instantiate(
    Type("BasicMRNetLauncherUsingBackendCreate")  );

  // Setup the Network
  Component::Instance network;
  network = Component::instantiate(Type("memNetwork"));

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

  // start mrnet network
  *topology_file = default_topology;

  // send the app name down the mrnet tree
  *input_value = cmd;

  std::vector<std::string> output;
  // get the output from each backend
  for(int numrec = 0; numrec < numBE; numrec++)
  {
    // wait for output
    output = *output_value;

    // print each line in the output vector
    for(std::vector<std::string>::const_iterator i = output.begin(); 
          i != output.end(); ++i)
    {
      std::cout << *i << std::endl;
    }
    std::cout << "----------------------" << std::endl;
  } // end for numBE

  return 0;
}
