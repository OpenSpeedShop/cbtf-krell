////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2011 Krell Institute. All Rights Reserved.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License as published by the Free
// Software Foundation; either version 2.1 of the License, or (at your option)
// any later version.
//
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
// details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, write to the Free Software Foundation, Inc.,
// 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
////////////////////////////////////////////////////////////////////////////////

/** @file Plugin used by unit tests for the CBTF MRNet library. */

#include <boost/bind.hpp>
#include <boost/operators.hpp>
#include <mrnet/MRNet.h>
#include <typeinfo>
#include <algorithm>

#include <KrellInstitute/CBTF/Component.hpp>
#include <KrellInstitute/CBTF/Type.hpp>
#include <KrellInstitute/CBTF/Version.hpp>

#include "KrellInstitute/Core/Address.hpp"
#include "KrellInstitute/Core/AddressBuffer.hpp"
#include "KrellInstitute/Core/AddressRange.hpp"
#include "KrellInstitute/Core/Blob.hpp"
#include "KrellInstitute/Core/LinkedObjectEntry.hpp"
#include "KrellInstitute/Core/Path.hpp"
#include "KrellInstitute/Core/PCData.hpp"
#include "KrellInstitute/Core/StacktraceData.hpp"
#include "KrellInstitute/Core/SymbolTable.hpp"
#include "KrellInstitute/Core/SymtabAPISymbols.hpp"
#include "KrellInstitute/Core/Time.hpp"
#include "KrellInstitute/Core/ThreadName.hpp"

#include "KrellInstitute/Messages/Address.h"
#include "KrellInstitute/Messages/DataHeader.h"
#include "KrellInstitute/Messages/EventHeader.h"
#include "KrellInstitute/Messages/File.h"
#include "KrellInstitute/Messages/LinkedObjectEvents.h"
#include "KrellInstitute/Messages/PCSamp_data.h"
#include "KrellInstitute/Messages/Usertime_data.h"
#include "KrellInstitute/Messages/Thread.h"
#include "KrellInstitute/Messages/ThreadEvents.h"


using namespace KrellInstitute::CBTF;
using namespace KrellInstitute::Core;

namespace { 


    int handled_threads = 0;

#ifndef NDEBUG
/** Flag indicating if debuging for LinkedObjects is enabled. */
bool is_debug_linkedobject_events_enabled =
    (getenv("CBTF_DEBUG_LINKEDOBJECT_EVENTS") != NULL);
#endif

}

/**
 * Component that records linked objects.
 */
