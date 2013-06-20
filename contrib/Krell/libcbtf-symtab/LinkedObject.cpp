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

/** @file Definition of the LinkedObject class. */

#include <KrellInstitute/SymbolTable/LinkedObject.hpp>

#include "LinkedObjectImpl.hpp"

using namespace KrellInstitute::SymbolTable;
using namespace KrellInstitute::SymbolTable::Impl;



//------------------------------------------------------------------------------
// Let the implementation do the real work.
//------------------------------------------------------------------------------
LinkedObject::LinkedObject(const boost::filesystem::path& path) :
    dm_impl(new LinkedObjectImpl(path))
{
}


        
//------------------------------------------------------------------------------
// Let the implementation do the real work.
//------------------------------------------------------------------------------
LinkedObject::LinkedObject(const CBTF_Protocol_SymbolTable& message) :
    dm_impl(new LinkedObjectImpl(message))
{
}



//------------------------------------------------------------------------------
// Let the implementation do the real work.
//------------------------------------------------------------------------------
LinkedObject::LinkedObject(const LinkedObject& other) :
    dm_impl(new LinkedObjectImpl(*other.dm_impl))
{
}



//------------------------------------------------------------------------------
// Let the implementation do the real work.
//------------------------------------------------------------------------------
LinkedObject::~LinkedObject()
{
    delete dm_impl;
}


        
//------------------------------------------------------------------------------
// Let the implementation do the real work.
//------------------------------------------------------------------------------
LinkedObject& LinkedObject::operator=(const LinkedObject& other)
{
    *dm_impl = *other.dm_impl;
    return *this;
}



//------------------------------------------------------------------------------
// Let the implementation do the real work.
//------------------------------------------------------------------------------
bool LinkedObject::operator<(const LinkedObject& other) const
{
    return *dm_impl < *other.dm_impl;
}



//------------------------------------------------------------------------------
// Let the implementation do the real work.
//------------------------------------------------------------------------------
bool LinkedObject::operator==(const LinkedObject& other) const
{
    return *dm_impl == *other.dm_impl;
}



//------------------------------------------------------------------------------
// Let the implementation do the real work.
//------------------------------------------------------------------------------
LinkedObject::operator CBTF_Protocol_SymbolTable() const
{
    return *dm_impl;
}



//------------------------------------------------------------------------------
// Let the implementation do the real work.
//------------------------------------------------------------------------------
LinkedObject LinkedObject::clone() const
{
    return LinkedObject(dm_impl->clone());
}



//------------------------------------------------------------------------------
// Let the implementation do the real work.
//------------------------------------------------------------------------------
boost::filesystem::path LinkedObject::getPath() const
{
    return dm_impl->getPath();
}



//------------------------------------------------------------------------------
// Let the implementation do the real work.
//------------------------------------------------------------------------------
boost::uint64_t LinkedObject::getChecksum() const
{
    return dm_impl->getChecksum();
}



//------------------------------------------------------------------------------
// Let the implementation do the real work.
//------------------------------------------------------------------------------
void LinkedObject::visitFunctions(FunctionVisitor& visitor) const
{
    dm_impl->visitFunctions(visitor);
}



//------------------------------------------------------------------------------
// Let the implementation do the real work.
//------------------------------------------------------------------------------
void LinkedObject::visitFunctionsAt(const Address& address,
                                    FunctionVisitor& visitor) const
{
    dm_impl->visitFunctionsAt(address, visitor);
}



//------------------------------------------------------------------------------
// Let the implementation do the real work.
//------------------------------------------------------------------------------
void LinkedObject::visitFunctionsByName(const std::string& name,
                                        FunctionVisitor& visitor) const
{
    dm_impl->visitFunctionsByName(name, visitor);
}



//------------------------------------------------------------------------------
// Let the implementation do the real work.
//------------------------------------------------------------------------------
void LinkedObject::visitStatements(StatementVisitor& visitor) const
{
    dm_impl->visitStatements(visitor);
}



//------------------------------------------------------------------------------
// Let the implementation do the real work.
//------------------------------------------------------------------------------
void LinkedObject::visitStatementsAt(const Address& address,
                                     StatementVisitor& visitor) const
{
    dm_impl->visitStatementsAt(address, visitor);
}



//------------------------------------------------------------------------------
// Let the implementation do the real work.
//------------------------------------------------------------------------------
void LinkedObject::visitStatementsBySourceFile(
    const boost::filesystem::path& path, StatementVisitor& visitor
    ) const
{
    dm_impl->visitStatementsBySourceFile(path, visitor);
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
LinkedObject::LinkedObject(Impl::LinkedObjectImpl* impl) :
    dm_impl(impl)
{
}
