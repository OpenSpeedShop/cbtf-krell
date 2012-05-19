////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2011 The Krell Institute. All Rights Reserved.
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
 * Declaration of the AddressEntry class.
 *
 */

#ifndef _OpenSpeedShop_Framework_AddressEntry_
#define _OpenSpeedShop_Framework_AddressEntry_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <vector>
#include "KrellInstitute/Core/Address.hpp"
#include "KrellInstitute/Core/AddressRange.hpp"



namespace KrellInstitute { namespace Core {

    
    /**
     * Address entry.
     *
     * Representation of a single address.
     *
     */
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

    typedef std::vector<AddressEntry> AddressEntryVec;

} }



#endif
