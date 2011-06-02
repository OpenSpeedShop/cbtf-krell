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

/** @file Unit tests for the CBTF MRNet library. */

#include <boost/shared_ptr.hpp>
#include <iostream>
#include <KrellInstitute/CBTF/BoostExts.hpp>
#include <KrellInstitute/CBTF/Component.hpp>
#include <KrellInstitute/CBTF/Type.hpp>
#include <KrellInstitute/CBTF/ValueSink.hpp>
#include <KrellInstitute/CBTF/ValueSource.hpp>
#include <KrellInstitute/CBTF/XDR.hpp>
#include <KrellInstitute/CBTF/XML.hpp>
#include <map>
#include <set>
#include <stdexcept>
#include <string>

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
 * Unit test for XML-defined distributed (via MRNet) component networks.
 */
int main (int argc, char **argv)
{
    // Test registration of distributed component network types from XML
    std::set<Type> available_types = Component::getAvailableTypes();
    registerXML(boost::filesystem::path(BUILDDIR) / "pcsampDemo.xml");
    
    // Test distributed component network instantiation and metadata
    Component::Instance network;
    network = Component::instantiate(Type("TestNetwork"));
    std::map<std::string, Type> inputs = network->getInputs();
    std::map<std::string, Type> outputs = network->getOutputs();

    // Test instantiation of the basic launcher component

    //Component::registerPlugin(boost::filesystem::path(TOPDIR) / "launchers" / "BasicMRNetLaunchers");
    Component::registerPlugin(boost::filesystem::path("/opt/cbtf-dev/lib64/") / "KrellInstitute/CBTF/BasicMRNetLaunchers");
    Component::Instance launcher;
    launcher = Component::instantiate(Type("BasicMRNetLauncherUsingBackendAttach"));
    
    // Test distributed component network intercommunication
    boost::shared_ptr<ValueSource<boost::filesystem::path> > topology_value =
        ValueSource<boost::filesystem::path>::instantiate();
    boost::shared_ptr<ValueSource<int> > input_value = 
        ValueSource<int>::instantiate();
    boost::shared_ptr<ValueSink<int> > output_value = 
        ValueSink<int>::instantiate();

    Component::Instance topology_value_component = 
        boost::reinterpret_pointer_cast<Component>(topology_value);
    Component::Instance input_value_component = 
        boost::reinterpret_pointer_cast<Component>(input_value);
    Component::Instance output_value_component = 
        boost::reinterpret_pointer_cast<Component>(output_value);

    Component::connect(
        topology_value_component, "value", launcher, "TopologyFile"
        );

    Component::connect(launcher, "Network", network, "Network");
    Component::connect(input_value_component, "value", network, "in");
    Component::connect(network, "out", output_value_component, "value");

    *topology_value = boost::filesystem::path(BUILDDIR) / "pcsampDemo.topology";

    for(;;);
}



