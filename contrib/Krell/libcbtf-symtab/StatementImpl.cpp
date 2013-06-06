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

/** @file Definition of the StatementImpl class. */

#include "LinkedObjectImpl.hpp"
#include "StatementImpl.hpp"

using namespace KrellInstitute::SymbolTable;
using namespace KrellInstitute::SymbolTable::Impl;



//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
StatementImpl::StatementImpl(const LinkedObject& linked_object,
                             const boost::filesystem::path& path,
                             const unsigned int& line,
                             const unsigned int& column)
{
    // ...
}


        
//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
StatementImpl::StatementImpl(const StatementImpl& other)
{
    // ...
}



//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
StatementImpl::~StatementImpl()
{
    // ...
}


        
//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
StatementImpl& StatementImpl::operator=(const StatementImpl& other)
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
bool StatementImpl::operator<(const StatementImpl& other) const
{
    // ...
    
    return false;
}



//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
bool StatementImpl::operator==(const StatementImpl& other) const
{
    // ...

    return false;
}



//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
StatementImpl StatementImpl::clone(LinkedObject& linked_object) const
{
    // ...
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
LinkedObject StatementImpl::getLinkedObject() const
{
    return LinkedObject(new LinkedObjectImpl(dm_symbol_table));
}



//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
boost::filesystem::path StatementImpl::getPath() const
{
    // ...

    return boost::filesystem::path();
}



//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
unsigned int StatementImpl::getLine() const
{
    // ...

    return 0;
}



//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
unsigned int StatementImpl::getColumn() const
{
    // ...

    return 0;
}



//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
std::set<AddressRange> StatementImpl::getAddressRanges() const
{
    // ...

    return std::set<AddressRange>();
}



//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
std::set<Function> StatementImpl::getFunctions() const
{
    // ...

    return std::set<Function>();
}



//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
void StatementImpl::addAddressRanges(const std::set<AddressRange>& ranges)
{
    // ...
}
