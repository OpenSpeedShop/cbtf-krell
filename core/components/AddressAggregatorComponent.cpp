////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2011 Krell Institute. All Rights Reserved.
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
#include <mrnet/MRNet.h>
#include <typeinfo>
#include <algorithm>

#include <KrellInstitute/CBTF/Component.hpp>
#include <KrellInstitute/CBTF/Type.hpp>
#include <KrellInstitute/CBTF/Version.hpp>

#include "KrellInstitute/Core/Address.hpp"
#include "KrellInstitute/Core/AddressBuffer.hpp"
#include "KrellInstitute/Core/AddressRange.hpp"
#include "KrellInstitute/Core/Blob.hpp"
#include "KrellInstitute/Core/PCData.hpp"
#include "KrellInstitute/Core/Path.hpp"
#include "KrellInstitute/Core/StacktraceData.hpp"
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

using namespace KrellInstitute::CBTF;
using namespace KrellInstitute::Core;

namespace {

#ifndef NDEBUG
/** Flag indicating if debuging for LinkedObjects is enabled. */
bool is_debug_aggregator_events_enabled =
    (getenv("CBTF_DEBUG_AGGR_EVENTS") != NULL);
#endif

    // count of threads handled
    int handled_threads = 0;

    bool is_finished = false;

    bool sent_buffer = false;

    // vector of incoming threadnames. For each thread we expect
    ThreadNameVec threadnames;

