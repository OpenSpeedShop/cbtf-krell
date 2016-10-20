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

/** @file MemAggregator component. */

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
#include <rpc/rpc.h>

#include <KrellInstitute/CBTF/Component.hpp>
#include <KrellInstitute/CBTF/Type.hpp>
#include <KrellInstitute/CBTF/Version.hpp>
#include <KrellInstitute/CBTF/Impl/MRNet.hpp>

#include "KrellInstitute/Core/Address.hpp"
#include "KrellInstitute/Core/AddressBuffer.hpp"
#include "KrellInstitute/Core/AddressRange.hpp"
#include "KrellInstitute/Core/Blob.hpp"
#include "KrellInstitute/Core/PerfData.hpp"
#include "KrellInstitute/Core/Time.hpp"
#include "KrellInstitute/Core/TimeInterval.hpp"
#include "KrellInstitute/Core/ThreadName.hpp"
#include "KrellInstitute/Messages/Blob.h"
#include "KrellInstitute/Messages/DataHeader.h"
#include "KrellInstitute/Messages/Address.h"
#include "KrellInstitute/Messages/ThreadEvents.h"
#include "KrellInstitute/Messages/PerformanceData.hpp"
#include "KrellInstitute/Services/Common.h"

using namespace KrellInstitute::CBTF;
using namespace KrellInstitute::Core;

typedef std::map<Address, std::pair<ThreadName,uint64_t> > AddrThreadCountMap;
typedef std::map<ThreadName,AddressBuffer>  ThreadAddrBufMap;
typedef std::map<ThreadName,AddressCounts>  ThreadAddrCountsMap;
typedef std::map<ThreadName,MemMetrics>  ThreadMemMetricsMap;

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
    int data_blobs_size = 0;
    int handled_buffers = 0;
    long numTerminated = 0;

    // unique addresses seen
    AddressBuffer abuffer;
    // mapping address to count within a thread.
    AddrThreadCountMap addrThreadCount;
    // vector of incoming threadnames. For each thread we expect
    ThreadNameVec threadnames;
    // map thread to address buffer.
    ThreadAddrBufMap threadaddrbufmap;
    // map thread to mem metrics 
    ThreadMemMetricsMap threadmemmetricsmap;
    // class that handles computing address buffer and any additional
    // metrics for a specific experiment.
    PerfData perfdata;
    // The mem metrics specific to this experiment.
    MemMetrics memMetrics;

    
    #define StackTraceBufferSize (CBTF_BlobSizeFactor * 384)
    #define EventBufferSize (CBTF_BlobSizeFactor * 200)
    #define MaxFramesPerStackTrace 48

    // helper to map address counts to threads.
    bool updateAddrThreadCountMap(AddressBuffer& buf,
				   AddrThreadCountMap& addrThreadCount,
				   ThreadName& tname)
    {
#ifndef NDEBUG
	std::stringstream output;
        if (is_trace_aggregator_events_enabled) {
	    output << debug_prefix.str() << "ENTERED MemAggregator updateAddrThreadCountMap"
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
	    output << debug_prefix.str() << "EXIT MemAggregator updateAddrThreadCountMap"
	    << " addrThreadCount size:" << addrThreadCount.size() << std::endl;
	    flushOutput(output);
	}
#endif
    }

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
 * Component aggregates address values and their counts.
 * Performs additional metrics used for the mem experiment.
 */
