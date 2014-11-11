////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2011-2014 Krell Institute. All Rights Reserved.
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

/** @file LinkedObjectComponent. */

#include <boost/bind.hpp>
#include <boost/make_shared.hpp>
#include <boost/operators.hpp>
#include <mrnet/MRNet.h>
#include <typeinfo>
#include <algorithm>
#include <sstream>

#include <KrellInstitute/CBTF/Component.hpp>
#include <KrellInstitute/CBTF/Type.hpp>
#include <KrellInstitute/CBTF/Version.hpp>
#include <KrellInstitute/CBTF/Impl/MRNet.hpp>

#include "KrellInstitute/Core/Address.hpp"
#include "KrellInstitute/Core/AddressBuffer.hpp"
#include "KrellInstitute/Core/AddressRange.hpp"
#include "KrellInstitute/Core/AddressSpace.hpp"
#include "KrellInstitute/Core/LinkedObject.hpp"
#include "KrellInstitute/Core/LinkedObjectEntry.hpp"
#include "KrellInstitute/Core/Path.hpp"
#include "KrellInstitute/Core/Time.hpp"
#include "KrellInstitute/Core/TimeInterval.hpp"
#include "KrellInstitute/Core/ThreadName.hpp"

#include "KrellInstitute/Messages/Address.h"
#include "KrellInstitute/Messages/EventHeader.h"
#include "KrellInstitute/Messages/File.h"
#include "KrellInstitute/Messages/LinkedObjectEvents.h"
#include "KrellInstitute/Messages/Thread.h"
#include "KrellInstitute/Messages/ThreadEvents.h"


using namespace KrellInstitute::CBTF;
using namespace KrellInstitute::Core;

/** requires std::ostringstream debug_prefix in namespace **/
#define DEBUGPREFIX(x,y) \
	if (debug_prefix.str().empty()) { \
	    if (x) debug_prefix << "FE:"; \
	    else  if (y == 1) debug_prefix << "LCP:"; \
	    else  debug_prefix << "ICP:"; \
	    debug_prefix << getpid() << " "; \
	}

namespace { 

/** count indicating number of threads attached. */
    int handled_threads = 0;

#ifndef NDEBUG
/** Flag indicating if debuging for LinkedObjects is enabled. */
bool is_debug_linkedobject_events_enabled =
    (getenv("CBTF_DEBUG_LINKEDOBJECT_EVENTS") != NULL);
bool is_trace_linkedobject_events_enabled =
    (getenv("CBTF_TRACE_LINKEDOBJECT_EVENTS") != NULL);
#endif

bool is_defer_emit = (getenv("CBTF_DEFER_LINKEDOBJECT_EMIT") != NULL);
bool isFE = false;
std::ostringstream debug_prefix;

}

/**
 * Component that records linked objects.
 */
