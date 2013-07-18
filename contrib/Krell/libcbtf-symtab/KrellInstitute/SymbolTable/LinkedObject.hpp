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

/** @file Declaration of the LinkedObject class. */

#pragma once

#include <boost/cstdint.hpp>
#include <boost/operators.hpp>
#include <boost/shared_ptr.hpp>
#include <KrellInstitute/Messages/Symbol.h>
#include <KrellInstitute/SymbolTable/AddressRange.hpp>
#include <KrellInstitute/SymbolTable/FileName.hpp>
#include <KrellInstitute/SymbolTable/FunctionVisitor.hpp>
#include <KrellInstitute/SymbolTable/StatementVisitor.hpp>

namespace KrellInstitute { namespace SymbolTable {

    class Function;
    class Statement;

    namespace Impl {
        class SymbolTable;
    }

    /**
     * A single executable or library (a "linked object").
     */
    class LinkedObject :
        public boost::totally_ordered<LinkedObject>
    {
        friend class Function;
        friend class Statement;
        
    public:

        /**
         * Construct a linked object from its name. This linked object initially
         * has no symbols (functions, statements, etc.)
         *
         * @param name    Name of this linked object.
         */
        LinkedObject(const FileName& name);
        
        /**
         * Construct a linked object from a CBTF_Protocol_SymbolTable.
         *
         * @param message    Message containing this linked object.
         */
        LinkedObject(const CBTF_Protocol_SymbolTable& message);

        /**
         * Construct a linked object from an existing linked object.
         *
         * @param other    Linked object to be copied.
         *
         * @note    This method creates a second reference to the existing
         *          linked object rather than a deep copy of it. Use clone()
         *          when a deep copy is required.
         */
        LinkedObject(const LinkedObject& other);
        
        /** Destructor. */
        virtual ~LinkedObject();
        
        /**
         * Replace this linked object with a copy of another one.
         *
         * @param other    Linked object to be copied.
         * @return         Resulting (this) linked object.
         *
         * @note    This method replaces this linked object with a reference
         *          to the existing linked object rather than a deep copy of
         *          it. Use clone() when a deep copy is required.
         */
        LinkedObject& operator=(const LinkedObject& other);

        /**
         * Is this linked object less than another one?
         *
         * @param other    Linked object to be compared.
         * @return         Boolean "true" if this linked object is less than the
         *                 linked object to be compared, or "false" otherwise.
         */
        bool operator<(const LinkedObject& other) const;

        /**
         * Is this linked object equal to another one?
         *
         * @param other    Linked object to be compared.
         * @return         Boolean "true" if the linked objects are equal,
         *                 or "false" otherwise.
         */
        bool operator==(const LinkedObject& other) const;

        /**
         * Type conversion to a CBTF_Protocol_SymbolTable.
         *
         * @return    Message containing this linked object.
         */
        operator CBTF_Protocol_SymbolTable() const;

        /**
         * Create a deep copy of this linked object.
         *
         * @return    Deep copy of this linked object.
         */
        LinkedObject clone() const;

        /**
         * Get the name of this linked object.
         *
         * @return    Name of this linked object.
         */
        FileName getName() const;

        /**
         * Visit the functions contained within this linked object.
         *
         * @param visitor    Visitor invoked for each function
         *                   contained within this linked object.
         */
        void visitFunctions(FunctionVisitor& visitor) const;

        /**
         * Visit the functions contained within this linked object intersecting
         * the given address range.
         *
         * @param range      Address range to be found.
         * @param visitor    Visitor invoked for each function contained within
         *                   this linked object intersecting that address range.
         *
         * @note    The addresses specified must be relative to the beginning of
         *          this linked object rather than an absolute address from the
         *          address space of a specific process.
         */
        void visitFunctions(const AddressRange& range,
                            FunctionVisitor& visitor) const;
        
        /**
         * Visit the statements contained within this linked object.
         *
         * @param visitor    Visitor invoked for each statement
         *                   contained within this linked object.
         */
        void visitStatements(StatementVisitor& visitor) const;
        
        /**
         * Visit the statements contained within this linked object intersecting
         * the given address range.
         *
         * @param range      Address range to be found.
         * @param visitor    Visitor invoked for each statement contained within
         *                   this linked object intersecting that address range.
         *
         * @note    The addresses specified must be relative to the beginning of
         *          this linked object rather than an absolute address from the
         *          address space of a specific process.
         */
        void visitStatements(const AddressRange& range,
                             StatementVisitor& vistor) const;
        
    private:

        /**
         * Construct a linked object from its symbol table.
         *
         * @param symbol_table    Symbol table containing this linked object.
         */
        LinkedObject(boost::shared_ptr<Impl::SymbolTable> symbol_table);
        
        /** Symbol table containing this linked object. */
        boost::shared_ptr<Impl::SymbolTable> dm_symbol_table;
        
    }; // class LinkedObject
        
} } // namespace KrellInstitute::SymbolTable