class __attribute__ ((visibility ("hidden"))) MemAggregator :
    public Component
{

public:

    /** Factory function for this component type. */
    static Component::Instance factoryFunction()
    {
        return Component::Instance(
            reinterpret_cast<Component*>(new MemAggregator())
            );
    }

private:


    uint64_t stacktraces[StackTraceBufferSize];
    CBTF_memt_event events[EventBufferSize];
 
    bool update_data(const MemEvent& event,CBTF_DataHeader& data_header, CBTF_mem_exttrace_data& data);
    void initialize_data(const ThreadName& tname, CBTF_DataHeader& data_header, CBTF_mem_exttrace_data& data);

    /** Default constructor. */
    MemAggregator() :
        Component(Type(typeid(MemAggregator)), Version(0, 0, 1))
    {
        declareInput<int>(
            "numBE", boost::bind(&MemAggregator::numBEHandler, this, _1)
            );
        declareInput<AddressBuffer>(
            "addressBuffer", boost::bind(&MemAggregator::addressBufferHandler, this, _1)
            );
        declareInput<boost::shared_ptr<CBTF_Protocol_Blob> >(
            "cbtf_protocol_blob",
            boost::bind(
                &MemAggregator::cbtf_protocol_blob_Handler, this, _1
                )
            );
	declareInput<ThreadNameVec>(
            "threadnames", boost::bind(&MemAggregator::threadnamesHandler, this, _1)
            );
	declareInput<long>(
            "numTerminatedIn", boost::bind(&MemAggregator::numTerminatedHandler, this, _1)
            );
       declareInput<bool>(
            "finished", boost::bind(&MemAggregator::finishedHandler, this, _1)
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
            << "ENTERED MemAggregator::numBEHandler number backends " << in
            << " numChildren:" << getNumChildren()
            << std::endl;
            flushOutput(output);
        }
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
		<< "MemAggregator::threadnamesHandler." << std::endl;
		flushOutput(output);
	    }
	}
#endif

        threadnames = in;

#ifndef NDEBUG
        if (is_trace_aggregator_events_enabled) {
            output << debug_prefix.str()
	    << "ENTERED MemAggregator::threadnamesHandler threadnames size:"
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
		<< "MemAggregator::numTerminatedHandler." << std::endl;
		flushOutput(output);
	    }
	}
#endif

	numTerminated += in;

