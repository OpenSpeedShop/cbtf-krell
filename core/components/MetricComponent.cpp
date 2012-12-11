////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2012 Krell Institute. All Rights Reserved.
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

/** @file Metric Plugin components. */

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
#include "KrellInstitute/Core/StackTrace.hpp"
#include "KrellInstitute/Core/ThreadName.hpp"
#include "KrellInstitute/Core/Time.hpp"
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
    (getenv("CBTF_DEBUG_METRIC_EVENTS") != NULL);
#endif

    // count of threads handled
    int handled_buffers = 0;

    bool is_finished = false;

    bool sent_buffer = false;

    // vector of incoming threadnames. For each thread we expect
    ThreadNameVec threadnames;

    void SampleMetric(const std::string id, const Blob &blob)
    {
	if (id == "pcsamp") {
            CBTF_pcsamp_data data;
            memset(&data, 0, sizeof(data));
            blob.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_CBTF_pcsamp_data), &data);

	    std::vector<uint64_t> values;
	    for(unsigned i = 0; i < data.pc.pc_len; ++i) {
		uint64_t t_sample =
			static_cast<uint64_t>(data.count.count_val[i]) *
			static_cast<uint64_t>(data.interval) / 1000000000.0;
std::cerr << "PCSAMP: Address:" << Address(data.pc.pc_val[i]) << " Time:" << Time(t_sample) << std::endl;

	    }
	    xdr_free(reinterpret_cast<xdrproc_t>(xdr_CBTF_pcsamp_data),
		     reinterpret_cast<char*>(&data));

	} else if (id == "hwc") {
            CBTF_hwc_data data;
            memset(&data, 0, sizeof(data));
            blob.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_CBTF_hwc_data), &data);

	    std::vector<uint64_t> values;
	    for(unsigned i = 0; i < data.pc.pc_len; ++i) {
		uint64_t t_sample = static_cast<uint64_t>(data.count.count_val[i]) *
                            static_cast<uint64_t>(data.interval);
	    }
	    xdr_free(reinterpret_cast<xdrproc_t>(xdr_CBTF_hwc_data),
		     reinterpret_cast<char*>(&data));

	} else if (id == "hwcsamp") {
            CBTF_hwcsamp_data data;
            memset(&data, 0, sizeof(data));
            blob.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_CBTF_hwcsamp_data), &data);

	    for(unsigned i = 0; i < data.pc.pc_len; ++i) {
		double t_sample =
			static_cast<double>(data.count.count_val[i]) *
			static_cast<double>(data.interval) / 1000000000.0;
	    }
	    xdr_free(reinterpret_cast<xdrproc_t>(xdr_CBTF_hwcsamp_data),
		     reinterpret_cast<char*>(&data));

	} else {
	    return;
	}

    }

    void STSampleMetric(const std::string id, const Blob &blob)
    {
	if (id == "usertime") {
            CBTF_usertime_data data;
            memset(&data, 0, sizeof(data));
            blob.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_CBTF_usertime_data), &data);
	} else if (id == "hwctime") {
            CBTF_hwctime_data data;
            memset(&data, 0, sizeof(data));
            blob.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_CBTF_hwctime_data), &data);
	} else {
	    return;
	}

    }

    void STTraceMetric(const std::string id, const Blob &blob)
    {

	if (id == "io") {
            CBTF_io_trace_data data;
            memset(&data, 0, sizeof(data));
            blob.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_CBTF_io_trace_data), &data);
	} else if (id == "iot") {
            CBTF_io_exttrace_data data;
            memset(&data, 0, sizeof(data));
            blob.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_CBTF_io_exttrace_data), &data);
	} else if (id == "mem") {
            CBTF_mem_exttrace_data data;
            memset(&data, 0, sizeof(data));
            blob.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_CBTF_mem_exttrace_data), &data);
	} else if (id == "mpi") {
            CBTF_mpi_trace_data data;
            memset(&data, 0, sizeof(data));
            blob.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_CBTF_mpi_trace_data), &data);
	} else if (id == "mpit") {
            CBTF_mpi_exttrace_data data;
            memset(&data, 0, sizeof(data));
            blob.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_CBTF_mpi_exttrace_data), &data);
	} else {
	    return;
	}

    }

}

/**
 * Component that aggregates address values and their counts.
 */
