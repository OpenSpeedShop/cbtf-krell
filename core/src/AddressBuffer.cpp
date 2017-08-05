////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2011-2014 The Krell Institue. All Rights Reserved.
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

/** @file
 *
 * Definition of AddressBuffer functions.
 *
 */

#include "KrellInstitute/Core/Address.hpp"
#include "KrellInstitute/Core/AddressEntry.hpp"
#include "KrellInstitute/Core/AddressBuffer.hpp"

using namespace KrellInstitute::Core;


void AddressBuffer::printResults() const {

	    //std::cout << "DisplayAddressBuffer, printResults interval is " << interval << std::endl;
	    AddressCounts::const_iterator aci;
	    uint64_t total_counts = 0;
	    double percent_total = 0.0;
	    double total_time = 0.0;

	    AddressEntryVec m;

	    // compute total samples over all addresses.
	    for (aci = addresscounts.begin(); aci != addresscounts.end();
								    ++aci) {
	        total_counts += aci->second;
	    }

	    // compute percent of total for each address.
	    for (aci = addresscounts.begin(); aci != addresscounts.end();
								    ++aci) {
	        double percent = (double) 100 * ((double)aci->second/(double)total_counts);
	        percent_total += percent;

	        AddressEntry entry;
	        entry.addr = aci->first;
	        entry.sample_count = aci->second;
	        entry.line = -1;
	        entry.file = "no file found";
	        entry.function_name = "no funcion name found";
#if 1
	        entry.total_time = static_cast<double>(aci->second) *
				static_cast<double>(100) / 1000000000.0;
	        total_time += entry.total_time;
#endif
	        entry.percent = percent;
	        m.push_back(entry);
	    }

	    // display each address and it's percent of total counts
	    AddressEntryVec::iterator mi;
	
	    for (mi = m.begin(); mi != m.end(); ++mi) {
	      if (mi->sample_count > 0 ) {
                std::cout << "Address " << mi->addr
        	<< ": " << mi->percent << "% of unique sampled addresses"
		<< std::endl;
	      }
	    }
	    std::cout << "\ntotal unique sampled addresses: " << total_counts
	    << "\n" << std::endl;
}

// FIXME: These methods return bool but are not returning as expected
// Do the callsites use a return value?
bool AddressBuffer::updateAddressCounts(uint64_t pc, uint64_t count)
{
    if (pc == 0) {
	return false;
    }

    Address thePC(pc);

    AddressCounts::iterator lb = addresscounts.lower_bound(thePC);

    if(lb != addresscounts.end() && !(addresscounts.key_comp()(thePC, lb->first))) {
	lb->second += count;
    } else {
	addresscounts.insert(lb, AddressCounts::value_type(thePC, count));
    }

}

bool AddressBuffer::updateAddressCounts(AddressBuffer& buf)
{
  AddressCounts::const_iterator aci;
  for (aci = buf.addresscounts.begin(); aci != buf.addresscounts.end(); ++aci) {

    AddressCounts::iterator lb = addresscounts.lower_bound(aci->first);

    if(lb != addresscounts.end() && !(addresscounts.key_comp()(aci->first, lb->first))) {
	lb->second += aci->second;
    } else {
	addresscounts.insert(lb, AddressCounts::value_type(aci->first, aci->second));
    }
  }
}

bool AddressBuffer::updateAddressCounts(AddressCounts& addrcounts)
{
  AddressCounts::const_iterator aci;
  for (aci = addrcounts.begin(); aci != addrcounts.end(); ++aci) {

    AddressCounts::iterator lb = addresscounts.lower_bound(aci->first);

    if(lb != addresscounts.end() && !(addresscounts.key_comp()(aci->first, lb->first))) {
	lb->second += aci->second;
    } else {
	addresscounts.insert(lb, AddressCounts::value_type(aci->first, aci->second));
    }
  }
}
