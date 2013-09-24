////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2013 Krell Institute. All Rights Reserved.
//
// This program is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free Software
// Foundation; either version 2 of the License, or (at your option) any later
// version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
// details.
//
// You should have received a copy of the GNU General Public License along with
// this program; if not, write to the Free Software Foundation, Inc., 59 Temple
// Place, Suite 330, Boston, MA  02111-1307  USA
////////////////////////////////////////////////////////////////////////////////

/** @file Definition of the Resolver class. */

#include <KrellInstitute/Base/Raise.hpp>
#include <KrellInstitute/SymbolTable/Resolver.hpp>
#include <stdexcept>

#if defined(SYMTABAPI_FOUND)
#include "SymtabAPIResolver.hpp"
#endif

using namespace KrellInstitute::SymbolTable;
using namespace KrellInstitute::SymbolTable::Impl;



//------------------------------------------------------------------------------
// Use the SymtabAPI resolver if it is available, otherwise throw an exception.
//------------------------------------------------------------------------------
Resolver::Handle Resolver::instantiate(AddressSpaces& spaces)
{
#if defined(SYMTABAPI_FOUND)
    return Handle(new SymtabAPIResolver(spaces));
#else
    raise<std::runtime_error>("There are no resolvers available.");
#endif    
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
Resolver::~Resolver()
{
}
