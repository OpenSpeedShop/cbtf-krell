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

/** @file Declaration of the LinkedObjectImpl class. */

#pragma once

#include <boost/cstdint.hpp>
#include <boost/filesystem.hpp>
#include <KrellInstitute/Messages/Symbol.h>
#include <KrellInstitute/SymbolTable/Address.hpp>
#include <KrellInstitute/SymbolTable/FunctionVisitor.hpp>
#include <KrellInstitute/SymbolTable/StatementVisitor.hpp>
#include <string>

#include "SymbolTable.hpp"

namespace KrellInstitute { namespace SymbolTable { namespace Impl {

    /**
     * Implementation details of the LinkedObject class. Anything that
     * would normally be a private member of LinkedObject is instead a
     * member of LinkedObjectImpl.
     */
    class LinkedObjectImpl
    {

    public:

        /** Construct a linked object from its symbol table. */
        LinkedObjectImpl(const SymbolTable::Handle& symbol_table);

        /** Construct a linked object from its full path name. */
        LinkedObjectImpl(const boost::filesystem::path& path);
        
        /** Construct a linked object from a CBTF_Protocol_SymbolTable. */
        LinkedObjectImpl(const CBTF_Protocol_SymbolTable& message);

        /** Construct a linked object from an existing linked object. */
        LinkedObjectImpl(const LinkedObjectImpl& other);

        /** Destructor. */
        virtual ~LinkedObjectImpl();
        
        /** Replace this linked object with a copy of another one. */
        LinkedObjectImpl& operator=(const LinkedObjectImpl& other);

        /** Is this linked object less than another one? */
        bool operator<(const LinkedObjectImpl& other) const;

        /** Is this linked object equal to another one? */
        bool operator==(const LinkedObjectImpl& other) const;

        /** Type conversion to a CBTF_Protocol_SymbolTable. */
        operator CBTF_Protocol_SymbolTable() const;

        /** Create a deep copy of this linked object. */
        LinkedObjectImpl clone() const;

        /** Get the full path name of this linked object. */
        boost::filesystem::path getPath() const;

        /** Get the checksum for this linked object. */
        boost::uint64_t getChecksum() const;

        /** Visit the functions contained within this linked object. */
        void visitFunctions(FunctionVisitor& visitor) const;

        /** Visit the functions at the given address. */
        void visitFunctionsAt(const Address& address,
                              FunctionVisitor& visitor) const;

        /** Visit the functions with the given name. */
        void visitFunctionsByName(const std::string& name,
                                  FunctionVisitor& visitor) const;

        /** Visit the statements contained within this linked object. */
        void visitStatements(StatementVisitor& visitor) const;

        /** Visit the statements at the given address. */
        void visitStatementsAt(const Address& address,
                               StatementVisitor& visitor) const;

        /** Visit the statements for the given source file. */
        void visitStatementsBySourceFile(const boost::filesystem::path& path,
                                         StatementVisitor& visitor) const;
        
    private:

        /** Symbol table containing this linked object. */
        SymbolTable::Handle dm_symbol_table;

    }; // class LinkedObjectImpl
        
} } } // namespace KrellInstitute::SymbolTable::Impl
