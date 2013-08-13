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

/** @file Definition of the AddressSpace class. */

#include <boost/bind.hpp>
#include <boost/ref.hpp>
#include <KrellInstitute/SymbolTable/AddressSpace.hpp>

using namespace KrellInstitute::SymbolTable;
using namespace KrellInstitute::SymbolTable::Impl;



/** Anonymous namespace hiding implementation details. */
namespace {

    /**
     * Visitor used to determine if an arbitrary set of visited mappings
     * contains a mapping equivalent to the given mapping. The visitation
     * is terminated as soon a such a mapping is found.
     */
    bool containsEquivalentMapping(const LinkedObject& linked_object,
                                   const AddressRange& range,
                                   const TimeInterval& interval,
                                   const LinkedObject& x_linked_object,
                                   const AddressRange& x_range,
                                   const TimeInterval& x_interval,
                                   bool& contains)
    {        
        contains |= ((linked_object.getFile() == x_linked_object.getFile()) &&
                     (range == x_range) &&
                     (interval == x_interval));
        return !contains;
    }
    
    /**
     * Visitor for determining if the given address space contains a mapping
     * equivalent to all mappings in an arbitrary set of visited mappings.
     * The visitation is terminated as soon as a mapping is encountered that
     * doesn't have an equivalent in the given address space.
     */
    bool containsAllMappings(const AddressSpace& address_space,
                             const LinkedObject& x_linked_object,
                             const AddressRange& x_range,
                             const TimeInterval& x_interval,
                             bool& contains)
    {
        bool contains_this = false;

        address_space.visitMappings(
            x_range, x_interval, boost::bind(
                containsEquivalentMapping,
                x_linked_object, x_range, x_interval,
                _1, _2, _3,
                boost::ref(contains_this)
                )
            );
        
        contains &= contains_this;
        return contains;
    }
    
} // namespace <anonymous>



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
AddressSpace::AddressSpace() :
    dm_linked_objects(),
    dm_mappings()
{
}


        
//------------------------------------------------------------------------------
// Iterate over each mapping in the given CBTF_Protocol_LinkedObjectGroup and
// construct the corresponding entries in this address space. Existing linked
// objects are used when found and new linked objects are added as necessary.
//------------------------------------------------------------------------------
AddressSpace::AddressSpace(const CBTF_Protocol_LinkedObjectGroup& message) :
    dm_linked_objects(),
    dm_mappings()
{
    for (u_int i = 0; i < message.linkedobjects.linkedobjects_len; ++i)
    {
        const CBTF_Protocol_LinkedObject& entry =
            message.linkedobjects.linkedobjects_val[i];
        
        FileName file = entry.linked_object;

        std::map<FileName, LinkedObject>::const_iterator j = 
            dm_linked_objects.find(file);

        if (j == dm_linked_objects.end())
        {
            j = dm_linked_objects.insert(
                std::make_pair(file, LinkedObject(file))
                ).first;
        }
        
        dm_mappings.push_back(
            MappingItem(j->second, entry.is_executable, entry.range,
                        TimeInterval(entry.time_begin, entry.time_end))
            );
    }
}



//------------------------------------------------------------------------------
// Iterate over each mapping in this address space and construct corresponding
// entries in a CBTF_Protocol_LinkedObjectGroup.
//------------------------------------------------------------------------------
AddressSpace::operator CBTF_Protocol_LinkedObjectGroup() const
{
    CBTF_Protocol_LinkedObjectGroup message;
  
    message.linkedobjects.linkedobjects_len = dm_mappings.size();

    message.linkedobjects.linkedobjects_val = 
        reinterpret_cast<CBTF_Protocol_LinkedObject*>(
            malloc(std::max(1U, message.linkedobjects.linkedobjects_len) *
                   sizeof(CBTF_Protocol_LinkedObject))
            );
    
    for (std::vector<MappingItem>::size_type i = 0; i < dm_mappings.size(); ++i)
    {
        const MappingItem& item = dm_mappings[i];
        
        CBTF_Protocol_LinkedObject& entry = 
            message.linkedobjects.linkedobjects_val[i];
        
        entry.linked_object = item.dm_linked_object.getFile();
        entry.range = item.dm_range;
        entry.time_begin = item.dm_interval.begin();
        entry.time_end = item.dm_interval.end();
        entry.is_executable = item.dm_is_executable;
    }
    
    return message;
}



//------------------------------------------------------------------------------
// Simply call loadLinkedObject() with values provided in the given message.
//------------------------------------------------------------------------------
void AddressSpace::applyMessage(const CBTF_Protocol_LoadedLinkedObject& message)
{
    loadLinkedObject(LinkedObject(message.linked_object),
                     message.is_executable ? true : false,
                     message.range, message.time);
}



//------------------------------------------------------------------------------
// Simply call unloadLinkedObject() with values provided in the given message.
//------------------------------------------------------------------------------
void AddressSpace::applyMessage(
    const CBTF_Protocol_UnloadedLinkedObject& message
    )
{
    unloadLinkedObject(LinkedObject(message.linked_object), message.time);
}