#ifndef NDEBUG
	    if (is_trace_aggregator_events_enabled) {
		output << debug_prefix.str()
		<< "ENTERED MemAggregator::numTerminatedHandler"
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
	    // Handle mem specific metrics here.
	    for (ThreadMemMetricsMap::iterator it = threadmemmetricsmap.begin();
		 it != threadmemmetricsmap.end(); ++it) {

		unsigned int stillAllocatedCount = 0;
		int id = it->first.getMPIRank() >= 0 ? it->first.getMPIRank() : it->first.getPid();
		//std::cerr << "Memory stats for thread " << id << ":" << it->first.getOmpTid() << std::endl;

		if (it->second.allocationSizes.size() > 0) {
		    //std::cerr << "\tAddresses still allocated:" << std::endl;
		    for (AddressCounts::const_iterator aci = it->second.allocationSizes.begin();
			 aci != it->second.allocationSizes.end(); ++aci) {
			if (aci->second != 0) {
			    ++stillAllocatedCount;
			    //std::cerr << "\taddress:" << aci->first << " size:" << aci->second << std::endl;
			}
		    }
		}

		std::pair<boost::shared_ptr<CBTF_DataHeader>,
			  boost::shared_ptr<CBTF_mem_exttrace_data> >
			   pack_message(
				boost::shared_ptr<CBTF_DataHeader>(new CBTF_DataHeader()),
				boost::shared_ptr<CBTF_mem_exttrace_data>(new CBTF_mem_exttrace_data())
				);

		// dm_mem_type
		int allocation_type_count = 0;
		int free_type_count = 0;
		// dm_reason
		int reason_highwater_count = 0;
		int reason_callstack_count = 0;
		int reason_stillallocated_count = 0;
		int reason_other_count = 0;
		
		bool emit_new_data = false;
		CBTF_DataHeader& data_header = *pack_message.first;
		CBTF_mem_exttrace_data& data = *pack_message.second;
		initialize_data((*it).first,data_header,data);
		for (MemEventVec::const_iterator mei = it->second.eventsOfInterest.begin();
		     mei != it->second.eventsOfInterest.end(); ++mei) {

		    // Update reduced data blob.
		    emit_new_data = update_data((*mei),data_header,data);

		    // If the blob is full, emit it.
		    if (emit_new_data) {

			data.stacktraces.stacktraces_val =
			    reinterpret_cast<CBTF_Protocol_Address*>(
			    malloc(std::max(1U, data.stacktraces.stacktraces_len)
						* sizeof(CBTF_Protocol_Address))
			    );
			memcpy(data.stacktraces.stacktraces_val, &stacktraces[0],
				data.stacktraces.stacktraces_len * sizeof(CBTF_Protocol_Address));

			data.events.events_val =
			    reinterpret_cast<CBTF_memt_event*>(
			    malloc(std::max(1U, data.events.events_len)
						* sizeof(CBTF_memt_event))
			    );
			memcpy(data.events.events_val, &events[0],
				data.events.events_len * sizeof(CBTF_memt_event));

			// emit a new blob of reduced data;
#ifndef NDEBUG
			if (is_trace_aggregator_events_enabled) {
			    std::cerr << "EMITTING new data blob on datablob_xdr_out"
			    << " data.stacktraces.stacktraces_len:" << data.stacktraces.stacktraces_len
			    << " data.events.events_len:" << data.events.events_len
			    << std::endl;
			}
#endif
			emitOutput<boost::shared_ptr<CBTF_Protocol_Blob> >( "datablob_xdr_out",
				KrellInstitute::Messages::pack<CBTF_mem_exttrace_data>(
				pack_message, reinterpret_cast<xdrproc_t>(xdr_CBTF_mem_exttrace_data)
				)
			);

			// re-initialize data structure for new data blob.
			initialize_data((*it).first,data_header,data);
		    }

		    switch ((*mei).dm_mem_type) {
			case CBTF_MEM_MALLOC: {
			    ++allocation_type_count;
			    break;
			}
			case CBTF_MEM_REALLOC: {
			    ++allocation_type_count;
			    break;
			}
			case CBTF_MEM_FREE: {
			    ++free_type_count;
			    break;
			}
			default: {
			}
		    }

		    switch ((*mei).dm_reason) {
			case CBTF_MEM_REASON_UNIQUE_CALLPATH: {
			    ++reason_callstack_count;
			    break;
			}
			case CBTF_MEM_REASON_HIGHWATER_SET: {
			    ++reason_highwater_count;
			    break;
			}
			default: {
			    ++reason_other_count;
			}
		    }
		}

		for (StackMemEventMap::const_iterator sei = it->second.stackMemEvents.begin();
		     sei != it->second.stackMemEvents.end(); ++sei) {

		    ++reason_callstack_count;
		    // Update reduced data blob.
		    emit_new_data = update_data((*sei).second,data_header,data);

		    // If the blob is full, emit it.
		    if (emit_new_data) {

			data.stacktraces.stacktraces_val =
			    reinterpret_cast<CBTF_Protocol_Address*>(
			    malloc(std::max(1U, data.stacktraces.stacktraces_len)
						* sizeof(CBTF_Protocol_Address))
			    );
			memcpy(data.stacktraces.stacktraces_val, &stacktraces[0],
				data.stacktraces.stacktraces_len * sizeof(CBTF_Protocol_Address));

			data.events.events_val =
			    reinterpret_cast<CBTF_memt_event*>(
			    malloc(std::max(1U, data.events.events_len)
						* sizeof(CBTF_memt_event))
			    );
			memcpy(data.events.events_val, &events[0],
				data.events.events_len * sizeof(CBTF_memt_event));

			// emit a new blob of reduced data;
#ifndef NDEBUG
			if (is_trace_aggregator_events_enabled) {
			    std::cerr << "EMITTING new data blob on datablob_xdr_out"
			    << " data.stacktraces.stacktraces_len:" << data.stacktraces.stacktraces_len
			    << " data.events.events_len:" << data.events.events_len
			    << std::endl;
			}
#endif
			emitOutput<boost::shared_ptr<CBTF_Protocol_Blob> >( "datablob_xdr_out",
				KrellInstitute::Messages::pack<CBTF_mem_exttrace_data>(
				pack_message, reinterpret_cast<xdrproc_t>(xdr_CBTF_mem_exttrace_data)
				)
			);

			// re-initialize data structure for new data blob.
			initialize_data((*it).first,data_header,data);
		    }
		}

		// Now add in any allocation events that were not freed.
		for (AddressMemEventMap::const_iterator aei = it->second.addrMemEvent.begin();
		     aei != it->second.addrMemEvent.end(); ++aei) {

		    ++reason_stillallocated_count;
		    // Update reduced data blob.
		    emit_new_data = update_data((*aei).second,data_header,data);

		    // If the blob is full, emit it.
		    if (emit_new_data) {

			data.stacktraces.stacktraces_val =
			    reinterpret_cast<CBTF_Protocol_Address*>(
			    malloc(std::max(1U, data.stacktraces.stacktraces_len)
						* sizeof(CBTF_Protocol_Address))
			    );
			memcpy(data.stacktraces.stacktraces_val, &stacktraces[0],
				data.stacktraces.stacktraces_len * sizeof(CBTF_Protocol_Address));

			data.events.events_val =
			    reinterpret_cast<CBTF_memt_event*>(
			    malloc(std::max(1U, data.events.events_len)
						* sizeof(CBTF_memt_event))
			    );
			memcpy(data.events.events_val, &events[0],
				data.events.events_len * sizeof(CBTF_memt_event));

			// emit a new blob of reduced data;
#ifndef NDEBUG
			if (is_trace_aggregator_events_enabled) {
			    std::cerr << "EMITTING new data blob on datablob_xdr_out"
			    << " data.stacktraces.stacktraces_len:" << data.stacktraces.stacktraces_len
			    << " data.events.events_len:" << data.events.events_len
			    << std::endl;
			}
#endif
			emitOutput<boost::shared_ptr<CBTF_Protocol_Blob> >( "datablob_xdr_out",
				KrellInstitute::Messages::pack<CBTF_mem_exttrace_data>(
				pack_message, reinterpret_cast<xdrproc_t>(xdr_CBTF_mem_exttrace_data)
				)
			);

			// re-initialize data structure for new data blob.
			initialize_data((*it).first,data_header,data);
		    }
		}

		int total_stack_size = 0;
		int total_stack_counts = 0;
		for (StackCountsMap::const_iterator sci = it->second.stackCounts.begin();
		     sci != it->second.stackCounts.end(); ++sci) {
		    total_stack_size += sci->first.size();
		    total_stack_counts += sci->second;
		}

#ifndef NDEBUG
		if (is_trace_aggregator_events_enabled) {
		std::cerr << "Memory stats for thread " << id << ":" << it->first.getOmpTid() << std::endl;
		std::cerr << "\tmemory allocation highwater:" << it->second.highwater 
			  << " final:" << it->second.currentAllocation << std::endl;
		std::cerr << "\ttotal allocation calls:" << it->second.totalAllocations << std::endl;
		std::cerr << "\ttotal free calls:" << it->second.totalFrees << std::endl;
		std::cerr << "\tunique memory address allocations:" << it->second.allocationSizes.size() << std::endl;
		std::cerr << "\tmemory still allocated events:" << it->second.addrMemEvent.size() << std::endl;
		std::cerr << "\tunique callstack events:" <<  it->second.stackMemEvents.size() << std::endl;
		std::cerr << "\tinteresting events:"
			  << it->second.eventsOfInterest.size() + it->second.addrMemEvent.size() << std::endl;
		std::cerr << "\treason unique callstack:" << reason_callstack_count << std::endl;
		std::cerr << "\treason highwater:" << reason_highwater_count << std::endl;
		std::cerr << "\treason still allocated:" << reason_stillallocated_count << std::endl;
	        std::cerr << "\ttotal size of datablobs:" << data_blobs_size << std::endl;
		std::cerr << std::endl;
		}
#endif


		// If there are events not sent, emit the final datablob.
		if (!emit_new_data && data.events.events_len > 0) {
		    data.stacktraces.stacktraces_val =
			reinterpret_cast<CBTF_Protocol_Address*>(
			    malloc(std::max(1U, data.stacktraces.stacktraces_len)
					    * sizeof(CBTF_Protocol_Address))
			);
		    memcpy(data.stacktraces.stacktraces_val, &stacktraces[0],
			data.stacktraces.stacktraces_len * sizeof(CBTF_Protocol_Address));
		    data.events.events_val =
			reinterpret_cast<CBTF_memt_event*>(
			    malloc(std::max(1U, data.events.events_len)
					    * sizeof(CBTF_memt_event))
			);
		    memcpy(data.events.events_val, &events[0],
			data.events.events_len * sizeof(CBTF_memt_event));
#ifndef NDEBUG
		    if (is_trace_aggregator_events_enabled) {
			std::cerr << "EMITTING final data blob on datablob_xdr_out"
			<< " data.stacktraces.stacktraces_len:" << data.stacktraces.stacktraces_len
			<< " data.events.events_len:" << data.events.events_len
			<< std::endl;
		    }
#endif
		    emitOutput<boost::shared_ptr<CBTF_Protocol_Blob> >( "datablob_xdr_out",
			KrellInstitute::Messages::pack<CBTF_mem_exttrace_data>(
			pack_message, reinterpret_cast<xdrproc_t>(xdr_CBTF_mem_exttrace_data))
			);
		}
	    }

	    //std::cerr << "\ttotal size of all datablobs:" << data_blobs_size << std::endl;
	    //std::cerr << std::endl;

#ifndef NDEBUG
	    if (is_trace_aggregator_events_enabled) {
		output << debug_prefix.str()
		<< "MemAggregator::numTerminatedHandler"
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
		"MemAggregator::numTerminatedHandler EMITS ThreadAddrBufMap size:"
		    << threadaddrbufmap.size() << std::endl;
	        flushOutput(output);
	    }
#endif
	    // This emit of the threadaddrbufmap from the leafCP is intended
	    // for the symbol resolver component. used to compute per thread
	    // counts per symbol.
            emitOutput<ThreadAddrBufMap>("ThreadAddrBufMap",threadaddrbufmap);
	}
    }

    // currently a no-op in MemAggregator.
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
		<< "MemAggregator::finishedHandler entered." << std::endl;
	}
