////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2011-2015 Krell Institute. All Rights Reserved.
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

/** @file AddressAggregator component. */

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
#if 0
#include "KrellInstitute/Core/Graph.hpp"
#include "KrellInstitute/Core/PCData.hpp"
#include "KrellInstitute/Core/Path.hpp"
#include "KrellInstitute/Core/StacktraceData.hpp"
#endif
#include "KrellInstitute/Core/PerfData.hpp"
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
    void flushOutput(std::stringstream &output) {
        if ( !output.str().empty() ) {
            std::cerr << output.str();
            output.str(std::string());
            output.clear();
        }
    }

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
    int data_blobs = 0;
    int handled_buffers = 0;
    long numTerminated = 0;

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
	    output << debug_prefix.str() << "ENTERED AddressAggregator updateAddrThreadCountMap"
	    << " thread:" << tname
	    << " addresscount size:" << buf.addresscounts.size()
	    << " addrThreadCount size:" << addrThreadCount.size()
	    << std::endl;
	    flushOutput(output);
	}
#endif
	threadaddrbufmap.insert(std::make_pair(tname,buf));
	AddressCounts::const_iterator aci;

	for (aci = buf.addresscounts.begin(); aci != buf.addresscounts.end(); ++aci) {
	    AddrThreadCountMap::iterator lb = addrThreadCount.lower_bound(aci->first);
	    if(lb != addrThreadCount.end() && !(addrThreadCount.key_comp()(aci->first, lb->first))) {
		// update this count or size
		if (aci->second > lb->second.second) {
		    lb->second.first = tname;
		    lb->second.second = aci->second;
		}
	    } else {
		// new entry
		std::pair<ThreadName,uint64_t> tcount(tname,aci->second);
		addrThreadCount.insert(lb, AddrThreadCountMap::value_type(aci->first, tcount));
	    }
	}

#ifndef NDEBUG
        if (is_trace_aggregator_events_enabled) {
	    output << debug_prefix.str() << "EXIT AddressAggregator updateAddrThreadCountMap"
	    << " addrThreadCount size:" << addrThreadCount.size() << std::endl;
	    flushOutput(output);
	}
