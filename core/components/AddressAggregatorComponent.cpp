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
#include "KrellInstitute/Core/Graph.hpp"
#include "KrellInstitute/Core/PCData.hpp"
#include "KrellInstitute/Core/Path.hpp"
#include "KrellInstitute/Core/StacktraceData.hpp"
#include "KrellInstitute/Core/Time.hpp"
#include "KrellInstitute/Core/TimeInterval.hpp"
#include "KrellInstitute/Core/ThreadName.hpp"
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

using namespace KrellInstitute::CBTF;
using namespace KrellInstitute::Core;

typedef std::map<Address, std::pair<ThreadName,uint64_t> > AddrThreadCountMap;
typedef std::map<ThreadName,AddressBuffer>  ThreadAddrBufMap;

namespace {


#ifndef NDEBUG
/** Flag indicating if debuging for LinkedObjects is enabled. */
bool is_debug_aggregator_events_enabled =
    (getenv("CBTF_DEBUG_AGGR_EVENTS") != NULL);
bool is_trace_aggregator_events_enabled =
    (getenv("CBTF_TRACE_AGGR_EVENTS") != NULL);
#endif

    bool is_finished = false;
    bool sent_buffer = false;

    // vector of incoming threadnames. For each thread we expect
    ThreadNameVec threadnames;
    ThreadAddrBufMap threadaddrbufmap;

    // map address counts to threads.
    bool updateAddrThreadCountMap(AddressBuffer& buf,
				   AddrThreadCountMap& addrThreadCount,
				   ThreadName& tname)
    {
#ifndef NDEBUG
        if (is_trace_aggregator_events_enabled) {
	    std::cerr << getpid() << " " << "ENTERED updateAddrThreadCountMap  buf size is "
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
		    std::cerr << "updateAddrThreadCountMap UPDATE addr " << aci->first
		        << " thread " << tname
		        << " new count " << aci->second << std::endl;
#endif
		    lb->second.first = tname;
		    lb->second.second = aci->second;
		}
	    } else {
#if 0
		std::cerr << "updateAddrThreadCountMap INSERTS addr " << aci->first
		    <<  " thread " << tname
		    << " count " << aci->second << std::endl;
#endif
		std::pair<ThreadName,uint64_t> tcount(tname,aci->second);
		addrThreadCount.insert(lb, AddrThreadCountMap::value_type(aci->first, tcount));
	    }
	}

#ifndef NDEBUG
        if (is_trace_aggregator_events_enabled) {
	    std::cerr << getpid() << " " << "updateAddrThreadCountMap exits, addrThreadCount size is "
	    << addrThreadCount.size() << std::endl;
	}
