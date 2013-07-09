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

/** @file Definition of the LinkedObject class. */

#include <KrellInstitute/SymbolTable/LinkedObject.hpp>

#include "SymbolTable.hpp"

using namespace KrellInstitute::SymbolTable;



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
LinkedObject::LinkedObject(const boost::filesystem::path& path) :
    dm_symbol_table(new Impl::SymbolTable(path))
{
}


        
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
LinkedObject::LinkedObject(const CBTF_Protocol_SymbolTable& message) :
    dm_symbol_table(new Impl::SymbolTable(message))
{
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
LinkedObject::LinkedObject(const LinkedObject& other) :
    dm_symbol_table(other.dm_symbol_table)
{
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
LinkedObject::~LinkedObject()
{
}


        
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
LinkedObject& LinkedObject::operator=(const LinkedObject& other)
{
    if (this != &other)
    {
        dm_symbol_table = other.dm_symbol_table;
    }
    return *this;
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool LinkedObject::operator<(const LinkedObject& other) const
{
    return dm_symbol_table < other.dm_symbol_table;
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool LinkedObject::operator==(const LinkedObject& other) const
{
    return dm_symbol_table == other.dm_symbol_table;
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
LinkedObject::operator CBTF_Protocol_SymbolTable() const
{
    return *dm_symbol_table;
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
LinkedObject LinkedObject::clone() const
{
    return LinkedObject(
        Impl::SymbolTable::Handle(new Impl::SymbolTable(*dm_symbol_table))
        );
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
boost::filesystem::path LinkedObject::getPath() const
{
    return dm_symbol_table->getPath();
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
boost::uint64_t LinkedObject::getChecksum() const
{
    return dm_symbol_table->getChecksum();
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void LinkedObject::visitFunctions(FunctionVisitor& visitor) const
{
    dm_symbol_table->visitFunctions(visitor);
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void LinkedObject::visitFunctionsAt(const Address& address,
                                    FunctionVisitor& visitor) const
{
    dm_symbol_table->visitFunctionsAt(address, visitor);
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void LinkedObject::visitFunctionsByName(const std::string& name,
                                        FunctionVisitor& visitor) const
{
    dm_symbol_table->visitFunctionsByName(name, visitor);
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void LinkedObject::visitStatements(StatementVisitor& visitor) const
{
    dm_symbol_table->visitStatements(visitor);
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void LinkedObject::visitStatementsAt(const Address& address,
                                     StatementVisitor& visitor) const
{
    dm_symbol_table->visitStatementsAt(address, visitor);
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void LinkedObject::visitStatementsBySourceFile(
    const boost::filesystem::path& path, StatementVisitor& visitor
    ) const
{
    dm_symbol_table->visitStatementsBySourceFile(path, visitor);
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
LinkedObject::LinkedObject(Impl::SymbolTable::Handle symbol_table) :
    dm_symbol_table(symbol_table)
{
}
