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

/** @file Declaration of the Function class. */

#pragma once

#include <boost/cstdint.hpp>
#include <boost/operators.hpp>
#include <boost/shared_ptr.hpp>
#include <KrellInstitute/SymbolTable/AddressRange.hpp>
#include <KrellInstitute/SymbolTable/StatementVisitor.hpp>
#include <set>
#include <string>

namespace KrellInstitute { namespace SymbolTable {

    class LinkedObject;

    namespace Impl {
        class SymbolTable;
    }

    /**
     * A source code function within a linked object.
     */
    class Function :
        public boost::totally_ordered<Function>
    {
        friend class Impl::SymbolTable;

    public:

        /**
         * Construct a function within the given linked object from its mangled
         * name. The function initially has no address ranges.
         *
         * @param linked_object    Linked object containing this function.
         * @param name             Mangled name of this function.
         */
        Function(const LinkedObject& linked_object, const std::string& name);
        
        /**
         * Is this function less than another one?
         *
         * @param other    Function to be compared.
         * @return         Boolean "true" if this function is less than the
         *                 function to be compared, or "false" otherwise.
         */
        bool operator<(const Function& other) const;

        /**
         * Is this function equal to another one?
         *
         * @param other    Function to be compared.
         * @return         Boolean "true" if the functions are equal,
         *                 or "false" otherwise.
         */
        bool operator==(const Function& other) const;

        /**
         * Create a deep copy of this function.
         *
         * @param linked_object    Linked object containing the deep copy.
         * @return                 Deep copy of this function.
         */
        Function clone(LinkedObject& linked_object) const;

        /**
         * Associate the given address ranges with this function. 
         *
         * @param ranges    Address ranges to associate with this function.
         *
         * @note    The addresses specified are relative to the beginning of
         *          the linked object containing this function rather than
         *          an absolute address from the address space of a specific
         *          process.
         */
        void addAddressRanges(const std::set<AddressRange>& ranges);

        /**
         * Get the linked object containing this function.
         *
         * @return    Linked object containing this function.
         */
        LinkedObject getLinkedObject() const;
        
        /**
         * Get the mangled name of this function.
         *
         * @return    Mangled name of this function.
         */
        std::string getMangledName() const;
        
        /**
         * Get the demangled name of this function.
         *
         * @return       Demangled name of this function.
         */
        std::string getDemangledName() const;

        /**
         * Get the address ranges associated with this function. An empty set
         * is returned if no address ranges are associated with this function.
         *
         * @return    Address ranges associated with this function.
         *
         * @note    The addresses specified are relative to the beginning of
         *          the linked object containing this function rather than
         *          an absolute address from the address space of a specific
         *          process.
         */
        std::set<AddressRange> getAddressRanges() const;

        /**
         * Visit the definitions of this function.
         *
         * @param visitor    Visitor invoked for each defintion of this
         *                   function.
         */
        void visitDefinitions(StatementVisitor& visitor) const;

        /**
         * Visit the statements associated with this function.
         *
         * @param visitor    Visitor invoked for each statement associated 
         *                   with this function.
         */
        void visitStatements(StatementVisitor& visitor) const;

    private:

        /**
         * Construct a function from its symbol table and unique identifier.
         *
         * @param symbol_table         Symbol table containing this function.
         * @param unique_identifier    Unique identifier for this function
         *                             within that symbol table.
         */
        Function(boost::shared_ptr<Impl::SymbolTable> symbol_table,
                 boost::uint32_t unique_identifier);

        /** Symbol table containing this function. */
        boost::shared_ptr<Impl::SymbolTable> dm_symbol_table;
        
        /** Unique identifier for this function within that symbol table. */
        boost::uint32_t dm_unique_identifier;
        
    }; // class Function
        
} } // namespace KrellInstitute::SymbolTable