#endif
    }

    void printAddrThreadCountMap(AddrThreadCountMap& addrTM)
    {
	std::stringstream output;
	output << "entered printAddrThreadCountMap addrThreadCount size " << addrTM.size() << std::endl;
	AddrThreadCountMap::const_iterator aci;
	for (aci = addrTM.begin(); aci != addrTM.end(); ++aci) {
	    output << "Address " << aci->first
		<< " thread:" << aci->second.first
		<< " count " << aci->second.second
		<< std::endl;
	}
	std::cerr << output.str();
    }


    Graph dGraph;

    // threads finished.
    int threads_finished = 0;

    // total size of performance data seen.
    int total_data_size = 0;

    void aggregatePCData(const std::string id, const Blob &blob,
			 AddressBuffer &buf, uint64_t &interval,
			 ThreadName &tname)
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
			 AddressBuffer &buf, uint64_t &interval,
			 ThreadName &tname)
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
			 AddressBuffer &buf, ThreadName &tname)
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
        declareInput<boost::shared_ptr<CBTF_pcsamp_data> >(
            "pcsamp", boost::bind(&AddressAggregator::pcsampHandler, this, _1)
            );
        declareInput<boost::shared_ptr<CBTF_usertime_data> >(
            "usertime", boost::bind(&AddressAggregator::usertimeHandler, this, _1)
            );
        declareInput<boost::shared_ptr<CBTF_io_trace_data> >(
            "io", boost::bind(&AddressAggregator::ioHandler, this, _1)
            );
        declareInput<boost::shared_ptr<CBTF_io_profile_data> >(
            "iop", boost::bind(&AddressAggregator::iopHandler, this, _1)
            );
        declareInput<boost::shared_ptr<CBTF_io_exttrace_data> >(
            "iot", boost::bind(&AddressAggregator::iotHandler, this, _1)
            );
        declareInput<boost::shared_ptr<CBTF_hwc_data> >(
            "hwc", boost::bind(&AddressAggregator::hwcHandler, this, _1)
            );
        declareInput<boost::shared_ptr<CBTF_hwcsamp_data> >(
            "hwcsamp", boost::bind(&AddressAggregator::hwcsampHandler, this, _1)
            );
        declareInput<boost::shared_ptr<CBTF_mem_exttrace_data> >(
            "mem", boost::bind(&AddressAggregator::memHandler, this, _1)
            );
        declareInput<boost::shared_ptr<CBTF_pthreads_exttrace_data> >(
            "pthreads", boost::bind(&AddressAggregator::pthreadsHandler, this, _1)
            );
        declareInput<boost::shared_ptr<CBTF_mpi_trace_data> >(
            "mpi", boost::bind(&AddressAggregator::mpiHandler, this, _1)
            );
        declareInput<boost::shared_ptr<CBTF_mpi_exttrace_data> >(
            "mpit", boost::bind(&AddressAggregator::mpitHandler, this, _1)
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
        threadnames = in;
#ifndef NDEBUG
        if (is_trace_aggregator_events_enabled) {
            std::cerr << getpid() << " "
	    << "Entered AddressAggregator::threadnamesHandler threadnames size is "
	    << threadnames.size() << std::endl;
	}
#endif
    }

    void finishedHandler(const bool& in)
    {
        is_finished = in;
#ifndef NDEBUG
        if (is_trace_aggregator_events_enabled) {
            std::cerr << getpid() << " "
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
            if (is_debug_aggregator_events_enabled) {
                std::cerr << getpid() << " "
	        << "AddressAggregator::finishedHandler sends buffer addr size " << abuffer.addresscounts.size()
		<< std::endl;
	    }
#endif
            //std::cerr << getpid() << " " << "AddressAggregator::finishedHandler EMITS buffer " << std::endl;
	    emitOutput<AddressBuffer>("Aggregatorout",  abuffer);
            //std::cerr << getpid() << " " << "AddressAggregator::finishedHandler EMITS threadaddrbufmap size "<< threadaddrbufmap.size() << std::endl;
            emitOutput<ThreadAddrBufMap>("ThreadAddrBufMap",threadaddrbufmap);
	    sent_buffer = true;
	}
    }
 

    /** Handler for the "in1" input.*/
    void pcsampHandler(const boost::shared_ptr<CBTF_pcsamp_data>& in)
    {
        CBTF_pcsamp_data *data = in.get();

	PCData pcdata;
        pcdata.aggregateAddressCounts(data->pc.pc_len,
                                data->pc.pc_val,
                                data->count.count_val,
                                abuffer);

	emitOutput<uint64_t>("interval",  data->interval);
        emitOutput<AddressBuffer>("Aggregatorout",  abuffer);
    }

    /** Handler for the "hwc" input.*/
    void hwcHandler(const boost::shared_ptr<CBTF_hwc_data>& in)
    {
        CBTF_hwc_data *data = in.get();

	PCData pcdata;
        pcdata.aggregateAddressCounts(data->pc.pc_len,
                                data->pc.pc_val,
                                data->count.count_val,
                                abuffer);

	emitOutput<uint64_t>("interval",  data->interval);
        emitOutput<AddressBuffer>("Aggregatorout",  abuffer);
    }

    /** Handler for the "hwcsamp" input.*/
    void hwcsampHandler(const boost::shared_ptr<CBTF_hwcsamp_data>& in)
    {
        CBTF_hwcsamp_data *data = in.get();

	PCData pcdata;
        pcdata.aggregateAddressCounts(data->pc.pc_len,
                                data->pc.pc_val,
                                data->count.count_val,
                                abuffer);

	emitOutput<uint64_t>("interval",  data->interval);
        emitOutput<AddressBuffer>("Aggregatorout",  abuffer);
    }

    /** Handler for the "usertime" input.*/
    void usertimeHandler(const boost::shared_ptr<CBTF_usertime_data>& in)
    {
        CBTF_usertime_data *data = in.get();

	StacktraceData stdata;
	stdata.aggregateAddressCounts(data->stacktraces.stacktraces_len,
				data->stacktraces.stacktraces_val,
				data->count.count_val,
				abuffer);

	emitOutput<uint64_t>("interval",  data->interval);
        emitOutput<AddressBuffer>("Aggregatorout",  abuffer);
    }

    /** Handler for the "io" input.*/
    void ioHandler(const boost::shared_ptr<CBTF_io_trace_data>& in)
    {
        CBTF_io_trace_data *data = in.get();

	StacktraceData stdata;
	stdata.aggregateAddressCounts(data->stacktraces.stacktraces_len,
				data->stacktraces.stacktraces_val,
				abuffer);

        emitOutput<AddressBuffer>("Aggregatorout",  abuffer);
    }

    /** Handler for the "iot" input.*/
    void iopHandler(const boost::shared_ptr<CBTF_io_profile_data>& in)
    {
        CBTF_io_profile_data *data = in.get();

	StacktraceData stdata;
	stdata.aggregateAddressCounts(data->stacktraces.stacktraces_len,
				data->stacktraces.stacktraces_val,
				abuffer);

        emitOutput<AddressBuffer>("Aggregatorout",  abuffer);
    }

    /** Handler for the "iot" input.*/
    void iotHandler(const boost::shared_ptr<CBTF_io_exttrace_data>& in)
    {
        CBTF_io_exttrace_data *data = in.get();

	StacktraceData stdata;
	stdata.aggregateAddressCounts(data->stacktraces.stacktraces_len,
				data->stacktraces.stacktraces_val,
				abuffer);

        emitOutput<AddressBuffer>("Aggregatorout",  abuffer);
    }

    /** Handler for the "mem" input.*/
    void memHandler(const boost::shared_ptr<CBTF_mem_exttrace_data>& in)
    {
        CBTF_mem_exttrace_data *data = in.get();

	StacktraceData stdata;
	stdata.aggregateAddressCounts(data->stacktraces.stacktraces_len,
				data->stacktraces.stacktraces_val,
				abuffer);

        emitOutput<AddressBuffer>("Aggregatorout",  abuffer);
    }

    /** Handler for the "mem" input.*/
    void pthreadsHandler(const boost::shared_ptr<CBTF_pthreads_exttrace_data>& in)
    {
        CBTF_pthreads_exttrace_data *data = in.get();

	StacktraceData stdata;
	stdata.aggregateAddressCounts(data->stacktraces.stacktraces_len,
				data->stacktraces.stacktraces_val,
				abuffer);

        emitOutput<AddressBuffer>("Aggregatorout",  abuffer);
    }

    /** Handler for the "mpi" input.*/
    void mpiHandler(const boost::shared_ptr<CBTF_mpi_trace_data>& in)
    {
        CBTF_mpi_trace_data *data = in.get();

	StacktraceData stdata;
	stdata.aggregateAddressCounts(data->stacktraces.stacktraces_len,
				data->stacktraces.stacktraces_val,
				abuffer);

        emitOutput<AddressBuffer>("Aggregatorout",  abuffer);
    }

    /** Handler for the "mpit" input.*/
    void mpitHandler(const boost::shared_ptr<CBTF_mpi_exttrace_data>& in)
    {
        CBTF_mpi_exttrace_data *data = in.get();

	StacktraceData stdata;
	stdata.aggregateAddressCounts(data->stacktraces.stacktraces_len,
				data->stacktraces.stacktraces_val,
				abuffer);

        emitOutput<AddressBuffer>("Aggregatorout",  abuffer);
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
        if (is_trace_aggregator_events_enabled) {
	    std::cerr << "ENTERED AddressAggregator::cbtf_protocol_blob_Handler" << std::endl;
	}
#endif

	if (in->data.data_len == 0 ) {
	    std::cerr << "EXIT AddressAggregator::cbtf_protocol_blob_Handler: data length 0" << std::endl;
	    abort();
	}

	//std::cerr << "AddressAggregator::cbtf_protocol_blob_Handler: EMIT CBTF_Protocol_Blob" << std::endl;
	emitOutput<boost::shared_ptr<CBTF_Protocol_Blob> >("datablob_xdr_out",in);

	Blob myblob(in.get()->data.data_len, in.get()->data.data_val);

	// decode this blobs data header and create a threadname object
	// and collector id object.
        CBTF_DataHeader header;
        memset(&header, 0, sizeof(header));
        unsigned header_size = myblob.getXDRDecoding(
            reinterpret_cast<xdrproc_t>(xdr_CBTF_DataHeader), &header
            );
        ThreadName threadname(header.host,header.pid,header.posix_tid,header.rank);
	std::string collectorID(header.id);

	// find the actual data blob after the header and create a Blob.
	unsigned data_size = myblob.getSize() - header_size;
	total_data_size += data_size;
	const void* data_ptr = &(reinterpret_cast<const char *>(myblob.getContents())[header_size]);
	Blob dblob(data_size,data_ptr);

#ifndef NDEBUG
        if (is_debug_aggregator_events_enabled) {
	    std::cerr << getpid() << " " << "Aggregating CBTF_Protocol_Blob addresses for "
	    << collectorID << " data from thread " << threadname
	    << " total data bytes so far: " << total_data_size
	    << std::endl;
	}
#endif

	// The following does a global aggregation of the data. Not per thread of execution.
	if (collectorID == "pcsamp" || collectorID == "hwc" || collectorID == "hwcsamp") {
	    AddressBuffer buf;
            aggregatePCData(collectorID, dblob, buf, interval, threadname);
	    // update aggregate addresses and counts. 
	    abuffer.updateAddressCounts(buf);

	    // load balance on address counts.
	    updateAddrThreadCountMap(buf, addrThreadCount, threadname);
	    emitOutput<uint64_t>("interval",  interval);
	} else if (collectorID == "usertime" || collectorID == "hwctime") {
	    AddressBuffer buf;
            aggregateSTSampleData(collectorID, dblob, buf, interval, threadname);
	    abuffer.updateAddressCounts(buf);
	    updateAddrThreadCountMap(buf, addrThreadCount, threadname);
	    emitOutput<uint64_t>("interval",  interval);
        } else if (collectorID == "io" || collectorID == "iot" ||
		   collectorID == "iop" || collectorID == "mem" ||
		   collectorID == "mpi" || collectorID == "mpit" ||
		   collectorID == "mpip" || collectorID == "pthreads") {
            //aggregateSTTraceData(collectorID, dblob, abuffer, threadname);
	    AddressBuffer buf;
            aggregateSTTraceData(collectorID, dblob, buf, threadname);
	    abuffer.updateAddressCounts(buf);
	    updateAddrThreadCountMap(buf, addrThreadCount, threadname);
	} else {
	    std::cerr << "Unknown collector data handled!" << std::endl;
	}

	//std::cerr << "AddressAggregator::cbtf_protocol_blob_Handler: EMITS Addressbuffer" << std::endl;
        emitOutput<AddressBuffer>("Aggregatorout",  abuffer);
        xdr_free(reinterpret_cast<xdrproc_t>(xdr_CBTF_DataHeader), reinterpret_cast<char*>(&header));

    }

    /** Pass Through Handler for the "CBTF_Protocol_Blob" input.*/
    void pass_cbtf_protocol_blob_Handler(const boost::shared_ptr<CBTF_Protocol_Blob>& in)
    {
	// Would be nice to group these on their way up the tree.
#ifndef NDEBUG
        if (is_trace_aggregator_events_enabled) {
	    std::cerr << "AddressAggregator::pass_cbtf_protocol_blob_Handler: EMITS CBTF_Protocol_Blob" << std::endl;
	}
#endif
	emitOutput<boost::shared_ptr<CBTF_Protocol_Blob> >("datablob_xdr_out",in);
    }

    /** Handler for the "addressBuffer" input.
      * Aggregate Addresbuffers coming in to this node.
      */
    void addressBufferHandler(const AddressBuffer& in)
    {
	
#ifndef NDEBUG
        if (is_trace_aggregator_events_enabled) {
	    std::cerr << "ENTERED AddressAggregator::addressBufferHandler" << std::endl;
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
 	    std::cerr << getpid() << " " << "AddressAggregator::addressBufferHandler"
	    << " handled buffers " << handled_buffers
	    << " known threads " << threadnames.size()
	    << " is_finished " << is_finished
	    << std::endl;
	}
#endif
        if (handled_buffers == threadnames.size()) {
#ifndef NDEBUG
            if (is_debug_aggregator_events_enabled) {
 	        std::cerr << getpid() << " " << "AddressAggregator::addressBufferHandler "
 	        << "handled " << handled_buffers
		<< " buffers for known total threads" << threadnames.size()
 	        << std::endl;
	    }
#endif
	}
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
	    std::cerr << getpid() << " blobHandler: Aggregating Blob addresses for "
	    << collectorID << " data from thread " << tname
	    << std::endl;
	}
#endif

	// find the actual data blob after the header
	unsigned data_size = in.getSize() - header_size;
	const void* data_ptr = &(reinterpret_cast<const char *>(in.getContents())[header_size]);
	Blob dblob(data_size,data_ptr);

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
    int handled_buffers;

}; // class AddressAggregator

KRELL_INSTITUTE_CBTF_REGISTER_FACTORY_FUNCTION(AddressAggregator)
