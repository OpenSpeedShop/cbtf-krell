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
 * Definition of PerfData methods.
 *
 */

#include <algorithm>

#include "KrellInstitute/Core/PerfData.hpp"


using namespace KrellInstitute::Core;

namespace {
#ifndef NDEBUG
/** Flag indicating if debuging for LinkedObjects is enabled. */
bool is_debug_aggregator_events_enabled =
    (getenv("CBTF_DEBUG_AGGR_EVENTS") != NULL);
#endif

    void aggregatePCData(const std::string id, const Blob &blob,
			 AddressBuffer &buf, uint64_t &interval)
    {
	if (id == "pcsamp") {
            CBTF_pcsamp_data data;
            memset(&data, 0, sizeof(data));
            unsigned bsize = blob.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_CBTF_pcsamp_data), &data);
	    interval = data.interval;
	    PCData pcdata;
            pcdata.aggregateAddressCounts(data.pc.pc_len, data.pc.pc_val, data.count.count_val, buf);
            xdr_free(reinterpret_cast<xdrproc_t>(xdr_CBTF_pcsamp_data),
                     reinterpret_cast<char*>(&data));

	} else if (id == "hwc") {
            CBTF_hwc_data data;
            memset(&data, 0, sizeof(data));
            unsigned bsize = blob.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_CBTF_hwc_data), &data);
	    interval = data.interval;
	    PCData pcdata;
            pcdata.aggregateAddressCounts(data.pc.pc_len, data.pc.pc_val, data.count.count_val, buf);
            xdr_free(reinterpret_cast<xdrproc_t>(xdr_CBTF_hwc_data),
                     reinterpret_cast<char*>(&data));
	} else if (id == "hwcsamp") {
            CBTF_hwcsamp_data data;
            memset(&data, 0, sizeof(data));
            unsigned bsize = blob.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_CBTF_hwcsamp_data), &data);
	    interval = data.interval;
	    PCData pcdata;
            pcdata.aggregateAddressCounts(data.pc.pc_len, data.pc.pc_val, data.count.count_val, buf);
            xdr_free(reinterpret_cast<xdrproc_t>(xdr_CBTF_hwcsamp_data),
                     reinterpret_cast<char*>(&data));
	} else {
	    return;
	}
    }

    void aggregateSTSampleData(const std::string id, const Blob &blob,
			 AddressBuffer &buf, uint64_t &interval)
    {
	StacktraceData stdata;
	if (id == "usertime") {
            CBTF_usertime_data data;
            memset(&data, 0, sizeof(data));
            unsigned bsize = blob.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_CBTF_usertime_data), &data);
	    interval = data.interval;
	    stdata.aggregateAddressCounts(data.stacktraces.stacktraces_len,
				data.stacktraces.stacktraces_val,
				data.count.count_val, buf);

#if defined(CREATE_GRAPH)
	    stdata.graphAddressCounts(data.stacktraces.stacktraces_len,
				data.stacktraces.stacktraces_val,
				data.count.count_val, dGraph);

	    dGraph.printGraph();
