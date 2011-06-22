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

/** @file Plugin used by unit tests for the CBTF MRNet library. */

#include <boost/bind.hpp>
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
#include "KrellInstitute/Core/Path.hpp"
#include "KrellInstitute/Core/PCData.hpp"
#include "KrellInstitute/Core/Time.hpp"
#include "KrellInstitute/Core/ThreadName.hpp"

#include "KrellInstitute/Messages/Address.h"
#include "KrellInstitute/Messages/DataHeader.h"
#include "KrellInstitute/Messages/EventHeader.h"
#include "KrellInstitute/Messages/File.h"
#include "KrellInstitute/Messages/LinkedObjectEvents.h"
#include "KrellInstitute/Messages/PCSamp_data.h"
#include "KrellInstitute/Messages/Thread.h"
#include "KrellInstitute/Messages/ThreadEvents.h"


using namespace KrellInstitute::CBTF;
using namespace KrellInstitute::Core;

namespace { 

    class AddressEntry {
	    public:
	    Address addr;
	    std::string function_name;
	    std::string file;
	    int      line;
	    uint64_t sample_count;
	    double percent;
	    double total_time;

	    bool operator<(AddressEntry rhs) { return sample_count < rhs.sample_count; }
    };
    typedef std::vector<AddressEntry > AddressEntryVec;

    class LinkedObjectEntry {
	//class ThreadName;
	public:
	    ThreadName tname;
	    Time time_loaded;
	    Time time_unloaded;
	    Address addr_begin;
	    Address addr_end;
	    Path path;
	    bool  is_executable;
    };
    typedef std::vector<LinkedObjectEntry > LinkedObjectEntryVec;

    // Sampling interval in nanoseconds.
    uint64_t interval;    
    AddressBuffer abuffer;
}

/**
 * Component that converts an integer value into a MRNet packet.
 */
class __attribute__ ((visibility ("hidden"))) ConvertAddressBufferToPacket :
    public Component
{

public:

    /** Factory function for this component type. */
    static Component::Instance factoryFunction()
    {
        return Component::Instance(
            reinterpret_cast<Component*>(new ConvertAddressBufferToPacket())
            );
    }

private:

    /** Default constructor. */
    ConvertAddressBufferToPacket() :
        Component(Type(typeid(ConvertAddressBufferToPacket)), Version(0, 0, 1))
    {
        declareInput<AddressBuffer>(
            "in", boost::bind(&ConvertAddressBufferToPacket::inHandler, this, _1)
            );
        declareOutput<MRN::PacketPtr>("out");
    }

    /** Handler for the "in" input.*/
    void inHandler(const AddressBuffer& in)
    {

	AddressCounts ac = in.addresscounts;
	int bufsize = ac.size();
	uint64_t * addr = NULL;
	uint64_t * counts = NULL;

	addr = reinterpret_cast<uint64_t*>(
            malloc(bufsize * sizeof(uint64_t*))
            );
	counts = reinterpret_cast<uint64_t*>(
            malloc(bufsize * sizeof(uint64_t*))
            );
	
	AddressCounts::iterator i;
	int j = 0;
	for (i = ac.begin(); i != ac.end(); ++i) {
	    addr[j] = i->first.getValue();
	    counts[j] = i->second;
	    j++;
	}

        emitOutput<MRN::PacketPtr>(
            "out", MRN::PacketPtr(new MRN::Packet(0, 0, "%auld %auld", addr,bufsize,counts,bufsize))
            );
    }
    
}; // class ConvertAddressBufferToPacket

KRELL_INSTITUTE_CBTF_REGISTER_FACTORY_FUNCTION(ConvertAddressBufferToPacket)



/**
 * Component that converts a MRNet packet into an integer value.
 */
class __attribute__ ((visibility ("hidden"))) ConvertPacketToAddressBuffer :
    public Component
{

public:

    /** Factory function for this component type. */
    static Component::Instance factoryFunction()
    {
        return Component::Instance(
            reinterpret_cast<Component*>(new ConvertPacketToAddressBuffer())
            );
    }

private:

    /** Default constructor. */
    ConvertPacketToAddressBuffer() :
        Component(Type(typeid(ConvertPacketToAddressBuffer)), Version(0, 0, 1))
    {
        declareInput<MRN::PacketPtr>(
            "in", boost::bind(&ConvertPacketToAddressBuffer::inHandler, this, _1)
            );
        declareOutput<AddressBuffer>("out");
    }

    /** Handler for the "in" input.*/
    void inHandler(const MRN::PacketPtr& in)
    {
        AddressBuffer out;
	uint64_t *addr = NULL;
	uint64_t *counts = NULL;
	int addrsize = 0;
	int countssize = 0;


        in->unpack("%auld %auld", &addr, &addrsize, &counts, &countssize);

	// TODO: error handling

	for (int i= 0; i < addrsize; ++i) {
	    out.updateAddressCounts(addr[i],counts[i]);
	}

        emitOutput<AddressBuffer>("out", out);
    }
    
}; // class ConvertPacketToAddressBuffer

