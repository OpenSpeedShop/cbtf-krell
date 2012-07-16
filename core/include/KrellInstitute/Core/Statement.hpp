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
 * Declaration of the Statement class.
 *
 */

#ifndef _KrellInstitute_Core_Statement_
#define _KrellInstitute_Core_Statement_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif


#include "KrellInstitute/Core/Address.hpp"
#include "KrellInstitute/Core/AddressRange.hpp"
#include "KrellInstitute/Core/Path.hpp"

#include <vector>
#include <set>
#include <string>



namespace KrellInstitute { namespace Core {

    class AddressRange;
    class Path;
    
    /**
     *
     */
    class Statement
    {
	
    public:

	AddressRange getAddressRange() const;
	Path getSourceFile() const;
	int getLine() const;
	int getColumn() const;

    private:

	Statement();

        Path _sourceFile;
        AddressRange _range;
	int _lineNum;
	int _column;
	
    };
    
} }



#endif
