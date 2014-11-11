////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
// Copyright (c) 2008 William Hachfeld. All Rights Reserved.
// Copyright (c) 2011 The Krell Institute. All Rights Reserved.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License as published by the Free
// Software Foundation; either version 2.1 of the License, or (at your option)
// any later version.
//
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
// details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, write to the Free Software Foundation, Inc.,
// 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
////////////////////////////////////////////////////////////////////////////////

/** @file
 *
 * Definition of the SymbolTable class.
 *
 */

#include "KrellInstitute/Core/Address.hpp"
#include "KrellInstitute/Core/AddressBitmap.hpp"
#include "KrellInstitute/Core/Blob.hpp"
#include "KrellInstitute/Core/LinkedObjectEntry.hpp"
#include "KrellInstitute/Core/Path.hpp"
#include "KrellInstitute/Core/SymbolTable.hpp"

#include <deque>

using namespace KrellInstitute::Core;


namespace {
    void convert(const Blob& in, CBTF_Protocol_Blob& out)
    {
    out.data.data_len = in.getSize();
    out.data.data_val = reinterpret_cast<uint8_t*>(malloc(
        std::max(static_cast<unsigned>(1), in.getSize()) * sizeof(char)
        ));
    if(in.getSize() > 0)
        memcpy(out.data.data_val, in.getContents(), in.getSize());
    }
};


/**
 * Constructor from address range.
 *
 * Constructs a new, empty, SymbolTable that occupies the specified address
 * range. The address range of the symbol table is used to check the validity
 * of functions, etc. that are later added.
 *
 * @param range    Address range occupied by this symbol table.
 */
SymbolTable::SymbolTable(const AddressRange& range) :
    dm_range(range),
    dm_functions(),
    dm_statements()
{
}



/**
 * Add a function.
 *
 * Adds the specified function with its associated address range to this symbol
 * table. Functions are discarded when they have an invalid address range, are
 * not contained entirely within this symbol table, or overlap previously added
 * functions.
 *
 * @note    In theory a symbol table will never contain invalid address ranges,
 *          overlapping address ranges, or address ranges outside the address
 *          range of the symbol table itself. There are various cases, however,
 *          where bogus symbol information results in this happening. This
 *          function contains sanity checks that help keep the experiment
 *          database, if not 100% correct, at least useable under such 
 *          circumstances.
 *
 * @param begin    Beginning address associated with this function.
 * @param end      Ending address associated with this function.
 * @param name     Name of this function.
 */
void SymbolTable::addFunction(const Address& begin, 
			      const Address& end,
			      const std::string& name)
{
    // Discard functions where (end <= begin)
    if(end <= begin)
	return;
    
    // Construct the address range [begin, end)
    AddressRange range(begin, end);

    // Discard functions not contained entirely within this symbol table
    if(!dm_range.doesContain(range)) {
	return;
    }
    
    // Discard functions overlapping functions already in this symbol table
    if(dm_functions.find(range) != dm_functions.end()) {
	return;
    }
    
    dm_functions.insert(std::make_pair(range, name));
}



void SymbolTable::addFunction(const Address& begin, 
			      const Address& end,
			      const Address& base,
			      const std::string& name)
{
    // Add this function to the symbol table
    addFunction(begin+base,end+base,name);
    AddressRange range(begin,end);
    FunctionTable::iterator fti =
            table_functions.insert(
                std::make_pair(name, std::vector<AddressRange>())
                ).first;
    fti->second.push_back(range);
}



/**
 * Add a statement.
 *
 * Adds the specified statement with its associated address range to this symbol
 * table. Statements are discarded when they have an invalid address range, or
 * are not contained entirely within this symbol table.
 *
 * @note    In theory a symbol table will never contain invalid address ranges,
 *          or address ranges outside the address range of the symbol table
 *          itself. There are various cases, however, where bogus symbol
 *          information results in this happening. This function contains
 *          sanity checks that help keep the experiment database, if not 100%
 *          correct, at least useable under such circumstances.
 *
 * @param begin     Beginning address associated with this statement.
 * @param end       Ending address associated with this statement.
 * @param path      Full path name of this statement's source file.
 * @param line      Line number of this statement.
 * @param column    Column number of this statement.
 */
