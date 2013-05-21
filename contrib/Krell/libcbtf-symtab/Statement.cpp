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

/** @file Definition of the Statement class. */

#include <KrellInstitute/SymbolTable/Statement.hpp>

#include "StatementImpl.hpp"

using namespace KrellInstitute::SymbolTable;
using namespace KrellInstitute::SymbolTable::Impl;



//------------------------------------------------------------------------------
// Let the implementation do the real work.
//------------------------------------------------------------------------------
Statement::Statement(const LinkedObject& linked_object,
                     const boost::filesystem::path& path,
                     const unsigned int& line,
                     const unsigned int& column) :
    dm_impl(new StatementImpl(linked_object, path, line, column))
{
}


        
//------------------------------------------------------------------------------
// Let the implementation do the real work.
//------------------------------------------------------------------------------
Statement::Statement(const Statement& other) :
    dm_impl(new StatementImpl(*other.dm_impl))
{
}



//------------------------------------------------------------------------------
// Let the implementation do the real work.
//------------------------------------------------------------------------------
Statement::~Statement()
{
    delete dm_impl;
}


        
//------------------------------------------------------------------------------
// Let the implementation do the real work.
//------------------------------------------------------------------------------
Statement& Statement::operator=(const Statement& other)
{
    *dm_impl = *other.dm_impl;
    return *this;
}



//------------------------------------------------------------------------------
// Let the implementation do the real work.
//------------------------------------------------------------------------------
bool Statement::operator<(const Statement& other) const
{
    return *dm_impl < *other.dm_impl;
}



//------------------------------------------------------------------------------
// Let the implementation do the real work.
//------------------------------------------------------------------------------
bool Statement::operator==(const Statement& other) const
{
    return *dm_impl == *other.dm_impl;
}



//------------------------------------------------------------------------------
// Let the implementation do the real work.
//------------------------------------------------------------------------------
LinkedObject Statement::getLinkedObject() const
{
    return dm_impl->getLinkedObject();
}



//------------------------------------------------------------------------------
// Let the implementation do the real work.
//------------------------------------------------------------------------------
boost::filesystem::path Statement::getPath() const
{
    return dm_impl->getPath();
}



//------------------------------------------------------------------------------
// Let the implementation do the real work.
//------------------------------------------------------------------------------
unsigned int Statement::getLine() const
{
    return dm_impl->getLine();
}



//------------------------------------------------------------------------------
// Let the implementation do the real work.
//------------------------------------------------------------------------------
unsigned int Statement::getColumn() const
{
    return dm_impl->getColumn();
}



//------------------------------------------------------------------------------
// Let the implementation do the real work.
//------------------------------------------------------------------------------
std::set<AddressRange> Statement::getAddressRanges() const
{
    return dm_impl->getAddressRanges();
}



//------------------------------------------------------------------------------
// Let the implementation do the real work.
//------------------------------------------------------------------------------
std::set<Function> Statement::getFunctions() const
{
    return dm_impl->getFunctions();
}



//------------------------------------------------------------------------------
// Let the implementation do the real work.
//------------------------------------------------------------------------------
void Statement::addAddressRanges(const std::set<AddressRange>& ranges)
{
    dm_impl->addAddressRanges(ranges);
}
