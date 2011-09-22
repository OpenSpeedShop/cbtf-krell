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
 * Definition of the StacktraceData class.
 *
 */
//#define DEBUG_OVERLAP 1
 
#include "KrellInstitute/Core/Assert.hpp"
#include "KrellInstitute/Core/AddressBuffer.hpp"
#include "KrellInstitute/Core/StacktraceData.hpp"

using namespace KrellInstitute::Core;



/**
 * Default constructor.
 *
 * StacktraceData.
 */
StacktraceData::StacktraceData()
{
}



void StacktraceData::aggregateAddressCounts(
	const unsigned& len,
	const uint64_t* st,
	const uint8_t* counts,
	AddressBuffer& buffer) const
{
    // Iterate over each of the stacktrace address entries.
    for(unsigned i = 0; i < len; ++i) {
#if 0
	std::cerr << "Address " << Address((uint64_t)st[i]) << " has count "
	 << (int)counts[i] <<  std::endl;
#endif
	buffer.updateAddressCounts(st[i], counts[i]);
    }
}

void StacktraceData::aggregateAddressCounts(
	const unsigned& len,
	const uint64_t* st,
	AddressBuffer& buffer) const
{
    // Iterate over each of the stacktrace address entries.
    for(unsigned i = 0; i < len; ++i) {
#if 0
	std::cerr << "Address " << Address((uint64_t)st[i]) << " has count 1" <<  std::endl;
#endif
	buffer.updateAddressCounts(st[i], 1);
    }
}