void SymbolTable::addStatement(const Address& begin, 
			       const Address& end,
			       const Path& path, 
			       const int& line, 
			       const int& column)
{
    // Discard statements where (end <= begin)
    if(end <= begin)
	return;
    
    // Construct the address range [begin, end)
    AddressRange range(begin, end);

    // Discard statements not contained entirely within this symbol table
    if(!dm_range.doesContain(range))
	return;
    
    // Add this statement to the symbol table (or find the existing statement)
    std::map<StatementEntry, std::vector<AddressRange> >::iterator
	i = dm_statements.insert(
	    std::make_pair(
		StatementEntry(path, line, column),
		std::vector<AddressRange>())
	    ).first;
    
    // Add this address range to the statement
    i->second.push_back(range);
}



/**
 * Process and store into a linked object.
 *
 * Process the information previously added to this symbol table and store it
 * for the specified linked object.
 *
 * @param linked_object    Linked object using this symbol table.
 */
void SymbolTable::processAndStore(const LinkedObjectEntry& linked_object)
{
    // Iterate over each function entry
    for(std::map<AddressRange, std::string>::const_iterator
	    i = dm_functions.begin(); i != dm_functions.end(); ++i) {

	// Get the function range
	Address addr_begin(i->first.getBegin() - dm_range.getBegin());
	Address addr_end(i->first.getEnd() - dm_range.getBegin());
	
#if 0
	std::cerr << "ProcessSTORE: F " << i->second
	<< " LOBJ " << linked_object.getPath()
	<< "\n dm_range begin: " << dm_range
	<< "\n dm_func begin: " << i->first.getBegin()
	<< " dm_func end: " << i->first.getEnd()
	<< "\n addr_begin: " << addr_begin
	<< " addr_end: " << addr_end
	<< std::endl;
#endif

	// Construct a valid bitmap for this (entire) function range
	AddressBitmap valid_bitmap(AddressRange(addr_begin, addr_end));
	for(Address addr = addr_begin; addr < addr_end; ++addr)
	    valid_bitmap.setValue(addr, true);

    }

    // Iterate over each statement entry
    for(std::map<StatementEntry, std::vector<AddressRange> >::const_iterator
	    i = dm_statements.begin(); i != dm_statements.end(); ++i) {

	// Construct the set of unique addresses for this statement
	std::set<Address> addresses;
	for(std::vector<AddressRange>::const_iterator
		j = i->second.begin(); j != i->second.end(); ++j)
	    for(Address k = j->getBegin(); k != j->getEnd(); ++k)
		addresses.insert(Address(k - dm_range.getBegin()));
	
	// Partition this statement's address set
	std::vector<std::set<Address> > address_sets =
	    partitionAddressSet(addresses);
	
	// Iterate over each partitioned address set
	for(std::vector<std::set<Address> >::const_iterator
		j = address_sets.begin(); j != address_sets.end(); ++j) {
	    
	    // Create and populate an address bitmap for this address set
	    AddressBitmap valid_bitmap(AddressRange(*(j->begin()),
						    *(j->rbegin()) + 1));
	    for(std::set<Address>::const_iterator
		    k = j->begin(); k != j->end(); ++k)
		valid_bitmap.setValue(*k, true);
	    
	}


    }
}




/**
 * Conversion to CBTF_Protocol_SymbolTable.
 *
 * Convert this symbol table to an CBTF_Protocol_SymbolTable object.
 *
 * @note    The caller assumes responsibility for releasing all allocated
 *          memory when it is no longer needed.
 *
 * @note    The "experiments" and "linked_object" fields of the returned object
 *          are not initialized to any value. It is the responsibility of the
 *          caller to fill in these fields.
 *
 * @return    An CBTF_Protocol_SymbolTable containing the contents of this
 *            symbol table.
 */
