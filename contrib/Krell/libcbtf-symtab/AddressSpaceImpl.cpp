////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2013 Krell Institute. All Rights Reserved.
//
// This program is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free Software
// Foundation; either version 2 of the License, or (at your option) any later
// version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
// details.
//
// You should have received a copy of the GNU General Public License along with
// this program; if not, write to the Free Software Foundation, Inc., 59 Temple
// Place, Suite 330, Boston, MA  02111-1307  USA
////////////////////////////////////////////////////////////////////////////////

/** @file Definition of the AddressSpaceImpl class. */

#include "AddressSpaceImpl.hpp"

using namespace KrellInstitute::SymbolTable;
using namespace KrellInstitute::SymbolTable::Impl;



//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
AddressSpaceImpl::AddressSpaceImpl()
{
    // ...
}


        
//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
AddressSpaceImpl::AddressSpaceImpl(
    const CBTF_Protocol_LinkedObjectGroup& message
    )
{
    // ...
}



//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
AddressSpaceImpl::AddressSpaceImpl(const AddressSpaceImpl& other)
{
    // ...
}



//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
AddressSpaceImpl::~AddressSpaceImpl()
{
    // ...
}


        
//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
AddressSpaceImpl& AddressSpaceImpl::operator=(const AddressSpaceImpl& other)
{
    if (this != &other)
    {
        // ...
    }
    return *this;
}



//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
AddressSpaceImpl::operator CBTF_Protocol_LinkedObjectGroup() const
{
    CBTF_Protocol_LinkedObjectGroup message;

    // ...

    return message;
}



//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
std::set<LinkedObject> AddressSpaceImpl::getLinkedObjects() const
{
    // ...

    return std::set<LinkedObject>();
}



//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
boost::optional<LinkedObject> AddressSpaceImpl::getLinkedObjectAt(
    const Address& address,
    const Time& time
    ) const
{
    // ...

    return boost::optional<LinkedObject>();
}



//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
std::set<LinkedObject> AddressSpaceImpl::getLinkedObjectsByPath(
    const boost::filesystem::path& path
    ) const
{
    // ...

    return std::set<LinkedObject>();
}



//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
void AddressSpaceImpl::addLinkedObject(const LinkedObject& linked_object,
                                       const AddressRange& range,
                                       const TimeInterval& interval)
{
    // ...
}



//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
void AddressSpaceImpl::apply(const CBTF_Protocol_LoadedLinkedObject& message)
{
    // ...
}



//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
void AddressSpaceImpl::apply(const CBTF_Protocol_UnloadedLinkedObject& message)
{
    // ...
}