class __attribute__ ((visibility ("hidden"))) LinkedObjectComponent :
    public Component
{

public:

    /** Factory function for this component type. */
    static Component::Instance factoryFunction()
    {
        return Component::Instance(
            reinterpret_cast<Component*>(new LinkedObjectComponent())
            );
    }

private:

    /** Default constructor. */
    LinkedObjectComponent() :
        Component(Type(typeid(LinkedObjectComponent)), Version(0, 0, 1))
    {
        declareInput<ThreadNameVec>(
            "threadnames", boost::bind(&LinkedObjectComponent::threadnamesHandler, this, _1)
            );
	declareInput<AddressBuffer>(
	    "abufferin", boost::bind(&LinkedObjectComponent::AddressBufferHandler, this, _1)
	    );
        declareInput<boost::shared_ptr<CBTF_Protocol_LoadedLinkedObject> >(
            "loaded", boost::bind(&LinkedObjectComponent::loadedHandler, this, _1)
            );
        declareInput<boost::shared_ptr<CBTF_Protocol_UnloadedLinkedObject> >(
            "unloaded", boost::bind(&LinkedObjectComponent::unloadedHandler, this, _1)
            );
        declareInput<boost::shared_ptr<CBTF_Protocol_LinkedObjectGroup> >(
            "group", boost::bind(&LinkedObjectComponent::groupHandler, this, _1)
            );

	declareOutput<boost::shared_ptr<CBTF_Protocol_LoadedLinkedObject> >("loaded_xdr_out");
	declareOutput<boost::shared_ptr<CBTF_Protocol_UnloadedLinkedObject> >("unloaded_xdr_out");
	declareOutput<boost::shared_ptr<CBTF_Protocol_LinkedObjectGroup> >("group_xdr_out");
	declareOutput<LinkedObjectEntryVec>("linkedobjectvec_out");
	declareOutput<AddressSpace>("linkedobject_threadmap_out");

    }

    /** Handlers for the inputs.*/

    /** Handler for the "abuffer" input.*/
    void AddressBufferHandler(const AddressBuffer& in)
    {
#ifndef NDEBUG
	std::stringstream output;
	DEBUGPREFIX(Impl::TheTopologyInfo.IsFrontend,Impl::TheTopologyInfo.MaxLeafDistance);
        if (is_trace_linkedobject_events_enabled) {
	    output << debug_prefix.str()
	    << "ENTERED LinkedObject::AddressBufferHandler"
	    << " with " << in.addresscounts.size() << " addresses"
	    << std::endl;
	}
#endif
	abuffer = in;
	isFE = Impl::TheTopologyInfo.IsFrontend;

#ifndef NDEBUG
	if ( !output.str().empty() ) {
            std::cerr << output.str();
	}
#endif
    }

    void threadnamesHandler(const ThreadNameVec& in)
    {
#ifndef NDEBUG
	std::stringstream output;
	DEBUGPREFIX(Impl::TheTopologyInfo.IsFrontend,Impl::TheTopologyInfo.MaxLeafDistance);
	if (is_trace_linkedobject_events_enabled) {
	    output << debug_prefix.str()
	     << "ENTERED LinkedObjectComponent::threadnamesHandler with threads:"
		<< in.size() << std::endl;
	}
#endif
	threadnames = in;
	isFE = Impl::TheTopologyInfo.IsFrontend;

#ifndef NDEBUG
	if ( !output.str().empty() ) {
            std::cerr << output.str();
	}
#endif
    }


    void groupHandler(const boost::shared_ptr<CBTF_Protocol_LinkedObjectGroup>& in)
    {
        CBTF_Protocol_LinkedObjectGroup *message = in.get();
	handled_threads++;

#ifndef NDEBUG
	std::stringstream output;
	DEBUGPREFIX(Impl::TheTopologyInfo.IsFrontend,Impl::TheTopologyInfo.MaxLeafDistance);
	if (is_trace_linkedobject_events_enabled) {
	    output << debug_prefix.str()
	    << "ENTERED LinkedObjectComponent::groupHandler linked objects num:"
	    << message->linkedobjects.linkedobjects_len
	    << " handled_threads:" << handled_threads
	    << " of:" << threadnames.size()
	    << std::endl;
	}
#endif


	ThreadName tname(message->thread);
	AddressCounts ac = abuffer.addresscounts;
	bool havecounts = (ac.size() > 0) ? true : false ;
	bool havethreads = (threadnames.size() > 0) ? true : false ;
	LinkedObjectEntryVec linkedobjects;
        LinkedObjectVec linkedobjectvec;
	
	// indicies in group of linkedobjects with samples.
	std::vector<int> found;

	for(int i = 0; i < message->linkedobjects.linkedobjects_len; ++i) {
	    const CBTF_Protocol_LinkedObject& msg_lo =
				message->linkedobjects.linkedobjects_val[i];

	    // Limit linked objects to those with sample addresses.
	    bool has_sample = false;
	    AddressRange addr_range(msg_lo.range.begin,msg_lo.range.end);
	    AddressCounts::const_iterator aci;
	    for (aci=ac.equal_range(addr_range.getBegin()).first;
                aci!=ac.equal_range(addr_range.getEnd()).second;aci++) {
		has_sample = true;
		break;
	    }

	    if (has_sample ||  !havecounts) {
		LinkedObjectEntry entry;
		entry.tname = tname;
		entry.path = msg_lo.linked_object.path;
		entry.addr_begin = msg_lo.range.begin;
		entry.addr_end = msg_lo.range.end;
		entry.is_executable = msg_lo.is_executable;
		entry.time_loaded = msg_lo.time_begin;
		entry.time_unloaded = msg_lo.time_end;
	        linkedobjectentryvec.push_back(entry);
		// save this index.
		found.push_back(i);

		LinkedObject e;
		e.path = msg_lo.linked_object.path;
		e.is_executable = msg_lo.is_executable;
		e.time = TimeInterval(msg_lo.time_begin,msg_lo.time_end);
		e.range = AddressRange(msg_lo.range.begin,msg_lo.range.end);
	        linkedobjectvec.push_back(e);
	    }
	}
	addresspace.insert(std::make_pair(tname,linkedobjectvec));

#ifndef NDEBUG
	if (is_debug_linkedobject_events_enabled) {
	    output << debug_prefix.str()
	        << "LinkedObjectComponent::groupHandler tname:" << tname
	        << " objects:" << linkedobjectentryvec.size()
		<< std::endl;
	}
#endif
	// emits to local component network and not over mrnet.
	if ( (havethreads && handled_threads == threadnames.size()) ||
	      !havethreads ) {
	    if (!is_defer_emit) {
#ifndef NDEBUG
		if (is_debug_linkedobject_events_enabled) {
	    	output << debug_prefix.str()
	        << "LinkedObjectComponent::groupHandler EMIT LinkedObjectEntryVec size:" << linkedobjectentryvec.size() << std::endl;
		}
#endif
	        emitOutput<LinkedObjectEntryVec>("linkedobjectvec_out",linkedobjectentryvec);
	    }
#ifndef NDEBUG
	    if (is_debug_linkedobject_events_enabled) {
	    	output << debug_prefix.str()
	        << "LinkedObjectComponent::groupHandler EMIT AddressSpace size:" << addresspace.size() << std::endl;
	    }
#endif
	    emitOutput<AddressSpace>("linkedobject_threadmap_out",addresspace);
	}


	if (havecounts) {
	    boost::shared_ptr<CBTF_Protocol_LinkedObjectGroup> logroup(
		new CBTF_Protocol_LinkedObjectGroup()
            );
	    CBTF_Protocol_LinkedObject dest[found.size()];
	    memcpy(&logroup->thread, &message->thread, sizeof(CBTF_Protocol_ThreadName));
	    logroup->thread.host = strdup(message->thread.host);
	    logroup->linkedobjects.linkedobjects_len = found.size();
	    logroup->linkedobjects.linkedobjects_val =
		reinterpret_cast<CBTF_Protocol_LinkedObject*>(
		malloc(found.size() * sizeof(CBTF_Protocol_LinkedObject))
		);

	    int k = 0;
	    for (std::vector<int>::iterator ii = found.begin(); ii < found.end(); ++ii) {
		const CBTF_Protocol_LinkedObject* source =
			&message->linkedobjects.linkedobjects_val[*ii];
		CBTF_Protocol_LinkedObject* destination =
			&logroup->linkedobjects.linkedobjects_val[k];
		memcpy(destination, source, sizeof(CBTF_Protocol_LinkedObject));
		destination->linked_object.path = strdup(source->linked_object.path);
		++k;
	    }
#ifndef NDEBUG
	    if (is_debug_linkedobject_events_enabled) {
#if 0
		if ( (havethreads && handled_threads == threadnames.size()) ||
		      !havethreads ) {
		    AddressSpace::iterator i;
		    for (i = addresspace.begin(); i != addresspace.end(); ++i) {
	    		output << debug_prefix.str() << "addresspace  thread:" << (*i).first << std::endl;
			for (LinkedObjectVec::iterator k = (*i).second.begin();
			     k != (*i).second.end(); ++k) {
	    		    output << debug_prefix.str() << "\t name:" << (*k).getPath() << std::endl;
			}
		    }
		}
#endif
	    }
#endif
	    if (!is_defer_emit) {
#ifndef NDEBUG
		if (is_trace_linkedobject_events_enabled) {
	        output << debug_prefix.str()
	        << "LinkedObjectComponent::groupHandler EMIT reduced CBTF_Protocol_LinkedObjectGroup size:" << found.size() << std::endl;
		}
#endif
	        emitOutput<boost::shared_ptr<CBTF_Protocol_LinkedObjectGroup> >("group_xdr_out", logroup);
	    }

	} else {

	    if (!is_defer_emit) {
#ifndef NDEBUG
		if (is_debug_linkedobject_events_enabled) {
	        output << debug_prefix.str()
	        << "LinkedObjectComponent::groupHandler EMIT passed CBTF_Protocol_LinkedObjectGroup" << std::endl;
		}
#endif
	        emitOutput<boost::shared_ptr<CBTF_Protocol_LinkedObjectGroup> >("group_xdr_out", in);
	    }
	}

#ifndef NDEBUG
	if ( !output.str().empty() ) {
            std::cerr << output.str();
	}
#endif

    }

    // Handler for dlopen events.
    void loadedHandler(const boost::shared_ptr<CBTF_Protocol_LoadedLinkedObject>& in)
    {
        CBTF_Protocol_LoadedLinkedObject *message = in.get();
#ifndef NDEBUG
	std::stringstream output;
	DEBUGPREFIX(Impl::TheTopologyInfo.IsFrontend,Impl::TheTopologyInfo.MaxLeafDistance);
	if (is_trace_linkedobject_events_enabled) {
	    output << debug_prefix.str()
	     << "ENTERED LinkedObjectComponent::loadedHandler" << std::endl;
	}
#endif
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

	    linkedobjectentryvec.push_back(entry);

// used to show the linkedobject information sent.
#ifndef NDEBUG
	    if (is_debug_linkedobject_events_enabled) {
	    output << "path " << entry.path
	    << " loaded at time " << entry.time_loaded.getValue()
	    << " unloaded at time " << entry.time_unloaded.getValue()
	    << " at " << AddressRange(entry.addr_begin,entry.addr_end)
	    << " in thread " << entry.tname.getHost()
	    << ":" << entry.tname.getPid()
	    << ":" <<  entry.tname.getPosixThreadId().second
	    << std::endl;
	    }
#endif
	}

#ifndef NDEBUG
	if (is_debug_linkedobject_events_enabled) {
	    output << debug_prefix.str()
	     << "LinkedObjectComponent::loadedHandler EMIT CBTF_Protocol_LoadedLinkedObject" << std::endl;
	}
#endif

	emitOutput<boost::shared_ptr<CBTF_Protocol_LoadedLinkedObject> >("loaded_xdr_out", in);

#ifndef NDEBUG
	if ( !output.str().empty() ) {
            std::cerr << output.str();
	}
#endif

    }

    // Handler for dlclose events.
    void unloadedHandler(const boost::shared_ptr<CBTF_Protocol_UnloadedLinkedObject>& in)
    {
#ifndef NDEBUG
	std::stringstream output;
	DEBUGPREFIX(Impl::TheTopologyInfo.IsFrontend,Impl::TheTopologyInfo.MaxLeafDistance);
	if (is_trace_linkedobject_events_enabled) {
	    output << debug_prefix.str()
	     << "ENTERED LinkedObjectComponent::unloadedHandler" << std::endl;
	}
#endif

#ifndef NDEBUG
	if (is_debug_linkedobject_events_enabled) {
	    output << debug_prefix.str()
	     << "LinkedObjectComponent::unloadedHandler EMIT CBTF_Protocol_UnloadedLinkedObject" << std::endl;
	}
#endif
	emitOutput<boost::shared_ptr<CBTF_Protocol_UnloadedLinkedObject> >("unloaded_xdr_out", in);

#ifndef NDEBUG
	if ( !output.str().empty() ) {
            std::cerr << output.str();
	}
#endif
    }

    AddressBuffer abuffer;

    // vector of linkedobjects with thread info. output when we handle the
    // number of pending threads.
    LinkedObjectEntryVec linkedobjectentryvec;
    LinkedObjectVec linkedobjectvec;
    AddressSpace addresspace;

    // vector of incoming threadnames. For each thread we expect
    ThreadNameVec threadnames;

}; // class LinkedObjectComponent

KRELL_INSTITUTE_CBTF_REGISTER_FACTORY_FUNCTION(LinkedObjectComponent)