#endif
    }

    void printAddrThreadCountMap(AddrThreadCountMap& addrTM)
    {
#ifndef NDEBUG
	std::stringstream output;
	output << debug_prefix.str() << "ENTERED printAddrThreadCountMap"
	    << " addrThreadCount size:" << addrTM.size() << std::endl;

	AddrThreadCountMap::const_iterator aci;
	for (aci = addrTM.begin(); aci != addrTM.end(); ++aci) {
	    output << "Address:" << aci->first
		<< " thread:" << aci->second.first
		<< " count:" << aci->second.second
		<< std::endl;
	}
	std::cerr << output.str();
#endif
    }

    // A boost graph object that could be used for making a directed graph.
    // In this component it would be a graph at the address level. It may
    // be better served to graph at the function symbol level.
    //Graph dGraph;

    // threads finished.
    int threads_finished = 0;

    // total size of performance data seen.
    int total_data_size = 0;

    int _MaxLeafDistance = 0;
    int _NumChildren = 0;

    bool isFrontend() {
	return Impl::TheTopologyInfo.IsFrontend;
    }
    bool isLeafCP() {
	return (!Impl::TheTopologyInfo.IsFrontend && _MaxLeafDistance == 1);
    }
    bool isNonLeafCP() {
	return (!Impl::TheTopologyInfo.IsFrontend && _MaxLeafDistance > 1);
    }
    int getNumChildren() {
	 return _NumChildren;
    }
    int getMaxLeafDistance() {
	 return _MaxLeafDistance;
    }

    bool initialized_topology_info = false;

    void init_TopologyInfo() {

	if (initialized_topology_info) return;

	bool initMaxLeafDistance = false;
	bool initNumChildren = false;
	if (_MaxLeafDistance == 0 && Impl::TheTopologyInfo.MaxLeafDistance > 0) {
	    _MaxLeafDistance = Impl::TheTopologyInfo.MaxLeafDistance;
	    initMaxLeafDistance = true;
	}
	if (_NumChildren == 0 && Impl::TheTopologyInfo.NumChildren > 0) {
	    _NumChildren = Impl::TheTopologyInfo.NumChildren;
	    initNumChildren = true;
	}
	initialized_topology_info = (initMaxLeafDistance && initNumChildren);
    }
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
        declareInput<int>(
            "numBE", boost::bind(&AddressAggregator::numBEHandler, this, _1)
            );
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
	declareInput<long>(
            "numTerminatedIn", boost::bind(&AddressAggregator::numTerminatedHandler, this, _1)
            );
       declareInput<bool>(
            "finished", boost::bind(&AddressAggregator::finishedHandler, this, _1)
            );
 
        declareOutput<AddressBuffer>("Aggregatorout");
        declareOutput<ThreadAddrBufMap>("ThreadAddrBufMap");
	declareOutput<boost::shared_ptr<CBTF_Protocol_Blob> >("datablob_xdr_out");

	init_TopologyInfo();
    }

    // While this informs us of how may BE's will be connecting it is
    // used to initialize the local component knowledge of the TopologyInfo
    // members we are interested once and only once as early as possible.
    void numBEHandler(const int& in)
    {
        init_TopologyInfo();
#ifndef NDEBUG
        std::stringstream output;
        DEBUGPREFIX(Impl::TheTopologyInfo.IsFrontend,Impl::TheTopologyInfo.MaxLeafDistance);
#endif

#ifndef NDEBUG
        if (is_trace_aggregator_events_enabled) {
            output << debug_prefix.str()
            << "ENTERED AddressAggregator::numBEHandler number backends " << in
            << " numChildren:" << getNumChildren()
            << std::endl;
            flushOutput(output);
        }
#endif

#ifndef NDEBUG
        //flushOutput(output);
#endif

    }

    // Update the threadnames passed from the local threadevent component.
    void threadnamesHandler(const ThreadNameVec& in)
    {
	init_TopologyInfo();
#ifndef NDEBUG
	std::stringstream output;
	DEBUGPREFIX(Impl::TheTopologyInfo.IsFrontend,Impl::TheTopologyInfo.MaxLeafDistance);
	if (is_time_aggregator_events_enabled) {
	    if (threadnames.size() == 0) {
		output << Time::Now() << " " << debug_prefix.str()
		<< "AddressAggregator::threadnamesHandler." << std::endl;
		flushOutput(output);
	    }
	}
#endif

        threadnames = in;

#ifndef NDEBUG
        if (is_trace_aggregator_events_enabled) {
            output << debug_prefix.str()
	    << "ENTERED AddressAggregator::threadnamesHandler threadnames size:"
	    << threadnames.size()
	    << " numChildren:" << getNumChildren()
	    << std::endl;
	    flushOutput(output);
	}
#endif
    }


    // Intended for LeafCP nodes.
    // When all known threads are terminated this handler emits
    // the final addressbuffer and mapping of per thread addresses.
    void numTerminatedHandler(const long& in)
    {
	init_TopologyInfo();
#ifndef NDEBUG
	std::stringstream output;
	DEBUGPREFIX(Impl::TheTopologyInfo.IsFrontend,Impl::TheTopologyInfo.MaxLeafDistance);
	if (is_time_aggregator_events_enabled) {
	    if (numTerminated == 0) {
		output << Time::Now() << " " << debug_prefix.str()
		<< "AddressAggregator::numTerminatedHandler." << std::endl;
		flushOutput(output);
	    }
	}
#endif

	numTerminated += in;

#ifndef NDEBUG
	    if (is_trace_aggregator_events_enabled) {
		output << debug_prefix.str()
		<< "ENTERED AddressAggregator::numTerminatedHandler"
		<< " numTerminated:" << numTerminated
	        << " numChildren:" << getNumChildren()
		<< " abuffer size:" << abuffer.addresscounts.size()
		<< " handled_buffers:" << handled_buffers
		<< " threadnames:" << threadnames.size()
		<< std::endl;
	        flushOutput(output);
	    }
#endif

	if (isLeafCP() && numTerminated == threadnames.size()) {
#ifndef NDEBUG
	    if (is_trace_aggregator_events_enabled) {
		output << debug_prefix.str()
		<< "AddressAggregator::numTerminatedHandler"
		<< " EMIT abuffer size:" << abuffer.addresscounts.size()
		<< " numTerminated:" << numTerminated
		<< " handled_buffers:" << handled_buffers
		<< std::endl;
	        flushOutput(output);
	    }
#endif
	    emitOutput<AddressBuffer>("Aggregatorout",  abuffer);
#ifndef NDEBUG
	    if (is_trace_aggregator_events_enabled) {
	        output << debug_prefix.str() <<
		"AddressAggregator::numTerminatedHandler EMITS ThreadAddrBufMap size:"
		    << threadaddrbufmap.size() << std::endl;
	        flushOutput(output);
	    }
#endif
	    // This emit of the threadaddrbufmap from the leafCP is intended
	    // for the symbol resolver component. used to compute per thread
	    // counts per symbol.
            emitOutput<ThreadAddrBufMap>("ThreadAddrBufMap",threadaddrbufmap);
	}

#ifndef NDEBUG
	//flushOutput(output);
#endif

    }

    // currently a no-op in AddressAggregator.
    void finishedHandler(const bool& in)
    {
	if (isLeafCP()) {
	    return;
	}

	init_TopologyInfo();
#ifndef NDEBUG
	std::stringstream output;
	DEBUGPREFIX(Impl::TheTopologyInfo.IsFrontend,Impl::TheTopologyInfo.MaxLeafDistance);
	if (is_time_aggregator_events_enabled) {
	    output << Time::Now() << " " << debug_prefix.str()
		<< "AddressAggregator::finishedHandler entered." << std::endl;
	}
#endif
	is_finished = true;

#ifndef NDEBUG
        if (is_trace_aggregator_events_enabled) {
            output << debug_prefix.str()
	    << "ENTERED AddressAggregator::finishedHandler "
	    << " threads:" << threadnames.size()
	    << " numTerminated:" << numTerminated
	    << " is_finished:" << is_finished
	    << " numChildren:" << getNumChildren()
	    << std::endl;
	    flushOutput(output);
	}
#endif

#ifndef NDEBUG
	if (is_time_aggregator_events_enabled) {
	    output << Time::Now() << " " << debug_prefix.str()
		<< "AddressAggregator::finishedHandler finished." << std::endl;
	    flushOutput(output);
	}
#endif
    }

 
    // Handler for the "CBTF_Protocol_Blob" input. This handler unpacks the
    // incoming blobs at the leafCP nodes only. All other nodes should just
    // pass this on for now. In the future one could selectively pass on
    // datablobs based on a thread of interest basis (metric important thread).
    // This is the main handler of performance data blobs streaming up from
    // the collector BE's connected to this filter node.
    //
    // If an experiment class needs to perform other metrics they may implement
    // their own Aggregation plugin and compute any specific metrics or
    // reductions. All experiments should implement the address aggregaton
    // performed here since that creates the list of addresses used to resolve
    // symbols in addition to aggregating counts or raw time per address.
    //
    // A CBTF_Protocol_Blob contains an xdr encoded header and an xdr encoded
    // data payload. The header maps the data payload to a specific collector
    // and the thread it came from. There can be 1 to N connections to a filter
    // node. Each connection can stream data from any pthreads that share the
    // connection. The total number of datablobs and the size of these blobs...
    // Therefore, an indeterminite number of data blobs can arrive per thread.
    //
    // TODO: queue the incoming datablobs rather than just resending them as
    // they arrive.  Once all known threads are terminated and no more blobs
    // are arriving, flush the queue.
    void cbtf_protocol_blob_Handler(const boost::shared_ptr<CBTF_Protocol_Blob>& in)
    {
	init_TopologyInfo();
	++data_blobs;
#ifndef NDEBUG
	std::stringstream output;
	DEBUGPREFIX(Impl::TheTopologyInfo.IsFrontend,Impl::TheTopologyInfo.MaxLeafDistance);
	if (is_time_aggregator_events_enabled) {
	    if (data_blobs == 0) {
		std::cerr << Time::Now() << " " << debug_prefix.str()
		<< "AddressAggregator::cbtf_protocol_blob_Handler data_blob." << data_blobs
		<< std::endl;
	    }
	}
#endif


#ifndef NDEBUG
        if (is_trace_aggregator_events_enabled) {
	    output << debug_prefix.str()
	    	<< "ENTERED AddressAggregator::cbtf_protocol_blob_Handler"
	        << " numChildren:" << getNumChildren()
		<< std::endl;
	    flushOutput(output);
	}
#endif

	// FIXME: Should we abort or just return here?
	if (in->data.data_len == 0 ) {
	    std::cerr << "EXIT AddressAggregator::cbtf_protocol_blob_Handler data length 0" << std::endl;
	    abort();
	}

	// Allow all levels to re-emit the data blob from this handler.
	if (isLeafCP()) {
#ifndef NDEBUG
	    if (is_trace_aggregator_events_enabled) {
		output << debug_prefix.str() << "AddressAggregator::cbtf_protocol_blob_Handler"
		    << "  EMITS CBTF_Protocol_Blob" << std::endl;
	        flushOutput(output);
	    }
#endif
	    emitOutput<boost::shared_ptr<CBTF_Protocol_Blob> >("datablob_xdr_out",in);
	} else if (!isLeafCP()) {
#ifndef NDEBUG
	    if (is_trace_aggregator_events_enabled) {
	        output << debug_prefix.str()
	        << "AddressAggregator::cbtf_protocol_blob_Handler PASS ON DATABLOB" << std::endl;
	        flushOutput(output);
	    }
	    emitOutput<boost::shared_ptr<CBTF_Protocol_Blob> >("datablob_xdr_out",in);
#endif
	    return;
	} else {
	}

	// From this point on only leafCP nodes decode and handle
	// the passed in performance data blobs.

	Blob perfdatablob(in.get()->data.data_len, in.get()->data.data_val);

	// decode this blobs data header and create a threadname object
	// and collector id object.
        CBTF_DataHeader header;
        memset(&header, 0, sizeof(header));
        unsigned header_size = perfdatablob.getXDRDecoding(
            reinterpret_cast<xdrproc_t>(xdr_CBTF_DataHeader), &header
            );
        ThreadName threadname(header.host,header.pid,header.posix_tid,header.rank,header.omp_tid);

	// find the actual data blob after the header and create a Blob.
	// TODO: Map the incoming data size to it's thread and increment as new
	// data for same thread arrives.  Could be use to identify threads
	// that are generating more data than others. REDUCTION.
	unsigned data_size = perfdatablob.getSize() - header_size;
	total_data_size += data_size;

	AddressBuffer buf;
	// update aggregate addresses and counts.
	total_data_size += perfdata.aggregate(perfdatablob,buf);

#ifndef NDEBUG
        if (is_debug_aggregator_events_enabled) {
	    output << debug_prefix.str()
	    << "AddressAggregator::cbtf_protocol_blob_Handler Aggregating blob"
	    << " addresses for data from thread:" << threadname
	    << " total data bytes: " << total_data_size
	    << std::endl;
	    flushOutput(output);
	}
#endif
	abuffer.updateAddressCounts(buf);

	// load balance on address counts or raw time.
	updateAddrThreadCountMap(buf, addrThreadCount, threadname);

        xdr_free(reinterpret_cast<xdrproc_t>(xdr_CBTF_DataHeader), reinterpret_cast<char*>(&header));


    }


    /** Pass Through Handler for the "CBTF_Protocol_Blob" input.*/
    // TODO: look to remove this due to new handling of these blobs.
    void pass_cbtf_protocol_blob_Handler(const boost::shared_ptr<CBTF_Protocol_Blob>& in)
    {
	init_TopologyInfo();
#ifndef NDEBUG
	std::stringstream output;
	DEBUGPREFIX(Impl::TheTopologyInfo.IsFrontend,Impl::TheTopologyInfo.MaxLeafDistance);
#endif
	if (!is_defer_emit) {
#ifndef NDEBUG
            if (is_trace_aggregator_events_enabled) {
		output << debug_prefix.str() <<
		"AddressAggregator::pass_cbtf_protocol_blob_Handler EMIT CBTF_Protocol_Blob" << std::endl;
		flushOutput(output);
	    }
#endif
	    emitOutput<boost::shared_ptr<CBTF_Protocol_Blob> >("datablob_xdr_out",in);
	}

#ifndef NDEBUG
	//flushOutput(output);
#endif

    }


    /** Handler for the "addressBuffer" input.
      * This code can only execute on the FE or Non LeafCP nodes
      * since the Leaf CPs create the buffers from the datablobs
      * as they arrive. Any CP's (or FE) connected to the leaf CP
      * nodes can receive an undetermined number of buffers so
      * the number of handled buffers can and will vary depending
      * on the nature of the data collected by the ltwt BE's.
      * But the initial CP that handles buffers from the leaf CP
      * nodes will onlt emit one merged buffer. So the condition
      * to emit the final buffers at this point must be based on
      * the condition that all known threads have also terminated.
      *
      * Aggregate Addresbuffers coming in to this node.
      * If this is the an intermediate or top level CP, then
      * we expect the number of buffers to be equal to the
      * number of children of this CP.
      * 
      * Since this handles merged buffers, it needs to emit the
      * final buffer after all expected buffers arrive.
      */
    void addressBufferHandler(const AddressBuffer& in)
    {
	init_TopologyInfo();
        ++handled_buffers;

#ifndef NDEBUG
	std::stringstream output;
	DEBUGPREFIX(Impl::TheTopologyInfo.IsFrontend,Impl::TheTopologyInfo.MaxLeafDistance);
	if (is_time_aggregator_events_enabled) {
	    if (handled_buffers == 0) {
		output << Time::Now() << " " << debug_prefix.str()
		<< "AddressAggregator::addressBufferHandler handled_buffers:" << handled_buffers << std::endl;
		flushOutput(output);
	    }
	}
#endif

	
#ifndef NDEBUG
        if (is_trace_aggregator_events_enabled) {
	    output << debug_prefix.str()
	    	<< "ENTERED AddressAggregator::addressBufferHandler"
		<< " addresscounts size:" << in.addresscounts.size()
		<< " handled_buffers:" << handled_buffers
	        << " known threads:" << threadnames.size()
	        << " numChildren:" << getNumChildren()
		<< std::endl;
	    flushOutput(output);
	}
#endif
	AddressCounts::const_iterator aci;
	for (aci = in.addresscounts.begin(); aci != in.addresscounts.end(); ++aci) {

	    AddressCounts::iterator lb = abuffer.addresscounts.lower_bound(aci->first);
	    abuffer.updateAddressCounts(aci->first.getValue(), aci->second);
	}


#ifndef NDEBUG
        if (is_debug_aggregator_events_enabled) {
 	    output << debug_prefix.str() << "AddressAggregator::addressBufferHandler"
	    << " handled buffers:" << handled_buffers
	    << " known threads:" << threadnames.size()
	    << " merged buffer size:" << abuffer.addresscounts.size()
	    << " is_finished:" << is_finished
	    << std::endl;
	}
#endif

	// multi level cp trees.
	// The condition to emit the final buffer could be when
	// handled_buffers == number of children of this node.
	// Note that multiple data blobs may have contributed to the
	// addressbuffers passed in.
        if (handled_buffers == getNumChildren()) {
#ifndef NDEBUG
            if (is_debug_aggregator_events_enabled) {
 	        output << debug_prefix.str() << "AddressAggregator::addressBufferHandler"
 	        << " handled:" << handled_buffers
 	        << " expect:" << getNumChildren()
		<< " buffers known threads:" << threadnames.size()
 	        << std::endl;
	    }
#endif
	}

	// multi level cp trees. emit merged addressbuffer here...
	// The condition to emit the final buffer could be when
	// handled_buffers == number of children at this node.
	// Allow this emit only on the frontend or intermediate
	// cp nodes.
        if ( (isFrontend() || isNonLeafCP()) && handled_buffers == getNumChildren()) {

#ifndef NDEBUG

	    if (is_trace_aggregator_events_enabled) {
		output << debug_prefix.str()
	    	<< "AddressAggregator::addressBufferHandler EMITS AddressBuffer size:"
		<< abuffer.addresscounts.size()
		<< std::endl;
	        flushOutput(output);
	    }
#endif
	    emitOutput<AddressBuffer>("Aggregatorout",  abuffer);
	}

#ifndef NDEBUG
	//flushOutput(output);
#endif

    }


    /** Handler for the "blob" input.*/
    void blobHandler(const Blob& in)
    {
	init_TopologyInfo();
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
	    std::cerr << debug_prefix.str()
	    << "blobHandler: Aggregating Blob addresses for " << collectorID
	    << " data from thread " << tname
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

}; // class AddressAggregator

KRELL_INSTITUTE_CBTF_REGISTER_FACTORY_FUNCTION(AddressAggregator)
