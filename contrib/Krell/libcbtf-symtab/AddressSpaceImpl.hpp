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

/** @file Declaration of the AddressSpaceImpl class. */

#pragma once

#include <boost/filesystem.hpp>
#include <boost/optional.hpp>
#include <KrellInstitute/Messages/LinkedObjectEvents.h>
#include <KrellInstitute/SymbolTable/Address.hpp>
#include <KrellInstitute/SymbolTable/AddressRange.hpp>
#include <KrellInstitute/SymbolTable/LinkedObject.hpp>
#include <KrellInstitute/SymbolTable/Time.hpp>
#include <KrellInstitute/SymbolTable/TimeInterval.hpp>
#include <set>

namespace KrellInstitute { namespace SymbolTable { namespace Impl {

    /**
     * Implementation details of the AddressSpace class. Anything that
     * would normally be a private member of AddressSpace is instead a
     * member of AddressSpaceImpl.
     */
    class AddressSpaceImpl
    {

    public:

        /** Construct an empty address space. */
        AddressSpaceImpl();

        /**
         * Construct an address space from a CBTF_Protocol_LinkedObjectGroup.
         */
        AddressSpaceImpl(const CBTF_Protocol_LinkedObjectGroup& message);

        /**
         * Construct an address space from an existing address space.
         *
         * @param other    Address space to be copied.
         */
        AddressSpaceImpl(const AddressSpaceImpl& other);
        
        /** Destructor. */
        virtual ~AddressSpaceImpl();
        
        /** Replace this address space with a copy of another one. */
        AddressSpaceImpl& operator=(const AddressSpaceImpl& other);

        /** Type conversion to a CBTF_Protocol_LinkedObjectGroup. */
        operator CBTF_Protocol_LinkedObjectGroup() const;

        /** Get the linked objects contained within this address space. */
        std::set<LinkedObject> getLinkedObjects() const;

        /**
         * Get the linked object contained within this address space at the
         * given address and time.
         */
        boost::optional<LinkedObject> getLinkedObjectAt(
            const Address& address, const Time& time = Time::Now()
            ) const;

        /**
         * Get the linked objects contained within this address space for the
         * given path.
         */
        std::set<LinkedObject> getLinkedObjectsByPath(
            const boost::filesystem::path& path
            ) const;

        /**
         * Add the given linked object to this address space at the specified
         * address range for the given time interval.
         */
        void addLinkedObject(const LinkedObject& linked_object,
                             const AddressRange& range,
                             const TimeInterval& interval = TimeInterval(
                                 Time::TheBeginning(), Time::TheEnd()
                                 ));
        
        /**
         * Apply the given message, describing the load of a linked object,
         * to this address space.
         */
        void apply(const CBTF_Protocol_LoadedLinkedObject& message);

        /**
         * Apply the given message, describing the unload of a linked object,
         * to this address space.
         */
        void apply(const CBTF_Protocol_UnloadedLinkedObject& message);
        
    private:
        
        // ...

    }; // class AddressSpaceImpl
        
} } } // namespace KrellInstitute::SymbolTable::Impl
