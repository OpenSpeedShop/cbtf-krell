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
#include <deque>

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
            std::set<AddressRange> bitmap_ranges = i->getContiguousRanges(true);
            ranges.insert(bitmap_ranges.begin(), bitmap_ranges.end());
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
            for (Address j = i->getBegin(); j != i->getEnd(); ++j)
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
                boost::int64_t gap = 
                    AddressRange(*prev, *current).getWidth() - 1;
                
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
    const boost::filesystem::path& path,
    const unsigned int& line,
    const unsigned int& column
    )
{
    dm_statements.push_back(StatementItem(path, line, column));
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
//------------------------------------------------------------------------------
std::set<AddressRange> SymbolTable::getStatementAddressRanges(
    const UniqueIdentifier& uid
    ) const
{
    BOOST_VERIFY(uid < dm_statements.size());
    return extract(dm_statements[uid].dm_addresses);
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
