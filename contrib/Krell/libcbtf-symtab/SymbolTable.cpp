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
#include <boost/dynamic_bitset.hpp>
#include <deque>
#include <KrellInstitute/SymbolTable/Function.hpp>
#include <KrellInstitute/SymbolTable/Statement.hpp>

#include "SymbolTable.hpp"

using namespace KrellInstitute::SymbolTable;
using namespace KrellInstitute::SymbolTable::Impl;



/** Anonymous namespace hiding implementation details. */
namespace {

    /**
     * Extract all contiguous address ranges within the given address bitmaps.
     *
     * @param bitmaps    Address bitmaps to be extracted.
     * @return           Contiguous address ranges in those address bitmaps.
     */
    std::set<AddressRange> extract(const std::vector<AddressBitmap>& bitmaps)
    {
        std::set<AddressRange> ranges;
        
        for (std::vector<AddressBitmap>::const_iterator
                 i = bitmaps.begin(); i != bitmaps.end(); ++i)
        {
            std::set<AddressRange> i_ranges = i->ranges(true);
            ranges.insert(i_ranges.begin(), i_ranges.end());
        }
        
        return ranges;
    }

    /**
     * Partition address ranges into address bitmaps. Addresses for functions
     * and statements are stored as pairings of an address range and a bitmap,
     * one bit per address in the range, that describe which addresses within
     * the range are associated with the function or statement. In the common
     * case where the addresses exhibit a high degree of spatial locality, a
     * single address range and bitmap is very effective. But there are cases,
     * such as inlined functions, where the degree of spatial locality can be
     * minimal. Under such circumstances, a single bitmap can grow very large
     * and it is more space efficient to use multiple bitmaps that individually
     * exhibit spatial locality. This function iteratively subdivides all the
     * addresses until each bitmap exhibits sufficient spatial locality.
     *
     * @param ranges    Address ranges to be partitioned.
     * @return          Address bitmaps representing these address ranges.
     *
     * @note    The criteria for subdividing an address set is as follows. The
     *          widest gap (spacing) between two adjacent addresses within the
     *          set is found. If the number of bits required to encode the gap
     *          within a bitmap is greater than the number of bits required to
     *          create a new address bitmap, the set is partitioned at the gap.
     */
    std::vector<AddressBitmap> partition(const std::set<AddressRange>& ranges)
    {
        std::vector<AddressBitmap> bitmaps;

        //
        // Set the partitioning criteria as the minimum number of bits required
        // for the binary representation of a CBTF_Protocol_AddressBitmap that
        // contains a single address. The size of this structure, rather than
        // that of the AddressBitmap class, is used because the main reason for
        // the partitioning is to minimize the eventual binary representation
        // of CBTF_Protocol_SymbolTable objects.
        // 
        
        const boost::int64_t kPartitioningCriteria = 8 /* Bits/Byte */ *
            (2 * sizeof(boost::uint64_t) /* Address Range */ +
             sizeof(boost::uint8_t) /* Single-Byte Bitmap */);
        
        // Convert the provided set of address ranges into a set of addresses
        std::set<Address> addresses;
        for (std::set<AddressRange>::const_iterator
                 i = ranges.begin(); i != ranges.end(); ++i)
        {
            for (Address j = i->begin(); j <= i->end(); ++j)
            {
                addresses.insert(j);
            }
        }
        
        //
        // Initialize a queue with this set of address and iterate over that
        // queue until it has been emptied.
        //
        
        std::deque<std::set<Address> > queue(1, addresses);        
        while (!queue.empty())
        {
            std::set<Address> i = queue.front();
            queue.pop_front();

            // Handle the special case of an empty address set by ignoring it
            if (i.empty())
            {
                continue;
            }

            //
            // Handle the special case of a single-element address set by
            // creating an address bitmap for it and adding that bitmap to
            // the results.
            //

            if (i.size() == 1)
            {
                bitmaps.push_back(AddressBitmap(i));
                continue;
            }

            //
            // Otherwise find the widest gap between any two adjacent addresses
            // within this address set. Also remember WHERE that widest gap was
            // located.
            //

            boost::int64_t widest_gap = 0;
            std::set<Address>::const_iterator widest_gap_at = i.begin();
            
            for (std::set<Address>::const_iterator
                     prev = i.begin(), current = ++i.begin();
                 current != i.end();
                 ++prev, ++current)
            {
                boost::int64_t gap = AddressRange(*prev, *current).width() - 1;
                
                if (gap > widest_gap)
                {
                    widest_gap = gap;
                    widest_gap_at = current;
                }
            }
            
            //
            // If the widest gap exceeds the partitioning criteria, partition
            // this address set at the gap and push both partitions onto the
            // queue. Otherwise create an address bitmap for this address set
            // and add that bitmap to the results.
            //
            
            if (widest_gap > kPartitioningCriteria)
            {
                queue.push_back(std::set<Address>(i.begin(), widest_gap_at));
                queue.push_back(std::set<Address>(widest_gap_at, i.end()));
            }
            else
            {
                bitmaps.push_back(AddressBitmap(i));
            }
            
        }

        // Return the final results to the caller        
        return bitmaps;
    }

} // namespace <anonymous>



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
SymbolTable::SymbolTable(const FileName& name) :
    dm_name(name),
    dm_functions(),
    dm_functions_index(),
    dm_statements(),
    dm_statements_index()
{
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
SymbolTable::SymbolTable(const CBTF_Protocol_SymbolTable& message) :
    dm_name(message.linked_object),
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
    // because AddressBitmap is constructable from CBTF_Protocol_AddressBitmap
    // directly without going through an intermediate set of address ranges.
    //
    
    for (u_int i = 0; i < message.functions.functions_len; ++i)
    {
        const CBTF_Protocol_FunctionEntry& entry =
            message.functions.functions_val[i];
        
        dm_functions.push_back(FunctionItem(entry.name));

        UniqueIdentifier uid = dm_functions.size() - 1;

        for (u_int j = 0; j < entry.bitmaps.bitmaps_len; ++j)
        {            
            dm_functions[uid].dm_addresses.push_back(
                AddressBitmap(entry.bitmaps.bitmaps_val[j])
                );
        }
        
        std::set<AddressRange> all_ranges = 
            extract(dm_functions[uid].dm_addresses);

        for (std::set<AddressRange>::const_iterator
                 j = all_ranges.begin(); j != all_ranges.end(); ++j)
        {
            dm_functions_index.left.insert(std::make_pair(*j, uid));
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

        for (u_int j = 0; j < entry.bitmaps.bitmaps_len; ++j)
        {            
            dm_statements[uid].dm_addresses.push_back(
                AddressBitmap(entry.bitmaps.bitmaps_val[j])
                );
        }
        
        std::set<AddressRange> all_ranges = 
            extract(dm_statements[uid].dm_addresses);

        for (std::set<AddressRange>::const_iterator
                 j = all_ranges.begin(); j != all_ranges.end(); ++j)
        {
            dm_statements_index.left.insert(std::make_pair(*j, uid));
        }
    }
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
SymbolTable::operator CBTF_Protocol_SymbolTable() const
{
    CBTF_Protocol_SymbolTable message;

    message.linked_object = dm_name;

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
        
        entry.bitmaps.bitmaps_len = item.dm_addresses.size();
        
        entry.bitmaps.bitmaps_val =
            reinterpret_cast<CBTF_Protocol_AddressBitmap*>(
                malloc(std::max(1U, entry.bitmaps.bitmaps_len) *
                       sizeof(CBTF_Protocol_AddressBitmap))
                );
        
        for (std::vector<AddressBitmap>::size_type
                 j = 0; j < item.dm_addresses.size(); ++j)
        {
            entry.bitmaps.bitmaps_val[j] = item.dm_addresses[j];
        }
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

        entry.path = item.dm_name;
        entry.line = static_cast<int>(item.dm_line);
        entry.column = static_cast<int>(item.dm_column);
        
        entry.bitmaps.bitmaps_len = item.dm_addresses.size();
        
        entry.bitmaps.bitmaps_val =
            reinterpret_cast<CBTF_Protocol_AddressBitmap*>(
                malloc(std::max(1U, entry.bitmaps.bitmaps_len) *
                       sizeof(CBTF_Protocol_AddressBitmap))
                );
        
        for (std::vector<AddressBitmap>::size_type
                 j = 0; j < item.dm_addresses.size(); ++j)
        {
            entry.bitmaps.bitmaps_val[j] = item.dm_addresses[j];
        }
    }
    
    // Return the completed CBTF_Protocol_SymbolTable to the caller
    return message;
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
const FileName& SymbolTable::getName() const
{
    return dm_name;
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
    BOOST_VERIFY(uid < dm_functions.size());

    //
    // Construct a set of address ranges that contains all current address
    // ranges for this function as well as the given new address ranges.
    // 
    
    std::set<AddressRange> all_ranges = 
        extract(dm_functions[uid].dm_addresses);
    
    all_ranges.insert(ranges.begin(), ranges.end());
    
    //
    // Partition this new set of address ranges into address bitmaps. The
    // new list of address bitmaps completely replaces any previous list.
    //
    
    dm_functions[uid].dm_addresses = partition(all_ranges);
    
    //
    // Update the index used to find functions by addresses. Remove all of
    // the existing index entries for this function. Extract the new set of
    // address ranges for this function and add them to the index.
    //
    
    dm_functions_index.right.erase(uid);

    all_ranges = extract(dm_functions[uid].dm_addresses);

    for (std::set<AddressRange>::const_iterator
             i = all_ranges.begin(); i != all_ranges.end(); ++i)
    {
        dm_functions_index.left.insert(std::make_pair(*i, uid));
    }
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
SymbolTable::UniqueIdentifier SymbolTable::addStatement(
    const FileName& name,
    const unsigned int& line,
    const unsigned int& column
    )
{
    dm_statements.push_back(StatementItem(name, line, column));
    return dm_statements.size() - 1;
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void SymbolTable::addStatementAddressRanges(
    const UniqueIdentifier& uid, const std::set<AddressRange>& ranges
    )
{
    BOOST_VERIFY(uid < dm_statements.size());

    //
    // Construct a set of address ranges that contains all current address
    // ranges for this statement as well as the given new address ranges.
    // 
    
    std::set<AddressRange> all_ranges = 
        extract(dm_statements[uid].dm_addresses);

    all_ranges.insert(ranges.begin(), ranges.end());
    
    //
    // Partition this new set of address ranges into address bitmaps. The
    // new list of address bitmaps completely replaces any previous list.
    //
    
    dm_statements[uid].dm_addresses = partition(all_ranges);
    
    //
    // Update the index used to find statements by addresses. Remove all of
    // the existing index entries for this statement. Extract the new set of
    // address ranges for this statement and add them to the index.
    //
    
    dm_statements_index.right.erase(uid);

    all_ranges = extract(dm_statements[uid].dm_addresses);

    for (std::set<AddressRange>::const_iterator
             i = all_ranges.begin(); i != all_ranges.end(); ++i)
    {
        dm_statements_index.left.insert(std::make_pair(*i, uid));
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
    BOOST_VERIFY(uid < symbol_table.dm_functions.size());

    // Create a clone of the original function's FunctionItem

    const FunctionItem& original = symbol_table.dm_functions[uid];
    dm_functions.push_back(FunctionItem(original.dm_name));

    UniqueIdentifier clone_uid = dm_functions.size() - 1;
    
    FunctionItem& clone = dm_functions[clone_uid];
    clone.dm_addresses = original.dm_addresses;
    
    // Update the index used to find functions by addresses
    
    std::set<AddressRange> ranges = extract(clone.dm_addresses);
    
    for (std::set<AddressRange>::const_iterator
             i = ranges.begin(); i != ranges.end(); ++i)
    {
        dm_functions_index.left.insert(std::make_pair(*i, clone_uid));
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
    BOOST_VERIFY(uid < symbol_table.dm_statements.size());

    // Create a clone of the original statement's StatementItem

    const StatementItem& original = symbol_table.dm_statements[uid];
    dm_statements.push_back(
        StatementItem(original.dm_name, original.dm_line, original.dm_column)
        );

    UniqueIdentifier clone_uid = dm_statements.size() - 1;
    
    StatementItem& clone = dm_statements[clone_uid];
    clone.dm_addresses = original.dm_addresses;
    
    // Update the index used to find statements by addresses
    
    std::set<AddressRange> ranges = extract(clone.dm_addresses);
    
    for (std::set<AddressRange>::const_iterator
             i = ranges.begin(); i != ranges.end(); ++i)
    {
        dm_statements_index.left.insert(std::make_pair(*i, clone_uid));
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
    BOOST_VERIFY(uid < dm_functions.size());
    return dm_functions[uid].dm_name;
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
std::set<AddressRange> SymbolTable::getFunctionAddressRanges(
    const UniqueIdentifier& uid
    ) const
{
    BOOST_VERIFY(uid < dm_functions.size());
    return extract(dm_functions[uid].dm_addresses);
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
const FileName& SymbolTable::getStatementName(const UniqueIdentifier& uid) const
{
    BOOST_VERIFY(uid < dm_statements.size());
    return dm_statements[uid].dm_name;
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
//------------------------------------------------------------------------------
std::set<AddressRange> SymbolTable::getStatementAddressRanges(
    const UniqueIdentifier& uid
    ) const
{
    BOOST_VERIFY(uid < dm_statements.size());
    return extract(dm_statements[uid].dm_addresses);
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
    Function value(shared_from_this(), 0);
    boost::dynamic_bitset<> visited(dm_functions.size());

    for (AddressRangeIndex::left_map::const_iterator
             i = dm_functions_index.left.lower_bound(range),
             iEnd = dm_functions_index.left.upper_bound(range);
         !terminate && (i != iEnd);
         ++i)
    {
        if ((i->second < visited.size()) && !visited[i->second])
        {
            visited[i->second] = true;
            value.dm_unique_identifier = i->second;
            terminate |= !visitor(value);
        }
    }
}


       
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void SymbolTable::visitFunctionDefinitions(const UniqueIdentifier& uid,
                                           const StatementVisitor& visitor)
{
    BOOST_VERIFY(uid < dm_functions.size());

    bool terminate = false;
    Statement value(shared_from_this(), 0);
    boost::dynamic_bitset<> visited(dm_statements.size());
    
    std::set<AddressRange> ranges = extract(dm_functions[uid].dm_addresses);

    for (AddressRangeIndex::left_map::const_iterator
             i = dm_functions_index.left.lower_bound(*ranges.begin()),
             iEnd = dm_functions_index.left.upper_bound(*ranges.begin());
         !terminate && (i != iEnd);
         ++i)
    {
        if ((i->second < visited.size()) && !visited[i->second])
        {
            visited[i->second] = true;
            value.dm_unique_identifier = i->second;
            terminate |= !visitor(value);
        }
    }
}


        
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void SymbolTable::visitFunctionStatements(const UniqueIdentifier& uid,
                                          const StatementVisitor& visitor)
{
    BOOST_VERIFY(uid < dm_functions.size());

    bool terminate = false;
    Statement value(shared_from_this(), 0);
    boost::dynamic_bitset<> visited(dm_statements.size());
    
    std::set<AddressRange> ranges = extract(dm_functions[uid].dm_addresses);
    
    for (std::set<AddressRange>::const_iterator
             i = ranges.begin(); !terminate && (i != ranges.end()); ++i)
    {
        for (AddressRangeIndex::left_map::const_iterator
                 j = dm_statements_index.left.lower_bound(*i),
                 jEnd = dm_statements_index.left.upper_bound(*i);
             !terminate && (j != jEnd);
             ++j)
        {
            if ((j->second < visited.size()) && !visited[j->second])
            {
                visited[j->second] = true;
                value.dm_unique_identifier = j->second;
                terminate |= !visitor(value);
            }
        }
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
    Statement value(shared_from_this(), 0);
    boost::dynamic_bitset<> visited(dm_statements.size());
    
    for (AddressRangeIndex::left_map::const_iterator
             i = dm_statements_index.left.lower_bound(range),
             iEnd = dm_statements_index.left.upper_bound(range);
         !terminate && (i != iEnd);
         ++i)
    {
        if ((i->second < visited.size()) && !visited[i->second])
        {
            visited[i->second] = true;
            value.dm_unique_identifier = i->second;
            terminate |= !visitor(value);
        }
    }
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void SymbolTable::visitStatementFunctions(const UniqueIdentifier& uid,
                                          const FunctionVisitor& visitor)
{
    BOOST_VERIFY(uid < dm_statements.size());

    bool terminate = false;
    Function value(shared_from_this(), 0);
    boost::dynamic_bitset<> visited(dm_functions.size());
    
    std::set<AddressRange> ranges = extract(dm_statements[uid].dm_addresses);
    
    for (std::set<AddressRange>::const_iterator
             i = ranges.begin(); !terminate && (i != ranges.end()); ++i)
    {
        for (AddressRangeIndex::left_map::const_iterator
                 j = dm_functions_index.left.lower_bound(*i),
                 jEnd = dm_functions_index.left.upper_bound(*i);
             !terminate && (j != jEnd);
             ++j)
        {
            if ((j->second < visited.size()) && !visited[j->second])
            {
                visited[j->second] = true;
                value.dm_unique_identifier = j->second;
                terminate |= !visitor(value);
            }
        }
    }
}