#endif

            xdr_free(reinterpret_cast<xdrproc_t>(xdr_CBTF_usertime_data),
                     reinterpret_cast<char*>(&data));
	} else if (id == "hwctime") {
            CBTF_hwctime_data data;
            memset(&data, 0, sizeof(data));
            unsigned bsize = blob.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_CBTF_hwctime_data), &data);
	    interval = data.interval;
	    stdata.aggregateAddressCounts(data.stacktraces.stacktraces_len,
				data.stacktraces.stacktraces_val,
				data.count.count_val, buf);
            xdr_free(reinterpret_cast<xdrproc_t>(xdr_CBTF_hwctime_data),
                     reinterpret_cast<char*>(&data));
	} else {
	    return;
	}

    }

    void aggregateSTTraceData(const std::string id, const Blob &blob,
			 AddressBuffer &buf)
    {
	StacktraceData stdata;
	if (id == "io") {
            CBTF_io_trace_data data;
            memset(&data, 0, sizeof(data));
            unsigned bsize = blob.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_CBTF_io_trace_data), &data);
	    int eventcount = 0;
	    AddressCounts addressTime;
	    for(unsigned i = 0; i < data.events.events_len; ++i) {
		++eventcount;
		uint64_t event_time = data.events.events_val[i].stop_time - data.events.events_val[i].start_time;

		for (unsigned j = data.events.events_val[i].stacktrace;
		     j < data.stacktraces.stacktraces_len; ++j) {

		    if (data.stacktraces.stacktraces_val[j] == 0) break; // end of stack
	        	Address a;
			a = Address(data.stacktraces.stacktraces_val[j]);

			AddressCounts::iterator it = addressTime.find(a);
			if (it == addressTime.end() ) {
			    addressTime.insert(std::make_pair(a,event_time));
			} else {
			    (*it).second += event_time;
		    }
		}
	    }
	    stdata.aggregateAddressCounts(addressTime,buf);
            xdr_free(reinterpret_cast<xdrproc_t>(xdr_CBTF_io_trace_data),
                     reinterpret_cast<char*>(&data));
	} else if (id == "iop") {
            CBTF_io_profile_data data;
            memset(&data, 0, sizeof(data));
            unsigned bsize = blob.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_CBTF_io_profile_data), &data);
	    int eventcount = 0;
	    AddressCounts addressTime;
	    for(unsigned i = 0; i < data.time.time_len; ++i) {
		++eventcount;
	        Address a;
		a = Address(data.stacktraces.stacktraces_val[i]);

		AddressCounts::iterator it = addressTime.find(a);
		if (it == addressTime.end() ) {
		    addressTime.insert(std::make_pair(a,data.time.time_val[i]));
		} else {
		    (*it).second += data.time.time_val[i];
		}
	    }

	    stdata.aggregateAddressCounts(addressTime,buf);
            xdr_free(reinterpret_cast<xdrproc_t>(xdr_CBTF_io_profile_data),
                     reinterpret_cast<char*>(&data));
	} else if (id == "iot") {
            CBTF_io_exttrace_data data;
            memset(&data, 0, sizeof(data));
            unsigned bsize = blob.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_CBTF_io_exttrace_data), &data);
	    int eventcount = 0;
	    AddressCounts addressTime;
	    for(unsigned i = 0; i < data.events.events_len; ++i) {
		++eventcount;
		uint64_t event_time = data.events.events_val[i].stop_time - data.events.events_val[i].start_time;

		for (unsigned j = data.events.events_val[i].stacktrace;
		     j < data.stacktraces.stacktraces_len; ++j) {

		    if (data.stacktraces.stacktraces_val[j] == 0) break; // end of stack
	        	Address a;
			a = Address(data.stacktraces.stacktraces_val[j]);

			AddressCounts::iterator it = addressTime.find(a);
			if (it == addressTime.end() ) {
			    addressTime.insert(std::make_pair(a,event_time));
			} else {
			    (*it).second += event_time;
		    }
		}
	    }
	    stdata.aggregateAddressCounts(addressTime,buf);
            xdr_free(reinterpret_cast<xdrproc_t>(xdr_CBTF_io_exttrace_data),
                     reinterpret_cast<char*>(&data));
	} else if (id == "mem") {
            CBTF_mem_exttrace_data data;
            memset(&data, 0, sizeof(data));
            unsigned bsize = blob.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_CBTF_mem_exttrace_data), &data);
	    int eventcount = 0;
	    AddressCounts addressTime;
	    for(unsigned i = 0; i < data.events.events_len; ++i) {
		++eventcount;
		uint64_t event_time = data.events.events_val[i].stop_time - data.events.events_val[i].start_time;

		for (unsigned j = data.events.events_val[i].stacktrace;
		     j < data.stacktraces.stacktraces_len; ++j) {

		    if (data.stacktraces.stacktraces_val[j] == 0) break; // end of stack

	            Address a;
		    a = Address(data.stacktraces.stacktraces_val[j]);

		    AddressCounts::iterator it = addressTime.find(a);
		    if (it == addressTime.end() ) {
			addressTime.insert(std::make_pair(a,event_time));
		    } else {
			(*it).second += event_time;
		    }
		}
	    }

	    stdata.aggregateAddressCounts(addressTime,buf);
            xdr_free(reinterpret_cast<xdrproc_t>(xdr_CBTF_mem_exttrace_data),
                     reinterpret_cast<char*>(&data));
	} else if (id == "omptp") {
            CBTF_ompt_profile_data data;
            memset(&data, 0, sizeof(data));
            unsigned bsize = blob.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_CBTF_ompt_profile_data), &data);
	    int eventcount = 0;
	    AddressCounts addressTime;
	    for(unsigned i = 0; i < data.time.time_len; ++i) {
		++eventcount;
	        Address a;
		a = Address(data.stacktraces.stacktraces_val[i]);

		AddressCounts::iterator it = addressTime.find(a);
		if (it == addressTime.end() ) {
		    addressTime.insert(std::make_pair(a,data.time.time_val[i]));
		} else {
		    (*it).second += data.time.time_val[i];
		}
	    }

	    StacktraceData stdata;
	    stdata.aggregateAddressCounts(addressTime,buf);
            xdr_free(reinterpret_cast<xdrproc_t>(xdr_CBTF_ompt_profile_data),
                     reinterpret_cast<char*>(&data));
	} else if (id == "pthreads") {
            CBTF_pthreads_exttrace_data data;
            memset(&data, 0, sizeof(data));
            unsigned bsize = blob.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_CBTF_pthreads_exttrace_data), &data);
	    int eventcount = 0;
	    AddressCounts addressTime;
	    for(unsigned i = 0; i < data.events.events_len; ++i) {
		++eventcount;
		uint64_t event_time = data.events.events_val[i].stop_time - data.events.events_val[i].start_time;

		for (unsigned j = data.events.events_val[i].stacktrace;
		     j < data.stacktraces.stacktraces_len; ++j) {

		    if (data.stacktraces.stacktraces_val[j] == 0) break; // end of stack
	        	Address a;
			a = Address(data.stacktraces.stacktraces_val[j]);

			AddressCounts::iterator it = addressTime.find(a);
			if (it == addressTime.end() ) {
			    addressTime.insert(std::make_pair(a,event_time));
			} else {
			    (*it).second += event_time;
		    }
		}
	    }
	    stdata.aggregateAddressCounts(addressTime,buf);
            xdr_free(reinterpret_cast<xdrproc_t>(xdr_CBTF_pthreads_exttrace_data),
                     reinterpret_cast<char*>(&data));
	} else if (id == "mpi") {
            CBTF_mpi_trace_data data;
            memset(&data, 0, sizeof(data));
            unsigned bsize = blob.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_CBTF_mpi_trace_data), &data);
	    int eventcount = 0;
	    AddressCounts addressTime;
	    for(unsigned i = 0; i < data.events.events_len; ++i) {
		++eventcount;
		uint64_t event_time = data.events.events_val[i].stop_time - data.events.events_val[i].start_time;

		for (unsigned j = data.events.events_val[i].stacktrace;
		     j < data.stacktraces.stacktraces_len; ++j) {

		    if (data.stacktraces.stacktraces_val[j] == 0) break; // end of stack
	        	Address a;
			a = Address(data.stacktraces.stacktraces_val[j]);

			AddressCounts::iterator it = addressTime.find(a);
			if (it == addressTime.end() ) {
			    addressTime.insert(std::make_pair(a,event_time));
			} else {
			    (*it).second += event_time;
		    }
		}
	    }
	    stdata.aggregateAddressCounts(addressTime,buf);
            xdr_free(reinterpret_cast<xdrproc_t>(xdr_CBTF_mpi_trace_data),
                     reinterpret_cast<char*>(&data));
	} else if (id == "mpip") {
            CBTF_mpi_profile_data data;
            memset(&data, 0, sizeof(data));
            unsigned bsize = blob.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_CBTF_mpi_profile_data), &data);
	    int eventcount = 0;
	    AddressCounts addressTime;
	    for(unsigned i = 0; i < data.time.time_len; ++i) {
		++eventcount;
	        Address a;
		a = Address(data.stacktraces.stacktraces_val[i]);

		AddressCounts::iterator it = addressTime.find(a);
		if (it == addressTime.end() ) {
		    addressTime.insert(std::make_pair(a,data.time.time_val[i]));
		} else {
		    (*it).second += data.time.time_val[i];
		}
	    }

	    stdata.aggregateAddressCounts(addressTime,buf);
            xdr_free(reinterpret_cast<xdrproc_t>(xdr_CBTF_mpi_profile_data),
                     reinterpret_cast<char*>(&data));
	} else if (id == "mpit") {
            CBTF_mpi_exttrace_data data;
            memset(&data, 0, sizeof(data));
            unsigned bsize = blob.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_CBTF_mpi_exttrace_data), &data);
	    int eventcount = 0;
	    AddressCounts addressTime;
	    for(unsigned i = 0; i < data.events.events_len; ++i) {
		++eventcount;
		uint64_t event_time = data.events.events_val[i].stop_time - data.events.events_val[i].start_time;

		for (unsigned j = data.events.events_val[i].stacktrace;
		     j < data.stacktraces.stacktraces_len; ++j) {

		    if (data.stacktraces.stacktraces_val[j] == 0) break; // end of stack
			Address a;
			a = Address(data.stacktraces.stacktraces_val[j]);

			AddressCounts::iterator it = addressTime.find(a);
			if (it == addressTime.end() ) {
			    addressTime.insert(std::make_pair(a,event_time));
			} else {
			    (*it).second += event_time;
		    }
		}
	    }
	    stdata.aggregateAddressCounts(addressTime,buf);
            xdr_free(reinterpret_cast<xdrproc_t>(xdr_CBTF_mpi_exttrace_data),
                     reinterpret_cast<char*>(&data));
	} else {
	    return;
	}

    }
};

