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

/** @file Declaration of the SymbolTable class. */

#pragma once

#include <boost/cstdint.hpp>
#include <boost/dynamic_bitset.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/shared_ptr.hpp>
#include <KrellInstitute/Base/AddressRange.hpp>
#include <KrellInstitute/Base/FileName.hpp>
#include <KrellInstitute/Messages/Symbol.h>
#include <KrellInstitute/SymbolTable/FunctionVisitor.hpp>
#include <KrellInstitute/SymbolTable/StatementVisitor.hpp>
#include <set>
#include <string>
#include <vector>

#include "AddressBitmap.hpp"

namespace KrellInstitute { namespace SymbolTable { namespace Impl {

    /**
     * Symbol table for a single executable or library. This class provides
     * the underlying implementation details for the LinkedObject, Function,
     * and Statement classes.
     */
    class SymbolTable :
        public boost::enable_shared_from_this<SymbolTable>
    {

    public:

        /** Type of handle (smart pointer) to a symbol table. */
        typedef boost::shared_ptr<SymbolTable> Handle;
        
        /**
         * Type of unique identifier used to refer to functions, statements,
         * etc. in a symbol table.
         */
        typedef boost::uint32_t UniqueIdentifier;

        /**
         * Construct a symbol table for the named linked object file. The symbol
         * table initially has no symbols (functions, statements, etc.)
         *
         * @param file    Name of this symbol table's linked object file.
         */
        SymbolTable(const Base::FileName& file);

        /**
         * Construct a symbol table from a CBTF_Protocol_SymbolTable.
         *
         * @param messsage    Message containing this symbol table.
         */
        SymbolTable(const CBTF_Protocol_SymbolTable& message);
        
        /**
         * Type conversion to a CBTF_Protocol_SymbolTable.
         *
         * @return    Message containing this symbol table.
         */
        operator CBTF_Protocol_SymbolTable() const;

        /**
         * Get the name of this symbol table's linked object file.
         *
         * @return    Name of this symbol table's linked object file.
         */
        const Base::FileName& getFile() const;
        
        /**
         * Add a new function to this symbol table.
         *
         * @param name    Mangled name of the function.
         * @return        Unique identifier of that function.
         */
        UniqueIdentifier addFunction(const std::string& name);

        /**
         * Associate the given address ranges with the given function.
         *
         * @param uid       Unique identifier of the function.
         * @param ranges    Address ranges to associate with that function.
         *
         * @note    The addresses specified must be relative to the beginning
         *          of this symbol table rather than an absolute address from
         *          the address space of a specific process.
         */
        void addFunctionAddressRanges(
            const UniqueIdentifier& uid,
            const std::set<Base::AddressRange>& ranges
            );

        /**
         * Add a new statement to this symbol table.
         *
         * @param file      Name of the statement's source file.
         * @param line      Line number of the statement.
         * @param column    Column number of the statement.
         * @return          Unique identifier of that statement.
         */
        UniqueIdentifier addStatement(const Base::FileName& file,
                                      const unsigned int& line,
                                      const unsigned int& column);
        
        /**
         * Associate the given address ranges with the given statement.
         *
         * @param uid       Unique identifier of the statement.
         * @param ranges    Address ranges to associate with that statement.
         *
         * @note    The addresses specified must be relative to the beginning
         *          of this symbol table rather than an absolute address from
         *          the address space of a specific process.
         */
        void addStatementAddressRanges(
            const UniqueIdentifier& uid,
            const std::set<Base::AddressRange>& ranges
            );

        /**
         * Add a copy of the given function to this symbol table.
         *
         * @param symbol_table    Symbol table containing the function.
         * @param uid             Unique identifier of the function.
         * @return                Unique identifier of the copied function.
         */
        UniqueIdentifier cloneFunction(const SymbolTable& symbol_table,
                                       const UniqueIdentifier& uid);
        
        /**
         * Add a copy of the given statement to this symbol table.
         *
         * @param symbol_table    Symbol table containing the statement.
         * @param uid             Unique identifier of the statement.
         * @return                Unique identifier of the copied statement.
         */
        UniqueIdentifier cloneStatement(const SymbolTable& symbol_table,
                                        const UniqueIdentifier& uid);

        /**
         * Get the mangled name of the given function.
         *
         * @param uid    Unique identifier of the function.
         * @return       Mangled name of that function.
         */
        const std::string& getFunctionMangledName(
            const UniqueIdentifier& uid
            ) const;

        /**
         * Get the address ranges associated with the given function. An
         * empty set is returned if no address ranges are associated with
         * the function.
         *
         * @param uid    Unique identifier of the function.
         * @return       Address ranges associated with that function.
         *
         * @note    The addresses specified are relative to the beginning of
         *          this symbol table rather than an absolute address from the
         *          address space of a specific process.
         */
        std::set<Base::AddressRange> getFunctionAddressRanges(
            const UniqueIdentifier& uid
            ) const;
        
        /**
         * Get the name of the given statement's source file.
         *
         * @param uid    Unique identifier of the statement.
         * @return       Name of that statement's source file.
         */
        const Base::FileName& getStatementFile(
            const UniqueIdentifier& uid
            ) const;

        /**
         * Get the line number of the given statement.
         *
         * @param uid    Unique identifier of the statement.
         * @return       Line number of that statement.
         */
        unsigned int getStatementLine(const UniqueIdentifier& uid) const;

        /**
         * Get the column number of the given statement.
         *
         * @param uid    Unique identifier of the statement.
         * @return       Column number of that statement.
         */
        unsigned int getStatementColumn(const UniqueIdentifier& uid) const;

        /**
         * Get the address ranges associated with the given statement. An
         * empty set is returned if no address ranges are associated with
         * the statement.
         *
         * @param uid    Unique identifier of the statement.
         * @return       Address ranges associated with that statement.
         *
         * @note    The addresses specified are relative to the beginning of
         *          this symbol table rather than an absolute address from the
         *          address space of a specific process.
         */
        std::set<Base::AddressRange> getStatementAddressRanges(
            const UniqueIdentifier& uid
            ) const;

        /**
         * Visit the functions contained within this symbol table.
         *
         * @param visitor    Visitor invoked for each function
         *                   contained within this symbol table.
         */
        void visitFunctions(const FunctionVisitor& visitor);

        /**
         * Visit the functions contained within this symbol table intersecting
         * the given address range.
         *
         * @param range      Address range to be found.
         * @param visitor    Visitor invoked for each function contained within
         *                   this symbol table intersecting that address range.
         *
         * @note    The addresses specified must be relative to the beginning
         *          of this symbol table rather than an absolute address from
         *          the address space of a specific process.
         */
        void visitFunctions(const Base::AddressRange& range,
                            const FunctionVisitor& visitor);
        
        /**
         * Visit the definitions of the given function.
         *
         * @param uid        Unique identifier of the function.
         * @param visitor    Visitor invoked for each defintion of that
         *                   function.
         */
        void visitFunctionDefinitions(const UniqueIdentifier& uid,
                                      const StatementVisitor& visitor);
        
        /**
         * Visit the statements associated with the given function.
         *
         * @param uid        Unique identifier of the function.
         * @param visitor    Visitor invoked for each statement associated 
         *                   with that function.
         */
        void visitFunctionStatements(const UniqueIdentifier& uid,
                                     const StatementVisitor& visitor);

        /**
         * Visit the statements contained within this symbol table.
         *
         * @param visitor    Visitor invoked for each statement
         *                   contained within this symbol table.
         */
        void visitStatements(const StatementVisitor& visitor);

        /**
         * Visit the statements contained within this symbol table intersecting
         * the given address range.
         *
         * @param range      Address range to be found.
         * @param visitor    Visitor invoked for each statement contained within
         *                   this symbol table intersecting that address range.
         *
         * @note    The addresses specified must be relative to the beginning
         *          of this symbol table rather than an absolute address from
         *          the address space of a specific process.
         */
        void visitStatements(const Base::AddressRange& range,
                             const StatementVisitor& visitor);

        /**
         * Visit the functions containing the given statement.
         *
         * @param uid        Unique identifier of the statement.
         * @param visitor    Visitor invoked for each function containing
         *                   that statement.
         */
        void visitStatementFunctions(const UniqueIdentifier& uid,
                                     const FunctionVisitor& visitor);
        
    private:

        /** Structure representing one row in an address range index. */
        struct AddressRangeIndexRow
        {
            /** Unique identifier for an entity within this symbol table. */
            UniqueIdentifier dm_uid;

            /** Closed beginning of an address range for that entity. */
            Base::Address dm_begin;
            
            /** Closed end of an address range for that entity. */
            Base::Address dm_end;
            
            /** Constructor from initial fields. */
            AddressRangeIndexRow(const UniqueIdentifier& uid,
                                 const Base::AddressRange& range) :
                dm_uid(uid),
                dm_begin(range.begin()),
                dm_end(range.end())
            {
            }
            
            /** Does this address range intersect another address range? */
            bool intersects(const Base::AddressRange& range) const
            {
                return Base::AddressRange(dm_begin, dm_end).intersects(range);
            }
            
        }; // struct AddressRangeIndexRow
        
        /**
         * Type of associative container used to search for the functions,
         * statements, etc. overlapping a given address range.
         */
        typedef boost::multi_index_container<
            AddressRangeIndexRow,
            boost::multi_index::indexed_by<
                boost::multi_index::ordered_non_unique<
                    boost::multi_index::member<
                        AddressRangeIndexRow,
                        UniqueIdentifier,
                        &AddressRangeIndexRow::dm_uid
                        >
                    >,
                boost::multi_index::ordered_non_unique<
                    boost::multi_index::member<
                        AddressRangeIndexRow,
                        Base::Address,
                        &AddressRangeIndexRow::dm_begin
                        >
                    >
                >
            > AddressRangeIndex;

        /** Structure representing one function in the symbol table. */
        struct FunctionItem
        {
            /** Mangled name of this function. */
            std::string dm_name;
            
            /** Bitmap(s) containing this function's addresses. */
            std::vector<AddressBitmap> dm_addresses;

            /** Constructor from initial fields. */
            FunctionItem(const std::string& name) :
                dm_name(name),
                dm_addresses()
            {
            }

        }; // struct FunctionItem

        /** Structure representing one statement in the symbol table. */
        struct StatementItem
        {
            /** Name of this statement's source file. */
            Base::FileName dm_file;
            
            /** Line number of this statement. */
            unsigned int dm_line;
            
            /** Column number of this statement. */
            unsigned int dm_column;
            
            /** Bitmap(s) containing this statement's addresses. */
            std::vector<AddressBitmap> dm_addresses;

            /** Constructor from initial fields. */
            StatementItem(const Base::FileName& file,
                          const unsigned int& line,
                          const unsigned int& column) :
                dm_file(file),
                dm_line(line),
                dm_column(column),
                dm_addresses()
            {
            }
            
        }; // struct StatementItem

        /** Visit functions using the index. */
        void visit(const Base::AddressRange& range,
                   const FunctionVisitor& visitor,
                   bool& terminate, boost::dynamic_bitset<>& visited);
        
        /** Visit statements using the index. */
        void visit(const Base::AddressRange& range,
                   const StatementVisitor& visitor,
                   bool& terminate, boost::dynamic_bitset<>& visited);
        
        /** Name of this symbol table's linked object file. */
        Base::FileName dm_file;
            
        /** List of functions in this symbol table. */
        std::vector<FunctionItem> dm_functions;

        /** Index used to find functions by addresses. */
        AddressRangeIndex dm_functions_index;

        /** List of statements in this symbol table. */
        std::vector<StatementItem> dm_statements;

        /** Index used to find statements by addresses. */
        AddressRangeIndex dm_statements_index;

    }; // class SymbolTable

} } } // namespace KrellInstitute::SymbolTable::Impl
