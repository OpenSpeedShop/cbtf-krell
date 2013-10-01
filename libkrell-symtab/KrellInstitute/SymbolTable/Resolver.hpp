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

/** @file Declaration of the Resolver class. */

#pragma once

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <KrellInstitute/Base/AddressRange.hpp>
#include <KrellInstitute/Base/ThreadName.hpp>
#include <KrellInstitute/Base/TimeInterval.hpp>
#include <KrellInstitute/SymbolTable/AddressSpaces.hpp>
#include <KrellInstitute/SymbolTable/LinkedObject.hpp>

namespace KrellInstitute { namespace SymbolTable {

    /**
     * Abstract base class for a symbol table resolver that accepts addresses
     * and adds the corresponding source code function(s), statement(s), etc.
     * to the appropriate linked object.
     */
    class Resolver :
        private boost::noncopyable
    {

    public:
        
        /** Type of handle (smart pointer) to a resolver. */
        typedef boost::shared_ptr<Resolver> Handle;
        
        /**
         * Instantiate a resolver for the given address spaces.
         *
         * @param spaces    Address spaces for which to resolve addresses.
         *
         * @throw std::runtime_error    There are no resolvers available.
         * 
         * @note    Because the resolver keeps a reference (rather than a 
         *          handle) to the address spaces, the caller must insure
         *          the resolver is released before the address spaces.
         */
        static Handle instantiate(AddressSpaces& spaces);

        /** Destructor. */
        virtual ~Resolver();
        
        /**
         * Resolve all addresses in the given linked object.
         *
         * @param linked_object    Linked object to be resolved.
         */
        virtual void operator()(const LinkedObject& linked_object) = 0;
        
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
                                const Base::TimeInterval& interval) = 0;
        
    }; // class Resolver
       
} } // namespace KrellInstitute::SymbolTable
