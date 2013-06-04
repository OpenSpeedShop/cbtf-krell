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

/** @file Definition of the SymbolTable class. */

#include "SymbolTable.hpp"

using namespace KrellInstitute::SymbolTable;
using namespace KrellInstitute::SymbolTable::Impl;



//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
SymbolTable::SymbolTable(const boost::filesystem::path& path) :
    dm_path(path),
    dm_checksum(0)
{
    // ...
}



//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
SymbolTable::SymbolTable(const CBTF_Protocol_SymbolTable& message) :
    dm_path(message.linked_object.path),
    dm_checksum(message.linked_object.checksum)
{
    // ...
}



//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
SymbolTable::SymbolTable(const SymbolTable& other) :
    dm_path(other.dm_path),
    dm_checksum(other.dm_checksum)
{
    // ...
}



//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
SymbolTable::~SymbolTable()
{
    // ...
}



//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
SymbolTable& SymbolTable::operator=(const SymbolTable& other)
{
    if (this != &other)
    {
        dm_path = other.dm_path;
        dm_checksum = other.dm_checksum;

        // ...
    }
    return *this;
}



//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
SymbolTable::operator CBTF_Protocol_SymbolTable() const
{
    CBTF_Protocol_SymbolTable message;

    // ...

    return message;
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
boost::filesystem::path SymbolTable::getPath() const
{
    return dm_path;
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
uint64_t SymbolTable::getChecksum() const
{
    return dm_checksum;
}
