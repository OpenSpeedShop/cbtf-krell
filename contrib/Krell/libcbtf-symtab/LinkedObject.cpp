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

#include <boost/format.hpp>
#include <KrellInstitute/SymbolTable/LinkedObject.hpp>
#include <sstream>

#include "SymbolTable.hpp"

using namespace KrellInstitute::SymbolTable;



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
LinkedObject::LinkedObject(const FileName& name) :
    dm_symbol_table(new Impl::SymbolTable(name))
{
}


        
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
LinkedObject::LinkedObject(const CBTF_Protocol_SymbolTable& message) :
    dm_symbol_table(new Impl::SymbolTable(message))
{
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
LinkedObject::operator CBTF_Protocol_SymbolTable() const
{
    return *dm_symbol_table;
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
LinkedObject::operator std::string() const
{
    std::ostringstream stream;
    stream << *this;
    return stream.str();    
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool LinkedObject::operator<(const LinkedObject& other) const
{
    return dm_symbol_table < other.dm_symbol_table;
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool LinkedObject::operator==(const LinkedObject& other) const
{
    return dm_symbol_table == other.dm_symbol_table;
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
LinkedObject LinkedObject::clone() const
{
    return LinkedObject(
        Impl::SymbolTable::Handle(new Impl::SymbolTable(*dm_symbol_table))
        );
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
FileName LinkedObject::getName() const
{
    return dm_symbol_table->getName();
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void LinkedObject::visitFunctions(const FunctionVisitor& visitor) const
{
    dm_symbol_table->visitFunctions(visitor);
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void LinkedObject::visitFunctions(const AddressRange& range,
                                  const FunctionVisitor& visitor) const
{
    dm_symbol_table->visitFunctions(range, visitor);
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void LinkedObject::visitStatements(const StatementVisitor& visitor) const
{
    dm_symbol_table->visitStatements(visitor);
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void LinkedObject::visitStatements(const AddressRange& range,
                                   const StatementVisitor& visitor) const
{
    dm_symbol_table->visitStatements(range, visitor);
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
std::ostream& KrellInstitute::SymbolTable::operator<<(
    std::ostream& stream,
    const LinkedObject& linked_object
    )
{
    stream << boost::str(boost::format("SymbolTable 0x%016X") % 
                         linked_object.dm_symbol_table.get());
    
    return stream;
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
LinkedObject::LinkedObject(Impl::SymbolTable::Handle symbol_table) :
    dm_symbol_table(symbol_table)
{
}
