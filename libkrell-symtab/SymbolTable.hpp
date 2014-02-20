////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2013,2014 Krell Institute. All Rights Reserved.
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
#include <KrellInstitute/Base/Address.hpp>
#include <KrellInstitute/Base/AddressRange.hpp>
#include <KrellInstitute/Base/AddressSet.hpp>
#include <KrellInstitute/Base/FileName.hpp>
#include <KrellInstitute/Messages/Symbol.h>
#include <KrellInstitute/SymbolTable/Function.hpp>
#include <KrellInstitute/SymbolTable/FunctionVisitor.hpp>
#include <KrellInstitute/SymbolTable/Loop.hpp>
#include <KrellInstitute/SymbolTable/LoopVisitor.hpp>
#include <KrellInstitute/SymbolTable/Statement.hpp>
#include <KrellInstitute/SymbolTable/StatementVisitor.hpp>
#include <set>
#include <string>
#include <vector>

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
         * Associate the specified address ranges with the given function.
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
         * Add a new loop to this symbol table.
         *
         * @param head    Head address of the loop.
         * @return        Unique identifier of that loop.
         */
        UniqueIdentifier addLoop(const Base::Address& head);
        
        /**
         * Associate the specified address ranges with the given loop.
         *
         * @param uid       Unique identifier of the loop.
         * @param ranges    Address ranges to associate with that loop.
         *
         * @note    The addresses specified must be relative to the beginning
         *          of this symbol table rather than an absolute address from
         *          the address space of a specific process.
         */
        void addLoopAddressRanges(
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
         * Associate the specified address ranges with the given statement.
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
         * Add a copy of the given loop to this symbol table.
         *
         * @param symbol_table    Symbol table containing the loop.
         * @param uid             Unique identifier of the loop.
         * @return                Unique identifier of the copied loop.
         */
        UniqueIdentifier cloneLoop(const SymbolTable& symbol_table,
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
         * Get the head address of the given loop.
         *
         * @param uid    Unique identifier of the loop.
         * @return       Head address of that loop.
         */
        const Base::Address& getLoopHeadAddress(
            const UniqueIdentifier& uid
            ) const;

        /**
         * Get the address ranges associated with the given loop. An empty
         * set is returned if no address ranges are associated with the loop.
         *
         * @param uid    Unique identifier of the loop.
         * @return       Address ranges associated with that loop.
         *
         * @note    The addresses specified are relative to the beginning of
         *          this symbol table rather than an absolute address from the
         *          address space of a specific process.
         */
        std::set<Base::AddressRange> getLoopAddressRanges(
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
         * Visit the loops associated with the given function.
         *
         * @param uid        Unique identifier of the function.
         * @param visitor    Visitor invoked for each loop associated 
         *                   with that function.
         */
        void visitFunctionLoops(const UniqueIdentifier& uid,
                                const LoopVisitor& visitor);

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
         * Visit the loops contained within this symbol table.
         *
         * @param visitor    Visitor invoked for each loop
         *                   contained within this symbol table.
         */
        void visitLoops(const LoopVisitor& visitor);
        
        /**
         * Visit the loops contained within this symbol table intersecting the
         * given address range.
         *
         * @param range      Address range to be found.
         * @param visitor    Visitor invoked for each loop contained within
         *                   this symbol table intersecting that address range.
         *
         * @note    The addresses specified must be relative to the beginning
         *          of this symbol table rather than an absolute address from
         *          the address space of a specific process.
         */
        void visitLoops(const Base::AddressRange& range,
                        const LoopVisitor& visitor);

        /**
         * Visit the definitions of the given loop.
         *
         * @param uid        Unique identifier of the loop.
         * @param visitor    Visitor invoked for each defintion of that loop.
         */
        void visitLoopDefinitions(const UniqueIdentifier& uid,
                                  const StatementVisitor& visitor);
        
        /**
         * Visit the functions containing the given loop.
         *
         * @param uid        Unique identifier of the loop.
         * @param visitor    Visitor invoked for each function containing
         *                   that loop.
         */
        void visitLoopFunctions(const UniqueIdentifier& uid,
                                const FunctionVisitor& visitor);
        
        /**
         * Visit the statements associated with the given loop.
         *
         * @param uid        Unique identifier of the loop.
         * @param visitor    Visitor invoked for each statement associated 
         *                   with that loop.
         */
        void visitLoopStatements(const UniqueIdentifier& uid,
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
        
        /**
         * Visit the loops containing the given statement.
         *
         * @param uid        Unique identifier of the statement.
         * @param visitor    Visitor invoked for each loop containing
         *                   that statement.
         */
        void visitStatementLoops(const UniqueIdentifier& uid,
                                 const LoopVisitor& visitor);
        
    private:

        /** Structure containing one row of an address range index. */
        struct AddressRangeIndexRow
        {
            /** Unique identifier for an entity within this symbol table. */
            UniqueIdentifier dm_uid;

            /** Address range for that entity. */
            Base::AddressRange dm_range;
            
            /** Constructor from initial fields. */
            AddressRangeIndexRow(const UniqueIdentifier& uid,
                                 const Base::AddressRange& range) :
                dm_uid(uid),
                dm_range(range)
            {
            }
            
            /** Key extractor for the address range's beginning. */
            struct range_begin
            {
                typedef Base::Address result_type;
                
                result_type operator()(const AddressRangeIndexRow& row) const
                {
                    return row.dm_range.begin();
                }
            };
            
        }; // struct AddressRangeIndexRow
        
        /**
         * Type of associative container used to search for the entities
         * overlapping a given address range.
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
                    AddressRangeIndexRow::range_begin
                    >
                >
            > AddressRangeIndex;

        /** Structure describing one function in a symbol table. */
        struct FunctionItem
        {
            /** Type of entity represented by this item. */
            typedef Function EntityType;
            
            /** Type of visitor for that entity. */
            typedef FunctionVisitor VisitorType;
            
            /** Mangled name of this function. */
            std::string dm_name;
            
            /** Set containing this function's addresses. */
            Base::AddressSet dm_addresses;

            /** Constructor from initial fields. */
            FunctionItem(const std::string& name) :
                dm_name(name),
                dm_addresses()
            {
            }

        }; // struct FunctionItem

        /** Structure describing one loop in a symbol table. */
        struct LoopItem
        {
            /** Type of entity represented by this item. */
            typedef Loop EntityType;
            
            /** Type of visitor for that entity. */
            typedef LoopVisitor VisitorType;

            /** Head address of this loop. */
            Base::Address dm_head;
            
            /** Set containing this loop's addresses. */
            Base::AddressSet dm_addresses;

            /** Constructor from initial fields. */
            LoopItem(const Base::Address& head) :
                dm_head(head),
                dm_addresses()
            {
            }

        }; // struct LoopItem

        /** Structure describing one statement in a symbol table. */
        struct StatementItem
        {
            /** Type of entity for which this is the item. */
            typedef Statement EntityType;
            
            /** Type of visitor for that entity. */
            typedef StatementVisitor VisitorType;

            /** Name of this statement's source file. */
            Base::FileName dm_file;
            
            /** Line number of this statement. */
            unsigned int dm_line;
            
            /** Column number of this statement. */
            unsigned int dm_column;

            /** Set containing this statement's addresses. */
            Base::AddressSet dm_addresses;

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

        /** Visit all of the items. */
        template <typename T>
        void visit(const std::vector<T>& items,
                   const typename T::VisitorType& visitor)
        {
            bool terminate = false;
            typename T::EntityType entity(shared_from_this(), 0);
            
            for (UniqueIdentifier i = 0, iEnd = items.size();
                 !terminate && (i != iEnd); 
                 ++i)
            {
                entity.dm_unique_identifier = i;
                terminate |= !visitor(entity);
            }
        }

        /**
         * Visit the items intersecting an address range.
         *
         * @todo    The current implementation of AddressSet is not efficient
         *          at storing a single AddressRange. Once that inadequacy is
         *          corrected, this template method should be eliminated, and
         *          all of its uses replaced with the variant that accepts an
         *          AddressSet.
         */
        template <typename T>
        void visit(const std::vector<T>& items,
                   const AddressRangeIndex& index,
                   const Base::AddressRange& range,
                   const typename T::VisitorType& visitor)
        {
            bool terminate = false;
            boost::dynamic_bitset<> visited(items.size());
            typename T::EntityType entity(shared_from_this(), 0);
            
            AddressRangeIndex::nth_index<1>::type::const_iterator i = 
                index.get<1>().lower_bound(range.begin());
            
            if (i != index.get<1>().begin())
            {
                --i;
            }
            
            AddressRangeIndex::nth_index<1>::type::const_iterator iEnd = 
                index.get<1>().upper_bound(range.end());
            
            for (; !terminate && (i != iEnd); ++i)
            {
                if (i->dm_range.intersects(range) &&
                    (i->dm_uid < visited.size()) && !visited[i->dm_uid])
                {
                    visited[i->dm_uid] = true;
                    entity.dm_unique_identifier = i->dm_uid;
                    terminate |= !visitor(entity);
                }
            }
        }
        
        /**
         * Visit the items intersecting an address set.
         *
         * @todo    The current implementation of AddressSet does not provide
         *          an interface for visiting the address ranges in the set.
         *          Once that inadequacy is corrected, this template method
         *          should be modified to use that visitation pattern instead.
         */
        template <typename T>
        void visit(const std::vector<T>& items,
                   const AddressRangeIndex& index,
                   const Base::AddressSet& set,
                   const typename T::VisitorType& visitor)
        {
            bool terminate = false;
            boost::dynamic_bitset<> visited(items.size());
            typename T::EntityType entity(shared_from_this(), 0);
            
            std::set<Base::AddressRange> ranges = set;
            
            for (std::set<Base::AddressRange>::const_iterator
                     r = ranges.begin(); !terminate && (r != ranges.end()); ++r)
            {
                const Base::AddressRange& range = *r;
                
                AddressRangeIndex::nth_index<1>::type::const_iterator i = 
                    index.get<1>().lower_bound(range.begin());
                
                if (i != index.get<1>().begin())
                {
                    --i;
                }
                
                AddressRangeIndex::nth_index<1>::type::const_iterator iEnd = 
                    index.get<1>().upper_bound(range.end());
                
                for (; !terminate && (i != iEnd); ++i)
                {
                    if (i->dm_range.intersects(range) &&
                        (i->dm_uid < visited.size()) && !visited[i->dm_uid])
                    {
                        visited[i->dm_uid] = true;
                        entity.dm_unique_identifier = i->dm_uid;
                        terminate |= !visitor(entity);
                    }
                }
            }
        }
        
        /** Name of this symbol table's linked object file. */
        Base::FileName dm_file;
            
        /** List of functions in this symbol table. */
        std::vector<FunctionItem> dm_functions;
        
        /** Index used to find functions by addresses. */
        AddressRangeIndex dm_functions_index;
        
        /** List of loops in this symbol table. */
        std::vector<LoopItem> dm_loops;

        /** Index used to find loops by addresses. */
        AddressRangeIndex dm_loops_index;

        /** List of statements in this symbol table. */
        std::vector<StatementItem> dm_statements;

        /** Index used to find statements by addresses. */
        AddressRangeIndex dm_statements_index;

    }; // class SymbolTable

} } } // namespace KrellInstitute::SymbolTable::Impl
