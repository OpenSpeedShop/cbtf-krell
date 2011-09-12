////////////////////////////////////////////////////////////////////////////////
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
 * Definition of the PC Data class.
 *
 */
 
#include "KrellInstitute/Core/Assert.hpp"
#include "KrellInstitute/Core/AddressBuffer.hpp"
#include "KrellInstitute/Core/PCData.hpp"

using namespace KrellInstitute::Core;



/**
 * Default constructor.
 *
 * Constructs a new PC sampling collector with the proper metadata.
 */
PCData::PCData()
{
}

// pcsamp allows a pc address to be counted 255 times
// before it creates another entry in the pc array for
// that same pc address.  i.e. an address can appear
// more than once in the pc array with the matching index
// in the counts array maintaining sample counts.
//
// The hwc data uses counts as a measure of how many times
// the pc address reached the threshold for the papi
// counter used.
//
// hwcsamp data matches pcsamp in it's use of the pc and
// counts arrays.  It has an additional array to record
// the counts for up to si hardware counters and is not
// used by the aggregator.
void PCData::aggregateAddressCounts(
	const unsigned& len,
	const uint64_t* pc,
	const uint8_t* counts,
	AddressBuffer& buffer) const
{
    // Iterate over each of the stacktrace address entries.
    for(unsigned i = 0; i < len; ++i) {
#if 0
	std::cerr << "Address " << Address((uint64_t)pc[i]) << " has count "
	 << (int)counts[i] <<  std::endl;
#endif
	buffer.updateAddressCounts(pc[i], counts[i]);
    }
}
