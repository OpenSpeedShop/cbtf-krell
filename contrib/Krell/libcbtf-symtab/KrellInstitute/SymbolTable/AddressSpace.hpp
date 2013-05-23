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

/** @file Declaration of the AddressSpace class. */

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

namespace KrellInstitute { namespace SymbolTable {

    namespace Impl {
        class AddressSpaceImpl;
    }

    /**
     * In-memory address space of a process.
     */
    class AddressSpace
    {

    public:

        /** Construct an empty address space. */
        AddressSpace();

        /**
         * Construct an address space from a CBTF_Protocol_LinkedObjectGroup.
         *
         * @param message    Message containing this address space.
         *
         * @note    Because this class contains no information regarding the
         *          thread(s) in the process containing this address space,
         *          the thread name field of the given message is ignored.
         */
        AddressSpace(const CBTF_Protocol_LinkedObjectGroup& message);

        /**
         * Construct an address space from an existing address space.
         *
         * @param other    Address space to be copied.
         */
        AddressSpace(const AddressSpace& other);
        
        /** Destructor. */
        virtual ~AddressSpace();
        
        /**
         * Replace this address space with a copy of another one.
         *
         * @param other    Address space to be copied.
         * @return         Resulting (this) address space.
         */
        AddressSpace& operator=(const AddressSpace& other);

        /**
         * Type conversion to a CBTF_Protocol_LinkedObjectGroup.
         *
         * @return    Message containing this address space.
         *
         * @note    Because this class contains no information regarding the
         *          thread(s) in the process containing this address space, 
         *          the thread name field of the returned message is empty.
         *          The caller must provide this information as needed.
         */
        operator CBTF_Protocol_LinkedObjectGroup() const;

        /**
         * Get the linked objects contained within this address space. An empty
         * set is returned if no linked objects are found within this address
         * space.
         *
         * @return    Linked Objects contained within this address space.
         */
        std::set<LinkedObject> getLinkedObjects() const;

        /**
         * Get the linked object contained within this address space at the
         * given address and time. An empty optional is returned if no linked
         * objects are found within this address space at that address and time.
         *
         * @param address    Address to be found.
         * @param time       Time to be found. Has a default value of "now".
         * @return           Linked object contained within this address space
         *                   at that address and time.
         */
        boost::optional<LinkedObject> getLinkedObjectAt(
            const Address& address, const Time& time = Time::Now()
            ) const;

        /**
         * Get the linked objects contained within this address space for the
         * given path. An empty set is returned if no linked objects are found
         * within this address space for that path.
         *
         * @param path    Path for which to obtain linked objects.
         * @return        Linked objects contained within 
         */
        std::set<LinkedObject> getLinkedObjectsByPath(
            const boost::filesystem::path& path
            ) const;

        /**
         * Add the given linked object to this address space at the specified
         * address range for the given time interval.
         *
         * @param linked_object    Linked object to add to this addres space.
         * @param range            Address range of this linked object.
         * @param interval         Time interval for this linked object.
         */
        void addLinkedObject(const LinkedObject& linked_object,
                             const AddressRange& range,
                             const TimeInterval& interval = TimeInterval(
                                 Time::TheBeginning(), Time::TheEnd()
                                 ));
        
        /**
         * Apply the given message, describing the load of a linked object,
         * to this address space.
         *
         * @param message    Message describing a loaded linked object.
         *
         * @note    Because this class contains no information regarding the
         *          thread(s) in the process containing this address space,
         *          the thread names field of the given message is ignored.
         */
        void apply(const CBTF_Protocol_LoadedLinkedObject& message);

        /**
         * Apply the given message, describing the unload of a linked object,
         * to this address space.
         *
         * @param message    Message describing an unloaded linked object.
         *
         * @note    Because this class contains no information regarding the
         *          thread(s) in the process containing this address space,
         *          the thread names field of the given message is ignored.
         */
        void apply(const CBTF_Protocol_UnloadedLinkedObject& message);

    private:

        /**
         * Opaque pointer to this object's internal implementation details.
         * Provides information hiding, improves binary compatibility, and
         * reduces compile times.
         *
         * @sa http://en.wikipedia.org/wiki/Opaque_pointer
         */
        Impl::AddressSpaceImpl* dm_impl;

    }; // class AddressSpace
        
} } // namespace KrellInstitute::SymbolTable
