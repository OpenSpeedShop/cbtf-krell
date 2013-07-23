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

#include <KrellInstitute/SymbolTable/AddressSpace.hpp>

using namespace KrellInstitute::SymbolTable;
using namespace KrellInstitute::SymbolTable::Impl;



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

        FileName name = entry.linked_object;

        std::map<FileName, LinkedObject>::const_iterator j = 
            dm_linked_objects.find(name);

        if (j == dm_linked_objects.end())
        {
            j = dm_linked_objects.insert(
                std::make_pair(name, LinkedObject(name))
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
        
        entry.linked_object = item.dm_linked_object.getName();
        entry.range = item.dm_range;
        entry.time_begin = item.dm_interval.getBegin();
        entry.time_end = item.dm_interval.getEnd();
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
    FileName name = message.linked_object;
    
    std::map<FileName, LinkedObject>::iterator i = dm_linked_objects.find(name);
    
    if (i == dm_linked_objects.end())
    {
        dm_linked_objects.insert(std::make_pair(name, LinkedObject(message)));
    }
    else
    {
        i->second = LinkedObject(message);
        
        for (std::vector<MappingItem>::iterator
                 j = dm_mappings.begin(); j != dm_mappings.end(); ++j)
        {
            if (j->dm_linked_object.getName() == name)
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
        dm_linked_objects.find(linked_object.getName());
    
    if (i == dm_linked_objects.end())
    {
        i = dm_linked_objects.insert(
            std::make_pair(linked_object.getName(), linked_object)
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
        if ((i->dm_linked_object.getName() == linked_object.getName()) &&
            (i->dm_interval.getEnd() == Time::TheEnd()))
        {
            i->dm_interval = TimeInterval(i->dm_interval.getBegin(), when);
        }
    }
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void AddressSpace::visitLinkedObjects(LinkedObjectVisitor& visitor) const
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
void AddressSpace::visitMappings(MappingVisitor& visitor) const
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
                                 MappingVisitor& visitor) const
{
    bool terminate = false;
    
    for (std::vector<MappingItem>::const_iterator
             i = dm_mappings.begin(), iEnd = dm_mappings.end();
         !terminate && (i != iEnd);
         ++i)
    {
        if (i->dm_range.doesIntersect(range) &&
            i->dm_interval.doesIntersect(interval))
        {
            terminate |= visitor(i->dm_linked_object,
                                 i->dm_range, i->dm_interval);
        }
    }
}
