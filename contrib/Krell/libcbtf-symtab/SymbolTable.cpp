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

#include <boost/assert.hpp>

#include "SymbolTable.hpp"

using namespace KrellInstitute::SymbolTable;
using namespace KrellInstitute::SymbolTable::Impl;



//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
SymbolTable::SymbolTable(const boost::filesystem::path& path) :
    dm_path(path),
    dm_checksum(0),
    dm_functions(),
    dm_functions_index(),
    dm_statements(),
    dm_statements_index()
{
    // ...
}



//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
SymbolTable::SymbolTable(const CBTF_Protocol_SymbolTable& message) :
    dm_path(message.linked_object.path),
    dm_checksum(message.linked_object.checksum),
    dm_functions(),
    dm_functions_index(),
    dm_statements(),
    dm_statements_index()
{
    // ...
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
SymbolTable::SymbolTable(const SymbolTable& other) :
    dm_path(other.dm_path),
    dm_checksum(other.dm_checksum),
    dm_functions(other.dm_functions),
    dm_functions_index(other.dm_functions_index),
    dm_statements(other.dm_statements),
    dm_statements_index(other.dm_statements_index)
{
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
SymbolTable::~SymbolTable()
{
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
SymbolTable& SymbolTable::operator=(const SymbolTable& other)
{
    if (this != &other)
    {
        dm_path = other.dm_path;
        dm_checksum = other.dm_checksum;
        dm_functions = other.dm_functions;
        dm_functions_index = other.dm_functions_index;
        dm_statements = other.dm_statements;
        dm_statements_index = other.dm_statements_index;
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
boost::uint64_t SymbolTable::getChecksum() const
{
    return dm_checksum;
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
SymbolTable::UniqueIdentifier SymbolTable::addFunction(
    const std::string& name
    )
{
    dm_functions.push_back(FunctionItem(name));    
    return dm_functions.size() - 1;
}



//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
void SymbolTable::addFunctionAddressRanges(const UniqueIdentifier& uid,
                                           const std::set<AddressRange>& ranges)
{
    // ...
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
SymbolTable::UniqueIdentifier SymbolTable::addStatement(
    const boost::filesystem::path& path,
    const unsigned int& line,
    const unsigned int& column
    )
{
    dm_statements.push_back(StatementItem(path, line, column));
    return dm_statements.size() - 1;
}



//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
void SymbolTable::addStatementAddressRanges(
    const UniqueIdentifier& uid, const std::set<AddressRange>& ranges
    )
{
    // ...
}



//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
SymbolTable::UniqueIdentifier SymbolTable::cloneFunction(
    const SymbolTable& symbol_table, const UniqueIdentifier& uid
    )
{
    // ...

    return 0;
}



//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
SymbolTable::UniqueIdentifier SymbolTable::cloneStatement(
    const SymbolTable& symbol_table, const UniqueIdentifier& uid
    )
{
    // ...

    return 0;
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
std::string SymbolTable::getFunctionMangledName(
    const UniqueIdentifier& uid
    ) const
{
    BOOST_VERIFY(uid < dm_functions.size());
    return dm_functions[uid].dm_name;
}



//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
std::set<AddressRange> SymbolTable::getFunctionAddressRanges(
    const UniqueIdentifier& uid
    ) const
{
    // ...

    return std::set<AddressRange>();
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
boost::filesystem::path SymbolTable::getStatementPath(
    const UniqueIdentifier& uid
    ) const
{
    BOOST_VERIFY(uid < dm_statements.size());
    return dm_statements[uid].dm_path;
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
unsigned int SymbolTable::getStatementLine(const UniqueIdentifier& uid) const
{
    BOOST_VERIFY(uid < dm_statements.size());
    return dm_statements[uid].dm_line;
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
unsigned int SymbolTable::getStatementColumn(const UniqueIdentifier& uid) const
{
    BOOST_VERIFY(uid < dm_statements.size());
    return dm_statements[uid].dm_column;
}



//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
std::set<AddressRange> SymbolTable::getStatementAddressRanges(
    const UniqueIdentifier& uid
    ) const
{
    // ...

    return std::set<AddressRange>();
}



//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
void SymbolTable::visitFunctions(FunctionVisitor& visitor) const
{
    // ...
}



//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
void SymbolTable::visitFunctionsAt(const Address& address,
                                   FunctionVisitor& visitor) const
{
    // ...
}


       
//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
void SymbolTable::visitFunctionsByName(const std::string& name,
                                       FunctionVisitor& visitor) const
{
    // ...
}



//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
void SymbolTable::visitFunctionDefinitions(const UniqueIdentifier& uid,
                                           StatementVisitor& visitor) const
{
    // ...
}


        
//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
void SymbolTable::visitFunctionStatements(const UniqueIdentifier& uid,
                                          StatementVisitor& visitor) const
{
    // ...
}



//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
void SymbolTable::visitStatements(StatementVisitor& visitor) const
{
    // ...
}



//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
void SymbolTable::visitStatementsAt(const Address& address,
                                    StatementVisitor& visitor) const
{
    // ...
}



//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
void SymbolTable::visitStatementsBySourceFile(
    const boost::filesystem::path& path, StatementVisitor& visitor
    ) const
{
    // ...
}



//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
void SymbolTable::visitStatementFunctions(const UniqueIdentifier& uid,
                                          FunctionVisitor& visitor) const
{
    // ...
}