KRELL_INSTITUTE_CBTF_REGISTER_FACTORY_FUNCTION(ConvertPacketToAddressBuffer)

/**
 * Component that aggregates PC address values and their counts.
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
        declareInput<boost::shared_ptr<CBTF_pcsamp_data> >(
            "in1", boost::bind(&AddressAggregator::pcsampHandler, this, _1)
            );
        declareInput<AddressBuffer>(
            "in2", boost::bind(&AddressAggregator::addressBufferHandler, this, _1)
            );
        declareInput<Blob>(
            "in3", boost::bind(&AddressAggregator::blobHandler, this, _1)
            );
        declareInput<boost::shared_ptr<CBTF_Protocol_Blob> >(
            "in4",
            boost::bind(
                &AddressAggregator::cbtf_protocol_blob_Handler, this, _1
                )
            );
        declareOutput<AddressBuffer>("Aggregatorout");
    }

    /** Handler for the "in1" input.*/
    void pcsampHandler(const boost::shared_ptr<CBTF_pcsamp_data>& in)
    {
        CBTF_pcsamp_data *data = in.get();


	interval = data->interval;

	PCData pcdata;
	pcdata.aggregateAddressCounts(*data,abuffer);

        emitOutput<AddressBuffer>("Aggregatorout",  abuffer);
    }

    /** Handler for the "in4" input.*/
    void cbtf_protocol_blob_Handler(const boost::shared_ptr<CBTF_Protocol_Blob>& in)
    {
        CBTF_Protocol_Blob *B = in.get();
	Blob myblob(B->data.data_len, B->data.data_val);

	if (in->data.data_len == 0 ) {
	    std::cerr << "EXIT AddressAggregator::cbtf_protocol_blob_Handler: data length 0" << std::endl;
	    abort();
	}

// FIXME: can not decode both xdr messages.
// Only the first xdr is decoded properly (CBTF_DataHeader).
//

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
	pcdata.aggregateAddressCounts(data,abuffer);
        emitOutput<AddressBuffer>("Aggregatorout",  abuffer);
    }

    /** Handler for the "in2" input.*/
    void addressBufferHandler(const AddressBuffer& in)
    {
        emitOutput<AddressBuffer>("Aggregatorout",  abuffer);
    }

    /** Handler for the "in3" input.*/
    void blobHandler(const Blob& in)
    {

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
	pcdata.aggregateAddressCounts(data,abuffer);
        emitOutput<AddressBuffer>("Aggregatorout",  abuffer);
    }

}; // class AddressAggregator

KRELL_INSTITUTE_CBTF_REGISTER_FACTORY_FUNCTION(AddressAggregator)

/**
 * Component that aggregates PC address values and their counts.
 */
class __attribute__ ((visibility ("hidden"))) DisplayAddressBuffer :
    public Component
{

public:

    /** Factory function for this component type. */
    static Component::Instance factoryFunction()
    {
        return Component::Instance(
            reinterpret_cast<Component*>(new DisplayAddressBuffer())
            );
    }

private:

    /** Default constructor. */
    DisplayAddressBuffer() :
        Component(Type(typeid(DisplayAddressBuffer)), Version(0, 0, 1))
    {
        declareInput<AddressBuffer>(
            "in", boost::bind(&DisplayAddressBuffer::displayHandler, this, _1)
            );

        declareOutput<AddressBuffer>("displayout");
    }

    /** Handler for the "in" input.*/
    void displayHandler(const AddressBuffer& in)
    {
	AddressCounts ac = in.addresscounts;
	AddressCounts::iterator aci;
	uint64_t total_counts = 0;
	double percent_total = 0.0;
	double total_time = 0.0;

	AddressEntryVec m;

	// compute total samples over all addresses.
	for (aci = ac.begin(); aci != ac.end(); ++aci) {
	    total_counts += aci->second;
	}

	// compute percent of total for each address.
	for (aci = ac.begin(); aci != ac.end(); ++aci) {
	    double percent = (double) 100 * ((double)aci->second/(double)total_counts);
	    percent_total += percent;

	    AddressEntry fe;
	    fe.addr = aci->first;
	    fe.sample_count = aci->second;
	    fe.line = -1;
	    fe.file = "no file found";
	    fe.function_name = "no funcion name found";
	    fe.total_time = static_cast<double>(aci->second) * static_cast<double>(interval) / 1000000000.0;
	    total_time += fe.total_time;
	    fe.percent = percent;
	    m.push_back(fe);
	}

	// display each address and it's percent of total counts in
	// descending order.
	AddressEntryVec::reverse_iterator mi;
	// HMM. having issues sorting this vector.
	//m.sort();
	//std::sort(m.begin(),m.end());
	for (mi = m.rbegin(); mi != m.rend(); ++mi) {
            std::cout << "Address " << mi->addr
        	<< " had " << mi->percent << " percent of samples "
        	<< " and " << mi->total_time << " total time "
		<< std::endl;
	}
	std::cout << "\ntotal samples: " << total_counts
	    << " percent of total samples: " << percent_total
	    << " total time: " << total_time
	    << "\n" << std::endl;

        emitOutput<AddressBuffer>("displayout",  abuffer);
    }

}; // class DisplayAddressBuffer

