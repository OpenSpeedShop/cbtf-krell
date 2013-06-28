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

using namespace KrellInstitute::SymbolTable;
using namespace KrellInstitute::SymbolTable::Impl;



//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
AddressSpace::AddressSpace()
{
    // ...
}


        
//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
AddressSpace::AddressSpace(const CBTF_Protocol_LinkedObjectGroup& message)
{
    // ...
}



//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
AddressSpace::AddressSpace(const AddressSpace& other)
{
    // ...
}



//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
AddressSpace::~AddressSpace()
{
    // ...
}


        
//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
AddressSpace& AddressSpace::operator=(const AddressSpace& other)
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
AddressSpace::operator CBTF_Protocol_LinkedObjectGroup() const
{
    CBTF_Protocol_LinkedObjectGroup message;
    
    // ...
    
    return message;
}



//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
void AddressSpace::addLinkedObject(const LinkedObject& linked_object,
                                   const AddressRange& range,
                                   const TimeInterval& interval)
{
    // ...
}



//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
void AddressSpace::apply(const CBTF_Protocol_LoadedLinkedObject& message)
{
    // ...
}



//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
void AddressSpace::apply(const CBTF_Protocol_UnloadedLinkedObject& message)
{
    // ...
}



//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
void AddressSpace::visitLinkedObjects(LinkedObjectVisitor& visitor) const
{
    // ...
}



//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
void AddressSpace::visitLinkedObjectsAt(const AddressRange& range,
                                        const TimeInterval& interval,
                                        LinkedObjectVisitor& visitor) const
{
    // ...
}



//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
void AddressSpace::visitLinkedObjectsByPath(const boost::filesystem::path& path,
                                            LinkedObjectVisitor& visitor) const
{
    // ...
}