int PerfData::aggregate(const Blob &blob, AddressBuffer &buf) {
	// decode this blobs data header
        CBTF_DataHeader header;
        memset(&header, 0, sizeof(header));
        unsigned header_size = blob.getXDRDecoding(
            reinterpret_cast<xdrproc_t>(xdr_CBTF_DataHeader), &header
            );
	std::string collectorID(header.id);

	// find the actual data blob after the header and create a Blob.
	// TODO at callsite: Map the incoming data size to it's thread and increment as new
	// data for same thread arrives.  Could be use to identify threads
	// that are generating more data than others. REDUCTION.
	unsigned data_size = blob.getSize() - header_size;
	const void* data_ptr = &(reinterpret_cast<const char *>(blob.getContents())[header_size]);
	Blob dblob(data_size,data_ptr);

#ifndef NDEBUG
        if (is_debug_aggregator_events_enabled) {
	    std::cerr << "Aggregating Data Blob addresses for "
	    << collectorID << " data bytes: " << data_size
	    << std::endl;
	}
#endif

	// The following does a global aggregation of the data. Not per thread of execution.
	if (collectorID == "pcsamp" || collectorID == "hwc" || collectorID == "hwcsamp") {
	    uint64_t interval;
            aggregatePCData(collectorID, dblob, buf, interval);
	} else if (collectorID == "usertime" || collectorID == "hwctime") {
	    uint64_t interval;
            aggregateSTSampleData(collectorID, dblob, buf, interval);
        } else if (collectorID == "io" || collectorID == "iot" ||
		   collectorID == "iop" || collectorID == "mem" ||
		   collectorID == "mpi" || collectorID == "mpit" ||
		   collectorID == "mpip" || collectorID == "pthreads" ||
		   collectorID == "omptp" ) {
            aggregateSTTraceData(collectorID, dblob, buf);
	} else {
	    std::cerr << "Unknown collector data handled!" << std::endl;
	}
	return data_size;
}

