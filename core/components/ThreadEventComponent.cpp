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

#include "KrellInstitute/Core/Path.hpp"
#include "KrellInstitute/Core/ThreadName.hpp"
#include "KrellInstitute/Core/ThreadState.hpp"
#include "KrellInstitute/Messages/Thread.h"
#include "KrellInstitute/Messages/ThreadEvents.h"


using namespace KrellInstitute::CBTF;
using namespace KrellInstitute::Core;


namespace {

    ThreadNameVec tvec;

    typedef std::vector< std::pair<ThreadName,ThreadState> > ThreadNameStateVec;
    ThreadNameStateVec tstatevec;

#ifndef NDEBUG
/** Flag indicating if debuging for LinkedObjects is enabled. */
bool is_debug_thread_events_enabled =
    (getenv("CBTF_DEBUG_THREAD_EVENTS") != NULL);
#endif

}

/**
 *
 * Component that handles thread state,
 */
class __attribute__ ((visibility ("hidden"))) ThreadsStateChanged :
    public Component
{

public:

    /** Factory function for this component type. */
    static Component::Instance factoryFunction()
    {
        return Component::Instance(
            reinterpret_cast<Component*>(new ThreadsStateChanged())
            );
    }

private:

    /** Default constructor. */
    ThreadsStateChanged() :
        Component(Type(typeid(ThreadsStateChanged)), Version(0, 0, 1))
    {
        declareInput<boost::shared_ptr<CBTF_Protocol_ThreadsStateChanged> >(
            "in", boost::bind(&ThreadsStateChanged::inHandler, this, _1)
            );
	declareOutput<ThreadState>("out");
	declareOutput<bool>("out1");
    }

    /** Handlers for the inputs.*/
    void inHandler(const boost::shared_ptr<CBTF_Protocol_ThreadsStateChanged>& in)
    {
        CBTF_Protocol_ThreadsStateChanged *message = in.get();
	for(int i = 0; i < message->threads.names.names_len; ++i) {
	    const CBTF_Protocol_ThreadName& msg_thread =
				message->threads.names.names_val[i];

	    ThreadName tname(msg_thread);

#ifndef NDEBUG
            if (is_debug_thread_events_enabled) {
	    std::cerr << "ThreadStateChanged " << tname.getHost()
	    << ":" << tname.getPid().second
	    << ":" <<  (uint64_t) tname.getPosixThreadId().second
	    << ":" <<  tname.getMPIRank().second
	    << " ThreadState: " << message->state
	    << std::endl;
	    }
#endif

	    tstatevec.push_back(std::make_pair(tname, (ThreadState) message->state));
	    emitOutput<ThreadState>("out", (ThreadState) message->state);

	    if (tvec.size() == tstatevec.size()) {
		std::cerr << "\nAll Threads are finished.\n" << std::endl;
	        emitOutput<bool>("out1", true);
	    }

	}

    }

}; // class ThreadsStateChanged

KRELL_INSTITUTE_CBTF_REGISTER_FACTORY_FUNCTION(ThreadsStateChanged)

/**
 * Component that handles thread state,
 */
class __attribute__ ((visibility ("hidden"))) AttachedToThreads :
    public Component
{

public:

    /** Factory function for this component type. */
    static Component::Instance factoryFunction()
    {
        return Component::Instance(
            reinterpret_cast<Component*>(new AttachedToThreads())
            );
    }

private:

    /** Default constructor. */
    AttachedToThreads() :
        Component(Type(typeid(AttachedToThreads)), Version(0, 0, 1))
    {
        declareInput<boost::shared_ptr<CBTF_Protocol_AttachedToThreads> >(
            "in", boost::bind(&AttachedToThreads::inHandler, this, _1)
            );
	declareOutput<ThreadNameVec>("out");
    }

    /** Handlers for the inputs.*/
    void inHandler(const boost::shared_ptr<CBTF_Protocol_AttachedToThreads>& in)
    {
        CBTF_Protocol_AttachedToThreads *message = in.get();
	for(int i = 0; i < message->threads.names.names_len; ++i) {
	    const CBTF_Protocol_ThreadName& msg_thread =
				message->threads.names.names_val[i];

	    ThreadName tname(msg_thread);

#ifndef NDEBUG
            if (is_debug_thread_events_enabled) {
	    std::cerr << "AttachedToThread " << tname.getHost()
	    << ":" << tname.getPid().second
	    << ":" <<  (uint64_t) tname.getPosixThreadId().second
	    << ":" <<  tname.getMPIRank().second
	    << std::endl;
	    }
#endif

	    tvec.push_back(tname);
	}

	emitOutput<ThreadNameVec>("out", tvec);

    }

}; // class AttachedToThreads

KRELL_INSTITUTE_CBTF_REGISTER_FACTORY_FUNCTION(AttachedToThreads)

/**
 * Component that handles thread state,
 */
class __attribute__ ((visibility ("hidden"))) CreatedProcess :
    public Component
{

public:

    /** Factory function for this component type. */
    static Component::Instance factoryFunction()
    {
        return Component::Instance(
            reinterpret_cast<Component*>(new CreatedProcess())
            );
    }

private:

    /** Default constructor. */
    CreatedProcess() :
        Component(Type(typeid(CreatedProcess)), Version(0, 0, 1))
    {
        declareInput<boost::shared_ptr<CBTF_Protocol_CreatedProcess> >(
            "in", boost::bind(&CreatedProcess::inHandler, this, _1)
            );

	declareOutput<ThreadName>("out");
    }

    /** Handlers for the inputs.*/
    void inHandler(const boost::shared_ptr<CBTF_Protocol_CreatedProcess>& in)
    {
        CBTF_Protocol_CreatedProcess *message = in.get();

	ThreadName created_threadname(message->created_thread);

#ifndef NDEBUG
        if (is_debug_thread_events_enabled) {
	std::cerr << "CreatedProcess " << created_threadname.getHost()
	<< ":" << created_threadname.getPid().second
	<< ":" <<  (uint64_t) created_threadname.getPosixThreadId().second
	<< ":" <<  created_threadname.getMPIRank().second
	<< std::endl;
	}
#endif

	emitOutput<ThreadName>("out", created_threadname);
    }

}; // class CreatedProcess

KRELL_INSTITUTE_CBTF_REGISTER_FACTORY_FUNCTION(CreatedProcess)