#endif
	is_finished = true;

#ifndef NDEBUG
        if (is_trace_aggregator_events_enabled) {
            output << debug_prefix.str()
	    << "ENTERED MemAggregator::finishedHandler "
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
		<< "MemAggregator::finishedHandler finished." << std::endl;
	    flushOutput(output);
	}
#endif
    }

 
    // Handler for the "CBTF_Protocol_Blob" input. This handler unpacks the
    // incoming blobs at the leafCP nodes only.
    // All other nodes will pass on any reduced perfdata blobs emitted
    // from the leafCP nodes.
    //
    // This is the main handler of mem performance data blobs streaming up from
    // the collector BE's connected to this filter node. A CBTF_Protocol_Blob
    // contains an xdr encoded header and an xdr encoded data payload.
    // The header maps the data payload to a specific collector and the
    // thread it came from. There can be 1 to N connections to a filter node.
    // Each connection can stream data from any pthreads that share the connection.
    // The total number of datablobs and the size of these blobs...
    // Therefore, an indeterminite number of data blobs can arrive per thread.
    //
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
		<< "MemAggregator::cbtf_protocol_blob_Handler data_blob." << data_blobs
		<< std::endl;
	    }
	}
#endif


#ifndef NDEBUG
        if (is_trace_aggregator_events_enabled) {
	    output << debug_prefix.str()
	    	<< "ENTERED MemAggregator::cbtf_protocol_blob_Handler"
	        << " numChildren:" << getNumChildren()
		<< std::endl;
	    flushOutput(output);
	}