// Memory specific metrics.
// The passed blob here is from a specific single thread known to the caller.
// The thread can be found from the header.
//
// All events are processed to record unique addresses in to an AddressBuffer
// used for resolving symbols (common to all collectors).
// On the caller side this addressbuffer is mapped to it's specific thread.
//
// Detecting leaks.
// Unpack the passed blob, loop thru events and update a map with
// allocation addresses and allocation sizes for malloc and friends.
// If an event is a free, the address in the free ptr argument is looked up
// and the event is removed from the map if it exists. The current memory
// allocation for the thread is adjusted. Any addresses that remain in the
// allocation map at termination of thread represent a leak.
//
// Unique callpath pseudo events.
// For each unique callstack seen, save the initial event, otherwise increment
// a count for the times we have seen this callstack. Record the max and min
// allocation seen allong this path for allocation calls.  Record the current
// total allocation for thread at time of initial event.
//
// Maintaing highwater mark and current memory allocation.
// For any event recorded into the interesting events list, record the current
// total allocation for the thread (use to create a timeline of memory growth).
// For a highwater mark, record the active memory allocations in a thread.
// If the event allocates memory and sets a new highwater mark, the event
// is recorded in to a general vector of interesting events and tagged
// with CBTF_MEM_REASON_HIGHWATER_SET.
//
// Recording events that hold memory longer than some threshold of time.
// Implemented but not active at this time.
// CBTF_MEM_REASON_DURATION_OF_ALLOCATION.
//
int PerfData::memMetrics(const Blob &blob, MemMetrics& metrics) {
    // decode this blobs data header
    CBTF_DataHeader header;
    memset(&header, 0, sizeof(header));
    unsigned header_size = blob.getXDRDecoding(
            reinterpret_cast<xdrproc_t>(xdr_CBTF_DataHeader), &header
            );
    std::string collectorID(header.id);

    // find the actual data blob after the header and create a Blob.
    const void* data_ptr =
	&(reinterpret_cast<const char *>(blob.getContents())[header_size]);
    Blob dblob(blob.getSize() - header_size,data_ptr);

    CBTF_mem_exttrace_data data;
    memset(&data, 0, sizeof(data));
    unsigned bsize =
	dblob.getXDRDecoding(
		reinterpret_cast<xdrproc_t>(xdr_CBTF_mem_exttrace_data),
					    &data);

    Time lasteventTime(data.events.events_val[data.events.events_len-1].start_time);

    for(unsigned i = 0; i < data.events.events_len; ++i) {

	bool is_interesting = false;
	CBTF_mem_reason reason = CBTF_MEM_REASON_UNKNOWN;
	uint64_t event_time = data.events.events_val[i].stop_time -
			      data.events.events_val[i].start_time;

	StackTrace stack;
	for (unsigned j = data.events.events_val[i].stacktrace;
		     j < data.stacktraces.stacktraces_len; ++j) {
	    stack.push_back(Address(data.stacktraces.stacktraces_val[j]));
	    // end of stack
	    if (data.stacktraces.stacktraces_val[j] == 0) break;
	}



	// Maintain current and highwater allocation metrics.
	switch (data.events.events_val[i].mem_type) {
	    case CBTF_MEM_MALLOC: {
		uint64_t size = data.events.events_val[i].size1;
		Address a(data.events.events_val[i].retval);
		AddressCounts::iterator it = metrics.allocationSizes.find(a);
		if (it == metrics.allocationSizes.end() ) {
		    metrics.allocationSizes.insert(std::make_pair(a,size));
		} else {
		    (*it).second += size;
		}
		metrics.currentAllocation += size;
		metrics.totalAllocations++;
		if (metrics.currentAllocation > metrics.highwater) {
		    metrics.highwater = metrics.currentAllocation;
		    reason = CBTF_MEM_REASON_HIGHWATER_SET;
		    is_interesting = true;
		}

		// addrMemEvent maintains active allocations.
		AddressMemEventMap::iterator ev = metrics.addrMemEvent.find(a);
		if (ev == metrics.addrMemEvent.end()) {
		    MemEvent m(data.events.events_val[i],stack);
		    m.dm_reason = CBTF_MEM_REASON_STILLALLOCATED;
		    m.dm_total_allocation = metrics.currentAllocation;
		    // count,max,min are no-ops for this class of event.
		    m.dm_count = 0;
		    m.dm_max = 0;
		    m.dm_min = 0;
		    metrics.addrMemEvent.insert(std::make_pair(a,m));
		}

		if (is_interesting) {
		    std::string r = "UNKNOWN REASON";
		    switch (reason) {
			case CBTF_MEM_REASON_HIGHWATER_SET: {
			    r = "NEW HIGHWATER";
			    break;
			}
			default: {
			    r = "UNKNOWN REASON";
			}
		    }

		    MemEvent m(data.events.events_val[i],stack);
		    m.dm_reason = reason;
		    m.dm_total_allocation = metrics.highwater;
		    // count,max,min are no-ops for this class of event.
		    m.dm_count = 0;
		    m.dm_max = 0;
		    m.dm_min = 0;
		    // This adds events such as a new highwater into the
		    // vector of interesting events as separate event items.
		    // ie.  there certainly will events with a similar
		    // callpath marked as unique callpath.  The idea is
		    // to allow the mem view to select by reason when
		    // displaying data.
		    metrics.eventsOfInterest.push_back(m);
		}
		break;
	    }
	    case CBTF_MEM_REALLOC: {
		uint64_t size = data.events.events_val[i].size1;
		Address a(data.events.events_val[i].retval);
		// this state acts as a malloc
		if (data.events.events_val[i].ptr == 0 && size > 0) {

		    AddressCounts::iterator it = metrics.allocationSizes.find(a);
		    if (it == metrics.allocationSizes.end()) {
			metrics.allocationSizes.insert(std::make_pair(a,size));
		    } else {
			(*it).second += size;
		    }

		    metrics.currentAllocation += size;
		    metrics.totalAllocations++;
		    if (metrics.currentAllocation > metrics.highwater) {
			metrics.highwater = metrics.currentAllocation;
			reason = CBTF_MEM_REASON_HIGHWATER_SET;
			is_interesting = true;
		    }

		    // addrMemEvent maintains active allocations.
		    AddressMemEventMap::iterator ev = metrics.addrMemEvent.find(a);
		    if (ev == metrics.addrMemEvent.end()) {
		        MemEvent m(data.events.events_val[i],stack);
			m.dm_reason = CBTF_MEM_REASON_STILLALLOCATED;
			m.dm_total_allocation = metrics.currentAllocation;
			// count,max,min are no-ops for this class of event.
			m.dm_count = 0;
			m.dm_max = 0;
			m.dm_min = 0;
		        metrics.addrMemEvent.insert(std::make_pair(a,m));
		    }

		    if (is_interesting) {
			std::string r = "UNKNOWN REASON";
			switch (reason) {
			    case CBTF_MEM_REASON_HIGHWATER_SET: {
				r = "NEW HIGHWATER";
			        break;
			    }
			    default: {
				r = "UNKNOWN REASON";
			    }
			}

			MemEvent m(data.events.events_val[i],stack);
		        m.dm_total_allocation = metrics.highwater;
			m.dm_reason = reason;
			// count,max,min are no-ops for this class of event.
			m.dm_count = 0;
			m.dm_max = 0;
			m.dm_min = 0;
			// This adds events such as a new highwater into the
			// vector of insteresting events as separate items.
			// ie.  there certainly will events with a similar
			// callpath marked as unique calltpath.  The idea is
			// to allow the mem view to select by reason when
			// displaying data.
			metrics.eventsOfInterest.push_back(m);
		    }
		}
		// TODO: this state acts as a free
		else if (data.events.events_val[i].ptr == 0 && size == 0) {
		    std::cerr << "REALLOC address:" << a
		    << "ptr and size arg are 0, this acts as a free" << std::endl;
		}
		// realloc with non NULL ptr and size > 0 requires ptr to
		// be a value in allocationSizes table returned from a previous
		// malloc,calloc,realloc.
		// if retval is nonNULL and different from ptr, then ptr is freed.
		// if realloc fails the original block is not moved or freed.
		else if (data.events.events_val[i].ptr > 0 && size > 0) {
		    bool free_ptr = false;
		    if (data.events.events_val[i].retval > 0 &&
			data.events.events_val[i].ptr != data.events.events_val[i].retval) {
			free_ptr = true;
		    }
		    bool is_ptr_allocated = false;
		    Address a_ptr(data.events.events_val[i].ptr);
		    AddressCounts::iterator it = metrics.allocationSizes.find(a_ptr);
		    if (it == metrics.allocationSizes.end()) {
			is_ptr_allocated = true;
		    }
#if defined(TRACE_DETAILS)
		    std::cerr << "REALLOC address:" << a
		    << " ptr address:" << Address(data.events.events_val[i].ptr)
		    << " size1:" << data.events.events_val[i].size1
		    << " free_ptr:" << free_ptr
		    << " is_ptr_allocated:" << is_ptr_allocated
		    << std::endl;
#endif
		}
		else {
		}
		break;
	    }
	    case CBTF_MEM_CALLOC: {
		// TODO
		break;
	    }
	    case CBTF_MEM_FREE: {
		// If ptr is NULL, no operation is performed.
		// If ptr has been freed before, undefined behavior.
		uint64_t tmp = 0;
		Address a(data.events.events_val[i].ptr);
		AddressCounts::iterator it = metrics.allocationSizes.find(a);
		if (it == metrics.allocationSizes.end() ) {
		    // This may happen if we have traced a free
		    // where we did not trace the allocating call.
		    metrics.allocationSizes.insert(std::make_pair(a,0));
		} else {
		    tmp = (*it).second;
		    (*it).second = 0;
		}
		uint64_t prev_allocation = metrics.currentAllocation;
		metrics.currentAllocation -= tmp;
		metrics.totalFrees++;

		AddressMemEventMap::iterator ev = metrics.addrMemEvent.find(a);
		if (ev != metrics.addrMemEvent.end() ) {
		    // this allocation was freed.  renove it.
		    metrics.addrMemEvent.erase(ev);
#if defined(RECORD_ALLOCATION_DURATIONS)
		    int64_t active_time = data.events.events_val[i].start_time
					  - (*ev).second.dm_stop_time;
		    //if (active_time >= 10*1000000) (*ev).second.dm_interesting = true;
		    if (! (*ev).second.dm_interesting) {
			metrics.addrMemEvent.erase(ev);
		    } else {
		        // do we want to make the active_time events interesting.
		        //metrics.eventsOfInterest.push_back((*ev).second);
		    }
#endif
		}

		MemEventVec::iterator eoi =
				std::find(metrics.eventsOfInterest.begin(),
					  metrics.eventsOfInterest.end(),
					  MemEvent(data.events.events_val[i].ptr));

		if (eoi != metrics.eventsOfInterest.end()) {
		    size_t idx = eoi - metrics.eventsOfInterest.begin();
#if defined(TRACE_DETAILS)
		    std::cerr << "FREE size:" << (*eoi).dm_size1
			<< " at address:" << a
			<< " current:" << metrics.currentAllocation
			<< " previous:" << prev_allocation
			<< " index " << idx
			<< " of " << metrics.eventsOfInterest.size()
			<< std::endl;
#endif
		}
		break;
	    }
	    case CBTF_MEM_MEMALIGN: {
		std::cerr << "DETECTED MEMALIGN" << std::endl;
		break;
	    }
	    case CBTF_MEM_POSIX_MEMALIGN: {
		std::cerr << "DETECTED POSIX_MEMALIGN" << std::endl;
		break;
	    }
	    case CBTF_MEM_UNKNOWN: {
		std::cerr << "DETECTED UNKNOWN MEM EVENT" << std::endl;
		break;
	    }
	    default: {
		std::cerr << "DETECTED UNKNOWN MEM EVENT" << std::endl;
	    }
	}

	// The MemEvent records a stack. The StackTraceCountMap stackCounts
	// records the number of times that path was called in the MemMetrics
	// struct. Ideally the OSS database schema could define a StackTrace
	// table that records the unique callstacks per thread separately from
	// the event data and have the MemEvent xdr use an ID into the
	// StackTrace table to find the correct stack. This would reduce the
	// size of event data items considerably.
	StackMemEventMap::iterator stmei = metrics.stackMemEvents.find(stack);
	if (stmei == metrics.stackMemEvents.end()) {
	    // these are pseudo events that record general stats for unique paths
	    // to a memory event.
	    // stackMemEvents records all unique callpaths and stats per callpath.
	    // For this category we would like to see things like max,min allocation
	    // on this path, count of calls through this path, etc.
	    // max = max allocation (or largest free)
	    // min = min allocation (or smallest free)
	    // count = counts on this path (for allocation type)
	    // total_allocation = total allocated on this path.
	    // retval, ptr, size1, size2 will be recorded from initial event.
	    MemEvent m(data.events.events_val[i],stack);
	    m.dm_reason = CBTF_MEM_REASON_UNIQUE_CALLPATH;
	    switch (data.events.events_val[i].mem_type) {
	        case CBTF_MEM_MALLOC: {
		    m.dm_total_allocation = data.events.events_val[i].size1;
		    m.dm_max = data.events.events_val[i].size1;
		    m.dm_min = data.events.events_val[i].size1;
		    m.dm_count = 1;
		    break;
		}
	        case CBTF_MEM_CALLOC: {
		    uint64_t allocsize =
			data.events.events_val[i].size1 * data.events.events_val[i].size2;
		    m.dm_total_allocation = allocsize;
		    m.dm_max = allocsize;
		    m.dm_min = allocsize;
		    m.dm_count = 1;
		    break;
		}
	        case CBTF_MEM_REALLOC: {
		    uint64_t allocsize = 0;
		    if (data.events.events_val[i].ptr == 0) {
			allocsize = data.events.events_val[i].size1;
		    }
		    m.dm_total_allocation = allocsize;
		    m.dm_max = allocsize;
		    m.dm_min = allocsize;
		    m.dm_count = 1;
		    break;
		}
	        case CBTF_MEM_FREE: {
		    m.dm_total_allocation = metrics.currentAllocation;
		    m.dm_max = 0;
		    m.dm_min = 0;
		    m.dm_count = 1;
		    break;
		}
		default: {
		}
	    }

            std::pair<StackMemEventMap::iterator, bool> tmp =
		metrics.stackMemEvents.insert(std::make_pair(stack,
							     m));
	} else {
	   // if this path already exists, update it's overall stats.
	   // The view code needs to understand the overloaded nature of the
	   // event's size1,size_t, and total_allocation members.
	    switch (data.events.events_val[i].mem_type) {
	        case CBTF_MEM_MALLOC: {
		    // malloc allocates size bytes. If size is 0, then malloc returns
		    // either NULL, or unique pointer value that can passed to free.
		    //
		    // bump count
		    stmei->second.dm_count++;
		    // update total allocation along this path
		    stmei->second.dm_total_allocation += data.events.events_val[i].size1;
		    // update max allocation along this path
		    if (data.events.events_val[i].size1 > stmei->second.dm_max) {
			stmei->second.dm_max = data.events.events_val[i].size1;
		    }
		    // update min allocation along this path
		    if (data.events.events_val[i].size1 < stmei->second.dm_min) {
			stmei->second.dm_min = data.events.events_val[i].size1;
		    }
		    break;
		}
	        case CBTF_MEM_CALLOC: {
		    // calloc allocates memory for an array of size1 elements of size2
		    // bytes each. If size 1 is 0 the calloc returns either NULL,
		    // or unique pointer value that can passed to free.
		    //
		    // bump count
		    stmei->second.dm_count++;
		    uint64_t allocsize =
			data.events.events_val[i].size1 * data.events.events_val[i].size2;
		    // update total allocation along this path
		    stmei->second.dm_total_allocation += allocsize;
		    // update max allocation along this path
		    if (allocsize > stmei->second.dm_max) {
			stmei->second.dm_max = allocsize;
		    }
		    // update min allocation along this path
		    if (allocsize < stmei->second.dm_min) {
			stmei->second.dm_min = allocsize;
		    }
		    break;
		}
	        case CBTF_MEM_REALLOC: {
		    // FIXME: Need to handle case where prt is not NULL, case where
		    // size is 0 and prt in not NULL (free), All non NULL values of
		    // ptr must must be in out tracked calls.
		    // realloc changes the size of the memory block pointed to by ptr
		    // to size1 bytes.  The contents will be unchanged in the range from
		    // the start of the region up to the minimum of the old and new sizes.
		    // If the new size is larger than the old size, the added memory will
		    // not be initialized.  If ptr is NULL, then the call is equivalent
		    // to malloc(size), for all values of size; if size is equal to zero,
		    // and ptr is not NULL, then the call is equivalent to free(ptr).
		    // Unless ptr is NULL, it must have been  returned by an earlier call
		    // to malloc, calloc or realloc.
		    // If the area pointed to was moved, a free(ptr) is done.
		    //
		    // bump count
		    stmei->second.dm_count++;
		    uint64_t allocsize = data.events.events_val[i].size1;
		    // update total allocation along this path
		    if (data.events.events_val[i].ptr == 0) {
			stmei->second.dm_total_allocation += allocsize;
		    }
		    // update max allocation along this path
		    if (allocsize > stmei->second.dm_max) {
			stmei->second.dm_max = allocsize;
		    }
		    // update min allocation along this path
		    if (allocsize < stmei->second.dm_min) {
			stmei->second.dm_min = allocsize;
		    }
		    break;
		}
	        case CBTF_MEM_FREE: {
		    // bump count
		    stmei->second.dm_count++;
		    break;
		}
		default: {
		}
	    }
	}
    }

    return bsize;

    xdr_free(reinterpret_cast<xdrproc_t>(xdr_CBTF_mem_exttrace_data),
                 reinterpret_cast<char*>(&data));
}
