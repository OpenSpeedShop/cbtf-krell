////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2014 The Krell Institue. All Rights Reserved.
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
 * Definition of the  Memory experiment additional metrics.
 *
 */
#ifndef _KrellInsitute_Core_MemEventMetrics_
#define _KrellInsitute_Core_MemEventMetrics_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "KrellInstitute/Core/AddressBuffer.hpp"
#include "KrellInstitute/Core/Address.hpp"
#include "KrellInstitute/Core/Time.hpp"
#include "KrellInstitute/Core/TimeInterval.hpp"
#include "KrellInstitute/Messages/Mem_data.h"


namespace KrellInstitute { namespace Core {

/**
 *
 *  Using a simple lulesh OpenMP run at 4 threads as motivation. 
 *  There are potentially millions of allocations and frees in even
 *  a very short run. For example, an OpenMP version of lulesh can
 *  generate 650000+ allocation events and 650000+ free events in
 *  a short 100 second run at 4 OpenMP threads.  In addition, lulesh
 *  has the habit of calling free(NULL) for most of it's free calls.
 *  (The collector wrapper now discards such frees to reduce message
 *  traffic and datablob size streaming to the filters).
 *
 *  - Recording all 1.3 million events for this 100 second run results
 *    in an openss database of 170MB.
 *  - The time spent in the actual allocation and free calls
 *    for the 100 second run is roughly 2100 ms.
 *  - 650000 Free calls averaged 0.003 ms
 *  - 650000 Allocation calls averaged 0.0003 ms per call.
 *  - Suggests that time spent in a memory call is not interesting.
 *  - Detected memory allocations at 107000 unique memory locations.
 *  - Detected 193 unique callpaths to allocations and free calls.
 *  - All of these allocations where performed in the master thread.
 *  - Detected that 70 of the allocations where never freed
 *    when the process exited.
 *  - Remaining allocated memory total at process exit was about 10.6MB.
 *  - Detected highwater mark was about 27MB.
 *
 *  What is interesting?
 *
 *  1) Memory foot print over time. For example, an event that set
 *     a new allocation highwater mark for the thread is interesting.
 *  2) Unique callpaths where allocations and frees occur. Adding a
 *     counter for how many times this path was seen can pinpoint locations
 *     in the code with frequent allocatons and frees.
 *  3) Allocation events that where never freed are interesting.
 *  4) Unique allocation addresses may be interesting.
 *  5) Allocation,free durations that exceed a threshold may be interesting.
 *  6) For any event considered interesting, recording the current total
 *     allocation of the current thread can track memory footprint and
 *     identify the event where the highwater mark was set.
 *
 */
    class MemEvent {
	public:
        uint64_t dm_start_time;    /**< start time of event */
        uint64_t dm_stop_time;     /**< end time of event */
        uint64_t dm_retval;        /**< return value */
        uint64_t dm_ptr;           /**< ptr arg */
        uint64_t dm_size1;         /**< size 1 arg */
        uint64_t dm_size2;         /**< size 2 arg */
        uint64_t dm_total_allocation; /**< current total allocation size at this time */
        uint64_t dm_max;	   /**< max allocation size seen */
        uint64_t dm_min;	   /**< min allocation size seen */
        uint32_t dm_count;            /**< id or count of event type */
        CBTF_mem_type dm_mem_type; /**< enumerated val idenitfying mem call */
	CBTF_mem_reason dm_reason; /**< Reason for interest */
	StackTrace dm_stacktrace;  /**< stacktrace of this event. */

	MemEvent() {};
	MemEvent(uint64_t& v) {
	    dm_retval = v;
	};
	MemEvent(CBTF_memt_event& e, StackTrace& st) {
	    dm_retval = e.retval;
	    dm_ptr = e.ptr;
	    dm_size1 = e.size1;
	    dm_size2 = e.size2;
	    dm_mem_type = e.mem_type;
	    dm_start_time = e.start_time;
	    dm_stop_time = e.stop_time;
	    dm_stacktrace = st;
	    dm_total_allocation = 0;
	    dm_max = 0;
	    dm_min = 0;
	    dm_count = 0;
	    dm_reason = CBTF_MEM_REASON_UNKNOWN;
	};

	friend bool operator== (const MemEvent &e1, const MemEvent &e2) {
		return (e1.dm_retval == e2.dm_retval);
#if 0
		if ((e1.dm_retval == e2.dm_retval) {
		    return true;
		}
#endif
	};
    };


    typedef std::map<Address,MemEvent> AddressMemEventMap;
    typedef std::map<StackTrace,MemEvent> StackMemEventMap;
    typedef std::vector<MemEvent> MemEventVec;
    typedef std::map<StackTrace,int> StackCountsMap;

    struct MemMetrics {
	AddressCounts allocationSizes;
	AddressMemEventMap addrMemEvent;
	MemEventVec  eventsOfInterest;
	StackCountsMap stackCounts;
	StackMemEventMap stackMemEvents;
	uint64_t highwater;
	uint64_t currentAllocation;
	int totalAllocations;
	int totalFrees;
    };

} }
#endif