#endif

	// FIXME: Should we abort or just return here?
	if (in->data.data_len == 0 ) {
	    std::cerr << "EXIT MemAggregator::cbtf_protocol_blob_Handler data length 0" << std::endl;
	    abort();
	}

	// Only reduced blobs will be emitted from leafCP rather than the original
	// blobs sent by the collector runtimes.
	if (!isLeafCP()) {
#ifndef NDEBUG
	    if (is_trace_aggregator_events_enabled) {
	        output << debug_prefix.str()
	        << "MemAggregator::cbtf_protocol_blob_Handler pass Incoming datablob" << std::endl;
	        flushOutput(output);
	    }
	    emitOutput<boost::shared_ptr<CBTF_Protocol_Blob> >("datablob_xdr_out",in);
#endif
	    return;
	}

	// From this point on only leafCP nodes should decode and handle
	// the passed in performance data blobs from lightweight backends.

	Blob perfdatablob(in.get()->data.data_len, in.get()->data.data_val);

	// decode this blobs data header and create a threadname object
	// and collector id object.
        CBTF_DataHeader header;
        memset(&header, 0, sizeof(header));
        unsigned header_size = perfdatablob.getXDRDecoding(
            reinterpret_cast<xdrproc_t>(xdr_CBTF_DataHeader), &header
            );
	std::string collectorID(header.id);
        ThreadName threadname(header.host,header.pid,header.posix_tid,
			      header.rank,header.omp_tid);


	// This is the ideal place to do additional metrics for collectors
	// that record detailed events.

	AddressBuffer buf;
	if (collectorID == "mem" ) {
	    total_data_size += perfdata.aggregate(perfdatablob,buf);

	    ThreadMemMetricsMap::iterator it = threadmemmetricsmap.find(threadname);
	    if (it == threadmemmetricsmap.end()) {
		MemMetrics M;
		M.highwater = 0;
		M.currentAllocation = 0;
		std::pair<ThreadMemMetricsMap::iterator, bool> tmp =
			threadmemmetricsmap.insert(std::make_pair(threadname,M));
		it = tmp.first;
	    }

#ifndef NDEBUG
	    if (is_debug_aggregator_events_enabled) {
		output << debug_prefix.str()
		<< "MemAggregator::cbtf_protocol_blob_Handler Aggregating"
		<< " addresses and determining reduced events for thread:" << threadname
		<< " total data bytes: " << total_data_size
		<< std::endl;
		flushOutput(output);
	    }
#endif
	    data_blobs_size += perfdata.memMetrics(perfdatablob,it->second);

	    abuffer.updateAddressCounts(buf);
	    updateAddrThreadCountMap(buf, addrThreadCount, threadname);
	}


        xdr_free(reinterpret_cast<xdrproc_t>(xdr_CBTF_DataHeader), reinterpret_cast<char*>(&header));
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
		<< "MemAggregator::addressBufferHandler handled_buffers:" << handled_buffers << std::endl;
		flushOutput(output);
	    }
	}
