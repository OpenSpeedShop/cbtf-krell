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

/** @file Definition of the Function class. */

#include <KrellInstitute/SymbolTable/Function.hpp>

#include "FunctionImpl.hpp"

using namespace KrellInstitute::SymbolTable;
using namespace KrellInstitute::SymbolTable::Impl;



//------------------------------------------------------------------------------
// Let the implementation do the real work.
//------------------------------------------------------------------------------
Function::Function(const LinkedObject& linked_object, const std::string& name) :
    dm_impl(new FunctionImpl(linked_object, name))
{
}


        
//------------------------------------------------------------------------------
// Let the implementation do the real work.
//------------------------------------------------------------------------------
Function::Function(const Function& other) :
    dm_impl(new FunctionImpl(*other.dm_impl))
{
}



//------------------------------------------------------------------------------
// Let the implementation do the real work.
//------------------------------------------------------------------------------
Function::~Function()
{
    delete dm_impl;
}


        
//------------------------------------------------------------------------------
// Let the implementation do the real work.
//------------------------------------------------------------------------------
Function& Function::operator=(const Function& other)
{
    *dm_impl = *other.dm_impl;
    return *this;
}



//------------------------------------------------------------------------------
// Let the implementation do the real work.
//------------------------------------------------------------------------------
bool Function::operator<(const Function& other) const
{
    return *dm_impl < *other.dm_impl;
}



//------------------------------------------------------------------------------
// Let the implementation do the real work.
//------------------------------------------------------------------------------
bool Function::operator==(const Function& other) const
{
    return *dm_impl == *other.dm_impl;
}



//------------------------------------------------------------------------------
// Let the implementation do the real work.
//------------------------------------------------------------------------------
LinkedObject Function::getLinkedObject() const
{
    return dm_impl->getLinkedObject();
}



//------------------------------------------------------------------------------
// Let the implementation do the real work.
//------------------------------------------------------------------------------
std::string Function::getMangledName() const
{
    return dm_impl->getMangledName();
}



//------------------------------------------------------------------------------
// Let the implementation do the real work.
//------------------------------------------------------------------------------
std::string Function::getDemangledName(const bool& all) const
{
    return dm_impl->getDemangledName(all);
}



//------------------------------------------------------------------------------
// Let the implementation do the real work.
//------------------------------------------------------------------------------
std::set<AddressRange> Function::getAddressRanges() const
{
    return dm_impl->getAddressRanges();
}



//------------------------------------------------------------------------------
// Let the implementation do the real work.
//------------------------------------------------------------------------------
std::set<Statement> Function::getDefinitions() const
{
    return dm_impl->getDefinitions();
}



//------------------------------------------------------------------------------
// Let the implementation do the real work.
//------------------------------------------------------------------------------
std::set<Statement> Function::getStatements() const
{
    return dm_impl->getStatements();
}



//------------------------------------------------------------------------------
// Let the implementation do the real work.
//------------------------------------------------------------------------------
void Function::addAddressRanges(const std::set<AddressRange>& ranges)
{
    dm_impl->addAddressRanges(ranges);
}
