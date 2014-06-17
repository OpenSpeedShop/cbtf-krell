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


// Handle data from sampling.
void StacktraceData::aggregateAddressCounts(
	const unsigned& len,
	const uint64_t* st,
	const uint8_t* counts,
	AddressBuffer& buffer) const
{
    // Iterate over each of the stacktrace address entries.
    for(unsigned i = 0; i < len; ++i) {
	buffer.updateAddressCounts(st[i], counts[i]);
    }
}

// Handle data from tracing.
void StacktraceData::aggregateAddressCounts(
	const unsigned& len,
	const uint64_t* st,
	AddressBuffer& buffer) const
{
    // Iterate over each of the stacktrace address entries.
    for(unsigned i = 0; i < len; ++i) {
	buffer.updateAddressCounts(st[i], 1);
    }
}

// Handle data from tracing using exclusive time in function as count.
void StacktraceData::aggregateAddressCounts(
	AddressCounts& addrcounts,
	AddressBuffer& buffer) const
{
	buffer.updateAddressCounts(addrcounts);
}

// graph g is a directed graph using an adjaceny list (boost).
void StacktraceData::graphAddressCounts(
	const unsigned& len,
	const uint64_t* st,
	const uint8_t* counts,
	Graph& g) const
{
    // Iterate over each of the stacktrace address entries.
    // The stacktrace buffer starts with a stacktrace where the first
    // element is a terminal frame and it's path follows until the
    // next terminal frame of the next stack (until no more stacks).
    // To represent a directed graph, any frame where the count is
    // greater than zero is a leaf and therefore can not have an edge
    // coming out of this node in the tree. To forward iterate the
    // buffer array we need to create edges from i+1 -> i until we
    // reach a new leaf frame (non zero count).
    // the code for this scheme is not working as of yet.
    // iterate stacktrace in reverse since the collector returns
    // a stacktrace buffer where the first element is a terminal frame.
    bool stackend = false;
    for (int i = len - 1; i >= 0; i--) {
	int k = i-1;
	if (k >= 0) {
  
	  // trace entries with a 0 count are simplty callstack frames.
	  // trace entries with a non zero count are sample frames and
	  // terminate the stack.  In some cases, OpenMP stacks are
	  // recorded with a begin frame of 0x0. Ignore these.
	  // do not create an edge out of a terminal frame...
	  if (counts[i] == 0 && st[i] > 0) {
	    Address out((uint64_t)st[i]);
	    Address in((uint64_t)st[k]);
	    uint64_t cost = counts[i];
	    g.addEdge(out,in,/*cost*/ cost);
	  }

#if 0
	  if (counts[i] > 0 ) {
		std::cerr << "StacktraceData::graphAddressCounts: Address "
		<< Address((uint64_t)st[i]) << " has count "
		<< (int)counts[i] <<  std::endl;
	  }
#endif
	}
    }
}

void StacktraceData::graphAddressCounts(
	const unsigned& len,
	const uint64_t* st,
	Graph& g) const
{
    // Iterate over each of the stacktrace address entries.
    for(unsigned i = 0; i < len; ++i) {
#if 0
	std::cerr << "Address " << Address((uint64_t)st[i]) << " has count 1" <<  std::endl;
#endif
    }
}
