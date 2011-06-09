////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2010,2011 Krell Institute. All Rights Reserved.
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

/** @file Unit tests for the CBTF MRNet library. */

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE libcbtf-mrnet

#include <boost/shared_ptr.hpp>
#include <boost/test/unit_test.hpp>
#include <iostream>
#include <KrellInstitute/CBTF/BoostExts.hpp>
#include <KrellInstitute/CBTF/Component.hpp>
#include <KrellInstitute/CBTF/Type.hpp>
#include <KrellInstitute/CBTF/ValueSink.hpp>
#include <KrellInstitute/CBTF/ValueSource.hpp>
#include <KrellInstitute/CBTF/XDR.hpp>
#include <KrellInstitute/CBTF/XML.hpp>
#include <map>
#include <set>
#include <stdexcept>
#include <string>

#include "KrellInstitute/Core/Address.hpp"
#include "KrellInstitute/Core/AddressBuffer.hpp"
#include "KrellInstitute/Core/Blob.hpp"
#include "KrellInstitute/Core/PCData.hpp"

#include "KrellInstitute/Messages/Address.h"
#include "KrellInstitute/Messages/DataHeader.h"
#include "KrellInstitute/Messages/EventHeader.h"
#include "KrellInstitute/Messages/File.h"
#include "KrellInstitute/Messages/LinkedObjectEvents.h"
#include "KrellInstitute/Messages/PCSamp_data.h"
#include "KrellInstitute/Messages/Thread.h"
#include "KrellInstitute/Messages/ThreadEvents.h"
#include "KrellInstitute/Services/Data.h"

using namespace KrellInstitute::CBTF;
using namespace KrellInstitute::Core;

namespace std {

    /**
     * Redirect a std::map<std:string, Type> const iterator to an output stream.
     * Defined in order to allow the Boost.Test macros to work properly.
     *
     * @param stream      Target output stream.
     * @param iterator    Const iterator to redirect.
     * @return            Target output stream.
     */
    std::ostream& operator<<(
        std::ostream& stream,
        const std::map<std::string, Type>::const_iterator& iterator
        )
    {
        stream << "std::map<std::string, Type>::const_iterator";
        return stream;
    }

    /**
     * Redirect a std::set<Type> const iterator to an output stream. Defined
     * in order to allow the Boost.Test macros to work properly.
     *
     * @param stream      Target output stream.
     * @param iterator    Const iterator to redirect.
     * @return            Target output stream.
     */
    std::ostream& operator<<(std::ostream& stream,
                             const std::set<Type>::const_iterator& iterator)
    {
        stream << "std::set<Type>::const_iterator";
        return stream;
    }
    
} // namespace std


#define UINT8_MAX 255

bool UpdatePCData(uint64_t pc, CBTF_PCData* buffer)
{
    unsigned bucket, entry;

    /*
     * Search the sample buffer for an existing entry corresponding to this
     * PC address. Use the hash table and a simple linear probe to accelerate
     * the search.
     */
    bucket = (pc >> 4) % CBTF_PCHashTableSize;
    while((buffer->hash_table[bucket] > 0) &&
          (buffer->pc[buffer->hash_table[bucket] - 1] != pc))
        bucket = (bucket + 1) % CBTF_PCHashTableSize;

    /* Increment count for existing entry if found and not already maxed */
    if((buffer->hash_table[bucket] > 0) &&
       (buffer->pc[buffer->hash_table[bucket] - 1] == pc) &&
       (buffer->count[buffer->hash_table[bucket] - 1] < UINT8_MAX)) {
        buffer->count[buffer->hash_table[bucket] - 1]++;
        return false;
    }

    /* Otherwise add a new entry for this PC address to the sample buffer */
    entry = buffer->length;
    buffer->pc[entry] = pc;
    buffer->count[entry] = 1;
    buffer->length++;

    /* Update the address interval in the sample buffer */
    if(pc < buffer->addr_begin)
        buffer->addr_begin = pc;
    if(pc > buffer->addr_end)
        buffer->addr_end = pc;

    /* Update the hash table with this new entry */
    buffer->hash_table[bucket] = entry + 1;

    /* Indicate to the caller if the sample buffer is full */
    return (buffer->length == CBTF_PCBufferSize);
}

static __thread  struct {
    CBTF_DataHeader header;
    CBTF_pcsamp_data data;
    CBTF_PCData buffer;
} tls;

