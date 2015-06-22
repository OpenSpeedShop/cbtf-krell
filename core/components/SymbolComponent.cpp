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

/** @file Plugin for Symbols. */

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

#include "KrellInstitute/Core/Address.hpp"
#include "KrellInstitute/Core/AddressBuffer.hpp"
#include "KrellInstitute/Core/AddressRange.hpp"
#include "KrellInstitute/Core/AddressSpace.hpp"
#if BFD_AVAILABLE_DPM
#include "KrellInstitute/Core/BFDSymbols.hpp"
#endif
#include "KrellInstitute/Core/Blob.hpp"
#include "KrellInstitute/Core/LinkedObjectEntry.hpp"
#include "KrellInstitute/Core/LinkedObject.hpp"
#include "KrellInstitute/Core/Path.hpp"
#include "KrellInstitute/Core/SymbolTable.hpp"
#include "KrellInstitute/Core/SymtabAPISymbols.hpp"
#include "KrellInstitute/Core/Time.hpp"
#include "KrellInstitute/Core/ThreadName.hpp"

#include "KrellInstitute/Messages/Address.h"
#include "KrellInstitute/Messages/EventHeader.h"
#include "KrellInstitute/Messages/File.h"
#include <KrellInstitute/Messages/Symbol.h>
#include "KrellInstitute/Messages/Thread.h"
#include "KrellInstitute/Messages/Stats.h"


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


// TODO: Move these to include file.
// Simple struct to map a function to a thread with sample count.
//
struct FuncThreadStats {
    std::string funcname;
    ThreadName  tname;
    uint64_t value;
    FuncThreadStats(const std::string& f,
		    const ThreadName& t, const uint64_t& v)
	: funcname(f), tname(t), value(v)
    {
    };

    FuncThreadStats(const CBTF_Protocol_FunctionThreadValue& object)
    {
	funcname = strdup(object.function);
	tname =  object.thread;
	value =  object.value;
    };

    bool operator==(const FuncThreadStats& other) const
    {
	// For our purposes, these are only a match for function
	// name and thread.
	if (funcname == other.funcname &&
	    tname == other.tname) {
	    return true;
	}
	return false;
    };

    bool operator<(const FuncThreadStats& other) const
    {
	// For our purposes, these are only compare  for function
	// name and value.
	if (funcname < other.funcname) {
	    return true;
	}
	if (funcname > other.funcname) {
	    return false;
	}
	if (value < other.value) {
	    return true;
	}
	if (value > other.value) {
	    return false;
	}
	return false;
    };
};

// vector to hold the mappings of function to threads with counts.
typedef std::vector<FuncThreadStats> FuncStatsVec;

// mapping of addressbuffers to thread from AddressAggregatorComponent.
typedef std::map<ThreadName,AddressBuffer>  ThreadAddrBufMap;

// mapping of function to thread and value.
typedef std::map<std::string,std::pair<ThreadName,uint64_t> > FunctionThreadCount;

// mapping of function to total value and total number of threads.
typedef std::map<std::string,std::pair<uint64_t,uint64_t> > FunctionAvgMap;

namespace {

