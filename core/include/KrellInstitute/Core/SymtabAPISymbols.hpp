////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009 The Krell Institute. All Rights Reserved.
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
 * Declaration of the SymtabAPISymbols class.
 *
 */

#include "KrellInstitute/Core/AddressBuffer.hpp"
#include "KrellInstitute/Core/LinkedObjectEntry.hpp"
#include "KrellInstitute/Core/SymbolTable.hpp"

namespace KrellInstitute { namespace Core {

class SymtabAPISymbols {

    public:
	void    getAllSymbols(const LinkedObjectEntry&, SymbolTable&);
	void    getSymbols(const AddressBuffer&,
			   const LinkedObjectEntry&, SymbolTable&);

    private:

#ifndef NDEBUG
    static bool is_debug_symtabapi_symbols_enabled;
#endif

};

} }