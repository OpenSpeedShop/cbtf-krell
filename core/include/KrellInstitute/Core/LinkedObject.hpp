////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
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
 * Declaration of the LinkedObject class.
 *
 */

#ifndef _KrellInsitute_Core_LinkedObject_
#define _KrellInsitute_Core_LinkedObject_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "KrellInstitute/Core/AddressRange.hpp"

#include <set>



namespace KrellInsitute { namespace Core {

    class AddressSpace;
    class Function;
    class FunctionCache;
    class Path;
    class Statement;
    class StatementCache;
    
    /**
     * Linked object.
     *
     * Representation of a single executable or library (a "linked object").
     * Provides member functions for requesting information about this linked
     * object, where it is contained, and what it contains.
     *
     * @ingroup CollectorAPI ToolAPI
     */
    class LinkedObject
    {
	friend class AddressSpace;
	friend class Function;
	friend class FunctionCache;
	friend class Statement;
	friend class StatementCache;
	
    public:

	Path getPath() const;
	bool isExecutable() const;
	
	std::set<Function> getFunctions() const;
	std::set<Statement> getStatements() const;

	// Used by the Offline Experiment BFDSymbol code.
	std::set<AddressRange> getAddressRange() const;

    private:

	LinkedObject();
	
    };
    
} }



#endif
