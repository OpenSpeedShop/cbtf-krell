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

/** @file Declaration of the SymtabAPIResolver class. */

#pragma once

#include <boost/shared_ptr.hpp>
#include <dyninst/Symtab.h>
#include <KrellInstitute/Base/AddressRange.hpp>
#include <KrellInstitute/Base/FileName.hpp>
#include <KrellInstitute/Base/ThreadName.hpp>
#include <KrellInstitute/Base/TimeInterval.hpp>
#include <KrellInstitute/SymbolTable/AddressSpaces.hpp>
#include <KrellInstitute/SymbolTable/LinkedObject.hpp>
#include <KrellInstitute/SymbolTable/Resolver.hpp>
#include <map>

namespace KrellInstitute { namespace SymbolTable { namespace Impl {

    /**
     * Concrete implementation of the Resolver abstract base class that uses
     * SymtabAPI to resolve addresses.
     *
     * @sa http://www.dyninst.org/symtab
     */
    class SymtabAPIResolver :
        public Resolver
    {

    public:
        
        /**
         * Construct a SymtabAPI resolver for the given address spaces.
         *
         * @param spaces    Address spaces for which to resolve addresses.
         */
        SymtabAPIResolver(AddressSpaces& spaces);

        /** Destructor. */
        virtual ~SymtabAPIResolver();
                
        /**
         * Resolve all addresses in the given linked object.
         *
         * @param linked_object    Linked object to be resolved.
         */
        virtual void operator()(const LinkedObject& linked_object);
        
        /**
         * Resolve all addresses in the specified address range for the given
         * thread and time interval.
         *
         * @param thread      Name of the thread containing the address range.
         * @param range       Address range to be resolved.
         * @param interval    Time interval to be resolved.
         */
        virtual void operator()(const Base::ThreadName& thread,
                                const Base::AddressRange& range,
                                const Base::TimeInterval& interval);

    private:
        
        /**
         * Open the symbol table for the given linked object.
         *
         * @param linked_object    Linked object to be opened.
         *
         * @throw std::runtime_error    The given linked object could not be
         *                              opened by SymtabAPI or has changed
         *                              recently and using it to resolve
         *                              symbols may result in the reporting
         *                              of inaccurate performance data.
         */
        Dyninst::SymtabAPI::Symtab* open(const LinkedObject& linked_object);
        
        /** Address spaces for which to resolve addresses. */
        AddressSpaces& dm_spaces;

        /** Indexed list of symbol tables opened by this resolver. */
        std::map<Base::FileName, Dyninst::SymtabAPI::Symtab*> dm_symtabs;
        
    }; // class SymtabAPIResolver
       
} } } // namespace KrellInstitute::SymbolTable::Impl
