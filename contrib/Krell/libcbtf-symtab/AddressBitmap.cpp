////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2013 Krell Institute. All Rights Reserved.
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

/** @file Definition of the AddressBitmap class. */

#include <algorithm>
#include <boost/assert.hpp>
#include <boost/cstdint.hpp>
#include <cstdlib>

#include "AddressBitmap.hpp"

using namespace KrellInstitute::SymbolTable;
using namespace KrellInstitute::SymbolTable::Impl;



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
AddressBitmap::AddressBitmap(const AddressRange& range) :
    dm_range(range),
    dm_bitmap(dm_range.width(), false)
{
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
AddressBitmap::AddressBitmap(const std::set<Address>& addresses) :
    dm_range(*(addresses.begin()), *(addresses.rbegin())),
    dm_bitmap(dm_range.width(), false)
{
    for (std::set<Address>::const_iterator
             i = addresses.begin(); i != addresses.end(); ++i)
    {
        setValue(*i, true);
    }
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
AddressBitmap::AddressBitmap(const CBTF_Protocol_AddressBitmap& message) :
    dm_range(message.range),
    dm_bitmap(dm_range.width(), false)
{
    boost::uint64_t size = std::max<boost::uint64_t>(
        1, ((dm_range.width() - 1) / 8) + 1
        );
    
    BOOST_VERIFY(message.bitmap.data.data_len == size);
    
    for (boost::uint64_t i = 0; i < dm_range.width(); ++i)
    {
        dm_bitmap[i] = message.bitmap.data.data_val[i / 8] & (1 << (i % 8));
    }
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
AddressBitmap::operator CBTF_Protocol_AddressBitmap() const
{
    boost::uint64_t size = std::max<boost::uint64_t>(
        1, ((dm_range.width() - 1) / 8) + 1
        );
    
    CBTF_Protocol_AddressBitmap message;
    message.range = dm_range;
    message.bitmap.data.data_len = size;
    message.bitmap.data.data_val = reinterpret_cast<uint8_t*>(malloc(size));

    memset(message.bitmap.data.data_val, 0, size);

    for (boost::uint64_t i = 0; i < dm_range.width(); ++i)
    {
        if (dm_bitmap[i])
        {
            message.bitmap.data.data_val[i / 8] |= 1 << (i % 8);
        }
    }

    return message;
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
const AddressRange& AddressBitmap::getRange() const
{
    return dm_range;
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool AddressBitmap::getValue(const Address& address) const
{
    BOOST_VERIFY(dm_range.contains(address));
    
    return dm_bitmap[address - dm_range.begin()];
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void AddressBitmap::setValue(const Address& address, const bool& value)
{
    BOOST_VERIFY(dm_range.contains(address));
    
    dm_bitmap[address - dm_range.begin()] = value;
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
std::set<AddressRange> AddressBitmap::getContiguousRanges(
    const bool& value
    ) const
{
    std::set<AddressRange> ranges;
    
    // Iterate over each address in this address bitmap
    bool in_range = false;
    Address range_begin;    
    for (Address i = dm_range.begin(); i <= dm_range.end(); ++i)
    {
        // Is this address the beginning of a range?
        if (!in_range && (getValue(i) == value))
        {
            in_range = true;
            range_begin = i;
        }
        
        // Is this address the end of a range?
        else if (in_range && (getValue(i) != value))
        {
            in_range = false;
            ranges.insert(AddressRange(range_begin, i));
        }	
    }
    
    // Does a range end at the end of the address bitmap?
    if (in_range)
    {
        ranges.insert(AddressRange(range_begin, dm_range.end()));
    }
    
    // Return the ranges to the caller
    return ranges;
}


 
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& stream, const AddressBitmap& bitmap)
{
    const AddressRange& range = bitmap.getRange();

    stream << range << " ";
    
    bool has_false = false, has_true = false;

    for (Address i = range.begin(); i <= range.end(); ++i)
    {
        if (bitmap.getValue(i) == true)
        {
            has_true = true;
        }
        else
        {
            has_false = true;
        }
    }
    
    if (has_false && !has_true)
    {
        stream << "0...0";
    }
    else if (!has_false && has_true)
    {
        stream << "1...1";
    }
    else
    {
        for (Address i = range.begin(); i <= range.end(); ++i)
        {
            stream << (bitmap.getValue(i) ? "1" : "0");
        }
    }
    
    return stream;
}