#endif

	
#ifndef NDEBUG
        if (is_trace_aggregator_events_enabled) {
	    output << debug_prefix.str()
	    	<< "ENTERED MemAggregator::addressBufferHandler"
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
 	    output << debug_prefix.str() << "MemAggregator::addressBufferHandler"
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
 	        output << debug_prefix.str() << "MemAggregator::addressBufferHandler"
 	        << " handled:" << handled_buffers
 	        << " expect:" << getNumChildren()
		<< " buffers known threads:" << threadnames.size()
 	        << std::endl;
		flushOutput(output);
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
	    	<< "MemAggregator::addressBufferHandler EMITS AddressBuffer size:"
		<< abuffer.addresscounts.size()
		<< std::endl;
	        flushOutput(output);
	    }
#endif
	    emitOutput<AddressBuffer>("Aggregatorout",  abuffer);
	}
    }
}; // class MemAggregator

// initialize a CBTF_mem_exttrace_data blob.
void MemAggregator::initialize_data(const ThreadName& tname,
				    CBTF_DataHeader& data_header,
				    CBTF_mem_exttrace_data& data)
{
	
    data_header.experiment = 0;  /* offline always 0 */
    data_header.collector = 1;  /* offline always 1 */
    data_header.id = strdup("mem");
    strcpy(data_header.host,tname.getHost().c_str());
    data_header.pid = tname.getPid();
    data_header.posix_tid = tname.getPosixThreadId().second;
    data_header.rank = tname.getMPIRank();
    data_header.omp_tid = tname.getOmpTid();
    // These will be filled in by the recorded events...
    data_header.time_begin = Time::TheEnd().getValue();
    data_header.time_end = Time::TheBeginning().getValue();
    data_header.addr_begin = ~0;
    data_header.addr_end = 0;

    // Re-initialize the actual data blob
    data.stacktraces.stacktraces_len = 0;
    data.stacktraces.stacktraces_val = stacktraces;
    data.events.events_len = 0;
    data.events.events_val = events;

    // Re-initialize the stacktraces and events
    memset(stacktraces, 0, sizeof(stacktraces));
    memset(events, 0, sizeof(events));
}

