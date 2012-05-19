/*******************************************************************************
** Copyright (c) 2007,2008 William Hachfeld. All Rights Reserved.
** Copyright (c) 2011 Krell Institute. All Rights Reserved.
**
** This program is free software; you can redistribute it and/or modify it under
** the terms of the GNU General Public License as published by the Free Software
** Foundation; either version 2 of the License, or (at your option) any later
** version.
**
** This program is distributed in the hope that it will be useful, but WITHOUT
** ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
** FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
** details.
**
** You should have received a copy of the GNU General Public License along with
** this program; if not, write to the Free Software Foundation, Inc., 59 Temple
** Place, Suite 330, Boston, MA  02111-1307  USA
*******************************************************************************/

/** @file
 *
 * Specification of the linkedobject events communication protocol.
 *
 */

%#include "Address.h"
%#include "Blob.h"
%#include "File.h"
%#include "Thread.h"
%#include "Time.h"


/**
 * Linked object has been loaded.
 *
 * Issued by a backend to indicate that the specified linked object has
 * been loaded into the address space of the specified threads. Includes
 * the time at which the load occurred as well as a description of what
 * was loaded.
 *
 * @note    This message is always sent multiple times in conjunction with
 *          an AttachedToThreads message. But it is also sent by a backend
 *          when a linked object is loaded due to a dlopen().
 */
struct CBTF_Protocol_LoadedLinkedObject
{
    /** Threads which loaded the linked object. */
    CBTF_Protocol_ThreadNameGroup threads;

    /** Time at which the linked object was loaded. */
    CBTF_Protocol_Time time;
    
    /** Address range at which this linked object was loaded. */
    CBTF_Protocol_AddressRange range;

    /** Name of the linked object's file. */
    CBTF_Protocol_FileName linked_object;

    /**
     * Boolean "true" if this linked object is an
     * executable, or "false" otherwise.
     */
    bool is_executable;
};



/**
 * Linked object has been unloaded.
 *
 * Issued by a backend to indicate that the specified linked object has
 * been unloaded from the address space of the specified threads. Includes
 * the time at which the unload occurred as well as a description of what
 * was unloaded.
 *
 * @note    This message is sent only when a linked object is unloaded
 *          due to a dlclose() in the threads.
 */
struct CBTF_Protocol_UnloadedLinkedObject
{
    /** Threads which unloaded the linked object. */
    CBTF_Protocol_ThreadNameGroup threads;

    /** Time at which the linked object was unloaded. */
    CBTF_Protocol_Time time;
    
    /** Name of the linked object's file. */
    CBTF_Protocol_FileName linked_object;
};


/** Structure of the blob containing our file objects.
 * Used by libmonitor based collection code.
*/
struct CBTF_Protocol_LinkedObject {

    /** Name of the linked object's file. */
    CBTF_Protocol_FileName linked_object;

    /** Address range at which this linked object was loaded. */
    CBTF_Protocol_AddressRange range;

    /** Time at which the linked object was loaded. */
    CBTF_Protocol_Time time_begin;

    /** Time at which the linked object was unloaded. */
    /** For this purpose, the end time is -ULL */
    CBTF_Protocol_Time time_end;

    /**
     * Boolean "true" if this linked object is an
     * executable, or "false" otherwise.
     */
    bool is_executable;
};

/* this message is the group of linked object initially
 * loaded in a process or thread. Used by libmonitor based
 * collection code.
 */
struct CBTF_Protocol_LinkedObjectGroup {
    /** Thread which contains these linked objects. */
    CBTF_Protocol_ThreadName thread;
    CBTF_Protocol_LinkedObject linkedobjects<>;
};