class __attribute__ ((visibility ("hidden"))) LinkedObject :
    public Component
{

public:

    /** Factory function for this component type. */
    static Component::Instance factoryFunction()
    {
        return Component::Instance(
            reinterpret_cast<Component*>(new LinkedObject())
            );
    }

private:

    /** Default constructor. */
    LinkedObject() :
        Component(Type(typeid(LinkedObject)), Version(0, 0, 1))
    {
        declareInput<ThreadNameVec>(
            "threadnames", boost::bind(&LinkedObject::threadnamesHandler, this, _1)
            );
        declareInput<boost::shared_ptr<CBTF_Protocol_LoadedLinkedObject> >(
            "loaded", boost::bind(&LinkedObject::loadedHandler, this, _1)
            );
	declareOutput<boost::shared_ptr<CBTF_Protocol_LoadedLinkedObject> >("loaded_xdr_out");

        declareInput<boost::shared_ptr<CBTF_Protocol_UnloadedLinkedObject> >(
            "unloaded", boost::bind(&LinkedObject::unloadedHandler, this, _1)
            );
	declareOutput<boost::shared_ptr<CBTF_Protocol_UnloadedLinkedObject> >("unloaded_xdr_out");

        declareInput<boost::shared_ptr<CBTF_Protocol_LinkedObjectGroup> >(
            "group", boost::bind(&LinkedObject::groupHandler, this, _1)
            );
	declareOutput<boost::shared_ptr<CBTF_Protocol_LinkedObjectGroup> >("group_xdr_out");
	declareOutput<LinkedObjectEntryVec>("linkedobjectvec_out");
    }

    /** Handlers for the inputs.*/
    void threadnamesHandler(const ThreadNameVec& in)
    {
	threadnames = in;
	//std::cerr  << "LinkedObject::threadnamesHandler threadnames size is " << threadnames.size() << std::endl;
    }


    void groupHandler(const boost::shared_ptr<CBTF_Protocol_LinkedObjectGroup>& in)
    {
        CBTF_Protocol_LinkedObjectGroup *message = in.get();
	ThreadName tname(message->thread);

	LinkedObjectEntryVec linkedobjects;
	for(int i = 0; i < message->linkedobjects.linkedobjects_len; ++i) {
	    const CBTF_Protocol_LinkedObject& msg_lo =
				message->linkedobjects.linkedobjects_val[i];
	    LinkedObjectEntry entry;
	    entry.tname = tname;
	    entry.path = msg_lo.linked_object.path;
	    entry.addr_begin = msg_lo.range.begin;
	    entry.addr_end = msg_lo.range.end;
	    entry.is_executable = msg_lo.is_executable;
	    entry.time_loaded = msg_lo.time_begin;
	    entry.time_unloaded = Time::TheEnd();
	    linkedobjectvec.push_back(entry);
	}

#ifndef NDEBUG
	if (is_debug_linkedobject_events_enabled) {
	    std::cerr  << "LinkedObject::groupHandler thread " << tname.getHost()
	    	<< ":" << tname.getPid().second
	    	<< ":" << tname.getPosixThreadId().second
	    	<< std::endl;
	    std::cerr  << "LinkedObject::groupHandler linkedobjects size is "
		<< linkedobjectvec.size() << std::endl;
	}
#endif

	// We process threads before linkedobjects. Therefore we know how
	// many threads to expect to handle for linkedobjectgroup's sent
	// from the collector.  Once we have them all we should send the
	// vector of linked objects to the client.
	handled_threads++;
	if (handled_threads == threadnames.size() ) {

#ifndef NDEBUG
	    if (is_debug_linkedobject_events_enabled) {
	    std::cerr  << "LinkedObject::groupHandler handled "
		<< threadnames.size() << " pending threads. " << std::endl;
	    }
#endif
	    
	    emitOutput<LinkedObjectEntryVec>("linkedobjectvec_out",linkedobjectvec);
	}

	// collector runtimes send this message.  we would rather emit the
	// linkedobject vector.
	//emitOutput<boost::shared_ptr<CBTF_Protocol_LinkedObjectGroup> >("group_xdr_out", in);
    }

    // Handler for dlopen events.
    void loadedHandler(const boost::shared_ptr<CBTF_Protocol_LoadedLinkedObject>& in)
    {
        CBTF_Protocol_LoadedLinkedObject *message = in.get();
	LinkedObjectEntry entry;

	for(int i = 0; i < message->threads.names.names_len; ++i) {
	    const CBTF_Protocol_ThreadName& msg_thread =
				message->threads.names.names_val[i];

	    ThreadName tname(msg_thread);
	    entry.tname = tname;
	    entry.path = message->linked_object.path;
	    entry.addr_begin = message->range.begin;
	    entry.addr_end = message->range.end;
	    entry.is_executable = message->is_executable;
	    entry.time_loaded = message->time;
	    entry.time_unloaded = Time::Now();

	    linkedobjectvec.push_back(entry);

// used to show the linkedobject information sent.
#ifndef NDEBUG
	    if (is_debug_linkedobject_events_enabled) {
	    std::cerr << "path " << entry.path
	    << " loaded at time " << entry.time_loaded.getValue()
	    << " unloaded at time " << entry.time_unloaded.getValue()
	    << " at " << AddressRange(entry.addr_begin,entry.addr_end)
	    << " in thread " << entry.tname.getHost()
	    << ":" << entry.tname.getPid().second
	    << ":" <<  entry.tname.getPosixThreadId().second
	    << std::endl;
	    }
#endif
	}
	emitOutput<boost::shared_ptr<CBTF_Protocol_LoadedLinkedObject> >("loaded_xdr_out", in);
    }

    // Handler for dlclose events.
    void unloadedHandler(const boost::shared_ptr<CBTF_Protocol_UnloadedLinkedObject>& in)
    {
	emitOutput<boost::shared_ptr<CBTF_Protocol_UnloadedLinkedObject> >("unloaded_xdr_out", in);
    }

    // vector of linkedobjects with thread info. output when we handle the
    // number of pending threads.
    LinkedObjectEntryVec linkedobjectvec;

    // vector of incoming threadnames. For each thread we expect
    ThreadNameVec threadnames;

}; // class LinkedObject

KRELL_INSTITUTE_CBTF_REGISTER_FACTORY_FUNCTION(LinkedObject)
