////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2011-2014 Krell Institute. All Rights Reserved.
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

/** @file Plugin components. */

#include <boost/bind.hpp>
#include <boost/operators.hpp>
#include <boost/shared_ptr.hpp>
#include <typeinfo>
#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>

#include <KrellInstitute/CBTF/Component.hpp>
#include <KrellInstitute/CBTF/Type.hpp>
#include <KrellInstitute/CBTF/Version.hpp>
#include <KrellInstitute/CBTF/Impl/MRNet.hpp>

#include "KrellInstitute/Core/Address.hpp"
#include "KrellInstitute/Core/AddressBuffer.hpp"
#include "KrellInstitute/Core/AddressRange.hpp"
#include "KrellInstitute/Core/Blob.hpp"
//#include "KrellInstitute/Core/Graph.hpp"
//#include "KrellInstitute/Core/PCData.hpp"
#include "KrellInstitute/Core/PerfData.hpp"
//#include "KrellInstitute/Core/Path.hpp"
//#include "KrellInstitute/Core/StacktraceData.hpp"
#include "KrellInstitute/Core/Time.hpp"
#include "KrellInstitute/Core/TimeInterval.hpp"
#include "KrellInstitute/Core/ThreadName.hpp"
#include "KrellInstitute/Messages/Blob.h"
#include "KrellInstitute/Messages/DataHeader.h"
#include "KrellInstitute/Messages/Address.h"
#if 0
#include "KrellInstitute/Messages/PCSamp_data.h"
#include "KrellInstitute/Messages/Usertime_data.h"
#include "KrellInstitute/Messages/Hwc_data.h"
#include "KrellInstitute/Messages/Hwcsamp_data.h"
#include "KrellInstitute/Messages/Hwctime_data.h"
#include "KrellInstitute/Messages/IO_data.h"
#include "KrellInstitute/Messages/Mem_data.h"
#include "KrellInstitute/Messages/Mpi_data.h"
#include "KrellInstitute/Messages/Pthreads_data.h"
#endif
#include "KrellInstitute/Messages/ThreadEvents.h"

using namespace KrellInstitute::CBTF;
using namespace KrellInstitute::Core;

typedef std::map<Address, std::pair<ThreadName,uint64_t> > AddrThreadCountMap;
typedef std::map<ThreadName,AddressBuffer>  ThreadAddrBufMap;

/** requires std::ostringstream debug_prefix in namespace **/
#define DEBUGPREFIX(x,y) \
	if (debug_prefix.str().empty()) { \
	    if (x) debug_prefix << "FE:"; \
	    else  if (y == 1) debug_prefix << "LCP:"; \
	    else  debug_prefix << "ICP:"; \
	    debug_prefix << getpid() << " "; \
	}

namespace {

    std::ostringstream debug_prefix;

#ifndef NDEBUG
/** Flag indicating if debuging for LinkedObjects is enabled. */
bool is_debug_aggregator_events_enabled =
    (getenv("CBTF_DEBUG_AGGR_EVENTS") != NULL);
bool is_trace_aggregator_events_enabled =
    (getenv("CBTF_TRACE_AGGR_EVENTS") != NULL);
bool is_time_aggregator_events_enabled =
    (getenv("CBTF_TIME_AGGR_EVENTS") != NULL);
#endif

    bool is_defer_emit = (getenv("CBTF_DEFER_AGGR_EMIT") != NULL);

    bool is_finished = false;
    bool sent_buffer = false;
    int data_blobs = 0;

    // vector of incoming threadnames. For each thread we expect
    ThreadNameVec threadnames;
    ThreadAddrBufMap threadaddrbufmap;

