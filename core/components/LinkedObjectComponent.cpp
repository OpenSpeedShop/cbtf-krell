////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2011-2015 Krell Institute. All Rights Reserved.
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

    std::ostringstream debug_prefix;
    void flushOutput(std::stringstream &output) {
	if ( !output.str().empty() ) {
	    std::cerr << output.str();
	    output.str(std::string());
	    output.clear();
	}
    }

/** count indicating number of linked object group messages handled. */
    int handled_threads = 0;

#ifndef NDEBUG
/** Flag indicating if debuging for LinkedObjects is enabled. */
    bool is_debug_linkedobject_events_enabled =
	(getenv("CBTF_DEBUG_LINKEDOBJECT_EVENTS") != NULL);
    bool is_trace_linkedobject_events_enabled =
	(getenv("CBTF_TRACE_LINKEDOBJECT_EVENTS") != NULL);
#endif

    bool is_defer_emit = (getenv("CBTF_DEFER_LINKEDOBJECT_EMIT") != NULL);
    long numTerminated = 0;
    long numThreads = 0;

    int _MaxLeafDistance = 0;
    int _NumChildren = 0;

    bool isFrontend() {
	return Impl::TheTopologyInfo.IsFrontend;
    }
    bool isLeafCP() {
	return (!Impl::TheTopologyInfo.IsFrontend && _MaxLeafDistance == 1);
    }
    bool isNonLeafCP() {
	return (!Impl::TheTopologyInfo.IsFrontend && _MaxLeafDistance > 1);
    }
    int getNumChildren() {
	 return _NumChildren;
    }
    int getMaxLeafDistance() {
	 return _MaxLeafDistance;
    }

    bool initialized_topology_info = false;

    void init_TopologyInfo() {

	if (initialized_topology_info) return;

	bool initMaxLeafDistance = false;
	bool initNumChildren = false;
	if (_MaxLeafDistance == 0 && Impl::TheTopologyInfo.MaxLeafDistance > 0) {
	    _MaxLeafDistance = Impl::TheTopologyInfo.MaxLeafDistance;
	    initMaxLeafDistance = true;
	}
	if (_NumChildren == 0 && Impl::TheTopologyInfo.NumChildren > 0) {
	    _NumChildren = Impl::TheTopologyInfo.NumChildren;
	    initNumChildren = true;
	}
	initialized_topology_info = (initMaxLeafDistance && initNumChildren);
    }

    /**
     * Convert std::string for mrnet use.
     *
     * @note    The caller assumes responsibility for releasing all allocated
     *          memory when it is no longer needed.
     *
     * @param in      std::string to be converted.
     * @retval out    Structure to hold the results.
     */
    void convert(const std::string& in, char*& out)
    {
        out = reinterpret_cast<char*>(malloc((in.size() + 1) * sizeof(char)));
        strcpy(out, in.c_str());
    }

    /**
     * Convert threadname for mrnet use.
     *
     * @note    The caller assumes responsibility for releasing all allocated
     *          memory when it is no longer needed.
     *
     * @param in      ThreadName to be converted.
     * @retval out    Structure to hold the results.
     */
    void convert(const ThreadName& in, CBTF_Protocol_ThreadName& out)
    {
	out.experiment = 1;
	convert(in.getHost(), out.host);
	out.pid = in.getPid();
	std::pair<bool, pthread_t> posix_tid = in.getPosixThreadId();
	out.has_posix_tid = posix_tid.first;
	if(posix_tid.first)
	    out.posix_tid = posix_tid.second;
	out.rank = in.getMPIRank();
	out.omp_tid = in.getOmpTid();
    }

    void convert(const LinkedObject& in, CBTF_Protocol_LinkedObject& out)
    {
	convert(in.getPath(), out.linked_object.path);
	out.is_executable = in.is_executable;
	out.is_executable = in.isExecutable();
	out.time_begin = in.getTimeInterval().getBegin().getValue();
	out.time_end = in.getTimeInterval().getEnd().getValue();
	out.range.begin = in.getAddressRange().getBegin().getValue();
	out.range.end = in.getAddressRange().getEnd().getValue();
    }

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
        declareInput<int>(
            "numBE", boost::bind(&LinkedObjectComponent::numBEHandler, this, _1)
            );
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
	declareInput<long>(
	    "numTerminatedIn", boost::bind(&LinkedObjectComponent::numTerminatedHandler, this, _1)
	);

	declareOutput<boost::shared_ptr<CBTF_Protocol_LoadedLinkedObject> >("loaded_xdr_out");
	declareOutput<boost::shared_ptr<CBTF_Protocol_UnloadedLinkedObject> >("unloaded_xdr_out");
	declareOutput<boost::shared_ptr<CBTF_Protocol_LinkedObjectGroup> >("group_xdr_out");
	declareOutput<LinkedObjectEntryVec>("linkedobjectvec_out");
	declareOutput<AddressSpace>("linkedobject_threadmap_out");

	init_TopologyInfo();
    }

    /** Handlers for the inputs.*/

    void numBEHandler(const int& in)
    {
        init_TopologyInfo();
#ifndef NDEBUG
        std::stringstream output;
        DEBUGPREFIX(Impl::TheTopologyInfo.IsFrontend,Impl::TheTopologyInfo.MaxLeafDistance);
#endif

#ifndef NDEBUG
        if (is_trace_linkedobject_events_enabled) {
            output << debug_prefix.str()
            << "ENTERED LinkedObjectComponent::numBEHandler number backends " << in
            << " numChildren:" << getNumChildren()
            << std::endl;
            flushOutput(output);
        }
#endif

#ifndef NDEBUG
        //flushOutput(output);
#endif

    }


    // Is this running just at the leafCP?
    // What does it do at the FE and intermediate CP levels?
    void numTerminatedHandler(const long& in)
    {
	init_TopologyInfo();

	numTerminated += in;

#ifndef NDEBUG
	std::stringstream output;
	DEBUGPREFIX(Impl::TheTopologyInfo.IsFrontend,getMaxLeafDistance());
#endif

#ifndef NDEBUG
	if (is_trace_linkedobject_events_enabled) {
            output << debug_prefix.str()
                << "ENTERED LinkedObjectComponent::numTerminatedHandler"
		<< " numTerminated:" << numTerminated
		<< " numThreads:" << numThreads
		<< " addressspace size:" << addressspace.size()
		<< " numChildren:" << getNumChildren()
		<< std::endl;
	    flushOutput(output);
	}
#endif

        if (numTerminated == numThreads && addressspace.size() == numThreads) {


	    AddressCounts ac = abuffer.addresscounts;
	    // ICP and FE levels do not have counts. Possibly due to no buffer yet?
	    bool havecounts = (ac.size() > 0) ? true : false ;
	    AddressSpace found;

	    AddressSpace::iterator i;
	    for (AddressSpace::iterator i = addressspace.begin(); i != addressspace.end(); ++i) {

		LinkedObjectVec tmp;
		for (LinkedObjectVec::iterator k = (*i).second.begin();
			     k != (*i).second.end(); ++k) {

		    bool has_sample = false;
		    AddressRange addr_range((*k).getAddressRange());
		    AddressCounts::const_iterator aci;
		    for (aci=ac.equal_range(addr_range.getBegin()).first;
			 aci!=ac.equal_range(addr_range.getEnd()).second;aci++) {
			has_sample = true;
			break;
		    }

		    if(has_sample || !havecounts) {
#ifndef NDEBUG
			if (is_trace_linkedobject_events_enabled) {
    			    output << debug_prefix.str()
				<< "\t HAS SAMPLE name:" << (*k).getPath()
				<< " range:" << (*k).getAddressRange()
				<< std::endl;
			}
#endif
			tmp.push_back(*k);
		    }
		}

#ifndef NDEBUG
		if (is_trace_linkedobject_events_enabled) {
		    output << debug_prefix.str()
		    << "LinkedObjectComponent::numTerminatedHandler CONVERT"
		    << " addressspace thread:" << (*i).first << std::endl;
		}
#endif

		if(tmp.size() > 0) {
		    found.insert( std::make_pair((*i).first,tmp) );
		}
	    }

	    for (AddressSpace::iterator i = found.begin(); i != found.end(); ++i) {

		boost::shared_ptr<CBTF_Protocol_LinkedObjectGroup> logroup(
		    new CBTF_Protocol_LinkedObjectGroup()
		    );

		CBTF_Protocol_ThreadName* ptr = &logroup->thread;
		convert((*i).first, *ptr);

		logroup->linkedobjects.linkedobjects_len = (*i).second.size();
		logroup->linkedobjects.linkedobjects_val =
		    reinterpret_cast<CBTF_Protocol_LinkedObject*>(
		    malloc((*i).second.size() * sizeof(CBTF_Protocol_LinkedObject))
		    );

		int j = 0;
		for (LinkedObjectVec::iterator k = (*i).second.begin();
			     k != (*i).second.end(); ++k) {
		    CBTF_Protocol_LinkedObject* destination =
			&logroup->linkedobjects.linkedobjects_val[j];
			convert((*k), *destination);
		    ++j;
		}

#ifndef NDEBUG
		if (is_trace_linkedobject_events_enabled) {
		    output << debug_prefix.str()
		    << "LinkedObjectComponent::numTerminatedHandler"
		    << " EMIT CBTF_Protocol_LinkedObjectGroup size:"
		    << logroup->linkedobjects.linkedobjects_len << std::endl;
		    flushOutput(output);
		}
#endif
		emitOutput<boost::shared_ptr<CBTF_Protocol_LinkedObjectGroup> >("group_xdr_out", logroup);
	    }

#ifndef NDEBUG
	    if (is_trace_linkedobject_events_enabled) {
	    	output << debug_prefix.str()
	        << "LinkedObjectComponent::numTerminatedHandler EMIT AddressSpace size:" << found.size() << std::endl;
		flushOutput(output);
	    }
#endif
	    emitOutput<AddressSpace>("linkedobject_threadmap_out",found);
	}
	
#ifndef NDEBUG
	//flushOutput(output);
#endif
    }

    // Handler for the "abuffer" input. The input buffer is used
    // to reduce the addressspace to only those linked objects that
    // contain a buffer address in the addressrange the object was
    // loaded into at runtime.
    void AddressBufferHandler(const AddressBuffer& in)
    {
	init_TopologyInfo();
#ifndef NDEBUG
	std::stringstream output;
	DEBUGPREFIX(Impl::TheTopologyInfo.IsFrontend,getMaxLeafDistance());
        if (is_trace_linkedobject_events_enabled) {
	    output << debug_prefix.str()
	    << "ENTERED LinkedObjectComponent::AddressBufferHandler"
	    << " with " << in.addresscounts.size() << " addresses"
	    << " numChildren:" << getNumChildren()
	    << std::endl;
	    flushOutput(output);
	}
#endif
	abuffer = in;

#ifndef NDEBUG
	//flushOutput(output);
#endif
    }

    // This message can only arrive on a local component network
    // as it currently has no mrnet converter to pass it on to other nodes.
    // Arrives on the "threadnames" input and exists here to inform
    // this component of existing threads.
    void threadnamesHandler(const ThreadNameVec& in)
    {
	init_TopologyInfo();
#ifndef NDEBUG
	std::stringstream output;
	DEBUGPREFIX(Impl::TheTopologyInfo.IsFrontend,getMaxLeafDistance());
	if (is_trace_linkedobject_events_enabled) {
	    output << debug_prefix.str()
	    << "ENTERED LinkedObjectComponent::threadnamesHandler with threads:"
	    << in.size()
	    << " numChildren:" << getNumChildren()
	    << std::endl;
	    flushOutput(output);
	}
#endif
	threadnames = in;
	numThreads = threadnames.size();
    }


    // This message contains a threadname and a list of linkedobjects.
    // At the LeafCP level it is reduced to only the linkedobjects for
    // which a matching address in the addressbuffer is found. The LeafCP
    // does not emit the reduced groups here.
    //
    // At the FE and Intermediate CP level we just place the passed group
    // into an AddressSpace object and once all expected groups have
    // arrived (addressspace.size == threads.size == numTerminated) this
    // handler will emit all the groups one at a time. Can this be done
    // in one larger CBTF_Protocol_AddressSpace message?
    void groupHandler(const boost::shared_ptr<CBTF_Protocol_LinkedObjectGroup>& in)
    {
	init_TopologyInfo();

        CBTF_Protocol_LinkedObjectGroup *message = in.get();
	ThreadName tname(message->thread);
	handled_threads++;

#ifndef NDEBUG
	std::stringstream output;
	DEBUGPREFIX(Impl::TheTopologyInfo.IsFrontend,getMaxLeafDistance());
	if (is_trace_linkedobject_events_enabled) {
	    output << debug_prefix.str()
	    << "ENTERED LinkedObjectComponent::groupHandler linked objects num:"
	    << message->linkedobjects.linkedobjects_len
	    << " handled_threads:" << handled_threads
	    << " of:" << threadnames.size()
	    << " numChildren:" << getNumChildren()
	    << std::endl;
	    output << debug_prefix.str()
	    << "LinkedObjectComponent::groupHandler message thread:" << tname
	    << std::endl;
	    flushOutput(output);
	}
#endif


        LinkedObjectVec linkedobjectvec;
	
	for(int i = 0; i < message->linkedobjects.linkedobjects_len; ++i) {
	        const CBTF_Protocol_LinkedObject& msg_lo =
				message->linkedobjects.linkedobjects_val[i];
		LinkedObject e;
		e.path = msg_lo.linked_object.path;
		e.is_executable = msg_lo.is_executable;
		e.time = TimeInterval(msg_lo.time_begin,msg_lo.time_end);
		e.range = AddressRange(msg_lo.range.begin,msg_lo.range.end);
	        linkedobjectvec.push_back(e);
	}
	addressspace.insert(std::make_pair(tname,linkedobjectvec));

#ifndef NDEBUG
	if (is_trace_linkedobject_events_enabled) {
	    output << debug_prefix.str()
	        << "LinkedObjectComponent::groupHandler"
	        << " addressspace size:" << addressspace.size()
	        << " threads:" << threadnames.size()
	        << " numTerminated:" << numTerminated
		<< std::endl;
	    flushOutput(output);
	}
#endif

	if ( !isLeafCP() &&
	     (addressspace.size() == threadnames.size()) &&
	     (numTerminated == threadnames.size()) ) {
	    for (AddressSpace::iterator i = addressspace.begin(); i != addressspace.end(); ++i) {

		boost::shared_ptr<CBTF_Protocol_LinkedObjectGroup> logroup(
		    new CBTF_Protocol_LinkedObjectGroup()
		    );

		CBTF_Protocol_ThreadName* ptr = &logroup->thread;
		convert((*i).first, *ptr);

		logroup->linkedobjects.linkedobjects_len = (*i).second.size();
		logroup->linkedobjects.linkedobjects_val =
		    reinterpret_cast<CBTF_Protocol_LinkedObject*>(
		    malloc((*i).second.size() * sizeof(CBTF_Protocol_LinkedObject))
		    );

		int j = 0;
		for (LinkedObjectVec::iterator k = (*i).second.begin();
			     k != (*i).second.end(); ++k) {
		    CBTF_Protocol_LinkedObject* destination =
			&logroup->linkedobjects.linkedobjects_val[j];
			convert((*k), *destination);
		    ++j;
		}

#ifndef NDEBUG
		if (is_trace_linkedobject_events_enabled) {
		    output << debug_prefix.str()
		    << "LinkedObjectComponent::groupHandler"
		    << " EMIT CBTF_Protocol_LinkedObjectGroup size:"
		    << logroup->linkedobjects.linkedobjects_len << std::endl;
		    flushOutput(output);
		}
#endif
		emitOutput<boost::shared_ptr<CBTF_Protocol_LinkedObjectGroup> >("group_xdr_out", logroup);
	    }
	}

#ifndef NDEBUG
        //flushOutput(output);
#endif
    }

    // Handler for dlopen events.
    void loadedHandler(const boost::shared_ptr<CBTF_Protocol_LoadedLinkedObject>& in)
    {
	init_TopologyInfo();
        CBTF_Protocol_LoadedLinkedObject *message = in.get();
#ifndef NDEBUG
	std::stringstream output;
	DEBUGPREFIX(Impl::TheTopologyInfo.IsFrontend,getMaxLeafDistance());
	if (is_trace_linkedobject_events_enabled) {
	    output << debug_prefix.str()
	     << "ENTERED LinkedObjectComponent::loadedHandler" << std::endl;
            flushOutput(output);
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
	    flushOutput(output);
	}
#endif
	emitOutput<boost::shared_ptr<CBTF_Protocol_LoadedLinkedObject> >("loaded_xdr_out", in);

#ifndef NDEBUG
        //flushOutput(output);
#endif
    }

    // Handler for dlclose events.
    void unloadedHandler(const boost::shared_ptr<CBTF_Protocol_UnloadedLinkedObject>& in)
    {
	init_TopologyInfo();
#ifndef NDEBUG
	std::stringstream output;
	DEBUGPREFIX(Impl::TheTopologyInfo.IsFrontend,getMaxLeafDistance());
	if (is_trace_linkedobject_events_enabled) {
	    output << debug_prefix.str()
	     << "ENTERED LinkedObjectComponent::unloadedHandler" << std::endl;
	    flushOutput(output);
	}
#endif

#ifndef NDEBUG
	if (is_debug_linkedobject_events_enabled) {
	    output << debug_prefix.str()
	     << "LinkedObjectComponent::unloadedHandler EMIT CBTF_Protocol_UnloadedLinkedObject" << std::endl;
	    flushOutput(output);
	}
#endif
	emitOutput<boost::shared_ptr<CBTF_Protocol_UnloadedLinkedObject> >("unloaded_xdr_out", in);

#ifndef NDEBUG
        //flushOutput(output);
#endif
    }

    // address buffer used to reduce incoming linkedobject groups
    // from the ltwt BEs.
    AddressBuffer abuffer;

    // vector of linkedobjectentry with thread info.
    LinkedObjectEntryVec linkedobjectentryvec;
    // vector of linkedobject info.
    LinkedObjectVec linkedobjectvec;
    // map of threadname to linkedobjectvec.
    AddressSpace addressspace;

    // vector of incoming threadnames.
    ThreadNameVec threadnames;

}; // class LinkedObjectComponent

KRELL_INSTITUTE_CBTF_REGISTER_FACTORY_FUNCTION(LinkedObjectComponent)