    std::ostringstream debug_prefix;
    void flushOutput(std::stringstream &output) {
	if ( !output.str().empty() ) {
	    std::cerr << output.str();
	    output.str(std::string());
	    output.clear();
	}
    }

#ifndef NDEBUG
/** Flag indicating if debuging for Symbol events is enabled. */
bool is_debug_symbol_events_enabled =
    (getenv("CBTF_DEBUG_SYMBOL_EVENTS") != NULL);
/** Flag indicating if tracing for Symbol events is enabled. */
bool is_trace_symbol_events_enabled =
    (getenv("CBTF_TRACE_SYMBOL_EVENTS") != NULL);
/** Flag indicating if timing for Symbol events is enabled. */
bool is_time_symbol_events_enabled =
    (getenv("CBTF_TIME_SYMBOL_EVENTS") != NULL);
#endif

/** Flag indicating if displaying of metric events is enabled. */
// This is not to be hidden by NDEBUG since it can be enabled
// to show the final min,max,avg values to std:cout.
bool is_show_metric_events_enabled =
    (getenv("CBTF_SHOW_METRIC_EVENTS") != NULL);


/** count indicating number of leaf CP's in mrnet tree. */
    int num_leafcp = 0;
/** count indicating number of max value messages to expect at FE. */
    int max_msgs = 0;
/** count indicating number of min value messages to expect at FE. */
    int min_msgs = 0;
/** count indicating number of avg value messages to expect at FE. */
    int avg_msgs = 0;
/** count indicating number of linkedobjectvec messages to expect at FE. */
    int lov_msgs = 0;
    int lovmap_msgs = 0;
    int thread_msgs = 0;
    int sym_msgs = 0;
/** flag indicating FE has count of leaf CP's in tree. */
    bool handle_leafcp_msg = false;
    long numTerminated = 0;
    long numThreads = 0;

// Track the topology info localy. This info is initiallized as
// soon as it is available once and only once.
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
 * Component that resolves addresses in an AddressBuffer
 * to symbols found in corresponding  linkedobjects.
 * There are metric's computed at the function symbol level here.
 * Currently max,min,avg (loadbalance at function level).
 * The max,min map a function name symbol to a thread and value.
 * The avg is a map of function name symbol to total value and thread count.
 */
class __attribute__ ((visibility ("hidden"))) ResolveSymbols :
    public Component
{

public:

    /** Factory function for this component type. */
    static Component::Instance factoryFunction()
    {
        return Component::Instance(
            reinterpret_cast<Component*>(new ResolveSymbols())
            );
    }

private:

    /** Default constructor. */
    ResolveSymbols() :
        Component(Type(typeid(ResolveSymbols)), Version(0, 0, 1))
    {
        num_leafcp = 0;

        declareInput<int>(
            "numBE", boost::bind(&ResolveSymbols::numBEHandler, this, _1)
            );

        declareInput<int>(
            "leafCPnumBE", boost::bind(&ResolveSymbols::leafCPHandler, this, _1)
            );

        declareInput<AddressBuffer>(
            "abufferin", boost::bind(&ResolveSymbols::AddressBufferHandler, this, _1)
            );
        declareInput<LinkedObjectEntryVec>(
            "linkedobjectvecin", boost::bind(&ResolveSymbols::LinkedObjectVecHandler, this, _1)
            );
        declareInput<AddressSpace>(
            "linkedobject_threadmap_in", boost::bind(&ResolveSymbols::AddressSpaceHandler, this, _1)
            );
        declareInput<SymtabAPISymbols>(
            "symtabapisymsin", boost::bind(&ResolveSymbols::symtabAPISymbolHandler, this, _1)
            );

        declareInput<ThreadAddrBufMap>(
            "threadaddrbufmap", boost::bind(&ResolveSymbols::ThreadAddrBufMapHandler, this, _1)
            );
#if BFD_AVAILABLE_DPM
        declareInput<BFDSymbols>(
            "bfdsymsin", boost::bind(&ResolveSymbols::BFDSymbolHandler, this, _1)
            );
        declareOutput<BFDSymbols>("bfdsymbolsout");
#endif
	declareInput<bool>(
            "finished", boost::bind(&ResolveSymbols::finishedHandler, this, _1)
            );
	declareInput<long>(
            "numTerminatedIn", boost::bind(&ResolveSymbols::numTerminatedHandler, this, _1)
            );
        declareInput<boost::shared_ptr<CBTF_Protocol_FunctionThreadValues> >(
            "maxfunctionvalues", boost::bind(&ResolveSymbols::MaxFunctionThreadValuesHandler, this, _1)
            );
        declareInput<boost::shared_ptr<CBTF_Protocol_FunctionThreadValues> >(
            "minfunctionvalues", boost::bind(&ResolveSymbols::MinFunctionThreadValuesHandler, this, _1)
            );
        declareInput<boost::shared_ptr<CBTF_Protocol_FunctionAvgValues> >(
            "avgfunctionvalues", boost::bind(&ResolveSymbols::AvgFunctionValuesHandler, this, _1)
            );
        declareInput<boost::shared_ptr<CBTF_Protocol_SymbolTable> >(
            "symboltable_xdr_in", boost::bind(&ResolveSymbols::CbtfProtocolSymbolTableHandler, this, _1)
            );
        declareInput<boost::shared_ptr<CBTF_Protocol_SymbolTable_Group> >(
            "symboltablegroup_xdr_in", boost::bind(&ResolveSymbols::CbtfProtocolSymbolTableGroupHandler, this, _1)
            );

        declareOutput<SymtabAPISymbols>("symtabapisymsout");
	declareOutput<boost::shared_ptr<CBTF_Protocol_SymbolTable> >(
	    "symboltable_xdr_out"
	    );
	declareOutput<boost::shared_ptr<CBTF_Protocol_SymbolTable_Group> >(
	    "symboltablegroup_xdr_out"
	    );
	declareOutput<boost::shared_ptr<CBTF_Protocol_FunctionThreadValues> >(
	    "maxfunctionvalues_xdr_out"
	    );
	declareOutput<boost::shared_ptr<CBTF_Protocol_FunctionThreadValues> >(
	    "minfunctionvalues_xdr_out"
	    );
	declareOutput<boost::shared_ptr<CBTF_Protocol_FunctionAvgValues> >(
	    "avgfunctionvalues_xdr_out"
	    );
    }

    // passed from client FE to indicate number of ltwt BE's to expect.
    // We use this here to initialize the topology info once so we
    // do not record this value...
    void numBEHandler(const int& in)
    {
        init_TopologyInfo();
#ifndef NDEBUG
        std::stringstream output;
        DEBUGPREFIX(Impl::TheTopologyInfo.IsFrontend,Impl::TheTopologyInfo.MaxLeafDistance);
#endif

#ifndef NDEBUG
        if (is_trace_symbol_events_enabled) {
            output << debug_prefix.str()
            << "ENTERED ResolveSymbols::numBEHandler number backends " << in
	    << " numChildren:" << getNumChildren()
            << std::endl;
            flushOutput(output);
        }
#endif

#ifndef NDEBUG
        flushOutput(output);
#endif

    }

    // notification from leafCP nodes. update the total leafCPs
    // at this node here. Likely unused here.
    // TODO: check if this can be removed.
    void leafCPHandler(const int& in)
    {
        init_TopologyInfo();
        num_leafcp++;

#ifndef NDEBUG
	std::stringstream output;
	DEBUGPREFIX(Impl::TheTopologyInfo.IsFrontend,Impl::TheTopologyInfo.MaxLeafDistance);
	if (is_time_symbol_events_enabled) {
	    if (num_leafcp == 0) {
		output << Time::Now() << " " << debug_prefix.str()
		<< "ResolveSymbols::leafCPHandler." << std::endl;
	        flushOutput(output);
	    }
	}
#endif

#ifndef NDEBUG
        if (is_trace_symbol_events_enabled) {
	    output << debug_prefix.str()
            << "ENTERED ResolveSymbols::leafCPHandler number leafCP's " << num_leafcp
            << " passsed in value:" << in
	    << " numChildren:" << getNumChildren()
            << std::endl;
	    flushOutput(output);
        }
#endif
    }


    /** Handler for the "maxfunctionvalues" input.*/
    // This does NOT run on the leafCP nodes.  It is a reduction handler.
    // There will be input from from the children of this node.
    // Therefore the values must be merged into final set
    // that represents the max from all direct children of this node.
    void MaxFunctionThreadValuesHandler(const boost::shared_ptr<CBTF_Protocol_FunctionThreadValues>& in)
    {
        init_TopologyInfo();
#ifndef NDEBUG
	std::stringstream output;
	DEBUGPREFIX(Impl::TheTopologyInfo.IsFrontend,Impl::TheTopologyInfo.MaxLeafDistance);
	if (is_time_symbol_events_enabled) {
	    if (max_msgs == 0) {
		output << Time::Now() << " " << debug_prefix.str()
		<< "ResolveSymbols::MaxFunctionThreadValuesHandler entered." << std::endl;
	        flushOutput(output);
	    }
	}
#endif

	++max_msgs;

	CBTF_Protocol_FunctionThreadValues *message = in.get();
#ifndef NDEBUG
        if (is_trace_symbol_events_enabled) {
	    output << debug_prefix.str()
	    << "ENTERED ResolveSymbols::MaxFunctionThreadValuesHandler"
	    << " num values " << message->values.values_len
	    << " num msgs " << max_msgs
	    << " numChildren:" << getNumChildren()
	    << std::endl;
	    flushOutput(output);
	}
#endif

	for(int i=0; i<message->values.values_len; ++i) {
	    std::string f(message->values.values_val[i].function);
	    std::pair<ThreadName,uint64_t> fts =
			std::make_pair(ThreadName(message->values.values_val[i].thread),
				       message->values.values_val[i].value);
	    FunctionThreadCount::iterator it = maxvals.find(f);
	    if ( it == maxvals.end() ) {
#ifndef NDEBUG
	        if (is_debug_symbol_events_enabled) {
		    output << debug_prefix.str() << "ResolveSymbols::MaxFunctionThreadValuesHandler: NEW max for " << f
			<< " in thread:" << fts.first << " value:" << fts.second << std::endl; 
		}
#endif
		maxvals.insert(std::make_pair(f,fts));
	    } else if ( f == (*it).first && fts.second > (*it).second.second ) {
#ifndef NDEBUG
	        if (is_debug_symbol_events_enabled) {
		    output << debug_prefix.str() << "ResolveSymbols::MaxFunctionThreadValuesHandler: UPDATE max for " << f
			<< " in thread:" << fts.first << " value:" << fts.second << std::endl; 
		}
#endif
		(*it).second.first = fts.first;
		(*it).second.second = fts.second;
	    }
	}

	if (max_msgs == getNumChildren()) {
	    // This output is for demo purposes.
	    if (isFrontend() && is_show_metric_events_enabled) {
		std::stringstream demo_output;
		for(FunctionThreadCount::const_iterator it = maxvals.begin(); it != maxvals.end(); ++it) {
		demo_output << "Max: "
		<< " function:" << (*it).first
		<< " thread:" << (*it).second.first
		<< " max:" << (*it).second.second
		<< std::endl;
		}

		std::cout << demo_output.str();
	    }

	    // EMIT final updated max values for ICP or FE.
	    CBTF_Protocol_FunctionThreadValues maxVals;
	    maxVals.values.values_len = maxvals.size();
	    maxVals.values.values_val =
            reinterpret_cast<CBTF_Protocol_FunctionThreadValue*>(
		malloc(std::max(1U, maxVals.values.values_len) *
		sizeof(CBTF_Protocol_FunctionThreadValue))
	    );
	    int i = 0;
	    for(FunctionThreadCount::iterator mfi = maxvals.begin(); mfi != maxvals.end(); ++mfi) {
		CBTF_Protocol_FunctionThreadValue entry;
		entry.function = strdup((*mfi).first.c_str());
		entry.thread = (*mfi).second.first;
		entry.value = (*mfi).second.second;
		maxVals.values.values_val[i] = entry;
		++i;
	    }
	    boost::shared_ptr<CBTF_Protocol_FunctionThreadValues> maxvals_xdr =
		boost::make_shared<CBTF_Protocol_FunctionThreadValues>(maxVals);
#ifndef NDEBUG
	    if (is_trace_symbol_events_enabled) {
		output << debug_prefix.str()
		<< "ResolveSymbols::MaxFunctionValuesHandler: EMIT maxfunctionvalues_xdr_out" << std::endl;
	        flushOutput(output);
	    }
#endif
	    emitOutput<boost::shared_ptr<CBTF_Protocol_FunctionThreadValues> >("maxfunctionvalues_xdr_out",maxvals_xdr);
	}

#ifndef NDEBUG
	//flushOutput(output);
#endif
    }

    /** Handler for the "minfunctionvalues" input.*/
    // This does NOT run on the leafCP nodes.  It is a reduction handler.
    // There will be input from from the children of this node.
    // Therefore the values must be merged into final set
    // that represents the min from all direct children of this node.
    void MinFunctionThreadValuesHandler(const boost::shared_ptr<CBTF_Protocol_FunctionThreadValues>& in)
    {
        init_TopologyInfo();
#ifndef NDEBUG
	std::stringstream output;
	DEBUGPREFIX(Impl::TheTopologyInfo.IsFrontend,Impl::TheTopologyInfo.MaxLeafDistance);
	if (is_time_symbol_events_enabled) {
	    if (min_msgs == 0) {
		output << Time::Now() << " " << debug_prefix.str()
		<< "ResolveSymbols::MinFunctionThreadValuesHandler entered." << std::endl;
	        flushOutput(output);
	    }
	}
#endif

	++min_msgs;

	CBTF_Protocol_FunctionThreadValues *message = in.get();
#ifndef NDEBUG
        if (is_trace_symbol_events_enabled) {
	    output << debug_prefix.str()
	    << "ENTERED ResolveSymbols::MinFunctionThreadValuesHandler"
	    << " num values " << message->values.values_len
	    << " num msgs " << min_msgs
	    << " numChildren:" << getNumChildren()
	    << std::endl;
	    flushOutput(output);
	}
#endif

	for(int i=0; i<message->values.values_len; ++i) {
	    std::string f(message->values.values_val[i].function);
	    std::pair<ThreadName,uint64_t> fts =
			std::make_pair(ThreadName(message->values.values_val[i].thread),
				       message->values.values_val[i].value);
	    FunctionThreadCount::iterator it = minvals.find(f);
	    if ( it == minvals.end() ) {
#ifndef NDEBUG
	        if (is_debug_symbol_events_enabled) {
		    output << debug_prefix.str() << "ResolveSymbols::MinFunctionThreadValuesHandler: NEW min for " << f
			<< " in thread:" << fts.first << " value:" << fts.second << std::endl; 
		}
#endif
		minvals.insert(std::make_pair(f,fts));

	    } else if ( f == (*it).first && fts.second < (*it).second.second ) {
#ifndef NDEBUG
	        if (is_debug_symbol_events_enabled) {
		    output << debug_prefix.str() << "ResolveSymbols::MinFunctionThreadValuesHandler: UPDATE min for " << f
			<< " in thread:" << fts.first << " value:" << fts.second << std::endl; 
		}
#endif
		(*it).second.first = fts.first;
		(*it).second.second = fts.second;
	    }
	}

	if (min_msgs == getNumChildren()) {
	    // This output is for demo purposes.
	    if (isFrontend() && is_show_metric_events_enabled) {
		std::stringstream demo_output;
		for(FunctionThreadCount::const_iterator it = minvals.begin(); it != minvals.end(); ++it) {
		demo_output << "Min: "
		<< " function:" << (*it).first
		<< " thread:" << (*it).second.first
		<< " min:" << (*it).second.second
		<< std::endl;
		}

		std::cout << demo_output.str();
	    }

	    // EMIT final updated min values for ICP or FE.
	    CBTF_Protocol_FunctionThreadValues minVals;
	    minVals.values.values_len = minvals.size();
	    minVals.values.values_val =
            reinterpret_cast<CBTF_Protocol_FunctionThreadValue*>(
		malloc(std::max(1U, minVals.values.values_len) *
		sizeof(CBTF_Protocol_FunctionThreadValue))
	    );
	    int i = 0;
	    for(FunctionThreadCount::iterator mfi = minvals.begin(); mfi != minvals.end(); ++mfi) {
		CBTF_Protocol_FunctionThreadValue entry;
		entry.function = strdup((*mfi).first.c_str());
		entry.thread = (*mfi).second.first;
		entry.value = (*mfi).second.second;
		minVals.values.values_val[i] = entry;
		++i;
	    }
	    boost::shared_ptr<CBTF_Protocol_FunctionThreadValues> minvals_xdr =
		boost::make_shared<CBTF_Protocol_FunctionThreadValues>(minVals);
#ifndef NDEBUG
	    if (is_trace_symbol_events_enabled) {
		output << debug_prefix.str()
		<< "ResolveSymbols::MinFunctionValuesHandler: EMIT minfunctionvalues_xdr_out" << std::endl;
	        flushOutput(output);
	    }
#endif
	    emitOutput<boost::shared_ptr<CBTF_Protocol_FunctionThreadValues> >("minfunctionvalues_xdr_out",minvals_xdr);
	}

#ifndef NDEBUG
	//flushOutput(output);
#endif
    }


    /** Handler for the "avgfunctionvalues" input.*/
    // This does NOT run on the leafCP nodes.  It is a reduction handler.
    // There will be input from from the children of this node.
    // Therefore the values must be merged into final set
    // that represents the avg from all direct children of this node.
    void AvgFunctionValuesHandler(const boost::shared_ptr<CBTF_Protocol_FunctionAvgValues>& in)
    {
        init_TopologyInfo();
#ifndef NDEBUG
	std::stringstream output;
	DEBUGPREFIX(Impl::TheTopologyInfo.IsFrontend,Impl::TheTopologyInfo.MaxLeafDistance);
	if (is_time_symbol_events_enabled) {
	    if (avg_msgs == 0) {
		output << Time::Now() << " " << debug_prefix.str()
		<< "ResolveSymbols::AvgFunctionValuesHandler entered." << std::endl;
	        flushOutput(output);
	    }
	}
#endif

	++avg_msgs;

	CBTF_Protocol_FunctionAvgValues *message = in.get();
#ifndef NDEBUG
        if (is_trace_symbol_events_enabled) {
	    output << debug_prefix.str()
	    << "ENTERED ResolveSymbols::AvgFunctionValuesHandler"
	    << " num values " << message->values.values_len
	    << " num msgs " << avg_msgs
	    << " numChildren:" << getNumChildren()
	    << std::endl;
	    flushOutput(output);
	}
#endif

	for(int i=0; i<message->values.values_len; ++i) {
	    std::string f(message->values.values_val[i].function);
	    std::pair<uint64_t,uint64_t> fts =
			std::make_pair(message->values.values_val[i].value,
				       message->values.values_val[i].num);
	    FunctionAvgMap::iterator it = avgvals.find(f);
	    if ( it == avgvals.end() ) {
#ifndef NDEBUG
	        if (is_debug_symbol_events_enabled) {
		    output << debug_prefix.str() << "ResolveSymbols::AvgFunctionValuesHandler: NEW avg for " << f
			<< " value:" << fts.first << " count:" << fts.second << std::endl; 
		}
#endif
		avgvals.insert(std::make_pair(f,fts));

	    } else if ( f == (*it).first) {
		(*it).second.first += fts.first;
		(*it).second.second += fts.second;
#ifndef NDEBUG
	        if (is_debug_symbol_events_enabled) {
		    output << debug_prefix.str() << "ResolveSymbols::AvgFunctionValuesHandler: UPDATE avg for " << f
			<< " value:" << fts.first << " count:" << fts.second << std::endl; 
		}
#endif
	    }
	}

	if (avg_msgs == getNumChildren()) {
	    // This output is for demo purposes.
	    if (isFrontend() && is_show_metric_events_enabled) {
		std::stringstream demo_output;
		for(FunctionAvgMap::const_iterator it = avgvals.begin(); it != avgvals.end(); ++it) {
		    if ((*it).second.first > 0) {
		    demo_output << "Avg: "
		    << " function:" << (*it).first
		    << " total value:" << (*it).second.first
		    << " total count:" << (*it).second.second
		    << " avg value:" << (*it).second.first / (*it).second.second
		    << std::endl;
		    }
		}

		std::cout << demo_output.str();
	    }

	    // EMIT final updated avg values for ICP or FE.
	    CBTF_Protocol_FunctionAvgValues avgVals;
	    avgVals.values.values_len = avgvals.size();
	    avgVals.values.values_val =
            reinterpret_cast<CBTF_Protocol_FunctionAvgValue*>(
		malloc(std::max(1U, avgVals.values.values_len) *
		sizeof(CBTF_Protocol_FunctionAvgValue))
	    );
	    int i = 0;
	    for(FunctionAvgMap::iterator mfi = avgvals.begin(); mfi != avgvals.end(); ++mfi) {
		CBTF_Protocol_FunctionAvgValue entry;
		entry.function = strdup((*mfi).first.c_str());
		entry.value = (*mfi).second.first;
		entry.num = (*mfi).second.second;
		avgVals.values.values_val[i] = entry;
		++i;
	    }
	    boost::shared_ptr<CBTF_Protocol_FunctionAvgValues> avgvals_xdr =
		boost::make_shared<CBTF_Protocol_FunctionAvgValues>(avgVals);
#ifndef NDEBUG
	    if (is_trace_symbol_events_enabled) {
		output << debug_prefix.str()
		<< "ResolveSymbols::AvgFunctionValuesHandler: EMIT avgfunctionvalues_xdr_out" << std::endl;
	        flushOutput(output);
	    }
#endif
	    emitOutput<boost::shared_ptr<CBTF_Protocol_FunctionAvgValues> >("avgfunctionvalues_xdr_out",avgvals_xdr);
	}

#ifndef NDEBUG
	//flushOutput(output);
#endif
    }

    /** Handler for the "abuffer" input.*/
    // Is this only needed for the leafCPs. All other levels of the
    // tree should not need the buffer directly with regards to the
    // symbol component. The Aggregator component itself is merging
    // this buffer at the non-leaf CP nodes but does not need to emit
    // it or connect it local.
    void AddressBufferHandler(const AddressBuffer& in)
    {
        init_TopologyInfo();
#ifndef NDEBUG
	std::stringstream output;
	DEBUGPREFIX(Impl::TheTopologyInfo.IsFrontend,Impl::TheTopologyInfo.MaxLeafDistance);
	if (is_time_symbol_events_enabled) {
	    if (abuffer.addresscounts.size() == 0) {
		output << Time::Now() << " " << debug_prefix.str()
		<< "ResolveSymbols::AddressBufferHandler." << std::endl;
	        flushOutput(output);
	    }
	}
#endif

#ifndef NDEBUG
        if (is_trace_symbol_events_enabled) {
	    output << debug_prefix.str()
	    << "ENTERED ResolveSymbols::AddressBufferHandler"
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

    /** Handler for the "threadaddrbufmap" input.*/
    // Local to leafCP nodes and provides a mapping of threads
    // to addressbuffers with counts. This should be a no-op at
    // the FE and nonLeafCP nodes.
    // This is used by the termination handler to create the
    // function stats vector once the addresses have been converted
    // to function symbols.
    void ThreadAddrBufMapHandler(const ThreadAddrBufMap& in)
    {
        init_TopologyInfo();
#ifndef NDEBUG
	std::stringstream output;
	DEBUGPREFIX(Impl::TheTopologyInfo.IsFrontend,Impl::TheTopologyInfo.MaxLeafDistance);
	if (is_time_symbol_events_enabled) {
	    if (thread_msgs == 0) {
		output << Time::Now() << " " << debug_prefix.str()
		<< "ResolveSymbols::ThreadAddrBufMapHandler." << std::endl;
	        flushOutput(output);
	    }
	}
#endif

	++thread_msgs;
	numThreads += in.size();

#ifndef NDEBUG
        if (is_trace_symbol_events_enabled) {
            output << debug_prefix.str()
	    << "ENTERED ResolveSymbols::ThreadAddrBufMapHandler"
	    << " with " << in.size() << " thread address buffers"
	    << " thread messages:" << thread_msgs
	    << " known threads:" << numThreads
	    << " numChildren:" << getNumChildren()
	    << std::endl;
	    flushOutput(output);
	}
#endif

	threadAddrBufMap = in;

#ifndef NDEBUG
        if (is_debug_symbol_events_enabled) {
	    ThreadAddrBufMap::const_iterator avi;
	    for (avi = threadAddrBufMap.begin(); avi != threadAddrBufMap.end(); ++avi) {
		output << debug_prefix.str() << "ResolveSymbols::ThreadAddrBufMapHandler thread:" << (*avi).first
		<< " buffer size " << (*avi).second.addresscounts.size()
		<< std::endl;
	    }
	    flushOutput(output);
	}
#endif

#ifndef NDEBUG
	//flushOutput(output);
#endif
    }

    /** Handler for the "linkedobjectvecin" input.*/
    // Local to the leafCP nodes. Used during symbol resolution.
    // This should be a no-op at the FE and nonLeafCP nodes.
    // The passed linkedobject vector is a reduced list of
    // linkedobjects (ie. these are involved in a sample or
    // callstack frame). Each entry contains the threadname info.
    void LinkedObjectVecHandler(const LinkedObjectEntryVec& in)
    {
        init_TopologyInfo();
#ifndef NDEBUG
	std::stringstream output;
	DEBUGPREFIX(Impl::TheTopologyInfo.IsFrontend,Impl::TheTopologyInfo.MaxLeafDistance);
	if (is_time_symbol_events_enabled) {
	    if (lov_msgs == 0) {
		output << Time::Now() << " " << debug_prefix.str()
		<< "ResolveSymbols::LinkedObjectVecHandler." << std::endl;
	        flushOutput(output);
	    }
	}
#endif

	++lov_msgs;

#ifndef NDEBUG
        if (is_trace_symbol_events_enabled) {
	    output << debug_prefix.str()
	    << "ENTERED ResolveSymbols::LinkedObjectVecHandler"
	    << " with " << in.size() << " objects"
	    << " linkedobjectvec messages:" << lov_msgs
	    << " numChildren:" << getNumChildren()
	    << std::endl;
	    flushOutput(output);
	}
#endif

#ifndef NDEBUG
	//flushOutput(output);
#endif
    }

    // Local to the leafCP nodes. Used during symbol resolution.
    // This should be a no-op at the FE and nonLeafCP nodes.
    // The passed addressspace is a mapping of a thread to
    // the group of linked objects know to contain sample
    // or callstack frame addresses.
    // This could be used to replace the linkedobject vector.
    // 
    void AddressSpaceHandler(const AddressSpace& in)
    {
        init_TopologyInfo();
#ifndef NDEBUG
	std::stringstream output;
	DEBUGPREFIX(Impl::TheTopologyInfo.IsFrontend,Impl::TheTopologyInfo.MaxLeafDistance);
	if (is_time_symbol_events_enabled) {
	    if (lovmap_msgs == 0) {
		output << Time::Now() << " " << debug_prefix.str()
		<< "ResolveSymbols::AddressSpaceHandler." << std::endl;
	        flushOutput(output);
	    }
	}
#endif

	++lovmap_msgs;
	addressspace = in;

	for (AddressSpace::iterator i = addressspace.begin(); i != addressspace.end(); ++i) {
	    for (LinkedObjectVec::iterator k = (*i).second.begin();
		     k != (*i).second.end(); ++k) {
 		LinkedObjectEntry e;
                e.tname = (*i).first;
                e.path = (*k).getPath();
                e.is_executable = (*k).isExecutable();
                e.time_loaded = (*k).getTimeInterval().getBegin();
                e.time_unloaded = (*k).getTimeInterval().getEnd();
                e.addr_begin = (*k).getAddressRange().getBegin();
                e.addr_end = (*k).getAddressRange().getEnd();
                linkedobjectvec.push_back(e);
	    }
	}

#ifndef NDEBUG
        if (is_trace_symbol_events_enabled) {
            output << debug_prefix.str()
	    << "ENTERED ResolveSymbols::AddressSpaceHandler"
	    << " with " << in.size() << " entries"
	    << " num messages:" << lovmap_msgs
	    << " numChildren:" << getNumChildren()
	    << std::endl;
	    flushOutput(output);
	}
#endif

#ifndef NDEBUG
        if (is_debug_symbol_events_enabled) {
	    for (AddressSpace::iterator i = addressspace.begin(); i != addressspace.end(); ++i) {
		output << debug_prefix.str() << "ResolveSymbols::AddressSpaceHandler addressspace  thread:" << (*i).first << std::endl;
		for (LinkedObjectVec::iterator k = (*i).second.begin();
		     k != (*i).second.end(); ++k) {
		    output << "\t name:" << (*k).getPath() << std::endl;
		}
	    }
	    output << std::endl;
	    flushOutput(output);
	}
#endif

#ifndef NDEBUG
	//flushOutput(output);
#endif
    }

    /** Handler for the "symtabapisymsin" input.*/
    // this is likely unused at this time.
    void symtabAPISymbolHandler(const SymtabAPISymbols& in)
    {
        init_TopologyInfo();

#ifndef NDEBUG
	std::stringstream output;
	DEBUGPREFIX(Impl::TheTopologyInfo.IsFrontend,Impl::TheTopologyInfo.MaxLeafDistance);
        if (is_trace_symbol_events_enabled) {
	    output << debug_prefix.str()
	    << "ENTERED ResolveSymbols::symtabAPISymbolHandler"
	    << " numChildren:" << getNumChildren()
	    << std::endl;
	    flushOutput(output);
	}
#endif
	AddressCounts ac = abuffer.addresscounts;
	AddressCounts::const_iterator aci;
	LinkedObjectEntryVec::iterator li;
	for (aci = ac.begin(); aci != ac.end(); ++aci) {
	    bool foundit = false;
	    for (li = linkedobjectvec.begin(); li != linkedobjectvec.end(); ++li) {
		AddressRange addr_range(li->addr_begin,li->addr_end);
		if (addr_range.doesContain(aci->first) ) {
		    symtabmap.insert(std::make_pair(addr_range,
				 std::make_pair(addr_range, *li )
				));
		    foundit = true;
		    break;
		}
	    }

#ifndef NDEBUG
      	    if (is_debug_symbol_events_enabled) {
		if(!foundit) {
		    output << "ResolveSymbols::symtabAPISymbolHandler: CANNOT RESOLVE symbols for address "
			<< aci->first  << std::endl;
		}
	    }
#endif
	}

	// Now cycle through these symboltables and find functions and statements.
	SymtabAPISymbols stapi_symbols;

	for(SymbolTableMap::iterator i = symtabmap.begin(); i != symtabmap.end(); ++i) {
		LinkedObjectEntry le = i->second.second;
		SymbolTable st = i->first;
#ifndef NDEBUG
        	if (is_debug_symbol_events_enabled) {
		    output << debug_prefix.str()
			<< "ResolveSymbols::symtabAPISymbolHandler: resolve symbols for "
			<< le.path  << std::endl;
		}
#endif
		stapi_symbols.getSymbols(abuffer,le,st);
		CBTF_Protocol_SymbolTable pst;
		pst = st;
		pst.linked_object.path = strdup(le.path.c_str());
		boost::shared_ptr<CBTF_Protocol_SymbolTable> symtable_out =
			boost::make_shared<CBTF_Protocol_SymbolTable>(pst);
#ifndef NDEBUG
        	if (is_debug_symbol_events_enabled) {
		    output << debug_prefix.str()
			<< "ResolveSymbols::symtabAPISymbolHandler: EMITS symboltable_xdr_out"
			<< std::endl;
		    flushOutput(output);
		}
#endif
		emitOutput<boost::shared_ptr<CBTF_Protocol_SymbolTable> >("symboltable_xdr_out",symtable_out);

	}

#ifndef NDEBUG
      	if (is_trace_symbol_events_enabled) {
	    output << debug_prefix.str()
	    << "ResolveSymbols::symtabAPISymbolHandler: EMIT SymtabAPISymbols" << std::endl;
	    flushOutput(output);
	}
#endif
	//emitOutput<SymtabAPISymbols>("symtabapisymsout",stapi_symbols);

#ifndef NDEBUG
	//flushOutput(output);
#endif
    }

#if defined(BFD_AVAILABLE_DPM) && defined(USE_LINKEDOBJECT_VEC)
    /** Handler for the "bfdsymsin" input.*/
    // unused for most platforms. could be needed on ARM
    // unless we get symtabapi support sooner.
    void BFDSymbolHandler(const BFDSymbols& in)
    {
	AddressCounts::const_iterator aci;
	LinkedObjectEntryVec::iterator li;
	for (aci = ac.begin(); aci != ac.end(); ++aci) {
	    bool foundit = false;
	    for (li = linkedobjectvec.begin(); li != linkedobjectvec.end(); ++li) {
		AddressRange addr_range(li->addr_begin,li->addr_end);
		if (addr_range.doesContain(aci->first) ) {
		    symtabmap.insert(std::make_pair(addr_range,
				 std::make_pair(addr_range, *li )
				));
		    foundit = true;
		    break;
		}
	    }

	    if(!foundit) {
		std::cerr << "CANNOT RESOLVE symbols for address "
			<< aci->first  << std::endl;
	    }
	}

	// Now cycle through these symboltables and find functions and statements.
	BFDSymbols bfd_symbols = in;

	for(SymbolTableMap::iterator i = symtabmap.begin(); i != symtabmap.end(); ++i) {
		LinkedObjectEntry le = i->second.second;
		SymbolTable st = i->first;
		bfd_symbols.getSymbols(abuffer,le,st);
	}
    }
#endif


    // This is intended to run only at the leaf CP levels.
    // Creates initial symboltables.
    // Creates the min,max,avg values.
    void finishedHandler(const bool& in)
    {
        init_TopologyInfo();

#ifndef NDEBUG
	std::stringstream output;
	DEBUGPREFIX(Impl::TheTopologyInfo.IsFrontend,Impl::TheTopologyInfo.MaxLeafDistance);
	if (is_time_symbol_events_enabled) {
	    output << Time::Now() << " " << debug_prefix.str()
	    << "ResolveSymbols::finishedHandler entered." << std::endl;
	    flushOutput(output);
	}
	if (is_trace_symbol_events_enabled) {
            output << debug_prefix.str()
                << "ENTERED ResolveSymbols::finishedHandler"
	        << " numChildren:" << getNumChildren()
		<< std::endl;
	    flushOutput(output);
	}
#endif

	if (isFrontend() || isNonLeafCP()) {
#ifndef NDEBUG
	    if (is_trace_symbol_events_enabled) {
		output << debug_prefix.str()
		<< "ResolveSymbols::finishedHandler no work at this level. ignoring." << std::endl;
	    }
#endif
	    return;
	}

	AddressCounts ac = abuffer.addresscounts;
	for (AddressCounts::const_iterator aci = ac.begin(); aci != ac.end(); ++aci) {
	    bool foundit = false;
	    for (LinkedObjectEntryVec::iterator li = linkedobjectvec.begin(); li != linkedobjectvec.end(); ++li) {
		AddressRange addr_range(li->addr_begin,li->addr_end);
		if (addr_range.doesContain(aci->first) ) {
		    symtabmap.insert(std::make_pair(addr_range,
				 std::make_pair(addr_range, *li )
				));
		    foundit = true;
		    break;
		}
	    }

	    if(!foundit) {
#ifndef NDEBUG
        	if (is_debug_symbol_events_enabled) {
		    output << debug_prefix.str()
		        << "ResolveSymbols::finishedHandler: CANNOT RESOLVE symbols for address "
			<< aci->first  << std::endl;
		}
#endif
	    }
	}

	// Now cycle through these symboltables and find functions and statements.
	SymtabAPISymbols stapi_symbols;

	for(SymbolTableMap::iterator ii = symtabmap.begin(); ii != symtabmap.end(); ++ii)
	{
	    LinkedObjectEntry le = ii->second.second;
	    SymbolTable st = ii->first;
#ifndef NDEBUG
            if (is_debug_symbol_events_enabled) {
	        output << debug_prefix.str()
		<< "ResolveSymbols::finishedHandler: resolve symbols for " << le.path
			<< std::endl;
	    }
#endif

	    stapi_symbols.getSymbols(abuffer,le,st);

	    CBTF_Protocol_SymbolTable pst;
	    pst = st;
	    pst.linked_object.path = strdup(le.path.c_str());
	    boost::shared_ptr<CBTF_Protocol_SymbolTable> symtable_out =
			boost::make_shared<CBTF_Protocol_SymbolTable>(pst);
#ifndef NDEBUG
            if (is_trace_symbol_events_enabled) {
	        output << debug_prefix.str()
		<< "ResolveSymbols::finishedHandler: EMIT symboltable_xdr_out"
			<< std::endl;
	    }
#endif
	    emitOutput<boost::shared_ptr<CBTF_Protocol_SymbolTable> >("symboltable_xdr_out",symtable_out);

	    AddressRange stRange = st.getAddressRange();
	    FunctionMap stFuncs = st.getFunctions();

#ifndef NDEBUG
            if (is_debug_symbol_events_enabled) {
	        output << debug_prefix.str()
		<< "ResolveSymbols::finishedHandler: num functions:" << stFuncs.size()
	        << " stRange:" << stRange << std::endl;
	    }
#endif

	    for (ThreadAddrBufMap::const_iterator avi = threadAddrBufMap.begin(); avi != threadAddrBufMap.end(); ++avi) {
#ifndef NDEBUG
		if (is_debug_symbol_events_enabled) {
		    output << debug_prefix.str()
		    << "ResolveSymbols::finishedHandler: thread:" << (*avi).first
		    << " buffer size " << (*avi).second.addresscounts.size()
		    << std::endl;
		}
#endif

		AddressCounts ac = (*avi).second.addresscounts;
		for(FunctionMap::const_iterator fi = stFuncs.begin(); fi != stFuncs.end(); ++fi) {
		    AddressRange frange(fi->first.getBegin(),fi->first.getEnd());
	 	    for (AddressCounts::const_iterator aci=ac.equal_range(frange.getBegin()).first;
			 aci!=ac.equal_range(frange.getEnd()).second;++aci) {
			if (frange.doesContain((*aci).first)) {
			    FuncThreadStats fts(fi->second,(*avi).first,(*aci).second);
			    FuncStatsVec::iterator it = std::find(fstatvec.begin(),fstatvec.end(),fts);
			    if (it == fstatvec.end()) {
		  		fstatvec.push_back(fts);
			    } else {
		     		(*it).value += (*aci).second;
			    }
			}
		    }
		}
	    }
	}

	std::set<std::string> functions;
	FunctionAvgMap functionscounts;
	for(FuncStatsVec::iterator fit = fstatvec.begin(); fit != fstatvec.end(); ++fit) {
#ifndef NDEBUG
	    if (is_debug_symbol_events_enabled) {
		output << debug_prefix.str() << "FuncStatsVec: function:" << (*fit).funcname
		<< " thread:" << (*fit).tname
		<< " count:" << (*fit).value
		<< std::endl;
	    }
#endif
	    functions.insert((*fit).funcname);
	    FunctionAvgMap::iterator it = functionscounts.find(std::string((*fit).funcname));
	    if (it == functionscounts.end() ) {
		functionscounts.insert(std::make_pair(std::string((*fit).funcname),
				       std::make_pair((*fit).value,1)));
#ifndef NDEBUG
		if (is_debug_symbol_events_enabled) {
		output << debug_prefix.str() << "FuncStatsVec: function:" << (*fit).funcname
		<< " total counts:" << (*fit).value << " num threads:" << 1
		<< " avg:" << (*fit).value << std::endl;
		}
#endif
	    } else {
		(*it).second.first += (*fit).value;
		++(*it).second.second;
#ifndef NDEBUG
		if (is_debug_symbol_events_enabled) {
		    output << debug_prefix.str()
		    << "FuncStatsVec: function:" << (*fit).funcname
		    << " UPDATE total counts:" << (*it).second.first
		    << " num threads:" << (*it).second.second
		    << " avg:" << (*it).second.first/(*it).second.second
		    << std::endl;
		}
#endif
	    }
	}

	CBTF_Protocol_FunctionAvgValues avgVals;
	{
	    avgVals.values.values_len = functionscounts.size();
	    avgVals.values.values_val =
            reinterpret_cast<CBTF_Protocol_FunctionAvgValue*>(
                malloc(std::max(1U, avgVals.values.values_len) *
                   sizeof(CBTF_Protocol_FunctionAvgValue)));
	    int i = 0;
	    for(FunctionAvgMap::iterator it = functionscounts.begin(); it != functionscounts.end(); ++it) {
#ifndef NDEBUG
		if (is_debug_symbol_events_enabled) {
		    output << debug_prefix.str()
		    << "FuncAvgMap: function:" << (*it).first
		    << " counts:" << (*it).second.first
		    << " threads:" << (*it).second.second
		    << " avg:" << (*it).second.first/(*it).second.second
		    << std::endl;
		}
#endif
		CBTF_Protocol_FunctionAvgValue entry;
		entry.function = strdup((*it).first.c_str());
		entry.value = (*it).second.first;
		entry.num = (*it).second.second;
		avgVals.values.values_val[i] = entry;
		++i;
	    }
	}

	boost::shared_ptr<CBTF_Protocol_FunctionAvgValues> avgvals_xdr =
               boost::make_shared<CBTF_Protocol_FunctionAvgValues>(avgVals);

	FunctionThreadCount maxfuncs;
	FunctionThreadCount minfuncs;
	std::set<std::string>::const_iterator f;
	for (std::set<std::string>::const_iterator f = functions.begin(); f != functions.end(); ++f) {
	    for(FuncStatsVec::iterator fit = fstatvec.begin(); fit != fstatvec.end(); ++fit) {
		if ( (*fit).value == 0) {
		    // only intersted in function sample/trace points.
		    continue;
		}
		if (*f == (*fit).funcname) {
		    std::pair<ThreadName,uint64_t> fts = std::make_pair((*fit).tname,(*fit).value);
		    // handle MAX.
		    FunctionThreadCount::iterator it = maxfuncs.find(*f);
		    if ( it == maxfuncs.end() ) {
			maxfuncs.insert(std::make_pair(*f,fts));
		    } else if ( (*fit).funcname == (*it).first && (*fit).value > (*it).second.second ) {
			(*it).second.first = (*fit).tname;
			(*it).second.second = (*fit).value;
		    }
		    // handle MIN.
		    it = minfuncs.find(*f);
		    if ( it == minfuncs.end() ) {
			minfuncs.insert(std::make_pair(*f,fts));
		    } else if ( (*fit).funcname == (*it).first && (*fit).value < (*it).second.second ) {
			(*it).second.first = (*fit).tname;
			(*it).second.second = (*fit).value;
		    }
		    
		}
	    }
	}

	CBTF_Protocol_FunctionThreadValues maxVals;
	maxVals.values.values_len = maxfuncs.size();
	maxVals.values.values_val =
        reinterpret_cast<CBTF_Protocol_FunctionThreadValue*>(
            malloc(std::max(1U, maxVals.values.values_len) *
                   sizeof(CBTF_Protocol_FunctionThreadValue))
            );
	int i = 0;
	for(FunctionThreadCount::iterator mfi = maxfuncs.begin(); mfi != maxfuncs.end(); ++mfi) {
	   CBTF_Protocol_FunctionThreadValue entry;
	   entry.function = strdup((*mfi).first.c_str());
	   entry.thread = (*mfi).second.first;
	   entry.value = (*mfi).second.second;
	   maxVals.values.values_val[i] = entry;
	   ++i;
	}
	boost::shared_ptr<CBTF_Protocol_FunctionThreadValues> maxvals_xdr =
               boost::make_shared<CBTF_Protocol_FunctionThreadValues>(maxVals);

	CBTF_Protocol_FunctionThreadValues minVals;
	minVals.values.values_len = minfuncs.size();
	minVals.values.values_val =
        reinterpret_cast<CBTF_Protocol_FunctionThreadValue*>(
            malloc(std::max(1U, minVals.values.values_len) *
                   sizeof(CBTF_Protocol_FunctionThreadValue))
            );
	i = 0;
	for(FunctionThreadCount::iterator mfi = minfuncs.begin(); mfi != minfuncs.end(); ++mfi) {
	   CBTF_Protocol_FunctionThreadValue entry;
	   entry.function = strdup((*mfi).first.c_str());
	   entry.thread = (*mfi).second.first;
	   entry.value = (*mfi).second.second;
	   minVals.values.values_val[i] = entry;
	   ++i;
	}

	boost::shared_ptr<CBTF_Protocol_FunctionThreadValues> minvals_xdr =
                boost::make_shared<CBTF_Protocol_FunctionThreadValues>(minVals);

#ifndef NDEBUG
	if (is_trace_symbol_events_enabled) {
	    output << debug_prefix.str()
		<< "ResolveSymbols::finishedHandler: EMIT max,min,avg functionvalues_xdr_out" << std::endl;
	    flushOutput(output);
	}
#endif
	emitOutput<boost::shared_ptr<CBTF_Protocol_FunctionThreadValues> >("maxfunctionvalues_xdr_out",maxvals_xdr);
	emitOutput<boost::shared_ptr<CBTF_Protocol_FunctionThreadValues> >("minfunctionvalues_xdr_out",minvals_xdr);
	emitOutput<boost::shared_ptr<CBTF_Protocol_FunctionAvgValues> >("avgfunctionvalues_xdr_out",avgvals_xdr);
	
#ifndef NDEBUG
	if (is_time_symbol_events_enabled) {
	    output << Time::Now() << " " << debug_prefix.str()
	    << "ResolveSymbols::finishedHandler exits." << std::endl;
	    flushOutput(output);
	}
#endif

#ifndef NDEBUG
	//flushOutput(output);
#endif
    }


    // notification of the number of terminated threads.
    // At the leaf CP nodes this informs the component of the
    // number of terminated threads.
    void numTerminatedHandler(const long& in)
    {

        init_TopologyInfo();
	numTerminated += in;

#ifndef NDEBUG
	std::stringstream output;
	DEBUGPREFIX(Impl::TheTopologyInfo.IsFrontend,Impl::TheTopologyInfo.MaxLeafDistance);
	if (is_time_symbol_events_enabled) {
	    output << Time::Now() << " " << debug_prefix.str()
	    << "ResolveSymbols::numTerminatedHandler entered." << std::endl;
	    flushOutput(output);
	}
#endif

#ifndef NDEBUG
	if (is_trace_symbol_events_enabled) {
            output << debug_prefix.str()
                << "ENTERED ResolveSymbols::numTerminatedHandler"
		<< " numTerminated:" << numTerminated
		<< " addressspace size:" << addressspace.size()
		<< " threadAddrBufMap size:" << threadAddrBufMap.size()
	        << " numChildren:" << getNumChildren()
		<< std::endl;
	    flushOutput(output);
	}
#endif
    }

    // This runs at the FE and non leaf CP nodes. It should merge
    // the incoming symbol tables. TODO: merge.
    // Currently just passing them on.
    void CbtfProtocolSymbolTableHandler(const boost::shared_ptr<CBTF_Protocol_SymbolTable>& in)
    {
        init_TopologyInfo();
#ifndef NDEBUG
	std::stringstream output;
	DEBUGPREFIX(Impl::TheTopologyInfo.IsFrontend,Impl::TheTopologyInfo.MaxLeafDistance);
	if (is_time_symbol_events_enabled) {
	    if (sym_msgs == 0) {
		output << Time::Now() << " " << debug_prefix.str()
		<< "ResolveSymbols::CbtfProtocolSymbolTableHandler entered." << std::endl;
	        flushOutput(output);
	    }
	}
#endif
	++sym_msgs;

	CBTF_Protocol_SymbolTable *message = in.get();

#ifndef NDEBUG
        if (is_trace_symbol_events_enabled) {
	    output << debug_prefix.str()
	    << "ENTERED ResolveSymbols::CbtfProtocolSymbolTableHandler"
	    << " linkedobject:" << message->linked_object.path
	    << " functions:" << message->functions.functions_len
	    << " statements:" << message->statements.statements_len
	    << " msgs:" << sym_msgs
	    << " numChildren:" << getNumChildren()
	    << std::endl;
	    flushOutput(output);
	}
#endif

#ifndef NDEBUG
        if (is_debug_symbol_events_enabled) {
	  for (int i=0; i< message->functions.functions_len; ++i) {
	    const CBTF_Protocol_FunctionEntry& msg_function =
                        message->functions.functions_val[i];
	    output << debug_prefix.str()
	    << "function name:" << msg_function.name;
	    for(int k = 0; k < msg_function.bitmaps.bitmaps_len; ++k) {
                const CBTF_Protocol_AddressBitmap& msg_bitmap =
                            msg_function.bitmaps.bitmaps_val[k];
		output << " range:" << AddressRange(Address(msg_bitmap.range.begin),
						     Address(msg_bitmap.range.end))
		<< std::endl;
	    }
	  }

	  output << debug_prefix.str() << " statements:" << message->statements.statements_len << std::endl;
	  for(int j = 0; j < message->statements.statements_len; ++j) {
		const CBTF_Protocol_StatementEntry& msg_statement =
                        message->statements.statements_val[j];
		output << debug_prefix.str()
                   << " FILE:" << msg_statement.path.path
                   << " LINE:" << msg_statement.line
                   << " COL:" << msg_statement.column
		<< std::endl;
	  }
	  flushOutput(output);
	}
#endif

	// Emit this symboltable xdr message.  Should these be held until finished or
	// numTerminated is known. We know fron addressspaces how many threads are
	// participating...
#ifndef NDEBUG
            if (is_trace_symbol_events_enabled) {
		output << debug_prefix.str()
		<< "ResolveSymbols::CbtfProtocolSymbolTableHandler: EMIT symboltable_xdr_out."
	        << std::endl;
		flushOutput(output);
	    }
#endif
	emitOutput<boost::shared_ptr<CBTF_Protocol_SymbolTable> >("symboltable_xdr_out",in);

#ifndef NDEBUG
	//flushOutput(output);
#endif
    }

    void CbtfProtocolSymbolTableGroupHandler(const boost::shared_ptr<CBTF_Protocol_SymbolTable_Group>& in)
    {
    }
    
    SymbolTableMap symtabmap;
    LinkedObjectEntryVec linkedobjectvec;
    AddressSpace addressspace;
    AddressBuffer abuffer;
    FuncStatsVec fstatvec;
    ThreadAddrBufMap threadAddrBufMap;
    FunctionThreadCount maxvals;
    FunctionThreadCount minvals;
    FunctionAvgMap avgvals;

}; // class ResolveSymbols

KRELL_INSTITUTE_CBTF_REGISTER_FACTORY_FUNCTION(ResolveSymbols)