    // map address counts to threads.
    bool updateAddrThreadCountMap(AddressBuffer& buf,
				   AddrThreadCountMap& addrThreadCount,
				   ThreadName& tname)
    {
#ifndef NDEBUG
	std::stringstream output;
        if (is_trace_aggregator_events_enabled) {
	    output << debug_prefix.str() << "ENTERED updateAddrThreadCountMap  addresscount size  "
	    << buf.addresscounts.size() << std::endl;
	}
#endif
	threadaddrbufmap.insert(std::make_pair(tname,buf));
	AddressCounts::const_iterator aci;

	for (aci = buf.addresscounts.begin(); aci != buf.addresscounts.end(); ++aci) {
	    AddrThreadCountMap::iterator lb = addrThreadCount.lower_bound(aci->first);
	    if(lb != addrThreadCount.end() && !(addrThreadCount.key_comp()(aci->first, lb->first))) {
		if (aci->second > lb->second.second) {
#if 0
		    output << "updateAddrThreadCountMap UPDATE addr " << aci->first
		        << " thread " << tname
		        << " new count " << aci->second << std::endl;
#endif
		    lb->second.first = tname;
		    lb->second.second = aci->second;
		}
	    } else {
#if 0
		output << "updateAddrThreadCountMap INSERTS addr " << aci->first
		    <<  " thread " << tname
		    << " count " << aci->second << std::endl;
#endif
		std::pair<ThreadName,uint64_t> tcount(tname,aci->second);
		addrThreadCount.insert(lb, AddrThreadCountMap::value_type(aci->first, tcount));
	    }
	}

#ifndef NDEBUG
        if (is_trace_aggregator_events_enabled) {
	    output << debug_prefix.str()
	    << "EXIT updateAddrThreadCountMap addrThreadCount size is "
	    << addrThreadCount.size() << std::endl;
	}

	if ( !output.str().empty() ) {
	    std::cerr << output.str();
	}
#endif
    }

    void printAddrThreadCountMap(AddrThreadCountMap& addrTM)
    {
	std::stringstream output;
	output << debug_prefix.str() << "ENTERED printAddrThreadCountMap addrThreadCount size " << addrTM.size() << std::endl;

	AddrThreadCountMap::const_iterator aci;
	for (aci = addrTM.begin(); aci != addrTM.end(); ++aci) {
	    output << "Address " << aci->first
		<< " thread:" << aci->second.first
		<< " count " << aci->second.second
		<< std::endl;
	}
	std::cerr << output.str();
    }


    //Graph dGraph;

    // threads finished.
    int threads_finished = 0;

    // total size of performance data seen.
    int total_data_size = 0;
}

/**
 * Component that aggregates address values and their counts.
 */
