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

/** @file Definition of the Function class. */

#include <KrellInstitute/SymbolTable/Function.hpp>
#include <KrellInstitute/SymbolTable/LinkedObject.hpp>

#include "SymbolTable.hpp"

using namespace KrellInstitute::SymbolTable;



//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
Function::Function(const LinkedObject& linked_object, const std::string& name) :
    dm_symbol_table(),
    dm_unique_identifier()
{
    // ..
}


        
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
Function::Function(const Function& other) :
    dm_symbol_table(other.dm_symbol_table),
    dm_unique_identifier(other.dm_unique_identifier)
{
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
Function::~Function()
{
}


        
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
Function& Function::operator=(const Function& other)
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
bool Function::operator<(const Function& other) const
{
    // ...
    
    return false;
}



//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
bool Function::operator==(const Function& other) const
{
    // ...

    return false;
}



//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
Function Function::clone(LinkedObject& linked_object) const
{
    // ...
    
    return *this;
}



//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
void Function::addAddressRanges(const std::set<AddressRange>& ranges)
{
    // ...
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
LinkedObject Function::getLinkedObject() const
{
    return LinkedObject(dm_symbol_table);
}



//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
std::string Function::getMangledName() const
{
    // ...

    return std::string();
}



//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
std::string Function::getDemangledName(const bool& all) const
{
    // ...

    return std::string();
}



//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
std::set<AddressRange> Function::getAddressRanges() const
{
    // ...

    return std::set<AddressRange>();
}



//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
void Function::visitDefinitions(StatementVisitor& visitor) const
{
    // ...
}



//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
void Function::visitStatements(StatementVisitor& visitor) const
{
    // ...
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
Function::Function(Impl::SymbolTable::Handle symbol_table,
                   Impl::SymbolTable::UniqueIdentifier unique_identifier) :
    dm_symbol_table(symbol_table),
    dm_unique_identifier(unique_identifier)
{
}