SymbolTable::operator CBTF_Protocol_SymbolTable() const
{
    CBTF_Protocol_SymbolTable object;

    // Allocate an appropriately sized array of functions
    object.functions.functions_len = table_functions.size();
    object.functions.functions_val =
	reinterpret_cast<CBTF_Protocol_FunctionEntry*>(malloc(
	    std::max(static_cast<FunctionTable::size_type>(1),
		     table_functions.size()) *
	    sizeof(CBTF_Protocol_FunctionEntry)
	    ));
	
    // Iterate over all the functions in this symbol table
    int idx = 0;
    for(FunctionTable::const_iterator
	    i = table_functions.begin(); i != table_functions.end(); ++i, ++idx) {
	CBTF_Protocol_FunctionEntry& entry = 
	    object.functions.functions_val[idx];

	// Convert the function's name
	//KrellInstitute::Core::convert(i->first, entry.name);
	entry.name = strdup(i->first.c_str());
	
	// Partition this function's address ranges into bitmaps
	std::vector<AddressBitmap> bitmaps = partitionAddressRanges(i->second);
	
	// Convert the function's bitmaps
	convert(bitmaps, entry.bitmaps.bitmaps_len, entry.bitmaps.bitmaps_val);
	
    }

    // Allocate an appropriately sized array of statements
    object.statements.statements_len = table_statements.size();
    object.statements.statements_val =
	reinterpret_cast<CBTF_Protocol_StatementEntry*>(malloc(
	    std::max(static_cast<StatementTable::size_type>(1),
		     table_statements.size()) *
	    sizeof(CBTF_Protocol_StatementEntry)
	    ));
    
    // Iterate over all the statements in this symbol table
    idx = 0;
    for(StatementTable::const_iterator
	    i = table_statements.begin(); i != table_statements.end(); ++i, ++idx) {
	CBTF_Protocol_StatementEntry& entry =
	    object.statements.statements_val[idx];
	
	// Convert the statement's path, line, and column
	entry.path.path = strdup(i->first.dm_path.c_str());
	entry.line = i->first.dm_line;
	entry.column = i->first.dm_column;
	
	// Partition this statement's address ranges into bitmaps
	std::vector<AddressBitmap> bitmaps = partitionAddressRanges(i->second);
	
	// Convert the function's bitmaps
	convert(bitmaps, entry.bitmaps.bitmaps_len, entry.bitmaps.bitmaps_val);
	
    }

    // Return the conversion to the caller
    return object;
}



/**
 * Partition address ranges.
 *
 * Partitions address ranges into address bitmaps. The addresses for a function
 * or statement are stored as pairings of an address range and a bitmap - one
 * bit per address in the range - that describes which addresses within the
 * range are associated with the function or statement. In the common case where
 * the addresses exhibit a high degree of spatial locality, storing a single
 * address range and bitmap is very effective. But there are cases, such as
 * inlined functions, where the degree of spatial locality is minimal. Under
 * such circumstances, a single bitmap can grow very large and it is more space
 * efficient to use multiple bitmaps that individually exhibit spatial locality.
 * This function iteratively subdivides all the addresses until each bitmap
 * exhibits a "sufficient" amount of spatial locality.
 *
 * @note    The criteria for subidiving an address set is as follows. The
 *          widest gap (spacing) between two adjacent addresses is found. If
 *          the number of bits required to encode this gap in the bitmap is
 *          greater than the number of bits required to create an AddressRange
 *          object, then the set is partitioned at this, widest, gap.
 *
 * @param ranges    Address ranges to be partitioned.
 * @return          Address bitmaps representing these address ranges.
 */
