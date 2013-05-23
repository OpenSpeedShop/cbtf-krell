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

/** @file Definition of the AddressSpace class. */

#include <KrellInstitute/SymbolTable/AddressSpace.hpp>

#include "AddressSpaceImpl.hpp"

using namespace KrellInstitute::SymbolTable;
using namespace KrellInstitute::SymbolTable::Impl;



//------------------------------------------------------------------------------
// Let the implementation do the real work.
//------------------------------------------------------------------------------
AddressSpace::AddressSpace() :
    dm_impl(new AddressSpaceImpl())
{
}


        
//------------------------------------------------------------------------------
// Let the implementation do the real work.
//------------------------------------------------------------------------------
AddressSpace::AddressSpace(const CBTF_Protocol_LinkedObjectGroup& message) :
    dm_impl(new AddressSpaceImpl(message))
{
}



//------------------------------------------------------------------------------
// Let the implementation do the real work.
//------------------------------------------------------------------------------
AddressSpace::AddressSpace(const AddressSpace& other) :
    dm_impl(new AddressSpaceImpl(other))
{
}



//------------------------------------------------------------------------------
// Let the implementation do the real work.
//------------------------------------------------------------------------------
AddressSpace::~AddressSpace()
{
    delete dm_impl;
}


        
//------------------------------------------------------------------------------
// Let the implementation do the real work.
//------------------------------------------------------------------------------
AddressSpace& AddressSpace::operator=(const AddressSpace& other)
{
    *dm_impl = *other.dm_impl;
    return *this;
}



//------------------------------------------------------------------------------
// Let the implementation do the real work.
//------------------------------------------------------------------------------
AddressSpace::operator CBTF_Protocol_LinkedObjectGroup() const
{
    return *dm_impl;
}



//------------------------------------------------------------------------------
// Let the implementation do the real work.
//------------------------------------------------------------------------------
std::set<LinkedObject> AddressSpace::getLinkedObjects() const
{
    return dm_impl->getLinkedObjects();
}



//------------------------------------------------------------------------------
// Let the implementation do the real work.
//------------------------------------------------------------------------------
boost::optional<LinkedObject> AddressSpace::getLinkedObjectAt(
    const Address& address,
    const Time& time
    ) const
{
    return dm_impl->getLinkedObjectAt(address, time);
}



//------------------------------------------------------------------------------
// Let the implementation do the real work.
//------------------------------------------------------------------------------
std::set<LinkedObject> AddressSpace::getLinkedObjectsByPath(
    const boost::filesystem::path& path
    ) const
{
    return dm_impl->getLinkedObjectsByPath(path);
}



//------------------------------------------------------------------------------
// Let the implementation do the real work.
//------------------------------------------------------------------------------
void AddressSpace::addLinkedObject(const LinkedObject& linked_object,
                                   const AddressRange& range,
                                   const TimeInterval& interval)
{
    return dm_impl->addLinkedObject(linked_object, range, interval);
}



//------------------------------------------------------------------------------
// Let the implementation do the real work.
//------------------------------------------------------------------------------
void AddressSpace::apply(const CBTF_Protocol_LoadedLinkedObject& message)
{
    dm_impl->apply(message);
}



//------------------------------------------------------------------------------
// Let the implementation do the real work.
//------------------------------------------------------------------------------
void AddressSpace::apply(const CBTF_Protocol_UnloadedLinkedObject& message)
{
    dm_impl->apply(message);
}
