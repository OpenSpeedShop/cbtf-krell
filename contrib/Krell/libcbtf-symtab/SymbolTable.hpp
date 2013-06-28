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
#include <boost/filesystem.hpp>
#include <boost/shared_ptr.hpp>
#include <KrellInstitute/Messages/Symbol.h>
#include <KrellInstitute/SymbolTable/Address.hpp>
#include <KrellInstitute/SymbolTable/AddressRange.hpp>
#include <map>
#include <string>
#include <vector>

#include "AddressBitmap.hpp"

namespace KrellInstitute { namespace SymbolTable { namespace Impl {

    /**
     * Symbol table for a single executable or library. This class provides the
     * <em>real</em> implementation details for the LinkedObject, Function, and
     * Statement classes.
     */
    class SymbolTable
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
         * Construct a symbol table from the full path name of its linked
         * object. The constructed symbol table initially has no symbols
         * (functions, statements, etc.)
         *
         * @param path    Full path name of this symbol table's linked object.
         */
        SymbolTable(const boost::filesystem::path& path);

        /**
         * Construct a symbol table from a CBTF_Protocol_SymbolTable.
         *
         * @param messsage    Message containing this symbol table.
         */
        SymbolTable(const CBTF_Protocol_SymbolTable& message);
        
        /**
         * Construct a symbol table from an existing symbol table.
         *
         * @param other    Symbol table to be copied.
         */
        SymbolTable(const SymbolTable& other);

        /** Destructor. */
        virtual ~SymbolTable();

        /**
         * Replace this symbol table with a copy of another one.
         *
         * @param other    Symbol table to be copied.
         * @return         Resulting (this) symbol table.
         */
        SymbolTable& operator=(const SymbolTable& other);

        /**
         * Type conversion to a CBTF_Protocol_SymbolTable.
         *
         * @return    Message containing this symbol table.
         */
        operator CBTF_Protocol_SymbolTable() const;
        
        /**
         * Get the full path name of this symbol table's linked object.
         *
         * @return    Full path name of this symbol table's linked object.
         */
        boost::filesystem::path getPath() const;

        /**
         * Get the checksum for this symbol table's linked object.
         *
         * @return    Checksum for this symbol table's linked object.
         *
         * @note    The exact algorithm used to calculate the checksum is left
         *          unspecified, but can be expected to be something similar to
         *          CRC-64-ISO. This checksum is either calculated automagically
         *          upon the construction of a new SymbolTable, or extracted
         *          from the CBTF_Protocol_SymbolTable, as appropriate.
         */
        boost::uint64_t getChecksum() const;

        // ...

    private:

        /**
         * Type of associative container used to search for the functions,
         * statements, etc. overlapping a given address range.
         */
        typedef std::multimap<AddressRange, UniqueIdentifier> AddressRangeIndex;
        
        /** Structure representing one function in the symbol table. */
        struct FunctionItem
        {
            /** Mangled name of this function. */
            std::string dm_mangled_name;
            
            /** Bitmap containing this function's addresses. */
            AddressBitmap dm_address_bitmap;
            
        }; // struct FunctionItem

        /** Structure representing one statement in the symbol table. */
        struct StatementItem
        {
            /** Full path name of this statement's source file. */
            boost::filesystem::path dm_path;
            
            /** Line number of this statement. */
            unsigned int dm_line;
            
            /** Column number of this statement. */
            unsigned int dm_column;
            
            /** Bitmap containing this statement's addresses. */
            AddressBitmap dm_address_bitmap;
            
        }; // struct StatementItem
        
        /** Full path name of this symbol table's linked object. */
        boost::filesystem::path dm_path;
        
        /** Checksum for this symbol table's linked object. */
        boost::uint64_t dm_checksum;

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
