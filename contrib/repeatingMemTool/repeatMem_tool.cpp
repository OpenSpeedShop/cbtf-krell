////////////////////////////////////////////////////////////////////////////////
// repeatMem_tool.cpp tool file for repeating Mem Tool
// LACC #:  LA-CC-13-051
// Copyright (C) 2013 Michael Mason, Anthony J. (TJ) Machado; HPC-3, LANL
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
#include <stdio.h>
#include <sstream>
#include <unistd.h>

#include "Memory.h"
#include "../components/filters/statistics/statisticsPlugin.hpp"

using namespace boost;
using namespace KrellInstitute::CBTF;

void printUsageExample() {
    std::cout << "  example >./repeatMemtool -be 2 mpi_hang 45" << std::endl;
}

/**
 * 
 */
int main(int argc,  char *argv[ ])
{
    if(argc != 5)
    {
        std::cout << "Error argc = " << argc 
            << " repeatMemtool must be run with the number of backends and the name of an application\n";
        printUsageExample();
        return 0; 
    }

    // parse argv
    std::string be = "";
    int numBE = 0;
    int freq = 30;
    std::string cmd = "";
    std::string tmparg = "";
    tmparg += argv[1];
    tmparg += " ";
    tmparg += argv[2];
    tmparg += " ";
    tmparg += argv[3];
    tmparg += " ";
    tmparg += argv[4];
    std::stringstream argstream(tmparg);

    argstream >> be;
    argstream >> numBE;
    argstream >> cmd;
    argstream >> freq;

    if( be != "-be" )
    {
        std::cout << "Error must specify number of backends with -be" << std::endl;
        printUsageExample();
    }
    else if( numBE <= 0 )
    {
        std::cout << "Error number of backends must be greater then 0" << std::endl;
        printUsageExample();
    }
    else if( cmd == "" )
    {
        std::cout << "Error must include vaild proc name" << std::endl;
        printUsageExample();
    }
    else if ( freq <= 0)
    {
        std::cout << "Error frequency must be greater than 0" << std::endl;
        printUsageExample();
    }

    char const* home = getenv("HOME");
    std::string default_topology(home);
    default_topology += "/.cbtf/cbtf_topology";

    // XML
    registerXML(filesystem::path(XMLDIR) / "repeatMem.xml");

    // Setup MRNet
    Component::registerPlugin(
            filesystem::path(LIBDIR) / "KrellInstitute/CBTF/BasicMRNetLaunchers.so"
            );

    Component::Instance launcher = Component::instantiate(
            Type("BasicMRNetLauncherUsingBackendCreate")  );

    // Setup the Network
    Component::Instance network;
    network = Component::instantiate(Type("repeatMemNetwork"));

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

    boost::shared_ptr<ValueSource<struct timespec> > freq_input_value 
        = ValueSource<struct timespec>::instantiate();
    Component::Instance freq_input_component 
        = boost::reinterpret_pointer_cast<Component>(freq_input_value);
    Component::connect(freq_input_component, "value", network, "freq_in");

    boost::shared_ptr<ValueSource<int> > numBE_input_value
        = ValueSource<int>::instantiate();
    Component::Instance numBE_input_component
        = boost::reinterpret_pointer_cast<Component>(numBE_input_value);
    Component::connect(numBE_input_component, "value", network, "numBE_in");

    // create the output
    boost::shared_ptr<ValueSink<std::vector<std::string> > > output_value = ValueSink<std::vector<std::string> >::instantiate();
    Component::Instance output_value_component = boost::reinterpret_pointer_cast<Component>(output_value);
    Component::connect(network, "out", output_value_component, "value");

    boost::shared_ptr<ValueSink<struct timeval> > elapsed_time_output_value 
        = ValueSink<struct timeval>::instantiate();
    Component::Instance elapsed_time_output_component 
        = boost::reinterpret_pointer_cast<Component>(elapsed_time_output_value);
    Component::connect(network,
            "elapsed_time_out",
            elapsed_time_output_component,
            "value");

    boost::shared_ptr<ValueSink<bool> > term_output_value 
        = ValueSink<bool>::instantiate();
    Component::Instance term_output_component 
        = boost::reinterpret_pointer_cast<Component>(term_output_value);
    Component::connect(network,
            "term_out",
            term_output_component,
            "value");

    boost::shared_ptr<ValueSink<std::vector<NodeMemory> > > mem_output_value 
        = ValueSink<std::vector<NodeMemory> >::instantiate();
    Component::Instance mem_output_component 
        = boost::reinterpret_pointer_cast<Component>(mem_output_value);
    Component::connect(network,
            "mem_out",
            mem_output_component,
            "value");

    boost::shared_ptr<ValueSink<int*> > bin_output_value 
        = ValueSink<int*>::instantiate();
    Component::Instance bin_output_component 
        = boost::reinterpret_pointer_cast<Component>(bin_output_value);
    Component::connect(network,
            "bin_out",
            bin_output_component,
            "value");

    boost::shared_ptr<ValueSink<Statistics<long double> > > stat_output_value 
        = ValueSink<Statistics<long double> >::instantiate();
    Component::Instance stat_output_component 
        = boost::reinterpret_pointer_cast<Component>(stat_output_value);
    Component::connect(network,
            "stat_out",
            stat_output_component,
            "value");

    // start mrnet network
    *topology_file = default_topology;

    // send the app name down the mrnet tree
    *input_value = cmd;

    struct timespec frequency;
    frequency.tv_sec = (time_t) freq;
    frequency.tv_nsec = 0.0;

    *freq_input_value = frequency;
    *numBE_input_value = numBE;

    std::vector<std::string> output;
    bool terminate = false;
    struct timeval elapsed_time_output;
    std::vector<NodeMemory> mem_output;
    int * bin_output;
    Statistics<long double> stat_output;
    // get the output from each backend
    // wait for output
    do {
        if (mem_output_value == NULL
                || stat_output_value == NULL
                || output_value == NULL) {
            std::cout << "One of the big three is null"
                << std::endl;
        }
        mem_output = *mem_output_value;
        stat_output = *stat_output_value;
        output = *output_value;
        elapsed_time_output = *elapsed_time_output_value;
        terminate = *term_output_value;
        std::cout << "Application Stats:\n"
            << stat_output
            << std::endl;
        for (int i = 0; i < mem_output.size(); i++) {
            std::cout << "Memory Info: \n"
                << mem_output[i].toString()
                << std::endl;
        }
        // print each line in the output vector
        for(std::vector<std::string>::const_iterator i = output.begin(); 
                i != output.end(); ++i)
        {
            std::cout << *i << std::endl;
        }
        std::cout << "----------------------" << std::endl;
        std::cout << "Elapsed time: "
            << elapsed_time_output.tv_sec
            << " sec "
            << elapsed_time_output.tv_usec
            << " microsec"
            << std::endl;

        if (terminate) {
            bin_output = *bin_output_value;
            std::cout << "Recieved Terminate Signal" << std::endl;
        } else {
            std::cout << "Did not recieve terminate signal" << std::endl;
        }

    } while (!terminate);

    //mem_output = *mem_output_value;
    //stat_output = *stat_output_value;
    //output = *output_value;
    //elapsed_time_output = *elapsed_time_output_value;
    for (int i = 0; i < 100; i++) {
        std::cout << "BIN["
            << i
            << "] = "
            << bin_output[i]
            << std::endl;
    }
    return 0;
}