class __attribute__ ((visibility ("hidden"))) AddressAggregator :
    public Component
{

public:

    /** Factory function for this component type. */
    static Component::Instance factoryFunction()
    {
        return Component::Instance(
            reinterpret_cast<Component*>(new AddressAggregator())
            );
    }

private:

    /** Default constructor. */
    AddressAggregator() :
        Component(Type(typeid(AddressAggregator)), Version(0, 0, 1))
    {
	handled_buffers = 0;
        declareInput<AddressBuffer>(
            "addressBuffer", boost::bind(&AddressAggregator::addressBufferHandler, this, _1)
            );
        declareInput<Blob>(
            "blob", boost::bind(&AddressAggregator::blobHandler, this, _1)
            );
        declareInput<boost::shared_ptr<CBTF_Protocol_Blob> >(
            "cbtf_protocol_blob",
            boost::bind(
                &AddressAggregator::cbtf_protocol_blob_Handler, this, _1
                )
            );
        declareInput<boost::shared_ptr<CBTF_Protocol_Blob> >(
            "pass_cbtf_protocol_blob",
            boost::bind(
                &AddressAggregator::pass_cbtf_protocol_blob_Handler, this, _1
                )
            );
	declareInput<ThreadNameVec>(
            "threadnames", boost::bind(&AddressAggregator::threadnamesHandler, this, _1)
            );
       declareInput<bool>(
            "finished", boost::bind(&AddressAggregator::finishedHandler, this, _1)
            );
 
        declareOutput<AddressBuffer>("Aggregatorout");
        declareOutput<ThreadAddrBufMap>("ThreadAddrBufMap");
	declareOutput<uint64_t>("interval");
	declareOutput<boost::shared_ptr<CBTF_Protocol_Blob> >("datablob_xdr_out");
    }

    /** Handlers for the inputs.*/
    void threadnamesHandler(const ThreadNameVec& in)
    {
#ifndef NDEBUG
	std::stringstream output;
	DEBUGPREFIX(Impl::TheTopologyInfo.IsFrontend,Impl::TheTopologyInfo.MaxLeafDistance);
	if (is_time_aggregator_events_enabled) {
	    if (threadnames.size() == 0) {
		output << Time::Now() << " " << debug_prefix.str()
		<< " AddressAggregator::threadnamesHandler." << std::endl;
	    }
	}
#endif

        threadnames = in;
#ifndef NDEBUG
        if (is_trace_aggregator_events_enabled) {
            output << debug_prefix.str() << " "
	    << "ENTERED AddressAggregator::threadnamesHandler threadnames size is "
	    << threadnames.size() << std::endl;
	}

	if ( !output.str().empty() ) {
	    std::cerr << output.str();
	}
#endif
    }

    void finishedHandler(const bool& in)
    {
#ifndef NDEBUG
	std::stringstream output;
	DEBUGPREFIX(Impl::TheTopologyInfo.IsFrontend,Impl::TheTopologyInfo.MaxLeafDistance);
	if (is_time_aggregator_events_enabled) {
	    output << Time::Now() << " " << debug_prefix.str()
		<< " AddressAggregator::finishedHandler entered." << std::endl;
	}
#endif
        is_finished = in;
#ifndef NDEBUG
        if (is_trace_aggregator_events_enabled) {
            output << debug_prefix.str() << " "
	    << "ENTERED AddressAggregator::finishedHandler finished is " << is_finished
	    << " for " << threadnames.size() << " threadnames seen"
	    << std::endl;
	}
#endif

	// the only time to send a final buffer to a client is
	// when all threads are finished.  This handler is the notification
	// that all threads have sent a terminated message and therefore all
	// data has been sent.  Likely we do not need the sent_buffer flag
	// anymore...
	if (!sent_buffer) {
#ifndef NDEBUG
	    if (is_trace_aggregator_events_enabled) {
	        output << debug_prefix.str() <<
		"AddressAggregator::finishedHandler EMIT abuffer size:"
		<< abuffer.addresscounts.size() << std::endl;
	    }
#endif

	    emitOutput<AddressBuffer>("Aggregatorout",  abuffer);

#ifndef NDEBUG
	    if (is_trace_aggregator_events_enabled) {
	        output << debug_prefix.str() <<
		"AddressAggregator::finishedHandler EMIT threadaddrbufmap size:"
		    << threadaddrbufmap.size() << std::endl;
	    }
#endif

            emitOutput<ThreadAddrBufMap>("ThreadAddrBufMap",threadaddrbufmap);

	    sent_buffer = true;
#ifndef NDEBUG
	    DEBUGPREFIX(Impl::TheTopologyInfo.IsFrontend,Impl::TheTopologyInfo.MaxLeafDistance);
	    if (is_time_aggregator_events_enabled) {
		output << Time::Now() << " " << debug_prefix.str()
		<< "AddressAggregator::finishedHandler finished." << std::endl;
	    }
#endif
	}

#ifndef NDEBUG
	if ( !output.str().empty() ) {
	    std::cerr << output.str();
	}
#endif
    }
 
    /** Handler for the "CBTF_Protocol_Blob" input.*/
    // This is the main handler of performance data blobs streaming up from
    // the collector BE's connected to this filter node. A CBTF_Protocol_Blob
    // contains an xdr encoded header and an xdr encoded data payload.
    // The header maps the data payload to a specific collector and the
    // thread it came from. There can be 1 to N connections to a filter node.
    // Each connection can stream data from any pthreads that share the connection.
    // The total number of datablobs and the size of these blobs...
    void cbtf_protocol_blob_Handler(const boost::shared_ptr<CBTF_Protocol_Blob>& in)
    {
#ifndef NDEBUG
	std::stringstream output;
	DEBUGPREFIX(Impl::TheTopologyInfo.IsFrontend,Impl::TheTopologyInfo.MaxLeafDistance);
	if (is_time_aggregator_events_enabled) {
	    if (data_blobs == 0) {
		output << Time::Now() << " " << debug_prefix.str()
		<< "AddressAggregator::cbtf_protocol_blob_Handler first data blob."
		<< std::endl;
	    }
	}
#endif

	++data_blobs;

#ifndef NDEBUG
        if (is_trace_aggregator_events_enabled) {
	    output << debug_prefix.str() <<
	    	"ENTERED AddressAggregator::cbtf_protocol_blob_Handler" << std::endl;
	}
#endif

	if (in->data.data_len == 0 ) {
	    std::cerr << "EXIT AddressAggregator::cbtf_protocol_blob_Handler: data length 0" << std::endl;
	    abort();
	}

#ifndef NDEBUG
        if (is_trace_aggregator_events_enabled) {
	    output << debug_prefix.str()
	    << "AddressAggregator::cbtf_protocol_blob_Handler: EMIT CBTF_Protocol_Blob" << std::endl;
	}
#endif
	emitOutput<boost::shared_ptr<CBTF_Protocol_Blob> >("datablob_xdr_out",in);

	Blob perfdatablob(in.get()->data.data_len, in.get()->data.data_val);

	// decode this blobs data header and create a threadname object
	// and collector id object.
        CBTF_DataHeader header;
        memset(&header, 0, sizeof(header));
        unsigned header_size = perfdatablob.getXDRDecoding(
            reinterpret_cast<xdrproc_t>(xdr_CBTF_DataHeader), &header
            );
        ThreadName threadname(header.host,header.pid,header.posix_tid,header.rank);

	// find the actual data blob after the header and create a Blob.
	// TODO: Map the incoming data size to it's thread and increment as new
	// data for same thread arrives.  Could be use to identify threads
	// that are generating more data than others. REDUCTION.
	//unsigned data_size = perfdatablob.getSize() - header_size;
	//total_data_size += data_size;
	//const void* data_ptr = &(reinterpret_cast<const char *>(perfdatablob.getContents())[header_size]);
	//Blob dblob(data_size,data_ptr);

	AddressBuffer buf;
        // CALL PERFDATA HERE
        // aggregatePCData(collectorID, dblob, buf, interval, threadname);
	// update aggregate addresses and counts. 
	//total_data_size += perfdata.aggregate(perfdatablob,buf,threadname);
	total_data_size += perfdata.aggregate(perfdatablob,buf);

#ifndef NDEBUG
        if (is_debug_aggregator_events_enabled) {
	    output << debug_prefix.str() << "Aggregating CBTF_Protocol_Blob addresses for "
	    << "data from thread " << threadname << " total data bytes: " << total_data_size
	    << std::endl;
	}
#endif
	abuffer.updateAddressCounts(buf);

	// load balance on address counts.
	updateAddrThreadCountMap(buf, addrThreadCount, threadname);

#ifndef NDEBUG
        if (is_trace_aggregator_events_enabled) {
	    output << debug_prefix.str()
	    << "AddressAggregator::cbtf_protocol_blob_Handler: EMIT Addressbuffer size:"
	    << abuffer.addresscounts.size() << std::endl;
	}
#endif

        emitOutput<AddressBuffer>("Aggregatorout",  abuffer);
        xdr_free(reinterpret_cast<xdrproc_t>(xdr_CBTF_DataHeader), reinterpret_cast<char*>(&header));

        //std::cerr << Time::Now() << " " << debug_prefix.str() << " AddressAggregator::cbtf_protocol_blob_Handler exits." << std::endl;
#ifndef NDEBUG
	if ( !output.str().empty() ) {
	    std::cerr << output.str();
	}
#endif

    }

    /** Pass Through Handler for the "CBTF_Protocol_Blob" input.*/
    void pass_cbtf_protocol_blob_Handler(const boost::shared_ptr<CBTF_Protocol_Blob>& in)
    {
#ifndef NDEBUG
	std::stringstream output;
	DEBUGPREFIX(Impl::TheTopologyInfo.IsFrontend,Impl::TheTopologyInfo.MaxLeafDistance);
#endif
	if (!is_defer_emit) {
#ifndef NDEBUG
            if (is_trace_aggregator_events_enabled) {
	    output << debug_prefix.str() <<
		"AddressAggregator::pass_cbtf_protocol_blob_Handler: EMIT CBTF_Protocol_Blob" << std::endl;
	    }
#endif
	    emitOutput<boost::shared_ptr<CBTF_Protocol_Blob> >("datablob_xdr_out",in);
	}

#ifndef NDEBUG
	if ( !output.str().empty() ) {
	    std::cerr << output.str();
	}
#endif

    }

    /** Handler for the "addressBuffer" input.
      * Aggregate Addresbuffers coming in to this node.
      */
    void addressBufferHandler(const AddressBuffer& in)
    {

	//if (is_finished) return;

#ifndef NDEBUG
	std::stringstream output;
	DEBUGPREFIX(Impl::TheTopologyInfo.IsFrontend,Impl::TheTopologyInfo.MaxLeafDistance);
	if (is_time_aggregator_events_enabled) {
	    if (handled_buffers == 0) {
		output << Time::Now() << " " << debug_prefix.str()
		<< "AddressAggregator::addressBufferHandler." << std::endl;
	    }
	}
#endif

	
#ifndef NDEBUG
        if (is_trace_aggregator_events_enabled) {
	    output << debug_prefix.str()
	    	<< "ENTERED AddressAggregator::addressBufferHandler addresscounts size:"
		<< in.addresscounts.size()  << std::endl;
	}
#endif
	AddressCounts::const_iterator aci;
	for (aci = in.addresscounts.begin(); aci != in.addresscounts.end(); ++aci) {

	    AddressCounts::iterator lb = abuffer.addresscounts.lower_bound(aci->first);
	    abuffer.updateAddressCounts(aci->first.getValue(), aci->second);
	}

        handled_buffers++;
#ifndef NDEBUG
        if (is_debug_aggregator_events_enabled) {
 	    output << debug_prefix.str() << "AddressAggregator::addressBufferHandler"
	    << " handled buffers " << handled_buffers
	    << " known threads " << threadnames.size()
	    << " is_finished " << is_finished
	    << std::endl;
	}
#endif
        if (handled_buffers == threadnames.size()) {
#ifndef NDEBUG
            if (is_debug_aggregator_events_enabled) {
 	        output << debug_prefix.str() << "AddressAggregator::addressBufferHandler "
 	        << "handled " << handled_buffers
		<< " buffers for known total threads" << threadnames.size()
 	        << std::endl;
	    }
#endif
	}

#ifndef NDEBUG
	if ( !output.str().empty() ) {
	    std::cerr << output.str();
	}
#endif

    }

    /** Handler for the "blob" input.*/
    void blobHandler(const Blob& in)
    {
	//std::cerr << "ENTERED AddressAggregator::blobHandler" << std::endl;
	// decode this blobs data header
        CBTF_DataHeader header;
        memset(&header, 0, sizeof(header));
        unsigned header_size = in.getXDRDecoding(
            reinterpret_cast<xdrproc_t>(xdr_CBTF_DataHeader), &header
            );

        ThreadName tname(header.host,header.pid,header.posix_tid,header.rank);
	std::string collectorID(header.id);
#ifndef NDEBUG
        if (is_debug_aggregator_events_enabled) {
	    std::cerr << debug_prefix.str() << " blobHandler: Aggregating Blob addresses for "
	    << collectorID << " data from thread " << tname
	    << std::endl;
	}
#endif

	// find the actual data blob after the header
	unsigned data_size = in.getSize() - header_size;
	const void* data_ptr = &(reinterpret_cast<const char *>(in.getContents())[header_size]);
	Blob dblob(data_size,data_ptr);

#if 0
	if (collectorID == "pcsamp" || collectorID == "hwc" || collectorID == "hwcsamp") {
            aggregatePCData(collectorID, dblob, abuffer, interval,tname);
	    emitOutput<uint64_t>("interval",  interval);
	} else if (collectorID == "usertime" || collectorID == "hwctime") {
            aggregateSTSampleData(collectorID, dblob, abuffer, interval,tname);
	    emitOutput<uint64_t>("interval",  interval);
        } else if (collectorID == "io" || collectorID == "iot" ||
		   collectorID == "iop" || collectorID == "mem" ||
		   collectorID == "mpi" || collectorID == "mpit" ||
		   collectorID == "mpip" || collectorID == "pthreads") {
            aggregateSTTraceData(collectorID, dblob, abuffer, tname);
	} else {
	    std::cerr << "Unknown collector data handled!" << std::endl;
	}
#endif

	//std::cerr << "AddressAggregator::blobHandler: EMIT Addressbuffer" << std::endl;
        emitOutput<AddressBuffer>("Aggregatorout",  abuffer);
    }

    void intervalHandler(const uint64_t in)
    {
        interval = in;
    }

    uint64_t interval;

    AddressBuffer abuffer;
    AddrThreadCountMap addrThreadCount;
    PerfData perfdata;
    int handled_buffers;

}; // class AddressAggregator

KRELL_INSTITUTE_CBTF_REGISTER_FACTORY_FUNCTION(AddressAggregator)
