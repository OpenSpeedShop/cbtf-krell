////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2011 The Krell Institue. All Rights Reserved.
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
 * Definition of the PCHash and Address counts functions.
 *
 */

#include "KrellInstitute/Core/Address.hpp"
#include "KrellInstitute/Core/AddressBuffer.hpp"

using namespace KrellInstitute::Core;


bool AddressBuffer::updateAddressCounts(uint64_t pc, uint64_t count)
{
    Address thePC(pc);

    AddressCounts::iterator lb = addresscounts.lower_bound(thePC);

    if(lb != addresscounts.end() && !(addresscounts.key_comp()(thePC, lb->first))) {
	lb->second += count;
    } else {
	addresscounts.insert(lb, AddressCounts::value_type(thePC, count));
    }

}
