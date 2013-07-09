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
#include <boost/filesystem.hpp>
#include <boost/operators.hpp>
#include <boost/shared_ptr.hpp>
#include <KrellInstitute/Messages/Symbol.h>
#include <KrellInstitute/SymbolTable/Address.hpp>
#include <KrellInstitute/SymbolTable/FunctionVisitor.hpp>
#include <KrellInstitute/SymbolTable/StatementVisitor.hpp>
#include <string>

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
         * Construct a linked object from its full path name. The constructed
         * linked object initially has no symbols (functions, statements, etc.)
         *
         * @param path    Full path name of this linked object.
         */
        LinkedObject(const boost::filesystem::path& path);
        
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
         * Get the full path name of this linked object.
         *
         * @return    Full path name of this linked object.
         */
        boost::filesystem::path getPath() const;

        /**
         * Get the checksum for this linked object.
         *
         * @return    Checksum for this linked object.
         *
         * @note    The exact algorithm used to calculate the checksum is left
         *          unspecified, but can be expected to be something similar to
         *          CRC-64-ISO. This checksum is either calculated automagically
         *          upon the construction of a new LinkedObject, or extracted
         *          from the CBTF_Protocol_SymbolTable, as appropriate.
         */
        boost::uint64_t getChecksum() const;
        
        /**
         * Visit the functions contained within this linked object.
         *
         * @param visitor    Visitor invoked for each function contained
         *                   within this linked object.
         */
        void visitFunctions(FunctionVisitor& visitor) const;

        /**
         * Visit the functions contained within this linked object at the given
         * address.
         *
         * @param address    Address to be found.
         * @param visitor    Visitor invoked for each function contained
         *                   within this linked object at that address.
         *
         * @note    The address specified must be relative to the beginning of
         *          this linked object rather than an absolute address from the
         *          address space of a specific process.
         */
        void visitFunctionsAt(const Address& address,
                              FunctionVisitor& visitor) const;
        
        /**
         * Visit the functions contained within this linked object with the
         * given name.
         *
         * @param name       Name of the function to find.
         * @param visitor    Visitor invoked for each function contained
         *                   within this linked object with that name.
         */
        void visitFunctionsByName(const std::string& name,
                                  FunctionVisitor& visitor) const;
        
        /**
         * Visit the statements contained within this linked object.
         *
         * @param visitor    Visitor invoked for each statement contained
         *                   within this linked object.
         */
        void visitStatements(StatementVisitor& visitor) const;
        
        /**
         * Visit the statements contained within this linked object at the
         * given address.
         *
         * @param address    Address to be found.
         * @param visitor    Visitor invoked for each statement contained
         *                   within this linked object at that address.
         *
         * @note    The address specified must be relative to the beginning of
         *          this linked object rather than an absolute address from the
         *          address space of a specific process.
         */
        void visitStatementsAt(const Address& address,
                               StatementVisitor& vistor) const;
        
        /**
         * Visit the statements contained within this linked object in the
         * given source file.
         *  
         * @param path       Source file to be found.
         * @param visitor    Visitor invoked for each statement contained
         *                   within this linked object in that source file.
         */
        void visitStatementsBySourceFile(const boost::filesystem::path& path,
                                         StatementVisitor& visitor) const;
        
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