class __attribute__ ((visibility ("hidden"))) MetricAggregator :
    public Component
{

public:

    /** Factory function for this component type. */
    static Component::Instance factoryFunction()
    {
        return Component::Instance(
            reinterpret_cast<Component*>(new MetricAggregator())
            );
    }

private:

    /** Default constructor. */
    MetricAggregator() :
        Component(Type(typeid(MetricAggregator)), Version(0, 0, 1))
    {
        declareInput<Blob>(
            "blob", boost::bind(&MetricAggregator::blobHandler, this, _1)
            );
        declareInput<boost::shared_ptr<CBTF_Protocol_Blob> >(
            "cbtf_protocol_blob",
            boost::bind(
                &MetricAggregator::cbtf_protocol_blob_Handler, this, _1
                )
            );
        declareInput<boost::shared_ptr<CBTF_Protocol_Blob> >(
            "pass_cbtf_protocol_blob",
            boost::bind(
                &MetricAggregator::pass_cbtf_protocol_blob_Handler, this, _1
                )
            );
	declareInput<ThreadNameVec>(
            "threadnames", boost::bind(&MetricAggregator::threadnamesHandler, this, _1)
            );
       declareInput<bool>(
            "finished", boost::bind(&MetricAggregator::finishedHandler, this, _1)
            );

        declareOutput<AddressBuffer>("Aggregatorout");
	declareOutput<boost::shared_ptr<CBTF_Protocol_Blob> >("datablob_xdr_out");
    }

    /** Handlers for the inputs.*/
    void threadnamesHandler(const ThreadNameVec& in)
    {
        threadnames = in;
#ifndef NDEBUG
        if (is_debug_aggregator_events_enabled) {
            std::cerr
	    << "MetricAggregator::threadnamesHandler threadnames size is "
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
	    << "MetricAggregator::finishedHandler finished is " << is_finished
	    << " for " << threadnames.size() << " threadnames seen"
	    << " from cbtf pid " << getpid()
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
                std::cerr
	        << "MetricAggregator::finishedHandler sends buffer " << sent_buffer
	        << " from cbtf pid " << getpid()
		<< std::endl;
		abuffer.printResults();
	    }
#endif
	    emitOutput<AddressBuffer>("Aggregatorout",  abuffer);
	    sent_buffer = true;
	}
    }
 

    /** Handler for the "CBTF_Protocol_Blob" input.*/
    void cbtf_protocol_blob_Handler(const boost::shared_ptr<CBTF_Protocol_Blob>& in)
    {
        CBTF_Protocol_Blob *B = in.get();
	Blob myblob(B->data.data_len, B->data.data_val);

	//std::cerr << "ENTER MetricAggregator::cbtf_protocol_blob_Handler" << std::endl;

	if (in->data.data_len == 0 ) {
	    std::cerr << "EXIT MetricAggregator::cbtf_protocol_blob_Handler: data length 0" << std::endl;
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
	    << header.host << ":" << header.pid
	    << " from cbtf pid " << getpid()
	    << std::endl;
	}
#endif

	// find the actual data blob after the header
	unsigned data_size = myblob.getSize() - header_size;
	const void* data_ptr = &(reinterpret_cast<const char *>(myblob.getContents())[header_size]);
	Blob dblob(data_size,data_ptr);

	if (collectorID == "pcsamp" || collectorID == "hwc" || collectorID == "hwcsamp") {
            SampleMetric(collectorID, dblob);
	} else if (collectorID == "usertime" || collectorID == "hwctime") {
            STSampleMetric(collectorID, dblob);
        } else if (collectorID == "io" || collectorID == "iot" || collectorID == "mem" ||
		   collectorID == "mpi" || collectorID == "mpit" || collectorID == "pthreads") {
            STTraceMetric(collectorID, dblob);
	} else {
	    std::cerr << "Unknown collector data handled!" << std::endl;
	}

	// Too bad the waitforall sync filter can't control the
	// emit of the AddressBuffer based on the number of
	// children below.
	//std::cerr << "MetricAggregator::cbtf_protocol_blob_Handler: EMIT Addressbuffer" << std::endl;
        emitOutput<AddressBuffer>("Aggregatorout",  abuffer);

	//std::cerr << "MetricAggregator::cbtf_protocol_blob_Handler: EMIT CBTF_Protocol_Blob" << std::endl;
	emitOutput<boost::shared_ptr<CBTF_Protocol_Blob> >("datablob_xdr_out",in);
    }

    /** Pass Through Handler for the "CBTF_Protocol_Blob" input.*/
    void pass_cbtf_protocol_blob_Handler(const boost::shared_ptr<CBTF_Protocol_Blob>& in)
    {
	// Would be nice to group these on their way up the tree.
	//std::cerr << "MetricAggregator::pass_cbtf_protocol_blob_Handler: EMIT CBTF_Protocol_Blob" << std::endl;
	emitOutput<boost::shared_ptr<CBTF_Protocol_Blob> >("datablob_xdr_out",in);
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
	    << header.host << ":" << header.pid
	    << " from cbtf pid " << getpid()
	    << std::endl;
	}
#endif

	// find the actual data blob after the header
	unsigned data_size = in.getSize() - header_size;
	const void* data_ptr = &(reinterpret_cast<const char *>(in.getContents())[header_size]);
	Blob dblob(data_size,data_ptr);

	if (collectorID == "pcsamp" || collectorID == "hwc" || collectorID == "hwcsamp") {
            SampleMetric(collectorID, dblob);
	} else if (collectorID == "usertime" || collectorID == "hwctime") {
            STSampleMetric(collectorID, dblob);
        } else if (collectorID == "io" || collectorID == "iot" || collectorID == "mem" ||
		   collectorID == "mpi" || collectorID == "mpit" || collectorID == "pthreads") {
            STTraceMetric(collectorID, dblob);
	} else {
	    std::cerr << "Unknown collector data handled!" << std::endl;
	}

	//std::cerr << "MetricAggregator::blobHandler: EMIT Addressbuffer" << std::endl;
        emitOutput<AddressBuffer>("Aggregatorout",  abuffer);
    }

    AddressBuffer abuffer;

}; // class MetricAggregator

KRELL_INSTITUTE_CBTF_REGISTER_FACTORY_FUNCTION(MetricAggregator)
