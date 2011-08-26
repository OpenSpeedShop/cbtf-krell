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

/** @file Plugin for usertime collection. */

#include <boost/bind.hpp>
#include <boost/operators.hpp>
#include <mrnet/MRNet.h>
#include <typeinfo>
#include <algorithm>

#include <KrellInstitute/CBTF/Component.hpp>
#include <KrellInstitute/CBTF/Type.hpp>
#include <KrellInstitute/CBTF/Version.hpp>

#include "KrellInstitute/Core/AddressBuffer.hpp"

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

    void printResults( const AddressCounts& ac) {

	    AddressCounts::const_iterator aci;
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

	        AddressEntry entry;
	        entry.addr = aci->first;
	        entry.sample_count = aci->second;
	        entry.line = -1;
	        entry.file = "no file found";
	        entry.function_name = "no funcion name found";
#if 0
	        entry.total_time = static_cast<double>(aci->second) *
				static_cast<double>(interval) / 1000000000.0;
#endif
	        total_time += entry.total_time;
	        entry.percent = percent;
	        m.push_back(entry);
	    }

	    // display each address and it's percent of total counts
	    AddressEntryVec::iterator mi;
	
	    for (mi = m.begin(); mi != m.end(); ++mi) {
	      if (mi->sample_count > 0 ) {
                std::cout << "Address " << mi->addr
        	<< " had %" << mi->percent << " of samples "
        	<< " and " << mi->total_time << " of total time "
		<< std::endl;
	      }
	    }
	    std::cout << "\ntotal samples: " << total_counts
	    << "\npercent of total samples: " << percent_total
	    << "\ntotal time: " << total_time
	    << "\n" << std::endl;
    }
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
	std::cout << "Intermediate aggregated results" << std::endl;
	printResults(in.addresscounts);
        emitOutput<AddressBuffer>("displayout",  in);
    }

}; // class DisplayAddressBuffer

KRELL_INSTITUTE_CBTF_REGISTER_FACTORY_FUNCTION(DisplayAddressBuffer)
