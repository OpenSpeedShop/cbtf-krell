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
#include <KrellInstitute/SymbolTable/Function.hpp>
#include <KrellInstitute/SymbolTable/Statement.hpp>

#include "SymbolTable.hpp"

using namespace KrellInstitute::Base;
using namespace KrellInstitute::SymbolTable;
using namespace KrellInstitute::SymbolTable::Impl;



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
SymbolTable::SymbolTable(const FileName& file) :
    dm_file(file),
    dm_functions(),
    dm_functions_index(),
    dm_statements(),
    dm_statements_index()
{
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
SymbolTable::SymbolTable(const CBTF_Protocol_SymbolTable& message) :
    dm_file(message.linked_object),
    dm_functions(),
    dm_functions_index(),
    dm_statements(),
    dm_statements_index()
{
    //
    // Iterate over each function in the given CBTF_Protocol_SymbolTable and
    // construct the corresponding entries in this symbol table. Most of the
    // code here is virtually identical to that in the methods addFunction()
    // and addFunctionAddressRanges(). The later, in particular, is inlined
    // because AddressSet is constructable from a CBTF_Protocol_AddressBitmap
    // array directly without going through an intermediate set of address
    // ranges.
    //
    
    for (u_int i = 0; i < message.functions.functions_len; ++i)
    {
        const CBTF_Protocol_FunctionEntry& entry =
            message.functions.functions_val[i];
        
        dm_functions.push_back(FunctionItem(entry.name));

        UniqueIdentifier uid = dm_functions.size() - 1;

        dm_functions[uid].dm_addresses = AddressSet(
            entry.bitmaps.bitmaps_val, entry.bitmaps.bitmaps_len
            );
        
        std::set<AddressRange> ranges = dm_functions[uid].dm_addresses;

        for (std::set<AddressRange>::const_iterator
                 j = ranges.begin(); j != ranges.end(); ++j)
        {
            dm_functions_index.insert(AddressRangeIndexRow(uid, *j));
        }
    }

    //
    // Iterate over each statement in the given CBTF_Protocol_SymbolTable
    // and construct the corresponding entries in this symbol table. Almost
    // identical to the loops above, and the same inlining comments apply.
    //

    for (u_int i = 0; i < message.statements.statements_len; ++i)
    {
        const CBTF_Protocol_StatementEntry& entry =
            message.statements.statements_val[i];

        dm_statements.push_back(
            StatementItem(entry.path,
                          static_cast<unsigned int>(entry.line),
                          static_cast<unsigned int>(entry.column))
            );
        
        UniqueIdentifier uid = dm_statements.size() - 1;

        dm_statements[uid].dm_addresses = AddressSet(
            entry.bitmaps.bitmaps_val, entry.bitmaps.bitmaps_len
            );

        std::set<AddressRange> ranges = dm_statements[uid].dm_addresses;

        for (std::set<AddressRange>::const_iterator
                 j = ranges.begin(); j != ranges.end(); ++j)
        {
            dm_statements_index.insert(AddressRangeIndexRow(uid, *j));
        }
    }
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
SymbolTable::operator CBTF_Protocol_SymbolTable() const
{
    CBTF_Protocol_SymbolTable message;

    message.linked_object = dm_file;

    //
    // Allocate an appropriately sized array of CBTF_Protocol_FunctionEntry
    // within the message. Iterate over each function in this symbol table,
    // copying them into the message.
    //

    message.functions.functions_len = dm_functions.size();
    
    message.functions.functions_val =
        reinterpret_cast<CBTF_Protocol_FunctionEntry*>(
            malloc(std::max(1U, message.functions.functions_len) *
                   sizeof(CBTF_Protocol_FunctionEntry))
            );
    
    for (std::vector<FunctionItem>::size_type
             i = 0; i < dm_functions.size(); ++i)
    {
        const FunctionItem& item = dm_functions[i];
        
        CBTF_Protocol_FunctionEntry& entry = 
            message.functions.functions_val[i];
 
        entry.name = strdup(item.dm_name.c_str());

        item.dm_addresses.extract(
            entry.bitmaps.bitmaps_val, entry.bitmaps.bitmaps_len
            );
    }
    
    //
    // Allocate an appropriately sized array of CBTF_Protocol_StatementEntry
    // within the message. Iterate over each statement in this symbol table,
    // copying them into the message.
    //

    message.statements.statements_len = dm_statements.size();

    message.statements.statements_val =
        reinterpret_cast<CBTF_Protocol_StatementEntry*>(
            malloc(std::max(1U, message.statements.statements_len) *
                   sizeof(CBTF_Protocol_StatementEntry))
            );
    
    for (std::vector<StatementItem>::size_type
             i = 0; i < dm_statements.size(); ++i)
    {
        const StatementItem& item = dm_statements[i];
        
        CBTF_Protocol_StatementEntry& entry = 
            message.statements.statements_val[i];

        entry.path = item.dm_file;
        entry.line = static_cast<int>(item.dm_line);
        entry.column = static_cast<int>(item.dm_column);

        item.dm_addresses.extract(
            entry.bitmaps.bitmaps_val, entry.bitmaps.bitmaps_len
            );
    }
    
    // Return the completed CBTF_Protocol_SymbolTable to the caller
    return message;
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
const FileName& SymbolTable::getFile() const
{
    return dm_file;
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
//------------------------------------------------------------------------------
void SymbolTable::addFunctionAddressRanges(const UniqueIdentifier& uid,
                                           const std::set<AddressRange>& ranges)
{    
    BOOST_ASSERT(uid < dm_functions.size());
    dm_functions[uid].dm_addresses += ranges;
    
    //
    // Update the index used to find functions by addresses. Remove all of
    // the existing index entries for this function. Obtain the new set of
    // address ranges for this function and add them to the index.
    //

    dm_functions_index.get<0>().erase(uid);

    std::set<AddressRange> all_ranges = dm_functions[uid].dm_addresses;
    
    for (std::set<AddressRange>::const_iterator
             i = all_ranges.begin(); i != all_ranges.end(); ++i)
    {
        dm_functions_index.insert(AddressRangeIndexRow(uid, *i));
    }
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
SymbolTable::UniqueIdentifier SymbolTable::addStatement(
    const FileName& file,
    const unsigned int& line,
    const unsigned int& column
    )
{
    dm_statements.push_back(StatementItem(file, line, column));
    return dm_statements.size() - 1;
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void SymbolTable::addStatementAddressRanges(
    const UniqueIdentifier& uid, const std::set<AddressRange>& ranges
    )
{
    BOOST_ASSERT(uid < dm_statements.size());
    dm_statements[uid].dm_addresses += ranges;
    
    //
    // Update the index used to find statements by addresses. Remove all of
    // the existing index entries for this statement. Obtain the new set of
    // address ranges for this statement and add them to the index.
    //
    
    dm_statements_index.get<0>().erase(uid);

    std::set<AddressRange> all_ranges = dm_statements[uid].dm_addresses;

    for (std::set<AddressRange>::const_iterator
             i = all_ranges.begin(); i != all_ranges.end(); ++i)
    {
        dm_statements_index.insert(AddressRangeIndexRow(uid, *i));
    }
}



//------------------------------------------------------------------------------
// By implementing the cloning as a SymbolTable method, rather than externally
// inside of Function, we avoid partitioning the cloned function's address
// ranges into address bitmaps because the original partitioning is copied.
//------------------------------------------------------------------------------
SymbolTable::UniqueIdentifier SymbolTable::cloneFunction(
    const SymbolTable& symbol_table, const UniqueIdentifier& uid
    )
{
    BOOST_ASSERT(uid < symbol_table.dm_functions.size());

    // Create a clone of the original function's FunctionItem

    const FunctionItem& original = symbol_table.dm_functions[uid];
    dm_functions.push_back(FunctionItem(original.dm_name));

    UniqueIdentifier clone_uid = dm_functions.size() - 1;
    
    FunctionItem& clone = dm_functions[clone_uid];
    clone.dm_addresses = original.dm_addresses;
    
    // Update the index used to find functions by addresses
    
    std::set<AddressRange> ranges = clone.dm_addresses;
    
    for (std::set<AddressRange>::const_iterator
             i = ranges.begin(); i != ranges.end(); ++i)
    {
        dm_functions_index.insert(AddressRangeIndexRow(clone_uid, *i));
    }

    // Return the unique identifier of the cloned function
    return clone_uid;
}



//------------------------------------------------------------------------------
// By implementing the cloning as a SymbolTable method, rather than externally
// inside of Statement, we avoid partitioning the cloned statement's address
// ranges into address bitmaps because the original partitioning is copied.
//------------------------------------------------------------------------------
SymbolTable::UniqueIdentifier SymbolTable::cloneStatement(
    const SymbolTable& symbol_table, const UniqueIdentifier& uid
    )
{
    BOOST_ASSERT(uid < symbol_table.dm_statements.size());

    // Create a clone of the original statement's StatementItem

    const StatementItem& original = symbol_table.dm_statements[uid];
    dm_statements.push_back(
        StatementItem(original.dm_file, original.dm_line, original.dm_column)
        );

    UniqueIdentifier clone_uid = dm_statements.size() - 1;
    
    StatementItem& clone = dm_statements[clone_uid];
    clone.dm_addresses = original.dm_addresses;
    
    // Update the index used to find statements by addresses
    
    std::set<AddressRange> ranges = clone.dm_addresses;
    
    for (std::set<AddressRange>::const_iterator
             i = ranges.begin(); i != ranges.end(); ++i)
    {
        dm_statements_index.insert(AddressRangeIndexRow(clone_uid, *i));
    }

    // Return the unique identifier of the cloned statement
    return clone_uid;
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
const std::string& SymbolTable::getFunctionMangledName(
    const UniqueIdentifier& uid
    ) const
{
    BOOST_ASSERT(uid < dm_functions.size());
    return dm_functions[uid].dm_name;
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
std::set<AddressRange> SymbolTable::getFunctionAddressRanges(
    const UniqueIdentifier& uid
    ) const
{
    BOOST_ASSERT(uid < dm_functions.size());
    return dm_functions[uid].dm_addresses;
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
const FileName& SymbolTable::getStatementFile(const UniqueIdentifier& uid) const
{
    BOOST_ASSERT(uid < dm_statements.size());
    return dm_statements[uid].dm_file;
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
unsigned int SymbolTable::getStatementLine(const UniqueIdentifier& uid) const
{
    BOOST_ASSERT(uid < dm_statements.size());
    return dm_statements[uid].dm_line;
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
unsigned int SymbolTable::getStatementColumn(const UniqueIdentifier& uid) const
{
    BOOST_ASSERT(uid < dm_statements.size());
    return dm_statements[uid].dm_column;
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
std::set<AddressRange> SymbolTable::getStatementAddressRanges(
    const UniqueIdentifier& uid
    ) const
{
    BOOST_ASSERT(uid < dm_statements.size());
    return dm_statements[uid].dm_addresses;
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void SymbolTable::visitFunctions(const FunctionVisitor& visitor)
{
    bool terminate = false;
    Function value(shared_from_this(), 0);
    
    for (UniqueIdentifier i = 0, iEnd = dm_functions.size(); 
         !terminate && (i != iEnd); 
         ++i)
    {
        value.dm_unique_identifier = i;
        terminate |= !visitor(value);
    }
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void SymbolTable::visitFunctions(const AddressRange& range,
                                 const FunctionVisitor& visitor)
{
    bool terminate = false;
    boost::dynamic_bitset<> visited(dm_functions.size());

    visit(range, visitor, terminate, visited);
}


       
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void SymbolTable::visitFunctionDefinitions(const UniqueIdentifier& uid,
                                           const StatementVisitor& visitor)
{
    BOOST_ASSERT(uid < dm_functions.size());

    bool terminate = false;
    boost::dynamic_bitset<> visited(dm_statements.size());

    std::set<AddressRange> ranges = dm_functions[uid].dm_addresses;
    
    visit(AddressRange(ranges.begin()->begin()), visitor, terminate, visited);
}


        
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void SymbolTable::visitFunctionStatements(const UniqueIdentifier& uid,
                                          const StatementVisitor& visitor)
{
    BOOST_ASSERT(uid < dm_functions.size());

    bool terminate = false;
    boost::dynamic_bitset<> visited(dm_statements.size());
    
    std::set<AddressRange> ranges = dm_functions[uid].dm_addresses;
    
    for (std::set<AddressRange>::const_iterator
             i = ranges.begin(); !terminate && (i != ranges.end()); ++i)
    {
        visit(*i, visitor, terminate, visited);
    }
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void SymbolTable::visitStatements(const StatementVisitor& visitor)
{
    bool terminate = false;
    Statement value(shared_from_this(), 0);
    
    for (UniqueIdentifier i = 0, iEnd = dm_statements.size();
         !terminate && (i != iEnd); 
         ++i)
    {
        value.dm_unique_identifier = i;
        terminate |= !visitor(value);
    }
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void SymbolTable::visitStatements(const AddressRange& range,
                                  const StatementVisitor& visitor)
{
    bool terminate = false;
    boost::dynamic_bitset<> visited(dm_statements.size());

    visit(range, visitor, terminate, visited);
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void SymbolTable::visitStatementFunctions(const UniqueIdentifier& uid,
                                          const FunctionVisitor& visitor)
{
    BOOST_ASSERT(uid < dm_statements.size());

    bool terminate = false;
    boost::dynamic_bitset<> visited(dm_functions.size());
    
    std::set<AddressRange> ranges = dm_statements[uid].dm_addresses;
    
    for (std::set<AddressRange>::const_iterator
             i = ranges.begin(); !terminate && (i != ranges.end()); ++i)
    {
        visit(*i, visitor, terminate, visited);
    }
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void SymbolTable::visit(const AddressRange& range,
                        const FunctionVisitor& visitor,
                        bool& terminate, boost::dynamic_bitset<>& visited)
{
    Function value(shared_from_this(), 0);

    AddressRangeIndex::nth_index<1>::type::const_iterator i = 
        dm_functions_index.get<1>().lower_bound(range.begin());
    
    if (i != dm_functions_index.get<1>().begin())
    {
        --i;
    }
    
    AddressRangeIndex::nth_index<1>::type::const_iterator iEnd = 
        dm_functions_index.get<1>().upper_bound(range.end());
    
    for (; !terminate && (i != iEnd); ++i)
    {
        if (i->dm_range.intersects(range) &&
            (i->dm_uid < visited.size()) && !visited[i->dm_uid])
        {
            visited[i->dm_uid] = true;
            value.dm_unique_identifier = i->dm_uid;
            terminate |= !visitor(value);
        }
    }
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void SymbolTable::visit(const AddressRange& range,
                        const StatementVisitor& visitor,
                        bool& terminate, boost::dynamic_bitset<>& visited)
{
    Statement value(shared_from_this(), 0);

    AddressRangeIndex::nth_index<1>::type::const_iterator i =
        dm_statements_index.get<1>().lower_bound(range.begin());
    
    if (i != dm_statements_index.get<1>().begin())
    {
        --i;
    }
    
    AddressRangeIndex::nth_index<1>::type::const_iterator iEnd =
        dm_statements_index.get<1>().upper_bound(range.end());
    
    for (; !terminate && (i != iEnd); ++i)
    {
        if (i->dm_range.intersects(range) && 
            (i->dm_uid < visited.size()) && !visited[i->dm_uid])
        {
            visited[i->dm_uid] = true;
            value.dm_unique_identifier = i->dm_uid;
            terminate |= !visitor(value);
        }
    }
}
