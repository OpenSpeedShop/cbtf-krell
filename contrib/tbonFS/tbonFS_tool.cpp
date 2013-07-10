////////////////////////////////////////////////////////////////////////////////
// tbonFS_tool.cpp tool file for the tbonFS tool
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
#include <sstream>
#include <stdio.h>
#include <sstream>
#include <unistd.h>
#include <vector>

using namespace KrellInstitute::CBTF;

/**
 */
int main(int argc, char *argv[])
{
  // XML
  registerXML(boost::filesystem::path(XMLDIR) / "tbonFS.xml");
  //registerXML("tbonFS.xml");

  // Setup MRNet
  Component::registerPlugin(
            boost::filesystem::path(LIBDIR) / "KrellInstitute/CBTF/BasicMRNetLaunchers.so"
            );

  Component::Instance launcher = Component::instantiate(
    Type("BasicMRNetLauncherUsingBackendCreate")
  );

  // Setup the Network
  Component::Instance network;
  network = Component::instantiate(Type("tbonFSNetwork"));

  Component::connect(launcher, "Network", network, "Network");

  // Setup Topology file
  boost::shared_ptr<ValueSource<boost::filesystem::path> > topology_file =
        ValueSource<boost::filesystem::path>::instantiate();

 Component::Instance topology_file_component = boost::reinterpret_pointer_cast<Component>(topology_file);

  Component::connect(
        topology_file_component, "value", launcher, "TopologyFile"
        );

  // create the input
  // topIn
  boost::shared_ptr<ValueSource<std::string> > topIn = ValueSource<std::string>::instantiate();
  Component::Instance topIn_component = boost::reinterpret_pointer_cast<Component>(topIn);
  Component::connect(topIn_component, "value", network, "topIn");
  // grepIn
  boost::shared_ptr<ValueSource<std::vector<std::string> > > grepIn = ValueSource<std::vector<std::string> >::instantiate();
  Component::Instance grepIn_component = boost::reinterpret_pointer_cast<Component>(grepIn);
  Component::connect(grepIn_component, "value", network, "grepIn");
  // tailIn
  boost::shared_ptr<ValueSource<std::string> > tailIn = ValueSource<std::string>::instantiate();
  Component::Instance tailIn_component = boost::reinterpret_pointer_cast<Component>(tailIn);
  Component::connect(tailIn_component, "value", network, "tailIn");
  // readIn
  boost::shared_ptr<ValueSource<std::string> > readIn = ValueSource<std::string>::instantiate();
  Component::Instance readIn_component = boost::reinterpret_pointer_cast<Component>(readIn);
  Component::connect(readIn_component, "value", network, "readIn");
  // writeIn
  boost::shared_ptr<ValueSource<std::vector<std::string> > > writeIn = ValueSource<std::vector<std::string> >::instantiate();
  Component::Instance writeIn_component = boost::reinterpret_pointer_cast<Component>(writeIn);
  Component::connect(writeIn_component, "value", network, "writeIn");
  // cpIn
  boost::shared_ptr<ValueSource<std::vector<std::string> > > cpIn = ValueSource<std::vector<std::string> >::instantiate();
  Component::Instance cpIn_component = boost::reinterpret_pointer_cast<Component>(cpIn);
  Component::connect(cpIn_component, "value", network, "cpIn");

  // create the output
  // vecOut
  boost::shared_ptr<ValueSink<std::vector<std::string> > > vecOut = ValueSink<std::vector<std::string> >::instantiate();
  Component::Instance vecOut_component = boost::reinterpret_pointer_cast<Component>(vecOut);
  Component::connect(network, "vecOut", vecOut_component, "value");

  char const* home = getenv("HOME");
  std::string default_topology(home);
  default_topology += "/.cbtf/cbtf_topology";

  // set topology file to start mrnet
  *topology_file = default_topology;

  // CLI
  std::string lineIn = "";
  std::string args = "";
  std::string arg1 = "";
  std::string arg2 = "";
  int waitForOutput = 0;
  int space;
  std::vector<std::string> vec;

  std::cout << std::endl;

  while(1)
  { // read commands until the user quits
    waitForOutput = 0;
    vec.clear();
    arg1 = "";
    arg2 = "";

    // read command to lineIn and any args to args
    std::cout << "tbonFS> ";
    std::cin >> lineIn;
    std::getline(std::cin,args);
    std::stringstream argstream(args);
    //parse args
    argstream >> arg1;
    argstream >> arg2;

    // determine what command the user just input
    if( lineIn == "quit" || lineIn == "exit" )
    {
      break;
    }
    else if( lineIn == "help" ) 
    {
      std::cout << "All commands will run on every backend.  The output will be concatenated at the filter levels." << std::endl;
      std::cout << "list of commands:" << std::endl;
      std::cout << "  gCp srcFile dstFile   - copy a file" << std::endl;
      std::cout << "  gGrep pattern File    - search a file(s) for pattern" << std::endl;
      std::cout << "  gRead File            - read the contents of a file" << std::endl;
      std::cout << "  gTail File            - view the last 5 lines fo a file" << std::endl;
      std::cout << "  gTop                  - view the output of top" << std::endl;
      std::cout << "  gWrite File String    - write string to file" << std::endl;
      std::cout << "  exit || quit          - exit tbonFS shell" << std::endl;
    }
    else if( lineIn == "gGrep" )
    {
      if(arg2 == "") {
        std::cout << "Error gGrep must have two arguments - gGrep pattern File" << std::endl;
      }
      else
      {
        std::cout << "running 'gGrep " << arg1 << " " << arg2 << "' on backends." << std::endl;
        // build vec to send down mrnet
        vec.push_back(arg1);
        vec.push_back(arg2);

        *grepIn = vec;
        waitForOutput = 1;
      }
    }
    else if( lineIn == "gTail" )
    {
      if(arg1 == "") {
        std::cout << "Error gTail must have an argument - gTail File" << std::endl;
      }
      else
      {
        std::cout << "running 'gTail " << arg1 << "' on backends." << std::endl;
        // build vec to send down mrnet

        *tailIn = arg1;
        waitForOutput = 1;
      }
    }
    else if( lineIn == "gRead" )
    {
      if(arg1 == "") {
        std::cout << "Error gRead must have an argument - gRead File" << std::endl;
      }
      else
      {
        std::cout << "running 'gRead " << arg1 << "' on backends." << std::endl;
        // build vec to send down mrnet

        *readIn = arg1;
        waitForOutput = 1;
      }
    }
    else if( lineIn == "gWrite" )
    {
      if(arg2 == "") {
        std::cout << "Error gWrite must have two arguments - gWrite string File" << std::endl;
      }
      else
      {
        std::cout << "running 'gWrite " << arg1 << " " << arg2 << "' on backends." << std::endl;
        // build vec to send down mrnet
        vec.push_back(arg1);
        vec.push_back(arg2);

        *writeIn = vec;
        waitForOutput = 1;
      }
    }
    else if( lineIn == "gCp" )
    {
      if(arg2 == "") {
        std::cout << "Error gCp must have two arguments - gCp srcFile dstFile" << std::endl;
      }
      else
      {
        std::cout << "running 'gCp " << arg1 << " " << arg2 << "' on backends." << std::endl;
        // build vec to send down mrnet
        vec.push_back(arg1);
        vec.push_back(arg2);

        *cpIn = vec;
        waitForOutput = 1;
      }
    }
    else if( lineIn == "gTop" )
    {
      std::cout << "not implemented yet" << std::endl;
    }
    else
    { 
      std::cout << " ERROR - unknown command \"" << lineIn << "\" type help for list of commands.\n" << std::endl;
    }

    // should we wait for output?
    if( waitForOutput )
    {
      // wait for all the output
      std::vector<std::string> out = *vecOut;

      // write output to window
      for(std::vector<std::string>::const_iterator i = out.begin(); 
          i != out.end(); ++i)
      {
        //temp = i->substr(0,i->rfind("\n"));
        std::cout << *i << std::endl;
      }
    }//end if(waitForOutput)

  } //end while(1) CLI

  return 0;
}

