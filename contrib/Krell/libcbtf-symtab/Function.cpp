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

#include <cxxabi.h>

#include <KrellInstitute/SymbolTable/Function.hpp>
#include <KrellInstitute/SymbolTable/LinkedObject.hpp>

#include "SymbolTable.hpp"

using namespace KrellInstitute::SymbolTable;



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
Function::Function(const LinkedObject& linked_object, const std::string& name) :
    dm_symbol_table(linked_object.dm_symbol_table),
    dm_unique_identifier(dm_symbol_table->addFunction(name))
{
}


        
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool Function::operator<(const Function& other) const
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
bool Function::operator==(const Function& other) const
{
    return (dm_symbol_table == other.dm_symbol_table) &&
        (dm_unique_identifier == other.dm_unique_identifier);
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
Function Function::clone(LinkedObject& linked_object) const
{
    return Function(
        linked_object.dm_symbol_table,
        linked_object.dm_symbol_table->cloneFunction(
            *dm_symbol_table, dm_unique_identifier
            )
        );
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void Function::addAddressRanges(const std::set<AddressRange>& ranges)
{
    dm_symbol_table->addFunctionAddressRanges(dm_unique_identifier, ranges);
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
LinkedObject Function::getLinkedObject() const
{
    return LinkedObject(dm_symbol_table);
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
std::string Function::getMangledName() const
{
    return dm_symbol_table->getFunctionMangledName(dm_unique_identifier);
}



//------------------------------------------------------------------------------
// Get the mangled name of the function and then use the __cxa_demangle() ABI
// function to demangle this name. Return the mangled name if the demangling
// fails for any reason.
//
// http://gcc.gnu.org/onlinedocs/libstdc++/libstdc++-html-USERS-4.3/a01696.html
//------------------------------------------------------------------------------
std::string Function::getDemangledName() const
{
    std::string mangled = getMangledName();
    
    int status = 0;
    char* raw = abi::__cxa_demangle(mangled.c_str(), NULL, 0, &status);
    
    if (status != 0)
    {
        return mangled;
    }
    
    std::string demangled(raw);
    free(raw);
    
    return demangled;
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
std::set<AddressRange> Function::getAddressRanges() const
{
    return dm_symbol_table->getFunctionAddressRanges(dm_unique_identifier);
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void Function::visitDefinitions(StatementVisitor& visitor) const
{
    dm_symbol_table->visitFunctionDefinitions(dm_unique_identifier, visitor);
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void Function::visitStatements(StatementVisitor& visitor) const
{
    dm_symbol_table->visitFunctionStatements(dm_unique_identifier, visitor);
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
Function::Function(Impl::SymbolTable::Handle symbol_table,
                   Impl::SymbolTable::UniqueIdentifier unique_identifier) :
    dm_symbol_table(symbol_table),
    dm_unique_identifier(unique_identifier)
{
}
