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

/** @file ThreadEventComponent */

#include <boost/bind.hpp>
#include <boost/operators.hpp>
#include <boost/make_shared.hpp>
#include <mrnet/MRNet.h>
#include <typeinfo>
#include <algorithm>
#include <sstream>

#include <KrellInstitute/CBTF/Component.hpp>
#include <KrellInstitute/CBTF/Type.hpp>
#include <KrellInstitute/CBTF/Version.hpp>
#include <KrellInstitute/CBTF/Impl/MRNet.hpp>

#include "KrellInstitute/Core/Path.hpp"
#include "KrellInstitute/Core/ThreadName.hpp"
#include "KrellInstitute/Core/ThreadState.hpp"
#include "KrellInstitute/Messages/Thread.h"
#include "KrellInstitute/Messages/ThreadEvents.h"


using namespace KrellInstitute::CBTF;
using namespace KrellInstitute::Core;


namespace {

/** Flag indicating all threads are terminated. */
    bool is_finished;
/** Count of threads that are terminated. */
    int num_finished = 0;
/** Count of threads that have connected to mrnet. */
    int num_attached = 0;
/** Count of leaf CP's connected directly to mrnet ltwt BE's. */
    int num_leafcp = 0;
/** Flag indicating all leaf CP's are connected. */
    bool handle_leafcp_msg = false;

#ifndef NDEBUG
/** Flag indicating if debuging for LinkedObjects is enabled. */
bool is_debug_thread_events_enabled =
    (getenv("CBTF_DEBUG_THREAD_EVENTS") != NULL);
bool is_trace_thread_events_enabled =
    (getenv("CBTF_TRACE_THREAD_EVENTS") != NULL);
#endif

    // Mrnet message conversion utility functions.
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

    /**
     * Convert threadname vector for mrnet use.
     *
     * @note    The caller assumes responsibility for releasing all allocated
     *          memory when it is no longer needed.
     *
     * @param in      ThreadNamevec to be converted.
     * @retval out    Structure to hold the results.
     */
    void convert(const ThreadNameVec& in, CBTF_Protocol_ThreadNameGroup& out)
    {
	// Allocate an appropriately sized array of thread entries
	out.names.names_len = in.size();
	out.names.names_val = 
	    reinterpret_cast<CBTF_Protocol_ThreadName*>(malloc(
	        std::max(static_cast<ThreadNameVec::size_type>(1), in.size()) *
		sizeof(CBTF_Protocol_ThreadName)
		));
	
	// Iterate over each thread of this group
	CBTF_Protocol_ThreadName* ptr = out.names.names_val;
	for(ThreadNameVec::const_iterator
		i = in.begin(); i != in.end(); ++i, ++ptr)
	    convert(*i, *ptr);
    }

    /**
     * Convert threadnamestate vector for mrnet use.
     *
     * @note    The caller assumes responsibility for releasing all allocated
     *          memory when it is no longer needed.
     *
     * @param in      ThreadNameStatevec to be converted.
     * @retval out    Structure to hold the results.
     */
    void convert(const ThreadNameStateVec& in, CBTF_Protocol_ThreadsStateChanged& out)
    {
	// Allocate an appropriately sized array of thread entries
	out.threads.names.names_len = in.size();
	out.threads.names.names_val = 
	    reinterpret_cast<CBTF_Protocol_ThreadName*>(malloc(
	        std::max(static_cast<ThreadNameStateVec::size_type>(1), in.size()) *
		sizeof(CBTF_Protocol_ThreadName)
		));
	
	// Iterate over each thread of this group
	CBTF_Protocol_ThreadName* ptr = out.threads.names.names_val;
	for(ThreadNameStateVec::const_iterator
		i = in.begin(); i != in.end(); ++i, ++ptr) {
	    convert((*i).first, *ptr);
 	    ThreadState ts = (*i).second;
	    out.state = static_cast<CBTF_Protocol_ThreadState>(ts);

	}
	
    }
}

/**
 * Component that handles thread event information.
 */