    void aggregatePCData(const std::string id, const Blob &blob,
			 AddressBuffer &buf, uint64_t &interval)
    {
	uint64_t *pc_val;
	u_int pc_len;
	uint8_t *count_val;

	if (id == "pcsamp") {
            CBTF_pcsamp_data data;
            memset(&data, 0, sizeof(data));
            blob.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_CBTF_pcsamp_data), &data);
	    pc_val = data.pc.pc_val;
	    count_val = data.count.count_val;
	    pc_len = data.pc.pc_len;
	    interval = data.interval;
	} else if (id == "hwc") {
            CBTF_hwc_data data;
            memset(&data, 0, sizeof(data));
            blob.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_CBTF_hwc_data), &data);
	    pc_val = data.pc.pc_val;
	    count_val = data.count.count_val;
	    pc_len = data.pc.pc_len;
	    interval = data.interval;
	} else if (id == "hwcsamp") {
            CBTF_hwcsamp_data data;
            memset(&data, 0, sizeof(data));
            blob.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_CBTF_hwcsamp_data), &data);
	    pc_val = data.pc.pc_val;
	    count_val = data.count.count_val;
	    pc_len = data.pc.pc_len;
	    interval = data.interval;
	} else {
	    return;
	}

	PCData pcdata;
        pcdata.aggregateAddressCounts(pc_len, pc_val, count_val, buf);
    }

    void aggregateSTSampleData(const std::string id, const Blob &blob,
			 AddressBuffer &buf, uint64_t &interval)
    {
	uint64_t *stacktraces_val;
	u_int stacktraces_len;
	uint8_t *count_val;

	if (id == "usertime") {
            CBTF_usertime_data data;
            memset(&data, 0, sizeof(data));
            blob.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_CBTF_usertime_data), &data);
	    stacktraces_val = data.stacktraces.stacktraces_val;
	    count_val = data.count.count_val;
	    stacktraces_len = data.stacktraces.stacktraces_len;
	    interval = data.interval;
	} else if (id == "hwctime") {
            CBTF_hwctime_data data;
            memset(&data, 0, sizeof(data));
            blob.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_CBTF_hwctime_data), &data);
	    stacktraces_val = data.stacktraces.stacktraces_val;
	    count_val = data.count.count_val;
	    stacktraces_len = data.stacktraces.stacktraces_len;
	    interval = data.interval;
	} else {
	    return;
	}

	StacktraceData stdata;
	stdata.aggregateAddressCounts(stacktraces_len,
				stacktraces_val, count_val, buf);
    }

    void aggregateSTTraceData(const std::string id, const Blob &blob,
			 AddressBuffer &buf)
    {
	uint64_t *stacktraces_val;
	u_int stacktraces_len;

	if (id == "io") {
            CBTF_io_trace_data data;
            memset(&data, 0, sizeof(data));
            blob.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_CBTF_io_trace_data), &data);
	    stacktraces_val = data.stacktraces.stacktraces_val;
	    stacktraces_len = data.stacktraces.stacktraces_len;
	} else if (id == "iot") {
            CBTF_io_exttrace_data data;
            memset(&data, 0, sizeof(data));
            blob.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_CBTF_io_exttrace_data), &data);
	    stacktraces_val = data.stacktraces.stacktraces_val;
	    stacktraces_len = data.stacktraces.stacktraces_len;
	} else if (id == "mem") {
            CBTF_mem_exttrace_data data;
            memset(&data, 0, sizeof(data));
            blob.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_CBTF_mem_exttrace_data), &data);
	    stacktraces_val = data.stacktraces.stacktraces_val;
	    stacktraces_len = data.stacktraces.stacktraces_len;
	} else if (id == "mpi") {
            CBTF_mpi_trace_data data;
            memset(&data, 0, sizeof(data));
            blob.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_CBTF_mpi_trace_data), &data);
	    stacktraces_val = data.stacktraces.stacktraces_val;
	    stacktraces_len = data.stacktraces.stacktraces_len;
	} else if (id == "mpit") {
            CBTF_mpi_exttrace_data data;
            memset(&data, 0, sizeof(data));
            blob.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_CBTF_mpi_exttrace_data), &data);
	    stacktraces_val = data.stacktraces.stacktraces_val;
	    stacktraces_len = data.stacktraces.stacktraces_len;
	} else {
	    return;
	}

	StacktraceData stdata;
	stdata.aggregateAddressCounts(stacktraces_len, stacktraces_val, buf);
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
	declareOutput<uint64_t>("interval");
	declareOutput<boost::shared_ptr<CBTF_Protocol_Blob> >("datablob_xdr_out");
    }

    /** Handlers for the inputs.*/
    void threadnamesHandler(const ThreadNameVec& in)
    {
        threadnames = in;
#ifndef NDEBUG
        if (is_debug_aggregator_events_enabled) {
            std::cerr
	    << "AddressAggregator::threadnamesHandler threadnames size is "
	    << threadnames.size() << std::endl;
	}
#endif
    }

    void finishedHandler(const bool& in)
    {
        is_finished = in;
#ifndef NDEBUG
        if (is_debug_aggregator_events_enabled) {
            std::cerr
	    << "AddressAggregator::threadnamesHandler finished is "
	    << is_finished << std::endl;
	}
#endif
	if (!sent_buffer) {
#ifndef NDEBUG
            if (is_debug_aggregator_events_enabled) {
                std::cerr
	        << "AddressAggregator::threadnamesHandler sends buffer "
	        << sent_buffer << std::endl;
	    }
#endif
	    emitOutput<AddressBuffer>("Aggregatorout",  abuffer);
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

        //std::cerr << "AddressAggregator::hwcHandler: input interval is " << data->interval <<  std::endl;
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

        //std::cerr << "AddressAggregator::hwcsampHandler: input interval is " << data->interval <<  std::endl;
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
    void cbtf_protocol_blob_Handler(const boost::shared_ptr<CBTF_Protocol_Blob>& in)
    {
        CBTF_Protocol_Blob *B = in.get();
	Blob myblob(B->data.data_len, B->data.data_val);

	//std::cerr << "ENTER AddressAggregator::cbtf_protocol_blob_Handler" << std::endl;

	if (in->data.data_len == 0 ) {
	    std::cerr << "EXIT AddressAggregator::cbtf_protocol_blob_Handler: data length 0" << std::endl;
	    abort();
	}

	// decode this blobs data header
        CBTF_DataHeader header;
        memset(&header, 0, sizeof(header));
        unsigned header_size = myblob.getXDRDecoding(
            reinterpret_cast<xdrproc_t>(xdr_CBTF_DataHeader), &header
            );

	std::string collectorID(header.id);

#ifndef NDEBUG
        if (is_debug_aggregator_events_enabled) {
	    std::cerr << "Aggregating CBTF_Protocol_Blob addresses for "
	    << collectorID << " data from "
	    << header.host << ":" << header.pid << std::endl;
	}
#endif

	// find the actual data blob after the header
	unsigned data_size = myblob.getSize() - header_size;
	const void* data_ptr = &(reinterpret_cast<const char *>(myblob.getContents())[header_size]);
	Blob dblob(data_size,data_ptr);

	if (collectorID == "pcsamp" || collectorID == "hwc" || collectorID == "hwcsamp") {
            aggregatePCData(collectorID, dblob, abuffer, interval);
	    emitOutput<uint64_t>("interval",  interval);
	} else if (collectorID == "usertime" || collectorID == "hwctime") {
            aggregateSTSampleData(collectorID, dblob, abuffer, interval);
	    emitOutput<uint64_t>("interval",  interval);
        } else if (collectorID == "io" || collectorID == "iot" || collectorID == "mem" ||
		   collectorID == "mpi" || collectorID == "mpit") {
            aggregateSTTraceData(collectorID, dblob, abuffer);
	} else {
	    std::cerr << "Unknown collector data handled!" << std::endl;
	}

	// Too bad the waitforall sync filter can't control the
	// emit of the AddressBuffer based on the number of
	// children below.
	//std::cerr << "AddressAggregator::cbtf_protocol_blob_Handler: EMIT Addressbuffer" << std::endl;
        emitOutput<AddressBuffer>("Aggregatorout",  abuffer);

	//std::cerr << "AddressAggregator::cbtf_protocol_blob_Handler: EMIT CBTF_Protocol_Blob" << std::endl;
	emitOutput<boost::shared_ptr<CBTF_Protocol_Blob> >("datablob_xdr_out",in);
    }

    /** Pass Through Handler for the "CBTF_Protocol_Blob" input.*/
    void pass_cbtf_protocol_blob_Handler(const boost::shared_ptr<CBTF_Protocol_Blob>& in)
    {
	// Would be nice to group these on their way up the tree.
	//std::cerr << "AddressAggregator::pass_cbtf_protocol_blob_Handler: EMIT CBTF_Protocol_Blob" << std::endl;
	emitOutput<boost::shared_ptr<CBTF_Protocol_Blob> >("datablob_xdr_out",in);
    }

    /** Handler for the "in2" input.*/
    void addressBufferHandler(const AddressBuffer& in)
    {
	
	AddressCounts::const_iterator aci;
	for (aci = in.addresscounts.begin(); aci != in.addresscounts.end(); ++aci) {

	    AddressCounts::iterator lb = abuffer.addresscounts.lower_bound(aci->first);
	    abuffer.updateAddressCounts(aci->first.getValue(), aci->second);
	}

        handled_threads++;
#ifndef NDEBUG
        if (is_debug_aggregator_events_enabled) {
 	    std::cerr << "AddressAggregator::addressBufferHandler"
	    << " handled threads " << handled_threads
	    << " known threads " << threadnames.size()
	    << " is_finished " << is_finished
	    << std::endl;
	}
#endif
        if (handled_threads == threadnames.size()) {
#ifndef NDEBUG
            if (is_debug_aggregator_events_enabled) {
 	        std::cerr << "AddressAggregator::addressBufferHandler "
 	        << "handled " << handled_threads << " threads."
 	        << std::endl;
	    }
#endif
	    emitOutput<AddressBuffer>("Aggregatorout",  abuffer);
	    sent_buffer = true;
	}
    }

    /** Handler for the "in3" input.*/
    void blobHandler(const Blob& in)
    {
	// decode this blobs data header
        CBTF_DataHeader header;
        memset(&header, 0, sizeof(header));
        unsigned header_size = in.getXDRDecoding(
            reinterpret_cast<xdrproc_t>(xdr_CBTF_DataHeader), &header
            );

	std::string collectorID(header.id);
#ifndef NDEBUG
        if (is_debug_aggregator_events_enabled) {
	    std::cerr << "Aggregating Blob addresses for "
	    << collectorID << " data from "
	    << header.host << ":" << header.pid << std::endl;
	}
#endif

	// find the actual data blob after the header
	unsigned data_size = in.getSize() - header_size;
	const void* data_ptr = &(reinterpret_cast<const char *>(in.getContents())[header_size]);
	Blob dblob(data_size,data_ptr);

	if (collectorID == "pcsamp" || collectorID == "hwc" || collectorID == "hwcsamp") {
            aggregatePCData(collectorID, dblob, abuffer, interval);
	    emitOutput<uint64_t>("interval",  interval);
	} else if (collectorID == "usertime" || collectorID == "hwctime") {
            aggregateSTSampleData(collectorID, dblob, abuffer, interval);
	    emitOutput<uint64_t>("interval",  interval);
        } else if (collectorID == "io" || collectorID == "iot" || collectorID == "mem" ||
		   collectorID == "mpi" || collectorID == "mpit") {
            aggregateSTTraceData(collectorID, dblob, abuffer);
	} else {
	    std::cerr << "Unknown collector data handled!" << std::endl;
	}

	//std::cerr << "AddressAggregator::blobHandler: EMIT Addressbuffer" << std::endl;
        emitOutput<AddressBuffer>("Aggregatorout",  abuffer);
    }

    void intervalHandler(const uint64_t in)
    {
        interval = in;
        //std::cerr << "AddressAggregator::intervalHandler: interval is " << interval <<  std::endl;
    }

    uint64_t interval;

    AddressBuffer abuffer;

}; // class AddressAggregator

KRELL_INSTITUTE_CBTF_REGISTER_FACTORY_FUNCTION(AddressAggregator)