std::vector<AddressBitmap>
SymbolTable::partitionAddressRanges(const std::vector<AddressRange>& ranges)
{
    std::vector<AddressBitmap> bitmaps;

    // Set the partitioning criteria
    static const Address::difference_type PartitioningCriteria =
	8 * (sizeof(uint32_t) + 2 * sizeof(uint64_t));

    // Construct the set of unique addresses in these address ranges
    std::set<Address> addresses;
    for(std::vector<AddressRange>::const_iterator
	    i = ranges.begin(); i != ranges.end(); ++i)
	for(Address j = i->getBegin(); j != i->getEnd(); ++j)
	    addresses.insert(j);
    
    // Initialize a queue with this initial, input, set of addresses
    std::deque<std::set<Address> > queue(1, addresses);

    // Iterate until the queue is empty
    while(!queue.empty()) {
        std::set<Address> i = queue.front();
        queue.pop_front();

	// Handle special case for empty sets (ignore them)
        if(i.size() == 0)
            continue;

	// Handle special case for single-element sets
        if(i.size() == 1) {

	    // Create and populate an address bitmap for this address set
	    AddressBitmap bitmap(AddressRange(*(i.begin()), *(i.rbegin()) + 1));
	    for(std::set<Address>::const_iterator
                    j = i.begin(); j != i.end(); ++j)
                bitmap.setValue(*j, true);

	    // Add this bitmap to the results
	    bitmaps.push_back(bitmap);
	    
            continue;
        }
        
	// Pair specifying the widest gap in this address set
        std::pair<std::set<Address>::const_iterator, Address::difference_type>
            widest_gap = std::make_pair(i.begin(), 0);
	
        // Iterate over each adjacent pair of addresses in this set
        for(std::set<Address>::const_iterator
		prev = i.begin(), current = ++i.begin();
            current != i.end();
            ++prev, ++current) {

            // Calculate the gap between this adjacent address pair
            Address::difference_type gap =
                AddressRange(*prev, *current).getWidth() - 1;

	    // Is this gap the widest so far?
            if(gap > widest_gap.second) {
		widest_gap.first = current;
                widest_gap.second = gap;
            }
            
        }

	// Is the widest gap greater than our partitioning criteria?
        if(widest_gap.second > PartitioningCriteria) {

	    // Partition the set at this gap
	    queue.push_back(std::set<Address>(i.begin(), widest_gap.first));
            queue.push_back(std::set<Address>(widest_gap.first, i.end()));

	}
        
        // Otherwise keep this set unpartitioned
        else {

	    // Create and populate an address bitmap for this address set
	    AddressBitmap bitmap(AddressRange(*(i.begin()), *(i.rbegin()) + 1));
            for(std::set<Address>::const_iterator 
		    j = i.begin(); j != i.end(); ++j)
                bitmap.setValue(*j, true);

	    // Add this bitmap to the results
	    bitmaps.push_back(bitmap);

	}

    }

    // Return the bitmaps to the caller
    return bitmaps;
}




/**
 * Partition an address set.
 *
 * Partitions an address set into one or more subsets. As a comprimise between
 * query speed and database size, the addresses associated with a statement are 
 * stored as an address range and a bitmap - one bit per address in the range -
 * that describes which addresses within the range are associated with the 
 * statement. In the common case where a statement's addresses exhibit a high
 * degree of spatial locality, storing one address range and bitmap is very
 * effective. But there are cases, such as inlined functions, where the degree
 * of spatial locality is minimal. Under such circumstances, the bitmap can
 * grow very large and it is more space efficient to partition the bitmap into
 * subsets that themselves exhibit spatial locality. This function iteratively
 * subdivides the address sets until each subset exhibits a "sufficient" amount
 * of spatial locality.
 *
 * @note    The criteria for subdividing an address set is as follows. The
 *          widest gap (spacing) between two adjacent addresses is found. If
 *          the number of bits required to represent this gap in the bitmap
 *          is greater than the number of bits required to store the initial
 *          header of a StatementRanges table row, then the set is partitioned
 *          at this, widest, gap.
 *
 * @param address_set    Set of addresses to be partitioned.
 * @return               Partitioned sets of addresses.
 */
