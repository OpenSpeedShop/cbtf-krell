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

#include "KrellInstitute/Core/Address.hpp"
#include "KrellInstitute/Core/AddressEntry.hpp"
//#include "KrellInstitute/Core/AddressBuffer.hpp"
#include "KrellInstitute/Core/PCData.hpp"
#include "KrellInstitute/Core/StacktraceData.hpp"
#include "KrellInstitute/Core/PerfData.hpp"
#include "KrellInstitute/Messages/Blob.h"
#include "KrellInstitute/Messages/DataHeader.h"
#include "KrellInstitute/Messages/Address.h"
#include "KrellInstitute/Messages/PCSamp_data.h"
#include "KrellInstitute/Messages/Usertime_data.h"
#include "KrellInstitute/Messages/Hwc_data.h"
#include "KrellInstitute/Messages/Hwcsamp_data.h"
#include "KrellInstitute/Messages/Hwctime_data.h"
#include "KrellInstitute/Messages/IO_data.h"
#include "KrellInstitute/Messages/Mem_data.h"
#include "KrellInstitute/Messages/Mpi_data.h"
#include "KrellInstitute/Messages/Pthreads_data.h"
#include "KrellInstitute/Messages/ThreadEvents.h"

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
	if (id == "usertime") {
            CBTF_usertime_data data;
            memset(&data, 0, sizeof(data));
            unsigned bsize = blob.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_CBTF_usertime_data), &data);
	    interval = data.interval;
	    StacktraceData stdata;
	    stdata.aggregateAddressCounts(data.stacktraces.stacktraces_len,
				data.stacktraces.stacktraces_val,
				data.count.count_val, buf);

#if 0
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
	    StacktraceData stdata;
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
	    StacktraceData stdata;
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

	    StacktraceData stdata;
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
	    StacktraceData stdata;
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
	    StacktraceData stdata;
	    stdata.aggregateAddressCounts(addressTime,buf);
            xdr_free(reinterpret_cast<xdrproc_t>(xdr_CBTF_mem_exttrace_data),
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
	    StacktraceData stdata;
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
	    StacktraceData stdata;
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

	    StacktraceData stdata;
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
	    StacktraceData stdata;
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
		   collectorID == "mpip" || collectorID == "pthreads") {
            aggregateSTTraceData(collectorID, dblob, buf);
	} else {
	    std::cerr << "Unknown collector data handled!" << std::endl;
	}
	return data_size;
}
