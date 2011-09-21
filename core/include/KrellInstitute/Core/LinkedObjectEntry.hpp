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
 * Declaration of the LinkedObjectEntry class.
 *
 */

#ifndef _OpenSpeedShop_Framework_LinkedObjectEntry_
#define _OpenSpeedShop_Framework_LinkedObjectEntry_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "KrellInstitute/Core/Address.hpp"
#include "KrellInstitute/Core/AddressRange.hpp"
#include "KrellInstitute/Core/Time.hpp"
#include "KrellInstitute/Core/Path.hpp"
#include "KrellInstitute/Core/ThreadName.hpp"

#include <vector>



namespace KrellInstitute { namespace Core {

    class AddressSpace;
    class Path;
    
    /**
     * Linked object.
     *
     * Representation of a single executable or library (a "linked object").
     * Provides member functions for requesting information about this linked
     * object, where it is contained, and what it contains.
     *
     * @ingroup CollectorAPI ToolAPI
     */
    class LinkedObjectEntry
    {
	friend class AddressSpace;
	
    public:

	Path getPath() const;
	bool isExecutable() const;
	
	//std::set<AddressRange> getAddressRange() const;
	AddressRange getAddressRange() const;


	ThreadName tname;
        Time time_loaded;
        Time time_unloaded;
        Address addr_begin;
        Address addr_end;
        Path path;
        bool  is_executable;

	LinkedObjectEntry();

    };
    
    typedef std::vector<LinkedObjectEntry > LinkedObjectEntryVec;
} }



#endif
