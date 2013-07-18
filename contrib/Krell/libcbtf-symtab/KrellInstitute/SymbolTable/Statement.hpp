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

/** @file Declaration of the Statement class. */

#pragma once

#include <boost/operators.hpp>
#include <KrellInstitute/SymbolTable/AddressRange.hpp>
#include <KrellInstitute/SymbolTable/FileName.hpp>
#include <KrellInstitute/SymbolTable/FunctionVisitor.hpp>
#include <set>

namespace KrellInstitute { namespace SymbolTable {

    class LinkedObject;

    namespace Impl {
        class SymbolTable;
    }

    /**
     * A source code statement from a symbol table.
     */
    class Statement :
        public boost::totally_ordered<Statement>
    {
        friend class Impl::SymbolTable;

    public:

        /**
         * Construct a statement within the given linked object from its
         * source file, line, and column numbers. The statement initially
         * has no address ranges.
         *
         * @param linked_object    Linked object containing this statement.
         * @param name             Name of this statement's source file.
         * @param line             Line number of this statement.
         * @param column           Column number of this statement.
         */
        Statement(const LinkedObject& linked_object,
                  const FileName& name,
                  const unsigned int& line,
                  const unsigned int& column);

        /**
         * Is this statement less than another one?
         *
         * @param other    Statement to be compared.
         * @return         Boolean "true" if this statement is less than the
         *                 statement to be compared, or "false" otherwise.
         */
        bool operator<(const Statement& other) const;

        /**
         * Is this statement equal to another one?
         *
         * @param other    Statement to be compared.
         * @return         Boolean "true" if the statements are equal,
         *                 or "false" otherwise.
         */
        bool operator==(const Statement& other) const;

        /**
         * Create a deep copy of this statement.
         *
         * @param linked_object    Linked object containing the deep copy.
         * @return                 Deep copy of this statement.
         */
        Statement clone(LinkedObject& linked_object) const;

        /**
         * Associate the given address ranges with this statement.
         *
         * @param ranges    Address ranges to associate with this statement.
         *
         * @note    The addresses specified are relative to the beginning of
         *          the linked object containing this statement rather than
         *          an absolute address from the address space of a specific
         *          process.
         */
        void addAddressRanges(const std::set<AddressRange>& ranges);

        /**
         * Get the linked object containing this statement.
         *
         * @return    Linked object containing this statement.
         */
        LinkedObject getLinkedObject() const;
        
        /**
         * Get the name of this statement's source file.
         *
         * @return    Name of this statement's source file.
         */
        FileName getName() const;

        /**
         * Get the line number of this statement.
         *
         * @return    Line number of this statement.
         */
        unsigned int getLine() const;

        /**
         * Get the column number of this statement.
         *
         * @return    Column number of this statement.
         */
        unsigned int getColumn() const;

        /**
         * Get the address ranges associated with this statement. An empty set
         * is returned if no address ranges are associated with this statement.
         *
         * @return    Address ranges associated with this statement.
         *
         * @note    The addresses specified are relative to the beginning of
         *          the linked object containing this statement rather than
         *          an absolute address from the address space of a specific
         *          process.
         */
        std::set<AddressRange> getAddressRanges() const;

        /**
         * Visit the functions containing this statement.
         *
         * @param visitor    Visitor invoked for each function containing
         *                   this statement.
         */
        void visitFunctions(FunctionVisitor& visitor) const;

    private:

        /**
         * Construct a statement from its symbol table and unique identifier.
         *
         * @param symbol_table         Symbol table containing this statement.
         * @param unique_identifier    Unique identifier for this statement
         *                             within that symbol table.
         */
        Statement(boost::shared_ptr<Impl::SymbolTable> symbol_table,
                  boost::uint32_t unique_identifier);

        /** Symbol table containing this statement. */
        boost::shared_ptr<Impl::SymbolTable> dm_symbol_table;
        
        /** Unique identifier for this statement within that symbol table. */
        boost::uint32_t dm_unique_identifier;
        
    }; // class Statement
    
} } // namespace KrellInstitute::SymbolTable