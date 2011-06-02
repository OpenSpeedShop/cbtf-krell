////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
// Copyright (c) 2007 William Hachfeld. All Rights Reserved.
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
 * Declaration of the Function class.
 *
 */

#ifndef _KrellInstitute_Core_Function_
#define _KrellInstitute_Core_Function_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif


#include <set>
#include <string>



namespace KrellInstitute { namespace Core {

    class FunctionCache;
    class LinkedObject;
    class Statement;
    
    /**
     * Source code function.
     *
     * Representation of a source code function. Provides member functions for
     * requesting information about this function, where it is contained, and
     * what it contains.
     *
     * @ingroup CollectorAPI ToolAPI
     */
    class Function
    {
	friend class FunctionCache;
	friend class LinkedObject;
	friend class Statement;
	
    public:
	
	LinkedObject getLinkedObject() const;

	// Used by Experiment::compressDB to prune an KrellInstitute database of
	// any entries not found in the experiments sampled addresses.
	AddressRange getAddressRange() const;
	
	std::string getName() const;
	std::string getMangledName() const;
	std::string getDemangledName(const bool& = true) const;
	
	std::set<Statement> getDefinitions() const;
	std::set<Statement> getStatements() const;

    private:

	static FunctionCache TheCache;
	
	Function();
	
    };
    
} }



#endif