class __attribute__ ((visibility ("hidden"))) ThreadEventComponent :
    public Component
{

public:

    /** Factory function for this component type. */
    static Component::Instance factoryFunction()
    {
        return Component::Instance(
            reinterpret_cast<Component*>(new ThreadEventComponent())
            );
    }


private:

    /** Default constructor. */
    ThreadEventComponent() :
        Component(Type(typeid(ThreadEventComponent)), Version(0, 0, 1))
    {
	is_finished = false;
	numBE = 0;
	num_leafcp = 0;

        declareInput<int>(
            "leafCPnumBE", boost::bind(&ThreadEventComponent::leafCPHandler, this, _1)
            );
        declareInput<int>(
            "numBE", boost::bind(&ThreadEventComponent::numBEHandler, this, _1)
            );
        declareInput<boost::shared_ptr<CBTF_Protocol_AttachedToThreads> >(
            "threads", boost::bind(&ThreadEventComponent::threadsHandler, this, _1)
            );
        declareInput<boost::shared_ptr<CBTF_Protocol_ThreadsStateChanged> >(
            "threadstate", boost::bind(&ThreadEventComponent::threadstateHandler, this, _1)
            );
	declareInput<bool>(
            "finished", boost::bind(&ThreadEventComponent::finishedHandler, this, _1)
            );
	declareOutput<ThreadNameVec>("ThreadNameVecOut");
	declareOutput<boost::shared_ptr<CBTF_Protocol_AttachedToThreads> >("AttachedToThreads_xdr_out");
	declareOutput<ThreadState>("ThreadStateOut");
	declareOutput<boost::shared_ptr<CBTF_Protocol_ThreadsStateChanged> >("ThreadsStateChanged_xdr_out");
	declareOutput<bool>("Threads_finished");
	declareOutput<int>("LeafCP_numBE");

        declareInput<boost::shared_ptr<CBTF_Protocol_CreatedProcess> >(
            "createdprocess", boost::bind(&ThreadEventComponent::createdprocessHandler, this, _1)
            );

	declareOutput<ThreadName>("threadname");
	declareOutput<boost::shared_ptr<CBTF_Protocol_CreatedProcess> >("CreatedProcess_xdr_out");

    }

    void numBEHandler(const int& in)
    {
#ifndef NDEBUG
	if (is_trace_thread_events_enabled) {
	    std::cerr << getpid() << " "
	    << "ENTERED ThreadEventComponent::numBEHandler number backends " << in
	    << std::endl;
	}
#endif
	numBE = in;
    }

    void leafCPHandler(const int& in)
    {
	num_leafcp++;
#ifndef NDEBUG
	if (is_trace_thread_events_enabled) {
	    std::cerr << getpid() << " "
	    << "ENTERED ThreadEventComponent::leafCPHandler number leafCP's " << num_leafcp
	    << std::endl;
	}
#endif
	if (Impl::TheTopologyInfo.IsFrontend) {
	    emitOutput<int>("LeafCP_numBE",num_leafcp);
	}
    }

    void threadstateHandler(const boost::shared_ptr<CBTF_Protocol_ThreadsStateChanged>& in)
    {
	num_finished++;
#ifndef NDEBUG
	if (is_trace_thread_events_enabled) {
	    std::cerr << getpid() << " " << "ENTERED ThreadEventComponent::threadstateHandler with state threads "
		<< in.get()->threads.names.names_len << std::endl;
	}

	if (is_debug_thread_events_enabled) {
            std::cerr << getpid() << " " << "ThreadEventComponent::threadstateHandler:"
	    << " numChildNodes " << Impl::TheTopologyInfo.NumChildren
	    << " numSiblings " << Impl::TheTopologyInfo.NumSiblings
	    << " numDescendants " << Impl::TheTopologyInfo.NumDescendants
	    << " rootDistance " << Impl::TheTopologyInfo.RootDistance
	    << " numLeafDescendants " << Impl::TheTopologyInfo.NumLeafDescendants
	    << " MaxLeafDistance " << Impl::TheTopologyInfo.MaxLeafDistance
	    << " isFE " << Impl::TheTopologyInfo.IsFrontend
	    << " isBE " << Impl::TheTopologyInfo.IsBackend
	    << " num_finished " << num_finished
	    << std::endl;
	}
	std::stringstream output;
#endif

        CBTF_Protocol_ThreadsStateChanged *message = in.get();
	CBTF_Protocol_ThreadState state = message->state;
	for(int i = 0; i < message->threads.names.names_len; ++i) {
	    const CBTF_Protocol_ThreadName& msg_thread =
				message->threads.names.names_val[i];
	    ThreadName tname(msg_thread);
	    threadnamestatevec.push_back(std::make_pair(tname, (ThreadState) message->state));

#ifndef NDEBUG
            if (is_debug_thread_events_enabled) {
		output << getpid() << " " << "ThreadEventComponent::threadstateHandler "
		<< "tname:" << tname
		<< " state: " << message->state
		<< std::endl;
		output << getpid() << " " << "ThreadEventComponent::threadstateHandler known threads " << threadnamevec.size()
		    << " state threads " << threadnamestatevec.size() << std::endl;
	    }
#endif

	}

#ifndef NDEBUG
        if (is_debug_thread_events_enabled) {
	    std::cerr << output.str();
	}
#endif

        bool send_messages = false;

	if (!Impl::TheTopologyInfo.IsFrontend &&
	    threadnamevec.size() > 0 && threadnamestatevec.size() > 0
	    && threadnamevec.size() == threadnamestatevec.size()) {

	    send_messages = true;

	} else if ( Impl::TheTopologyInfo.IsFrontend &&
		    (num_attached == num_leafcp && num_finished == num_leafcp )
		    && threadnamevec.size() == threadnamestatevec.size()) {

	    std::cerr << "All Threads are finished.\n" << std::endl;
	    send_messages = true;

	}

	if (send_messages) {
	    CBTF_Protocol_ThreadNameGroup tng;
	    convert(threadnamevec,tng);
	    CBTF_Protocol_AttachedToThreads message;
	    message.threads = tng;
	    boost::shared_ptr<CBTF_Protocol_AttachedToThreads> attachedthreads_out =
		boost::make_shared<CBTF_Protocol_AttachedToThreads>(message);
	    //std::cerr << getpid() << " " << "ThreadEventComponent::threadstateHandler EMITS CBTF_Protocol_AttachedToThreads" << std::endl;
	    emitOutput<boost::shared_ptr<CBTF_Protocol_AttachedToThreads> >("AttachedToThreads_xdr_out", attachedthreads_out);

	    CBTF_Protocol_ThreadsStateChanged new_message;
	    convert(threadnamestatevec,new_message);
	    boost::shared_ptr<CBTF_Protocol_ThreadsStateChanged> threadsstate_out =
			boost::make_shared<CBTF_Protocol_ThreadsStateChanged>(new_message);
	    //std::cerr << getpid() << " " << "ThreadEventComponent::threadstateHandler EMITS CBTF_Protocol_ThreadsStateChanged" << std::endl;
	    emitOutput<boost::shared_ptr<CBTF_Protocol_ThreadsStateChanged> >("ThreadsStateChanged_xdr_out", threadsstate_out);

	    emitOutput<ThreadNameVec>("ThreadNameVecOut", threadnamevec);
	
	    //std::cerr << getpid() << " " << "ThreadEventComponent::threadstateHandler EMITS finished" << std::endl;
	    emitOutput<bool>("Threads_finished", true);
	    is_finished = true;
	}
    }

    // UNUSED...
    void finishedHandler(const bool& in)
    {
    }

    /** Handlers for the inputs.*/
    void threadsHandler(const boost::shared_ptr<CBTF_Protocol_AttachedToThreads>& in)
    {
	num_attached++;
#ifndef NDEBUG
	if (is_trace_thread_events_enabled) {
	    std::cerr << getpid() << " " << "ENTERED ThreadEventComponent::threadsHandler with num threads "
	    << in.get()->threads.names.names_len << " num_attached " << num_attached
	    << std::endl;
	}
	std::stringstream output;
#endif
	if (!Impl::TheTopologyInfo.IsFrontend && !handle_leafcp_msg && Impl::TheTopologyInfo.NumChildren > 0) {
	     emitOutput<int>("LeafCP_numBE",Impl::TheTopologyInfo.NumChildren);
	     handle_leafcp_msg = true;
	}

        CBTF_Protocol_AttachedToThreads *message = in.get();
	for(int i = 0; i < message->threads.names.names_len; ++i) {
	    const CBTF_Protocol_ThreadName& msg_thread =
				message->threads.names.names_val[i];

	    ThreadName tname(msg_thread);

#ifndef NDEBUG
            if (is_debug_thread_events_enabled) {
		output << getpid() << " " << "ThreadEventComponent::threadsHandler "
		<< "tname:" << tname << " num_attached:" << num_attached
		<< std::endl;
	    }
#endif

	    threadnamevec.push_back(tname);
	}

#ifndef NDEBUG
        if (is_debug_thread_events_enabled) {
	    std::cerr << output.str();
	}
#endif

	//std::cerr << "ThreadEventComponent::threadsHandler EMITS ThreadNameVec of size " << threadnamevec.size() << std::endl;
	//emitOutput<ThreadNameVec>("ThreadNameVecOut", threadnamevec);
    }

    void createdprocessHandler(const boost::shared_ptr<CBTF_Protocol_CreatedProcess>& in)
    {
        CBTF_Protocol_CreatedProcess *message = in.get();
	ThreadName created_threadname(message->created_thread);

#ifndef NDEBUG
        if (is_debug_thread_events_enabled) {
	    std::cerr << "ThreadEventComponent::createdprocessHandler "
		<< ":" << created_threadname << std::endl;
	}
#endif

	emitOutput<ThreadName>("ThreadName_out", created_threadname);
	emitOutput<boost::shared_ptr<CBTF_Protocol_CreatedProcess> >("CreatedProcess_xdr_out", in);
    }

    int numBE;
    ThreadNameVec threadnamevec;
    ThreadNameStateVec threadnamestatevec;
    
}; // class ThreadEventComponent

KRELL_INSTITUTE_CBTF_REGISTER_FACTORY_FUNCTION(ThreadEventComponent)
