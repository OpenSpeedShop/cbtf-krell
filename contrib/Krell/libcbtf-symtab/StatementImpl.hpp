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

/** @file Declaration of the StatementImpl class. */

#pragma once

#include <boost/filesystem.hpp>
#include <KrellInstitute/SymbolTable/AddressRange.hpp>
#include <KrellInstitute/SymbolTable/FunctionVisitor.hpp>
#include <KrellInstitute/SymbolTable/LinkedObject.hpp>
#include <set>

#include "SymbolTable.hpp"

namespace KrellInstitute { namespace SymbolTable { namespace Impl {

    /**
     * Implementation details of the Statement class. Anything that
     * would normally be a private member of Statement is instead a
     * member of StatementImpl.
     */
    class StatementImpl
    {
        
    public:

        /**
         * Construct a statement within the given linked object from its source
         * file, line, and column numbers.
         */
        StatementImpl(const LinkedObject& linked_object,
                      const boost::filesystem::path& path,
                      const unsigned int& line,
                      const unsigned int& column);

        /** Construct a statement from an existing statement. */
        StatementImpl(const StatementImpl& other);
        
        /** Destructor. */
        virtual ~StatementImpl();
        
        /** Replace this statement with a copy of another one. */
        StatementImpl& operator=(const StatementImpl& other);

        /** Is this statement less than another one? */
        bool operator<(const StatementImpl& other) const;

        /** Is this statement equal to another one? */
        bool operator==(const StatementImpl& other) const;

        /** Create a deep copy of this statement. */
        StatementImpl clone(LinkedObject& linked_object) const;

        /** Associate the given address ranges with this statement. */
        void addAddressRanges(const std::set<AddressRange>& ranges);

        /** Get the linked object containing this statement. */
        LinkedObject getLinkedObject() const;
        
        /** Get the full path name of this statement's source file. */
        boost::filesystem::path getPath() const;

        /** Get the line number of this statement. */
        unsigned int getLine() const;

        /** Get the column number of this statement. */
        unsigned int getColumn() const;

        /** Get the address ranges associated with this statement. */
        std::set<AddressRange> getAddressRanges() const;
        
        /** Visit the functions containing this statement. */
        void visitFunctions(FunctionVisitor& visitor) const;

    private:

        /** Symbol table containing this statement. */
        SymbolTable::Handle dm_symbol_table;
        
        // ...

    }; // class StatementImpl

} } } // namespace KrellInstitute::SymbolTable::Impl