std::vector<std::set<Address> >
SymbolTable::partitionAddressSet(const std::set<Address>& address_set)
{
    std::vector<std::set<Address> > result;
    
    // Set the partitioning criteria
    static const Address::difference_type PartitioningCriteria =
	8 * (sizeof(uint32_t) + 2 * sizeof(uint64_t));
    
    // Initialize a queue with the initial, input, set of addresses
    std::deque<std::set<Address> > queue(1, address_set);
    
    // Iterate until the queue is empty
    while(!queue.empty()) {	
	std::set<Address> i = queue.front();
	queue.pop_front();
	
	// Handle special case for empty sets (ignore them)
	if(i.size() == 0)
	    continue;
	
	// Handle special case for single-element sets
	if(i.size() == 1) {
	    result.push_back(i);
	    continue;
	}
	
	// Pair specifying the widest gap in this address set
	std::pair<std::set<Address>::const_iterator, Address::difference_type>
	    widest_gap = std::make_pair(i.begin(), 0);

	// Iterate over each adjacent pair of addresses in this set
	for(std::set<Address>::const_iterator
		prev = i.begin(), current = ++i.begin();
	    current != i.end();
	    ++prev, ++current) {

	    // Calculate the gap between this adjacent address pair
	    Address::difference_type gap = 
		AddressRange(*prev, *current).getWidth() - 1;
	    
	    // Is this gap the widest so far?
	    if(gap > widest_gap.second) {
		widest_gap.first = current;
		widest_gap.second = gap;
	    }
	    
	}
	
	// Is the widest gap greater than our partitioning criteria?
	if(widest_gap.second > PartitioningCriteria) {

	    // Partition the set at this gap
	    queue.push_back(std::set<Address>(i.begin(), widest_gap.first));
	    queue.push_back(std::set<Address>(widest_gap.first, i.end()));
	    
	}
	
	// Otherwise keep this set unpartitioned
	else
	    result.push_back(i);
	
    }

    // Return the results to the caller
    return result;
}


/**
 * Convert bitmaps for protocol use.
 *
 * Converts the specified address bitmaps to the structure used in protocol
 * messages.
 *
 * @note    The caller assumes responsibility for releasing all allocated
 *          memory when it is no longer needed.
 *
 * @param bitmaps         Address bitmaps to be converted.
 * @retval bitmaps_len    Length of array containing the results.
 * @retval bitmaps_val    Array containing the results.
 */
void SymbolTable::convert(const std::vector<AddressBitmap>& bitmaps,
			  u_int& bitmaps_len,
			  CBTF_Protocol_AddressBitmap*& bitmaps_val)
{
    // Allocate an appropriately sized array of bitmaps
    bitmaps_len = bitmaps.size();
    bitmaps_val = 
	reinterpret_cast<CBTF_Protocol_AddressBitmap*>(malloc(
            std::max(static_cast<std::vector<AddressBitmap>::size_type>(1),
		     bitmaps.size()) *
	    sizeof(CBTF_Protocol_AddressBitmap)
	    ));
    
    // Iterate over all the bitmaps
    for(std::vector<AddressBitmap>::size_type i = 0; i < bitmaps.size(); ++i) {
	CBTF_Protocol_AddressBitmap& entry = bitmaps_val[i];
	
	// Convert the bitmap's address range
	entry.range.begin = bitmaps[i].getRange().getBegin().getValue();
	entry.range.end = bitmaps[i].getRange().getEnd().getValue();
	
	// Convert the actual bitmap
	::convert(bitmaps[i].getBlob(), entry.bitmap);
	
    }
}