BOOST_AUTO_TEST_CASE(TestBlobXDRConverters)
{
    typedef boost::shared_ptr<CBTF_Protocol_Blob> CBTF_Protocol_BlobPtr;

    boost::shared_ptr<ValueSource<CBTF_Protocol_BlobPtr> > blobinput_value = 
        ValueSource<CBTF_Protocol_BlobPtr>::instantiate();
    Component::Instance blobinput_value_component = 
        boost::reinterpret_pointer_cast<Component>(blobinput_value);
    
    Component::Instance blobxdr_to_mrnet;
    BOOST_CHECK_NO_THROW(
        blobxdr_to_mrnet = Component::instantiate(Type(
            "KrellInstitute::CBTF::ConvertXDRToMRNet<CBTF_Protocol_Blob>"
            ))
        );
    
    Component::Instance blobmrnet_to_xdr;
    BOOST_CHECK_NO_THROW(
        blobmrnet_to_xdr = Component::instantiate(Type(
            "KrellInstitute::CBTF::ConvertMRNetToXDR<CBTF_Protocol_Blob>"
            ))
        );

    boost::shared_ptr<ValueSink<CBTF_Protocol_BlobPtr> > bloboutput_value = 
        ValueSink<CBTF_Protocol_BlobPtr>::instantiate();
    Component::Instance bloboutput_value_component = 
        boost::reinterpret_pointer_cast<Component>(bloboutput_value);


    Component::connect(blobinput_value_component, "value", blobxdr_to_mrnet, "in");
    Component::connect(blobxdr_to_mrnet, "out", blobmrnet_to_xdr, "in");
    Component::connect(blobmrnet_to_xdr, "out", bloboutput_value_component, "value");

    CBTF_Protocol_BlobPtr binput(new CBTF_Protocol_Blob());

    memset(&tls.header, 0, sizeof(CBTF_DataHeader));
    tls.header.experiment = 0;
    tls.header.collector = 1;
    gethostname(tls.header.host, 255);
    tls.header.pid = getpid();
    tls.header.posix_tid = pthread_self();

    tls.data.interval =
        (uint64_t)(1000000000) / (uint64_t)(100);
    tls.data.pc.pc_val = tls.buffer.pc;
    tls.data.count.count_val = tls.buffer.count;

    tls.buffer.addr_begin = ~0;
    tls.buffer.addr_end = 0;
    tls.buffer.length = 0;
    memset(&tls.buffer.hash_table, 0, sizeof(tls.buffer.hash_table));

    UpdatePCData(0xDEADBEEF, &tls.buffer);
    UpdatePCData(0xFEEDBEEF, &tls.buffer);
    UpdatePCData(0xBEEFBEEF, &tls.buffer);
    UpdatePCData(0xBEEFDEAD, &tls.buffer);
    UpdatePCData(0xDEADBEEF, &tls.buffer);
    UpdatePCData(0xDEADBEEF, &tls.buffer);
    UpdatePCData(0xDEADBEEF, &tls.buffer);
    UpdatePCData(0xDEADBEEF, &tls.buffer);

    tls.header.time_begin = 0;
    tls.header.time_end = 0;
    tls.header.addr_begin = tls.buffer.addr_begin;
    tls.header.addr_end = tls.buffer.addr_end;

    std::cerr << "TLS header time begin = " << tls.header.time_begin << std::endl;
    std::cerr << "TLS header time end = " << tls.header.time_end << std::endl;
    std::cerr << "TLS header host = " << tls.header.host << std::endl;
    std::cerr << "TLS header addr_begin = " << tls.header.addr_begin << std::endl;
    std::cerr << "TLS header addr_end = " << tls.header.addr_end << std::endl;

    tls.data.pc.pc_len = tls.buffer.length;
    tls.data.count.count_len = tls.buffer.length;

    const size_t EncodingBufferSize = (15 * 1024);
    unsigned bsize;
    char* sbuffer = NULL;
    XDR xdrs;

    sbuffer = (char*)malloc(EncodingBufferSize);

    xdrmem_create(&xdrs, sbuffer, EncodingBufferSize, XDR_ENCODE);
#if 0
    Assert(xdr_CBTF_DataHeader(&xdrs, &tls.header) == TRUE);
#endif
    Assert((xdr_CBTF_pcsamp_data)(&xdrs, &tls.data) == TRUE);
    Assert(xdr_CBTF_DataHeader(&xdrs, &tls.header) == TRUE);
    bsize = xdr_getpos(&xdrs);
    xdr_destroy(&xdrs);

    binput->data.data_len = bsize;
    binput->data.data_val = (uint8_t*)sbuffer;
    *blobinput_value = binput;
    CBTF_Protocol_BlobPtr bloboutput = *bloboutput_value;

    Blob myblob(bloboutput->data.data_len, bloboutput->data.data_val);


#if 0
    CBTF_DataHeader blobheader;
    memset(&blobheader, 0, sizeof(blobheader));
    unsigned blobheader_size = myblob.getXDRDecoding(
            reinterpret_cast<xdrproc_t>(xdr_CBTF_DataHeader), &blobheader
            );
    std::cerr << "blobheader_size = " << blobheader_size << std::endl;
    std::cerr << "header time begin = " << blobheader.time_begin << std::endl;
    std::cerr << "header time end = " << blobheader.time_end << std::endl;
    std::cerr << "header host = " << blobheader.host << std::endl;
    std::cerr << "header addr_begin = " << blobheader.addr_begin << std::endl;
    std::cerr << "header addr_end = " << blobheader.addr_end << std::endl;
#endif

    CBTF_pcsamp_data blobdata;
    memset(&blobdata, 0, sizeof(blobdata));
    unsigned datasize = myblob.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_CBTF_pcsamp_data), &blobdata);

    std::cerr << "datasize = " << datasize << std::endl;
    std::cerr << "pc length = " << blobdata.pc.pc_len << std::endl;
    std::cerr << "count length = " << blobdata.count.count_len << std::endl;

    AddressBuffer abuffer;

    PCData pcdata;
    pcdata.aggregateAddressCounts(blobdata,abuffer);
}