// Convert the pass MemEvent opject into a CBTF_memt_event and
// add it to the passed CBTF_mem_exttrace_data blob.
bool MemAggregator::update_data(const MemEvent& event,
				CBTF_DataHeader& data_header,
				CBTF_mem_exttrace_data& data)
{

    bool retval = false;
    CBTF_memt_event ev;
    ev.mem_type = event.dm_mem_type;
    ev.reason = event.dm_reason;
    ev.start_time = event.dm_start_time;
    ev.stop_time = event.dm_stop_time;
    ev.retval = event.dm_retval;
    ev.ptr = event.dm_ptr;
    ev.size1 = event.dm_size1;
    ev.size2 = event.dm_size2;
    ev.total_allocation = event.dm_total_allocation;
    ev.count = event.dm_count;
    ev.max = event.dm_max;
    ev.min = event.dm_min;
    ev.stacktrace = 0;

    // update this data blobs time interval in data header.
    if (event.dm_start_time < data_header.time_begin) {
	data_header.time_begin = event.dm_start_time;
    }
    if (event.dm_stop_time >= data_header.time_end) {
	data_header.time_end = event.dm_stop_time + 1;
    }

    // handle event.dm_stacktrace.  place into stacktraces buffer.
    // update ev.stacktrace with index in stacktraces buffer.
    unsigned int entry = 0,start,i;
    for(start = 0, i = 0; (i < event.dm_stacktrace.size() ) &&
	((start + i) < data.stacktraces.stacktraces_len); ++i) {
	uint64_t a = event.dm_stacktrace[i].getValue();
	if(event.dm_stacktrace[i].getValue() != stacktraces[start + i]) {
	    for(start += i; (stacktraces[start] != 0) &&
                (start < data.stacktraces.stacktraces_len); ++start);
	    ++start;
	    i = 0;
	}
    }

    if(i == event.dm_stacktrace.size()) {
	entry = start;
    } else {
	if((data.stacktraces.stacktraces_len + event.dm_stacktrace.size() + 1) >= StackTraceBufferSize) {
	    retval = true;
	}
	entry = data.stacktraces.stacktraces_len;
        for(i = 0; i < event.dm_stacktrace.size(); ++i) {
	    stacktraces[entry + i] = event.dm_stacktrace[i].getValue();
	    // update this data blobs addr interval in data header with
	    // highest and lowest addresses seen.
	    if(event.dm_stacktrace[i].getValue() < data_header.addr_begin)
		data_header.addr_begin = event.dm_stacktrace[i].getValue();
	    if(event.dm_stacktrace[i].getValue() > data_header.addr_end)
		data_header.addr_end = event.dm_stacktrace[i].getValue();
	}
	stacktraces[entry + event.dm_stacktrace.size()] = 0;
	data.stacktraces.stacktraces_len += (event.dm_stacktrace.size() + 1);
    }

    // add event to events buffer.
    memcpy(&events[data.events.events_len], &ev, sizeof(CBTF_memt_event));
    events[data.events.events_len].stacktrace = entry;
    data.events.events_len++;
    if(data.events.events_len == EventBufferSize) {
	retval = true;
    }

    return retval;
}

KRELL_INSTITUTE_CBTF_REGISTER_FACTORY_FUNCTION(MemAggregator)
