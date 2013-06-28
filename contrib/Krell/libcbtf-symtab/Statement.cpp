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

/** @file Definition of the Statement class. */

#include <KrellInstitute/SymbolTable/LinkedObject.hpp>
#include <KrellInstitute/SymbolTable/Statement.hpp>

#include "SymbolTable.hpp"

using namespace KrellInstitute::SymbolTable;



//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
Statement::Statement(const LinkedObject& linked_object,
                     const boost::filesystem::path& path,
                     const unsigned int& line,
                     const unsigned int& column) :
    dm_symbol_table(),
    dm_unique_identifier()
{
    // ...
}


        
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
Statement::Statement(const Statement& other) :
    dm_symbol_table(other.dm_symbol_table),
    dm_unique_identifier(other.dm_unique_identifier)
{
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
Statement::~Statement()
{
}


        
//------------------------------------------------------------------------------
// Let the implementation do the real work.
//------------------------------------------------------------------------------
Statement& Statement::operator=(const Statement& other)
{
    if (this != &other)
    {
        dm_symbol_table = other.dm_symbol_table;
        dm_unique_identifier = other.dm_unique_identifier;
    }
    return *this;
}



//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
bool Statement::operator<(const Statement& other) const
{
    // ...

    return false;
}



//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
bool Statement::operator==(const Statement& other) const
{
    // ...

    return false;
}



//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
Statement Statement::clone(LinkedObject& linked_object) const
{
    // ...

    return *this;
}



//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
void Statement::addAddressRanges(const std::set<AddressRange>& ranges)
{
    // ...
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
LinkedObject Statement::getLinkedObject() const
{
    return LinkedObject(dm_symbol_table);
}



//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
boost::filesystem::path Statement::getPath() const
{
    // ...

    return boost::filesystem::path();
}



//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
unsigned int Statement::getLine() const
{
    // ...

    return 0;
}



//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
unsigned int Statement::getColumn() const
{
    // ...

    return 0;
}



//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
std::set<AddressRange> Statement::getAddressRanges() const
{
    // ...

    return std::set<AddressRange>();
}



//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
void Statement::visitFunctions(FunctionVisitor& visitor) const
{
    // ...
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
Statement::Statement(Impl::SymbolTable::Handle symbol_table,
                     Impl::SymbolTable::UniqueIdentifier unique_identifier) :
    dm_symbol_table(symbol_table),
    dm_unique_identifier(unique_identifier)
{
}
