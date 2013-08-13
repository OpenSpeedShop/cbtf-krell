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

#include <KrellInstitute/Messages/LinkedObjectEvents.h>
#include <KrellInstitute/Messages/Symbol.h>
#include <KrellInstitute/SymbolTable/AddressRange.hpp>
#include <KrellInstitute/SymbolTable/FileName.hpp>
#include <KrellInstitute/SymbolTable/LinkedObject.hpp>
#include <KrellInstitute/SymbolTable/LinkedObjectVisitor.hpp>
#include <KrellInstitute/SymbolTable/MappingVisitor.hpp>
#include <KrellInstitute/SymbolTable/TimeInterval.hpp>
#include <map>
#include <vector>

namespace KrellInstitute { namespace SymbolTable {

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
         * Apply the given message, describing the load of a linked object,
         * to this address space.
         *
         * @param message    Message describing a loaded linked object.
         *
         * @note    Because this class contains no information regarding the
         *          thread(s) in the process containing this address space,
         *          the thread names field of the given message is ignored.
         */
        void applyMessage(const CBTF_Protocol_LoadedLinkedObject& message);

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
        void applyMessage(const CBTF_Protocol_UnloadedLinkedObject& message);

        /**
         * Apply the given message, describing the symbol table of a linked
         * object, to this address space.
         *
         * @param message    Message describing a linked object's symbol table.
         */
        void applyMessage(const CBTF_Protocol_SymbolTable& message);

        /**
         * Load the given linked object in this address space at the specified
         * address range and time.
         *
         * @param linked_object    Linked object to be loaded.
         * @param is_executable    Is this linked object an executable?
         * @param range            Address range of this linked object.
         * @param when             Time when this linked object was loaded.
         */
        void loadLinkedObject(const LinkedObject& linked_object,
                              bool is_executable,
                              const AddressRange& range,
                              const Time& when = Time::TheBeginning());
        
        /**
         * Unloaded the given linked object from this address space at the
         * specified time.
         *
         * @param linked_object    Linked object to be unloaded.
         * @param when             Time when this linked object was unloaded.
         */
        void unloadLinkedObject(const LinkedObject& linked_object,
                                const Time& when = Time::TheEnd());
        
        /**
         * Visit the linked objects contained within this address space.
         *
         * @param visitor    Visitor invoked for each linked object contained
         *                   within this address space.
         */
        void visitLinkedObjects(const LinkedObjectVisitor& visitor) const;

        /**
         * Visit the mappings contained within this address space.
         *
         * @param visitor    Visitor invoked for each mapping contained
         *                   within this address space.
         */
        void visitMappings(const MappingVisitor& visitor) const;
        
        /**
         * Visit the mappings contained within this address space intersecting
         * the given address range and time interval.
         *
         * @param range       Address range to be found.
         * @param interval    Time interval to be found.
         * @param visitor     Visitor invoked for each mapping contained
         *                    within this address space that intersect that
         *                    address range and time interval.
         */
        void visitMappings(const AddressRange& range,
                           const TimeInterval& interval,
                           const MappingVisitor& visitor) const;
        
    private:

        /** Structure representing one mapping into an address space. */
        struct MappingItem
        {
            /** Linked object being mapped into the address space. */
            LinkedObject dm_linked_object;
            
            /** Is that linked object an executable? */
            bool dm_is_executable;
            
            /** Address range of that linked object in the address space. */
            AddressRange dm_range;
            
            /** Time interval for this linked object in the address space. */
            TimeInterval dm_interval;
            
            /** Constructor from initial fields. */
            MappingItem(const LinkedObject& linked_object,
                        bool is_executable,
                        const AddressRange& range,
                        const TimeInterval& interval) :
                dm_linked_object(linked_object),
                dm_is_executable(is_executable),
                dm_range(range),
                dm_interval(interval)
            {
            }
            
        }; // struct MappingItem
        
        /** (Indexed) List of linked objects in this address space. */
        std::map<FileName, LinkedObject> dm_linked_objects;
        
        /** List of mappings into this address space. */
        std::vector<MappingItem> dm_mappings;
        
    }; // class AddressSpace

    /**
     * Are the two given address spaces equivalent?
     *
     * @param first     First address space to be compared.
     * @param second    Second address space to be compared.
     * @return          Boolean "true" if the two address spaces are
     *                  equivalent, or "false" otherwise.
     */
    bool equivalent(const AddressSpace& first, const AddressSpace& second);
       
} } // namespace KrellInstitute::SymbolTable