//------------------------------------------------------------------------------
// Construct a LinkedObject for the given CBTF_Protocol_SymbolTable, and then
// add it to this address space if the linked object isn't already present, or
// replace the existing linked object if it is already present. Replacement is
// made more complex because not only does the (indexed) list of linked objects
// need to be updated, but all of the relevant mappings as well. Otherwise the
// mappings would point to the previous linked object.
//
// Note: A linear search is currently used to locate the mappings of a linked
//       object that was already present in this address space. If the search
//       becomes a bottleneck, a simple fix is to introduce an index to find
//       mappings by their linked object's file name.
//------------------------------------------------------------------------------
void AddressSpace::applyMessage(const CBTF_Protocol_SymbolTable& message)
{
    FileName file = message.linked_object;
    
    std::map<FileName, LinkedObject>::iterator i = dm_linked_objects.find(file);
    
    if (i == dm_linked_objects.end())
    {
        dm_linked_objects.insert(std::make_pair(file, LinkedObject(message)));
    }
    else
    {
        i->second = LinkedObject(message);
        
        for (std::vector<MappingItem>::iterator
                 j = dm_mappings.begin(); j != dm_mappings.end(); ++j)
        {
            if (j->dm_linked_object.getFile() == file)
            {
                j->dm_linked_object = i->second;
            }
        }
    }
}



//------------------------------------------------------------------------------
// Construct a MappingItem from the given values and add it to this address
// space. Existing linked objects are used when found and new linked objects
// are added as necessary.
//------------------------------------------------------------------------------
void AddressSpace::loadLinkedObject(const LinkedObject& linked_object,
                                    bool is_executable,
                                    const AddressRange& range,
                                    const Time& when)
{
    std::map<FileName, LinkedObject>::const_iterator i = 
        dm_linked_objects.find(linked_object.getFile());
    
    if (i == dm_linked_objects.end())
    {
        i = dm_linked_objects.insert(
            std::make_pair(linked_object.getFile(), linked_object)
            ).first;
    }
    
    dm_mappings.push_back(
        MappingItem(i->second, is_executable, range,
                    TimeInterval(when, Time::TheEnd()))
        );
}



//------------------------------------------------------------------------------
// Locate all of the mappings that reference the linked object being unloaded,
// and that have an end time that is the last possible time, and update those
// end times to be the given time.
//
// Note: A linear search is currently used to locate the mappings. If the
//       search becomes a bottleneck, the fix is to introduce an index to
//       find mappings by their linked object's file name.
//------------------------------------------------------------------------------
void AddressSpace::unloadLinkedObject(const LinkedObject& linked_object,
                                      const Time& when)
{
    for (std::vector<MappingItem>::iterator
             i = dm_mappings.begin(); i != dm_mappings.end(); ++i)
    {
        if ((i->dm_linked_object.getFile() == linked_object.getFile()) &&
            (i->dm_interval.end() == Time::TheEnd()))
        {
            i->dm_interval = TimeInterval(i->dm_interval.begin(), when);
        }
    }
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void AddressSpace::visitLinkedObjects(const LinkedObjectVisitor& visitor) const
{
    bool terminate = false;
    
    for (std::map<FileName, LinkedObject>::const_iterator
             i = dm_linked_objects.begin();
         !terminate && (i != dm_linked_objects.end());
         ++i)
    {
        terminate |= visitor(i->second);
    }
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void AddressSpace::visitMappings(const MappingVisitor& visitor) const
{
    bool terminate = false;
    
    for (std::vector<MappingItem>::const_iterator
             i = dm_mappings.begin(), iEnd = dm_mappings.end();
         !terminate && (i != iEnd);
         ++i)
    {
        terminate |= visitor(i->dm_linked_object, i->dm_range, i->dm_interval);
    }
}



//------------------------------------------------------------------------------
// Note: A linear search is currently used to locate the mappings. If the
//       search becomes a bottleneck, the fix is to introduce an index to
//       find mappings by their address ranges and time intervals.
//------------------------------------------------------------------------------
void AddressSpace::visitMappings(const AddressRange& range,
                                 const TimeInterval& interval,
                                 const MappingVisitor& visitor) const
{
    bool terminate = false;
    
    for (std::vector<MappingItem>::const_iterator
             i = dm_mappings.begin(), iEnd = dm_mappings.end();
         !terminate && (i != iEnd);
         ++i)
    {
        if (i->dm_range.intersects(range) &&
            i->dm_interval.intersects(interval))
        {
            terminate |= visitor(i->dm_linked_object,
                                 i->dm_range, i->dm_interval);
        }
    }
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool KrellInstitute::SymbolTable::equivalent(const AddressSpace& first,
                                             const AddressSpace& second)
{
    // Is "second" missing any of the mappings from "first"?
    
    bool contains = true;
    
    first.visitMappings(
        boost::bind(
            containsAllMappings, second, _1, _2, _3, boost::ref(contains)
            )
        );
    
    if (!contains)
    {
        return false;
    }
    
    // Is "first" missing any of the mappings from "second"?

    contains = true;
    
    second.visitMappings(
        boost::bind(
            containsAllMappings, first, _1, _2, _3, boost::ref(contains)
            )
        );
    
    if (!contains)
    {
        return false;
    }
    
    // Otherwise the address spaces are equivalent
    return true;
}
