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

/** @file Definition of the LinkedObjectImpl class. */

#include "LinkedObjectImpl.hpp"

using namespace KrellInstitute::SymbolTable;
using namespace KrellInstitute::SymbolTable::Impl;



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
LinkedObjectImpl::LinkedObjectImpl(const SymbolTable::Handle& symbol_table) :
    dm_symbol_table(symbol_table)
{
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
LinkedObjectImpl::LinkedObjectImpl(const boost::filesystem::path& path) :
    dm_symbol_table(new SymbolTable(path))
{
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
LinkedObjectImpl::LinkedObjectImpl(const CBTF_Protocol_SymbolTable& message) :
    dm_symbol_table(new SymbolTable(message))
{
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
LinkedObjectImpl::LinkedObjectImpl(const LinkedObjectImpl& other) :
    dm_symbol_table(other.dm_symbol_table)
{
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
LinkedObjectImpl::~LinkedObjectImpl()
{
}


        
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
LinkedObjectImpl& LinkedObjectImpl::operator=(const LinkedObjectImpl& other)
{
    if (this != &other)
    {
        dm_symbol_table = other.dm_symbol_table;
    }
    return *this;
}



//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
bool LinkedObjectImpl::operator<(const LinkedObjectImpl& other) const
{
    // ...

    return false;
}



//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
bool LinkedObjectImpl::operator==(const LinkedObjectImpl& other) const
{
    // ...

    return false;
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
LinkedObjectImpl::operator CBTF_Protocol_SymbolTable() const
{
    return *dm_symbol_table;
}




//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
LinkedObjectImpl LinkedObjectImpl::clone() const
{
    return LinkedObjectImpl(
        SymbolTable::Handle(new SymbolTable(*dm_symbol_table))
        );
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
boost::filesystem::path LinkedObjectImpl::getPath() const
{
    return dm_symbol_table->getPath();
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
uint64_t LinkedObjectImpl::getChecksum() const
{
    return dm_symbol_table->getChecksum();
}



//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
void LinkedObjectImpl::visitFunctions(FunctionVisitor& visitor) const
{
    // ...
}



//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
void LinkedObjectImpl::visitFunctionsAt(const Address& address,
                                        FunctionVisitor& visitor) const
{
    // ...
}



//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
void LinkedObjectImpl::visitFunctionsByName(const std::string& name,
                                            FunctionVisitor& visitor) const
{
    // ...
}



//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
void LinkedObjectImpl::visitStatements(StatementVisitor& visitor) const
{
    // ...
}



//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
void LinkedObjectImpl::visitStatementsAt(const Address& address,
                                         StatementVisitor& visitor) const
{
    // ...
}



//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
void LinkedObjectImpl::visitStatementsBySourceFile(
    const boost::filesystem::path& path, StatementVisitor& visitor
    ) const
{
    // ...
}
