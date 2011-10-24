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
#include "KrellInstitute/Core/StacktraceData.hpp"
#include "KrellInstitute/Messages/Blob.h"
#include "KrellInstitute/Messages/DataHeader.h"
#include "KrellInstitute/Messages/Address.h"
#include "KrellInstitute/Messages/PCSamp_data.h"
#include "KrellInstitute/Messages/Usertime_data.h"
#include "KrellInstitute/Messages/Hwc_data.h"
#include "KrellInstitute/Messages/IO_data.h"
#include "KrellInstitute/Messages/Mem_data.h"

using namespace KrellInstitute::CBTF;
using namespace KrellInstitute::Core;
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
        declareInput<boost::shared_ptr<CBTF_pcsamp_data> >(
            "pcsamp", boost::bind(&AddressAggregator::pcsampHandler, this, _1)
            );
        declareInput<boost::shared_ptr<CBTF_usertime_data> >(
            "usertime", boost::bind(&AddressAggregator::usertimeHandler, this, _1)
            );
        declareInput<boost::shared_ptr<CBTF_io_trace_data> >(
            "io", boost::bind(&AddressAggregator::ioHandler, this, _1)
            );
        declareInput<boost::shared_ptr<CBTF_hwc_data> >(
            "hwc", boost::bind(&AddressAggregator::hwcHandler, this, _1)
            );
        declareInput<boost::shared_ptr<CBTF_mem_trace_data> >(
            "mem", boost::bind(&AddressAggregator::memHandler, this, _1)
            );
        declareOutput<AddressBuffer>("Aggregatorout");
	declareOutput<uint64_t>("interval");
    }

    /** Handler for the "in1" input.*/
    void pcsampHandler(const boost::shared_ptr<CBTF_pcsamp_data>& in)
    {
        CBTF_pcsamp_data *data = in.get();

	//interval = data->interval;

        //std::cerr << "AddressAggregator::pcsampHandler: input interval is " << data->interval <<  std::endl;
	PCData pcdata;
        pcdata.aggregateAddressCounts(data->pc.pc_len,
                                data->pc.pc_val,
                                data->count.count_val,
                                abuffer);

	emitOutput<uint64_t>("interval",  data->interval);
        emitOutput<AddressBuffer>("Aggregatorout",  abuffer);
    }

    /** Handler for the "in1" input.*/
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

    /** Handler for the "usertime" input.*/
    void usertimeHandler(const boost::shared_ptr<CBTF_usertime_data>& in)
    {
        CBTF_usertime_data *data = in.get();

	StacktraceData stdata;
	stdata.aggregateAddressCounts(data->bt.bt_len,
				data->bt.bt_val,
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

    /** Handler for the "io" input.*/
    void memHandler(const boost::shared_ptr<CBTF_mem_trace_data>& in)
    {
        CBTF_mem_trace_data *data = in.get();

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

	if (in->data.data_len == 0 ) {
	    std::cerr << "EXIT AddressAggregator::cbtf_protocol_blob_Handler: data length 0" << std::endl;
	    abort();
	}

#if 0
// FIXME: can not decode both xdr messages.
// Only the first xdr is decoded properly (CBTF_DataHeader).
// Likely need to reposition the xdr stream...

        CBTF_DataHeader header;
        memset(&header, 0, sizeof(header));
        unsigned header_size = myblob.getXDRDecoding(
            reinterpret_cast<xdrproc_t>(xdr_CBTF_DataHeader), &header
            );

// FIXME:
// this currently is hard coded to pcsamp data.
// the data header has an int for experiment type which in OSS
// is used to match the expId.  Maybe in cbtf this can be an
// enum to define each type of data type and then used to
// choose the appropro data xdrproc....
//
        CBTF_pcsamp_data data;
        memset(&data, 0, sizeof(data));
        myblob.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_CBTF_pcsamp_data), &data);

	interval = data.interval;

	PCData pcdata;
        pcdata.aggregateAddressCounts(data.pc.pc_len,
                                data.pc.pc_val,
                                data.count.count_val,
                                abuffer);
#endif
        emitOutput<AddressBuffer>("Aggregatorout",  abuffer);
    }

    /** Handler for the "in2" input.*/
    void addressBufferHandler(const AddressBuffer& in)
    {
        emitOutput<AddressBuffer>("Aggregatorout",  in /*abuffer*/);
    }

    /** Handler for the "in3" input.*/
    void blobHandler(const Blob& in)
    {

#if 0
// FIXME: can not decode both xdr messages.
// Only the first xdr is decoded properly (CBTF_DataHeader).
//
        CBTF_DataHeader header;
        memset(&header, 0, sizeof(header));
        unsigned header_size = in.getXDRDecoding(
            reinterpret_cast<xdrproc_t>(xdr_CBTF_DataHeader), &header
            );

// FIXME:
// this currently is hard coded to pcsamp data.
// the data header has an int for experiment type which in OSS
// is used to match the expId.  Maybe in cbtf this can be an
// enum to define each type of data type and then used to
// choose the appropro data xdrproc....
//
        CBTF_pcsamp_data data;
        memset(&data, 0, sizeof(data));
        in.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_CBTF_pcsamp_data), &data);

	interval = data.interval;

	PCData pcdata;
        pcdata.aggregateAddressCounts(data.pc.pc_len,
                                data.pc.pc_val,
                                data.count.count_val,
                                abuffer);
#endif
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
