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

/** @file Definition of the FunctionImpl class. */

#include "FunctionImpl.hpp"
#include "LinkedObjectImpl.hpp"

using namespace KrellInstitute::SymbolTable;
using namespace KrellInstitute::SymbolTable::Impl;



//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
FunctionImpl::FunctionImpl(const LinkedObject& linked_object,
                           const std::string& name)
{
    // ...
}



//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
FunctionImpl::FunctionImpl(const FunctionImpl& other)
{
    // ...
}



//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
FunctionImpl::~FunctionImpl()
{
    // ...
}


        
//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
FunctionImpl& FunctionImpl::operator=(const FunctionImpl& other)
{
    if (this != &other)
    {
        // ...
    }
    return *this;
}



//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
bool FunctionImpl::operator<(const FunctionImpl& other) const
{
    // ...
    
    return false;
}



//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
bool FunctionImpl::operator==(const FunctionImpl& other) const
{
    // ...
    
    return false;
}



//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
FunctionImpl FunctionImpl::clone(LinkedObject& linked_object) const
{
    // ...
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
LinkedObject FunctionImpl::getLinkedObject() const
{
    return LinkedObject(new LinkedObjectImpl(dm_symbol_table));
}



//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
std::string FunctionImpl::getMangledName() const
{
    // ...

    return std::string();
}



//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
std::string FunctionImpl::getDemangledName(const bool& all) const
{
    // ...

    return std::string();
}



//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
std::set<AddressRange> FunctionImpl::getAddressRanges() const
{
    // ...

    return std::set<AddressRange>();
}



//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
std::set<Statement> FunctionImpl::getDefinitions() const
{
    // ...

    return std::set<Statement>();
}



//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
std::set<Statement> FunctionImpl::getStatements() const
{
    // ...

    return std::set<Statement>();
}



//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
void FunctionImpl::addAddressRanges(const std::set<AddressRange>& ranges)
{
    // ...
}
