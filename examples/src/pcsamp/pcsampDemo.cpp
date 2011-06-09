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

/** @file Example PC sampling tool. */

#include <boost/shared_ptr.hpp>
#include <KrellInstitute/CBTF/BoostExts.hpp>
#include <KrellInstitute/CBTF/Component.hpp>
#include <KrellInstitute/CBTF/Type.hpp>
#include <KrellInstitute/CBTF/ValueSource.hpp>
#include <KrellInstitute/CBTF/XML.hpp>

using namespace boost;
using namespace KrellInstitute::CBTF;



/**
 * Main function for the example PC sampling tool.
 */
int main(int argc, char** argv)
{
    registerXML(filesystem::path(BUILDDIR) / "pcsampDemo.xml");

    Component::registerPlugin(
        filesystem::path("/opt/cbtf-dev/lib64/KrellInstitute/CBTF") /
        "BasicMRNetLaunchers"
        );
    
    Component::Instance network = Component::instantiate(
        Type("PC_Sampling_Demo")
        );
    
    Component::Instance launcher = Component::instantiate(
        Type("BasicMRNetLauncherUsingBackendAttach")
        );

    shared_ptr<ValueSource<unsigned int> > backend_attach_count =
        ValueSource<unsigned int>::instantiate();
    Component::Instance backend_attach_count_component = 
        reinterpret_pointer_cast<Component>(backend_attach_count);
    Component::connect(
        backend_attach_count_component, "value", launcher, "BackendAttachCount"
        );

    shared_ptr<ValueSource<filesystem::path> > backend_attach_file =
        ValueSource<filesystem::path>::instantiate();
    Component::Instance backend_attach_file_component = 
        reinterpret_pointer_cast<Component>(backend_attach_file);
    Component::connect(
        backend_attach_file_component, "value", launcher, "BackendAttachFile"
        );    

    shared_ptr<ValueSource<filesystem::path> > topology_file =
        ValueSource<filesystem::path>::instantiate();
    Component::Instance topology_file_component = 
        reinterpret_pointer_cast<Component>(topology_file);
    Component::connect(
        topology_file_component, "value", launcher, "TopologyFile"
        );

    Component::connect(launcher, "Network", network, "Network");

    //*backend_attach_count = 0;
    *topology_file = filesystem::path(BUILDDIR) / "pcsampDemo.topology";

    while (true);
}
