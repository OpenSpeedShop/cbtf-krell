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
//------------------------------------------------------------------------------
Statement::Statement(const LinkedObject& linked_object,
                     const FileName& name,
                     const unsigned int& line,
                     const unsigned int& column) :
    dm_symbol_table(linked_object.dm_symbol_table),
    dm_unique_identifier(dm_symbol_table->addStatement(name, line, column))
{
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
//------------------------------------------------------------------------------
bool Statement::operator<(const Statement& other) const
{
    if (dm_symbol_table < other.dm_symbol_table)
    {
        return true;
    }
    else if (other.dm_symbol_table < dm_symbol_table)
    {
        return false;
    }
    else
    {
        return dm_unique_identifier < other.dm_unique_identifier;
    }
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool Statement::operator==(const Statement& other) const
{
    return (dm_symbol_table == other.dm_symbol_table) &&
        (dm_unique_identifier == other.dm_unique_identifier);
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
Statement Statement::clone(LinkedObject& linked_object) const
{
    return Statement(
        linked_object.dm_symbol_table,
        linked_object.dm_symbol_table->cloneStatement(
            *dm_symbol_table, dm_unique_identifier
            )
        );
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void Statement::addAddressRanges(const std::set<AddressRange>& ranges)
{
    dm_symbol_table->addStatementAddressRanges(dm_unique_identifier, ranges);
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
LinkedObject Statement::getLinkedObject() const
{
    return LinkedObject(dm_symbol_table);
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
FileName Statement::getName() const
{
    return dm_symbol_table->getStatementName(dm_unique_identifier);
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
unsigned int Statement::getLine() const
{
    return dm_symbol_table->getStatementLine(dm_unique_identifier);
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
unsigned int Statement::getColumn() const
{
    return dm_symbol_table->getStatementColumn(dm_unique_identifier);
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
std::set<AddressRange> Statement::getAddressRanges() const
{
    return dm_symbol_table->getStatementAddressRanges(dm_unique_identifier);
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void Statement::visitFunctions(FunctionVisitor& visitor) const
{
    dm_symbol_table->visitStatementFunctions(dm_unique_identifier, visitor);
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
Statement::Statement(Impl::SymbolTable::Handle symbol_table,
                     Impl::SymbolTable::UniqueIdentifier unique_identifier) :
    dm_symbol_table(symbol_table),
    dm_unique_identifier(unique_identifier)
{
}
