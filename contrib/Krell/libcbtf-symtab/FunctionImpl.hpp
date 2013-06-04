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

/** @file Declaration of the FunctionImpl class. */

#pragma once

#include <KrellInstitute/SymbolTable/AddressRange.hpp>
#include <KrellInstitute/SymbolTable/LinkedObject.hpp>
#include <KrellInstitute/SymbolTable/Statement.hpp>
#include <set>
#include <string>

#include "SymbolTable.hpp"

namespace KrellInstitute { namespace SymbolTable { namespace Impl {

    /**
     * Implementation details of the Function class. Anything that
     * would normally be a private member of Function is instead a
     * member of FunctionImpl.
     */
    class FunctionImpl
    {
        
    public:

        /**
         * Construct a function witin the given linked object from its mangled
         * name.
         */
        FunctionImpl(const LinkedObject& linked_object,
                     const std::string& name);
        
        /** Construct a function from an existing function. */
        FunctionImpl(const FunctionImpl& other);
        
        /** Destructor. */
        virtual ~FunctionImpl();
        
        /** Replace this function with a copy of another one. */
        FunctionImpl& operator=(const FunctionImpl& other);

        /** Is this function less than another one? */
        bool operator<(const FunctionImpl& other) const;

        /** Is this function equal to another one? */
        bool operator==(const FunctionImpl& other) const;
        
        /** Get the linked object containing this function. */
        LinkedObject getLinkedObject() const;

        /** Get the mangled name of this function. */
        std::string getMangledName() const;
        
        /** Get the demangled name of this function. */
        std::string getDemangledName(const bool& all = true) const;

        /** Get the address ranges associated with this function. */
        std::set<AddressRange> getAddressRanges() const;
        
        /** Get the definitions of this function. */
        std::set<Statement> getDefinitions() const;

        /** Get the statements associated with this function. */
        std::set<Statement> getStatements() const;

        /** Associate the given address ranges with this function. */
        void addAddressRanges(const std::set<AddressRange>& ranges);
        
    private:

        /** Symbol table containing this function. */
        SymbolTable::Handle dm_symbol_table;
        
        // ...
        
    }; // class FunctionImpl

} } } // namespace KrellInstitute::SymbolTable::Impl
