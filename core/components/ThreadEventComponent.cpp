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
#include "KrellInstitute/Core/Time.hpp"

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

/** Flag indicating all threads are terminated. */
    bool is_finished = false;
/** Count of threads that are terminated. */
    int threads_finished = 0;
/** Count of threads finished messages. */
    int threads_finished_msgs = 0;
/** Count of threads that have connected to mrnet. */
    int threads_attached = 0;

    int numBE = 0;
    long numTerminated = 0;

#ifndef NDEBUG
/** Flag indicating if debuging for LinkedObjects is enabled. */
bool is_debug_thread_events_enabled =
    (getenv("CBTF_DEBUG_THREAD_EVENTS") != NULL);
bool is_trace_thread_events_enabled =
    (getenv("CBTF_TRACE_THREAD_EVENTS") != NULL);
bool is_time_thread_events_enabled =
    (getenv("CBTF_TIME_THREAD_EVENTS") != NULL);

    std::ostringstream debug_prefix;
    void flushOutput(std::stringstream &output) {
        if ( !output.str().empty() ) {
            std::cerr << output.str();
            output.str(std::string());
            output.clear();
        }
    }
#endif

    // Developer env var to defer emmiting final attach threads message
    // to the client.  For testing only...
    bool is_defer_emit = (getenv("CBTF_DEFER_THREAD_EMIT") != NULL);

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
        declareInput<int>(
            "numBE", boost::bind(&ThreadEventComponent::numBEHandler, this, _1)
            );
        declareInput<long>(
            "numTerminatedIn", boost::bind(&ThreadEventComponent::numTerminatedHandler, this, _1)
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

	declareOutput<boost::shared_ptr<CBTF_Protocol_AttachedToThreads> >("AttachedToThreads_xdr_out");
	declareOutput<ThreadNameVec>("ThreadNameVecOut");
	declareOutput<bool>("Threads_finished");
	declareOutput<long>("numTerminatedOut");
	declareOutput<int>("numBE");

	init_TopologyInfo();
    }

    // Intended for NonLeafCP or FE nodes. This records the
    // number of threads that have terminated. It is useful as a condition
    // of when to emit messages.
    void numTerminatedHandler(const long& in)
    {
	init_TopologyInfo();
#ifndef NDEBUG
	std::stringstream output;
	DEBUGPREFIX(Impl::TheTopologyInfo.IsFrontend,Impl::TheTopologyInfo.MaxLeafDistance);
	if (is_time_thread_events_enabled) {
	    if (numTerminated == 0) {
		output << Time::Now() << " " << debug_prefix.str()
		<< "ThreadEventComponent::numTerminatedHandler." << std::endl;
	        flushOutput(output);
	    }
	}
#endif

	numTerminated += in;

#ifndef NDEBUG
	if (is_trace_thread_events_enabled) {
	    output << debug_prefix.str()
	    << "ENTERED ThreadEventComponent::numTerminatedHandler input terminated:" << in
	    << " numTerminated:" << numTerminated
	    << " known threads:" << threadnamevec.size()
	    << " numChildren:" << getNumChildren()
	    << std::endl;
	    flushOutput(output);
	}
#endif

#ifndef NDEBUG
	//flushOutput(output);
#endif

    }


    // Notification of the number of ltwt BE's to expect.
    void numBEHandler(const int& in)
    {
	init_TopologyInfo();
#ifndef NDEBUG
	std::stringstream output;
	DEBUGPREFIX(Impl::TheTopologyInfo.IsFrontend,Impl::TheTopologyInfo.MaxLeafDistance);
	if (is_time_thread_events_enabled) {
	    if (numBE == 0) {
		output << Time::Now() << " " << debug_prefix.str()
		<< "ThreadEventComponent::numBEHandler." << std::endl;
	    }
	    flushOutput(output);
	}
#endif

#ifndef NDEBUG
	if (is_trace_thread_events_enabled) {
	    output << debug_prefix.str()
	    << "ENTERED ThreadEventComponent::numBEHandler number backends " << in
	    << " numChildren:" << getNumChildren()
	    << std::endl;
	    flushOutput(output);
	}
#endif
	numBE = in;
	emitOutput<int>("numBE",numBE);

#ifndef NDEBUG
	//flushOutput(output);
#endif

    }


    // This handler runs at the leaf CP level. It handles the thread state
    // messages from the ltwt BE processes that contain the threads
    // terminated state.  At the leaf CP level, once all threads that have
    // sent an attached message are terminated, this handler emits the
    // final threads attached message, the number of terminated threads,
    // a vector of threadnames to local components on the leaf CP and
    // a threads_finished message.
    void threadstateHandler(const boost::shared_ptr<CBTF_Protocol_ThreadsStateChanged>& in)
    {
	init_TopologyInfo();

#ifndef NDEBUG
	std::stringstream output;
	DEBUGPREFIX(Impl::TheTopologyInfo.IsFrontend,Impl::TheTopologyInfo.MaxLeafDistance);
	if (is_time_thread_events_enabled) {
	    if (threads_finished == 0) {
		output << Time::Now() << " " << debug_prefix.str()
		<< "ThreadEventComponent::threadstateHandler." << std::endl;
	        flushOutput(output);
	    }
	}
#endif

	threads_finished++;

#ifndef NDEBUG
	if (is_trace_thread_events_enabled) {
	    output << debug_prefix.str()
	    << "ENTERED ThreadEventComponent::threadstateHandler with state threads "
	    << in.get()->threads.names.names_len 
	    << " threads_attached:" << threads_attached
	    << " threads_finished:" << threads_finished
	    << " numChildren:" << getNumChildren()
	    << std::endl;
	    flushOutput(output);
	}

	if (is_debug_thread_events_enabled) {
	    output << debug_prefix.str() << "ThreadEventComponent::threadstateHandler"
	    << " numChildNodes:" << getNumChildren()
	    << " MaxLeafDistance:" << Impl::TheTopologyInfo.MaxLeafDistance
	    << std::endl;
	    flushOutput(output);
	}
#endif

	// Only the leafCP should handle threadstate messages.
	if ( isLeafCP()) {

#ifndef NDEBUG
	    if (is_debug_thread_events_enabled) {
	      CBTF_Protocol_ThreadsStateChanged *message = in.get();
	      CBTF_Protocol_ThreadState state = message->state;
	      for(int i = 0; i < message->threads.names.names_len; ++i) {
		const CBTF_Protocol_ThreadName& msg_thread =
					message->threads.names.names_val[i];
		ThreadName tname(msg_thread);
		output << debug_prefix.str() << "ThreadEventComponent::threadstateHandler"
		<< " tname:" << tname << " state: " << message->state
		<< std::endl;
	      }
	      flushOutput(output);
	    }
#endif
	
	    bool send_attached = false;
            bool send_finished = false;
            bool send_threadvec = false;

	    // At the leafCP level the condition that all threads have finished is
	    // when the number of attached threads is equal to the number
	    // of finished threads (threads.state terminated). Additional checks
	    // on ensuring that the vectors recording the local lists of threads
	    // and threadstates are non zero and also match.
	    // NOTE: seems there can be a race when we get a threadstate before
	    // all threadnames are known at the leafCP level.
	    if ( threads_finished == threads_attached ) {

#ifndef NDEBUG
		if (is_debug_thread_events_enabled) {
		    output << debug_prefix.str() << "ThreadEventComponent::threadstateHandler"
		    << " threads_finished:" << threads_finished
		    << " threads_attached:" << threads_attached
		    << std::endl;
		}
#endif

		send_threadvec = true;

		// Flag to indicate it is safe to emit the attached threads
		// upstream to other nodes.
		send_attached = true;

		// disabling this message would save traffic.  At this point
		// we are in a leaf CP and have a list of attached threads.
		// when we receive  a terminated state from each of the attached
		// threads, we are finished anyways and just need a thread list.
		// no sense sending the same thread list with just the additional
		// state message. (or we should have just added that state to
		// the main threadlist message and changed it to terminated here).
		send_finished = true;

	    } else {
	    }


	    // Send the final local threadname vector to local component here.
	    if (send_threadvec) {
#ifndef NDEBUG
		if (is_trace_thread_events_enabled) {
		    output << debug_prefix.str() << "ThreadEventComponent::threadstateHandler"
		    << " EMITS ThreadNameVec size:" << threadnamevec.size() << std::endl;
		    flushOutput(output);
		}
#endif
		// emit this on the local component network (not across nodes).
		emitOutput<ThreadNameVec>("ThreadNameVecOut", threadnamevec);
	    }

	    // Send the final CBTF_Protocol_AttachedToThreads message from the leafCP here.
	    if (send_attached) {
		CBTF_Protocol_ThreadNameGroup tng;
		convert(threadnamevec,tng);
		CBTF_Protocol_AttachedToThreads message;
		message.threads = tng;
		boost::shared_ptr<CBTF_Protocol_AttachedToThreads> attachedthreads_out =
				boost::make_shared<CBTF_Protocol_AttachedToThreads>(message);
#ifndef NDEBUG
		if (is_trace_thread_events_enabled) {
		    output << debug_prefix.str() << "ThreadEventComponent::threadstateHandler"
			<< " EMITS CBTF_Protocol_AttachedToThreads" << std::endl;
		    flushOutput(output);
		}
#endif
		emitOutput<boost::shared_ptr<CBTF_Protocol_AttachedToThreads> >("AttachedToThreads_xdr_out", attachedthreads_out);
	    }

	    // Notification to the local component network of the number
	    // of terminated threads from the leafCP.
	    if ( threads_finished == threads_attached ) {
#ifndef NDEBUG
		if (is_trace_thread_events_enabled) {
		    output << debug_prefix.str() << "ThreadEventComponent::threadstateHandler"
			<< " EMITS numTerminatedOut:" << threads_finished
			<< std::endl;
		    flushOutput(output);
		}
#endif
		emitOutput<long>("numTerminatedOut",threads_finished);

#ifndef NDEBUG
		if (is_trace_thread_events_enabled) {
		    output << debug_prefix.str() << "ThreadEventComponent::threadstateHandler"
			<< " EMITS finished" << std::endl;
		    flushOutput(output);
		}
#endif
		emitOutput<bool>("Threads_finished", true);
		is_finished = true;
	    }

	} else {
	    // Should not get here....
#ifndef NDEBUG
	    if (is_trace_thread_events_enabled) {
		output << debug_prefix.str() << "ThreadEventComponent::threadstateHandler"
		<< " NON LEAFCP OR FE HANDLED THREADSTATE..." << std::endl;
	    }
#endif
	}

#ifndef NDEBUG
	//flushOutput(output);
#endif

    }

    // Counts number of threads finished messages received.
    // At the intermediate CP and FE nodes, this should
    // match the number of children nodes to this node.
    // This is a no-op on the leafCP nodes.
    void finishedHandler(const bool& in)
    {
	init_TopologyInfo();

	if (isLeafCP()) {
	    return;
	}

#ifndef NDEBUG
	std::stringstream output;
	DEBUGPREFIX(Impl::TheTopologyInfo.IsFrontend,Impl::TheTopologyInfo.MaxLeafDistance);
	if (is_time_thread_events_enabled) {
	    if (threads_finished_msgs == 0) {
		output << Time::Now() << " " << debug_prefix.str()
		<< " ThreadEventComponent::finishedHandler." << std::endl;
	        flushOutput(output);
	    }
	}
	
#endif
        threads_finished_msgs++;

#ifndef NDEBUG
	if (is_trace_thread_events_enabled) {
	    output << debug_prefix.str()
	    << "ENTERED ThreadEventComponent::finishedHandler"
	    << " num finished_msgs:" << threads_finished_msgs
	    << " threads_attached:" << threads_attached
	    << " numChildren:" << getNumChildren()
	    << std::endl;
	    flushOutput(output);
	}
#endif

	if ((isFrontend() || isNonLeafCP()) && (threads_finished_msgs == getNumChildren()) ) {
#ifndef NDEBUG
		if (is_trace_thread_events_enabled) {
		    output << debug_prefix.str() << "ThreadEventComponent::finishedHandler"
			<< " EMITS finished" << std::endl;
		    flushOutput(output);
		}
#endif
		// This is client output for the benefit of notifying the user
		// that the tool has finished receiving data from the collectors.
		if(isFrontend()) {
		    std::cout << "All Threads are finished." << std::endl;
		}

		emitOutput<bool>("Threads_finished", true);
		is_finished = true;
	}
    }


    // This handles the incoming list of attached threads.
    // At the Leaf CP level we update a vector Threadname for each incoming
    // thread. The LeafCP does not emit any messages.
    //
    // At the FE and intermediate CP levels we update a vector of Threadname
    // and when we have received one message from each child of this node
    // this handle will emit a final list of attached threads upstream as well
    // as the vector of Threadname to the other components on this local node..
    void threadsHandler(const boost::shared_ptr<CBTF_Protocol_AttachedToThreads>& in)
    {
	init_TopologyInfo();

#ifndef NDEBUG
	std::stringstream output;
	DEBUGPREFIX(Impl::TheTopologyInfo.IsFrontend,Impl::TheTopologyInfo.MaxLeafDistance);
	if (is_time_thread_events_enabled) {
	    if (threads_attached == 0) {
		output << Time::Now() << " " << debug_prefix.str()
		<< " ThreadEventComponent::threadsHandler." << std::endl;
	        flushOutput(output);
	    }
	}
#endif

	bool send_attached = false;
	bool send_threadvec = false;
	bool send_numTerminated = false;

	// count the number of attached messages seen
	threads_attached++;

#ifndef NDEBUG
	if (is_trace_thread_events_enabled) {
	    output << debug_prefix.str()
	    << "ENTERED ThreadEventComponent::threadsHandler"
	    << " message threads:" << in.get()->threads.names.names_len
	    << " threads_attached:" << threads_attached
	    << " numChildren:" << getNumChildren()
	    << std::endl;
	    flushOutput(output);
	}
#endif

	// This determines if this handler can forward the merged thread attached list
	// here rather than from the state handler.
	if ( isLeafCP()) {
	    // This is a leaf CP.
	    // Do not emit anything here.
	} else if ( isNonLeafCP() && getNumChildren() == threads_attached) {
	    // This is not a leaf CP.
	    send_threadvec = true;
	    send_attached = true;
	    send_numTerminated = true;
	} else if (isFrontend() && getNumChildren() == threads_attached) {
	    // This is the FE.
	    send_threadvec = true;
	    send_attached = true;
	    send_numTerminated = true;
	}

	// update the threadnamevec
        CBTF_Protocol_AttachedToThreads *message = in.get();
	for(int i = 0; i < message->threads.names.names_len; ++i) {
	    const CBTF_Protocol_ThreadName& msg_thread =
				message->threads.names.names_val[i];

	    ThreadName tname(msg_thread);

#ifndef NDEBUG
            if (is_debug_thread_events_enabled) {
	        output << debug_prefix.str() << "ThreadEventComponent::threadsHandler"
		<< " tname:" << tname << " threads_attached:" << threads_attached
		<< std::endl;
	    }
#endif

	    threadnamevec.push_back(tname);
	}
#ifndef NDEBUG
	if (is_debug_thread_events_enabled) {
	    flushOutput(output);
	}
#endif

	if (send_threadvec) {
#ifndef NDEBUG
	    if (is_trace_thread_events_enabled) {
	        output << debug_prefix.str()
		<< "ThreadEventComponent::threadsHandler EMITS ThreadNameVec of size "
		<< threadnamevec.size() << std::endl;
	        flushOutput(output);
	    }
#endif
	    emitOutput<ThreadNameVec>("ThreadNameVecOut", threadnamevec);
	}

	// This sends the updated,merged thread list up the tree.
	if (send_attached) {
	    CBTF_Protocol_ThreadNameGroup tng;
	    convert(threadnamevec,tng);
	    CBTF_Protocol_AttachedToThreads message;
	    message.threads = tng;
	    boost::shared_ptr<CBTF_Protocol_AttachedToThreads> attachedthreads_out =
		boost::make_shared<CBTF_Protocol_AttachedToThreads>(message);
#ifndef NDEBUG
	    if (is_trace_thread_events_enabled) {
	        output << debug_prefix.str() <<
		"ThreadEventComponent::threadsHandler EMITS CBTF_Protocol_AttachedToThreads"
		<< std::endl;
	        flushOutput(output);
	    }
#endif
	    emitOutput<boost::shared_ptr<CBTF_Protocol_AttachedToThreads> >("AttachedToThreads_xdr_out", attachedthreads_out);
	}

	// At NON leaf CP and FE levels we assume that all threads in
	// threadnamevec are also terminated.  So one we are informed
	// of the number of terminated by all children, we send the
	// number of attached threads as the number of terminated.
	if (send_numTerminated) {
#ifndef NDEBUG
	    if (is_trace_thread_events_enabled) {
		    output << debug_prefix.str() << "ThreadEventComponent::threadsHandler"
			<< " EMITS numTerminatedOut:" << threadnamevec.size()
		        << " threads:" << threadnamevec.size()
			<< std::endl;
	        flushOutput(output);
	    }
#endif
	    emitOutput<long>("numTerminatedOut",threadnamevec.size());

	    if (isFrontend() || isNonLeafCP()) {
#ifndef NDEBUG
		if (is_trace_thread_events_enabled) {
		    output << debug_prefix.str() << "ThreadEventComponent::threadsHandler"
			<< " EMITS finished" << std::endl;
		    flushOutput(output);
		}
#endif
		// This is client output for the benefit of notifying the user
		// that the tool has finished receiving data from the collectors.
		//if(isFrontend()) {
		 //   std::cout << "All Threads are finished." << std::endl;
		//}

		emitOutput<bool>("Threads_finished", true);
		is_finished = true;
	    }

	}

#ifndef NDEBUG
	//flushOutput(output);
#endif

    }

    ThreadNameVec threadnamevec;
    
}; // class ThreadEventComponent

KRELL_INSTITUTE_CBTF_REGISTER_FACTORY_FUNCTION(ThreadEventComponent)