KRELL_INSTITUTE_CBTF_REGISTER_FACTORY_FUNCTION(DisplayAddressBuffer)

/**
 * Component that records linked objects.
 */
class __attribute__ ((visibility ("hidden"))) LinkedObject :
    public Component
{

public:

    /** Factory function for this component type. */
    static Component::Instance factoryFunction()
    {
        return Component::Instance(
            reinterpret_cast<Component*>(new LinkedObject())
            );
    }

private:

    /** Default constructor. */
    LinkedObject() :
        Component(Type(typeid(LinkedObject)), Version(0, 0, 1))
    {
        declareInput<boost::shared_ptr<CBTF_Protocol_LoadedLinkedObject> >(
            "loaded", boost::bind(&LinkedObject::loadedHandler, this, _1)
            );
        declareInput<boost::shared_ptr<CBTF_Protocol_UnloadedLinkedObject> >(
            "unloaded", boost::bind(&LinkedObject::unloadedHandler, this, _1)
            );
    }

    /** Handlers for the inputs.*/
    void loadedHandler(const boost::shared_ptr<CBTF_Protocol_LoadedLinkedObject>& in)
    {
        CBTF_Protocol_LoadedLinkedObject *message = in.get();
	LinkedObjectEntry entry;

	for(int i = 0; i < message->threads.names.names_len; ++i) {
	    const CBTF_Protocol_ThreadName& msg_thread =
				message->threads.names.names_val[i];

	    ThreadName tname(msg_thread);
	    entry.tname = tname;
	    entry.path = message->linked_object.path;
	    entry.addr_begin = message->range.begin;
	    entry.addr_end = message->range.end;
	    entry.is_executable = message->is_executable;
	    entry.time_loaded = message->time;

#if 0
std::cerr << "path " << entry.path
	<< " loaded at time " << entry.time_loaded
	<< " at " << AddressRange(entry.addr_begin,entry.addr_end)
	<< " in thread " << entry.tname.getHost()
	<< ":" << entry.tname.getPid().second
	<< ":" <<  entry.tname.getPosixThreadId().second
	<< std::endl;
#endif
	}
    }

    void unloadedHandler(const boost::shared_ptr<CBTF_Protocol_UnloadedLinkedObject>& in)
    {
    }

}; // class LinkedObject

KRELL_INSTITUTE_CBTF_REGISTER_FACTORY_FUNCTION(LinkedObject)

/**
 * Component that handles thread state,
 */
class __attribute__ ((visibility ("hidden"))) ThreadsStateChanged :
    public Component
{

public:

    /** Factory function for this component type. */
    static Component::Instance factoryFunction()
    {
        return Component::Instance(
            reinterpret_cast<Component*>(new ThreadsStateChanged())
            );
    }

private:

    /** Default constructor. */
    ThreadsStateChanged() :
        Component(Type(typeid(ThreadsStateChanged)), Version(0, 0, 1))
    {
        declareInput<boost::shared_ptr<CBTF_Protocol_ThreadsStateChanged> >(
            "in", boost::bind(&ThreadsStateChanged::inHandler, this, _1)
            );
    }

    /** Handlers for the inputs.*/
    void inHandler(const boost::shared_ptr<CBTF_Protocol_ThreadsStateChanged>& in)
    {
        CBTF_Protocol_ThreadsStateChanged *message = in.get();
	for(int i = 0; i < message->threads.names.names_len; ++i) {
	    const CBTF_Protocol_ThreadName& msg_thread =
				message->threads.names.names_val[i];

	ThreadName tname(msg_thread);
	std::cerr << "thread " << tname.getHost()
	<< ":" << tname.getPid().second
	<< ":" <<  (uint64_t) tname.getPosixThreadId().second
	<< ":" <<  tname.getMPIRank().second
	<< " ThreadState: " << message->state
	<< std::endl;
	}

    }

}; // class ThreadsStateChanged

KRELL_INSTITUTE_CBTF_REGISTER_FACTORY_FUNCTION(ThreadsStateChanged)
