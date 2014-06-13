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
#if BFD_AVAILABLE_DPM
#include "KrellInstitute/Core/BFDSymbols.hpp"
#endif
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
#include <KrellInstitute/Messages/Symbol.h>
#include "KrellInstitute/Messages/Thread.h"
#include "KrellInstitute/Messages/ThreadEvents.h"
#include "KrellInstitute/Messages/Stats.h"


using namespace KrellInstitute::CBTF;
using namespace KrellInstitute::Core;


// FIXME: Move these to include file.
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
//
typedef std::vector<FuncThreadStats> FuncStatsVec;
// mapping of addressbuffers to thread from AddressAggregatorComponent.
//
typedef std::map<ThreadName,AddressBuffer>  ThreadAddrBufMap;
// mapping of function to thread and value.
//
typedef std::map<std::string,std::pair<ThreadName,uint64_t> > FunctionThreadCount;
// mapping of function to total value and total number of threads.
//
typedef std::map<std::string,std::pair<uint64_t,uint64_t> > FunctionAvgMap;

namespace {

#ifndef NDEBUG
/** Flag indicating if debuging for Symbol events is enabled. */
bool is_debug_symbol_events_enabled =
    (getenv("CBTF_DEBUG_SYMBOL_EVENTS") != NULL);
/** Flag indicating if tracing for Symbol events is enabled. */
bool is_trace_symbol_events_enabled =
    (getenv("CBTF_TRACE_SYMBOL_EVENTS") != NULL);
#endif


/** count indicating number of leaf CP's in mrnet tree. */
    int num_leafcp = 0;
/** count indicating number of max value messages to expect at FE. */
    int max_msgs = 0;
/** count indicating number of min value messages to expect at FE. */
    int min_msgs = 0;
/** count indicating number of avg value messages to expect at FE. */
    int avg_msgs = 0;
/** flag indicating FE has count of leaf CP's in tree. */
    bool handle_leafcp_msg = false;

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
            "leafCPnumBE", boost::bind(&ResolveSymbols::leafCPHandler, this, _1)
            );

        declareInput<AddressBuffer>(
            "abufferin", boost::bind(&ResolveSymbols::AddressBufferHandler, this, _1)
            );
        declareInput<LinkedObjectEntryVec>(
            "linkedobjectvecin", boost::bind(&ResolveSymbols::LinkedObjectVecHandler, this, _1)
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
        declareInput<boost::shared_ptr<CBTF_Protocol_FunctionThreadValues> >(
            "maxfunctionvalues", boost::bind(&ResolveSymbols::MaxFunctionThreadValuesHandler, this, _1)
            );
        declareInput<boost::shared_ptr<CBTF_Protocol_FunctionThreadValues> >(
            "minfunctionvalues", boost::bind(&ResolveSymbols::MinFunctionThreadValuesHandler, this, _1)
            );
        declareInput<boost::shared_ptr<CBTF_Protocol_FunctionAvgValues> >(
            "avgfunctionvalues", boost::bind(&ResolveSymbols::AvgFunctionValuesHandler, this, _1)
            );

        declareOutput<SymtabAPISymbols>("symtabapisymsout");
	declareOutput<boost::shared_ptr<CBTF_Protocol_SymbolTable> >(
	    "symboltable_xdr_out"
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
	declareOutput<bool>("symbols_finished");
    }

    void leafCPHandler(const int& in)
    {
        num_leafcp++;
#ifndef NDEBUG
        if (is_trace_symbol_events_enabled) {
            std::cerr << getpid() << " "
            << "ENTERED ResolveSymbols::leafCPHandler number leafCP's " << num_leafcp
            << std::endl;
        }
#endif
    }


    /** Handler for the "maxfunctionvalues" input.*/
    // There will be input from each leafCP in the network.
    // Therefore the values must be merged into final set
    // that represents the max from all leafCPs.
    void MaxFunctionThreadValuesHandler(const boost::shared_ptr<CBTF_Protocol_FunctionThreadValues>& in)
    {
	++max_msgs;
	CBTF_Protocol_FunctionThreadValues *message = in.get();
#ifndef NDEBUG
        if (is_trace_symbol_events_enabled) {
            std::cerr << getpid() << " "
	    << "ENTERED ResolveSymbols::MaxFunctionThreadValuesHandler"
	    << " num values " << message->values.values_len
	    << " num msgs " << max_msgs
	    << " num leafCP " << num_leafcp
	    << std::endl;
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
		    std::cerr << "ResolveSymbols::MaxFunctionThreadValuesHandler: NEW max for " << f
			<< " in thread:" << fts.first << " value:" << fts.second << std::endl; 
		}
#endif
		maxvals.insert(std::make_pair(f,fts));
	    } else if ( f == (*it).first && fts.second > (*it).second.second ) {
#ifndef NDEBUG
	        if (is_debug_symbol_events_enabled) {
		    std::cerr << "ResolveSymbols::MaxFunctionThreadValuesHandler: UPDATE max for " << f
			<< " in thread:" << fts.first << " value:" << fts.second << std::endl; 
		}
#endif
		(*it).second.first = fts.first;
		(*it).second.second = fts.second;
	    }
	}

	// This output is for demo purposes.
	if (max_msgs == num_leafcp) {
	    FunctionThreadCount::const_iterator it;
	    std::stringstream output;
	    for(it = maxvals.begin(); it != maxvals.end(); ++it) {
		output << "Max: "
		<< " function:" << (*it).first
		<< " thread:" << (*it).second.first
		<< " max:" << (*it).second.second
		<< std::endl;
	    }
	    std::cerr << output.str();
	}
    }

    /** Handler for the "minfunctionvalues" input.*/
    // There will be input from each leafCP in the network.
    // Therefore the values must be merged into final set
    // that represents the min from all leafCPs.
    void MinFunctionThreadValuesHandler(const boost::shared_ptr<CBTF_Protocol_FunctionThreadValues>& in)
    {
	++min_msgs;
	CBTF_Protocol_FunctionThreadValues *message = in.get();
#ifndef NDEBUG
        if (is_trace_symbol_events_enabled) {
            std::cerr << getpid() << " "
	    << "ENTERED ResolveSymbols::MinFunctionThreadValuesHandler"
	    << " num values " << message->values.values_len
	    << " num msgs " << min_msgs
	    << " num leafCP " << num_leafcp
	    << std::endl;
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
		    std::cerr << "ResolveSymbols::MinFunctionThreadValuesHandler: NEW min for " << f
			<< " in thread:" << fts.first << " value:" << fts.second << std::endl; 
		}
#endif
		minvals.insert(std::make_pair(f,fts));

	    } else if ( f == (*it).first && fts.second < (*it).second.second ) {
#ifndef NDEBUG
	        if (is_debug_symbol_events_enabled) {
		    std::cerr << "ResolveSymbols::MinFunctionThreadValuesHandler: UPDATE min for " << f
			<< " in thread:" << fts.first << " value:" << fts.second << std::endl; 
		}
#endif
		(*it).second.first = fts.first;
		(*it).second.second = fts.second;
	    }
	}

	// This output is for demo purposes.
	if (min_msgs == num_leafcp) {
	   std::stringstream output;
	   FunctionThreadCount::const_iterator it;
	   for(it = minvals.begin(); it != minvals.end(); ++it) {
		output << "Min: "
		<< " function:" << (*it).first
		<< " thread:" << (*it).second.first
		<< " min:" << (*it).second.second
		<< std::endl;
	    }
	    std::cerr << output.str();
	}
    }

    /** Handler for the "avgfunctionvalues" input.*/
    // There will be input from each leafCP in the network.
    // Therefore the values must be merged into final set
    // that represents the avg from all leafCPs.
    void AvgFunctionValuesHandler(const boost::shared_ptr<CBTF_Protocol_FunctionAvgValues>& in)
    {
	++avg_msgs;
	CBTF_Protocol_FunctionAvgValues *message = in.get();
#ifndef NDEBUG
        if (is_trace_symbol_events_enabled) {
            std::cerr << getpid() << " "
	    << "ENTERED ResolveSymbols::AvgFunctionValuesHandler"
	    << " num values " << message->values.values_len
	    << " num msgs " << avg_msgs
	    << " num leafCP " << num_leafcp
	    << std::endl;
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
		    std::cerr << "ResolveSymbols::AvgFunctionValuesHandler: NEW avg for " << f
			<< " value:" << fts.first << " count:" << fts.second << std::endl; 
		}
#endif
		avgvals.insert(std::make_pair(f,fts));

	    } else if ( f == (*it).first) {
		(*it).second.first += fts.first;
		(*it).second.second += fts.second;
#ifndef NDEBUG
	        if (is_debug_symbol_events_enabled) {
		    std::cerr << "ResolveSymbols::AvgFunctionValuesHandler: UPDATE avg for " << f
			<< " value:" << fts.first << " count:" << fts.second << std::endl; 
		}
#endif
	    }
	}

	// This output is for demo purposes.
	if (avg_msgs == num_leafcp) {
	    std::stringstream output;
	    FunctionAvgMap::const_iterator it;
	    for(it = avgvals.begin(); it != avgvals.end(); ++it) {
	      if ((*it).second.first > 0) {
		output << "Avg: "
		<< " function:" << (*it).first
		<< " total value:" << (*it).second.first
		<< " total count:" << (*it).second.second
		<< " avg value:" << (*it).second.first / (*it).second.second
		<< std::endl;
	      }
	    }
	    std::cerr << output.str();
	}
    }

    /** Handler for the "abuffer" input.*/
    void AddressBufferHandler(const AddressBuffer& in)
    {
#ifndef NDEBUG
        if (is_trace_symbol_events_enabled) {
            std::cerr << getpid() << " "
	    << "ENTERED ResolveSymbols::AddressBufferHandler"
	    << " with " << in.addresscounts.size() << " addresses"
	    << std::endl;
	}
#endif
	abuffer = in;
    }

    /** Handler for the "threadaddrbufmap" input.*/
    void ThreadAddrBufMapHandler(const ThreadAddrBufMap& in)
    {
#ifndef NDEBUG
        if (is_trace_symbol_events_enabled) {
            std::cerr << getpid() << " "
	    << "ENTERED ResolveSymbols::ThreadAddrBufMapHandler"
	    << " with " << in.size() << " thread address buffers"
	    << std::endl;
	}
#endif

	threadAddrBufMap = in;

#ifndef NDEBUG
        if (is_debug_symbol_events_enabled) {
	ThreadAddrBufMap::const_iterator avi;
	std::stringstream output;
	for (avi = threadAddrBufMap.begin(); avi != threadAddrBufMap.end(); ++avi) {
	    output << "ResolveSymbols::ThreadAddrBufMapHandler thread:" << (*avi).first
		<< " buffer size " << (*avi).second.addresscounts.size()
		<< std::endl;
	}
	std::cerr << output.str();
	}
#endif
    }

    /** Handler for the "linkedobjectvecin" input.*/
    void LinkedObjectVecHandler(const LinkedObjectEntryVec& in)
    {
#ifndef NDEBUG
        if (is_trace_symbol_events_enabled) {
            std::cerr << getpid() << " "
	    << "ENTERED ResolveSymbols::LinkedObjectVecHandler"
	    << " with " << in.size() << " objects"
	    << std::endl;
	}
#endif
	linkedobjectvec = in;
    }

    /** Handler for the "symtabapisymsin" input.*/
    void symtabAPISymbolHandler(const SymtabAPISymbols& in)
    {
#ifndef NDEBUG
        if (is_trace_symbol_events_enabled) {
            std::cerr << getpid() << " "
	    << "ENTERED ResolveSymbols::symtabAPISymbolHandler"
	    << std::endl;
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

	    if(!foundit) {
		std::cerr << "ResolveSymbols::symtabAPISymbolHandler: CANNOT RESOLVE symbols for address "
			<< aci->first  << std::endl;
	    }
	}

	// Now cycle through these symboltables and find functions and statements.
	SymtabAPISymbols stapi_symbols;

	for(SymbolTableMap::iterator i = symtabmap.begin(); i != symtabmap.end(); ++i) {
		LinkedObjectEntry le = i->second.second;
		SymbolTable st = i->first;
#ifndef NDEBUG
        	if (is_debug_symbol_events_enabled) {
		    std::cerr << getpid() << " "
			<< "ResolveSymbols::symtabAPISymbolHandler: resolve symbols for "
			<< le.path  << std::endl;
		}
#endif
		stapi_symbols.getSymbols(abuffer,le,st);
	}
	emitOutput<SymtabAPISymbols>("symtabapisymsout",stapi_symbols);
    }

#if BFD_AVAILABLE_DPM
    /** Handler for the "bfdsymsin" input.*/
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


    void finishedHandler(const bool& in)
    {
#ifndef NDEBUG
        if (is_trace_symbol_events_enabled) {
            std::cerr << getpid() << " " << "ENTERED ResolveSymbols::finishedHandler" << std::endl;
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

	    if(!foundit) {
#ifndef NDEBUG
        	if (is_debug_symbol_events_enabled) {
		    std::cerr << getpid() << " "
		        << "ResolveSymbols::finishedHandler: CANNOT RESOLVE symbols for address "
			<< aci->first  << std::endl;
		}
#endif
	    }
	}

	// Now cycle through these symboltables and find functions and statements.
	SymtabAPISymbols stapi_symbols;

	for(SymbolTableMap::iterator ii = symtabmap.begin(); ii != symtabmap.end(); ++ii) {
	    LinkedObjectEntry le = ii->second.second;
	    SymbolTable st = ii->first;
#ifndef NDEBUG
	    std::stringstream output;
            if (is_debug_symbol_events_enabled) {
	        output << "ResolveSymbols::finishedHandler: resolve symbols for " << le.path
			<< std::endl;
	    }
#endif

	    stapi_symbols.getSymbols(abuffer,le,st);
	    AddressRange stRange = st.getAddressRange();
	    FunctionMap stFuncs = st.getFunctions();

#ifndef NDEBUG
            if (is_debug_symbol_events_enabled) {
	        output << "ResolveSymbols::finishedHandler: num functions:" << stFuncs.size()
	        << " stRange:" << stRange << std::endl;
	    }
#endif

	    ThreadAddrBufMap::const_iterator avi;
	    AddressCounts::const_iterator aci;
	    for (avi = threadAddrBufMap.begin(); avi != threadAddrBufMap.end(); ++avi) {
#ifndef NDEBUG
		if (is_debug_symbol_events_enabled) {
		    output << "ResolveSymbols::finishedHandler: thread:" << (*avi).first
		    << " buffer size " << (*avi).second.addresscounts.size()
		    << std::endl;
		}
#endif

		AddressCounts ac = (*avi).second.addresscounts;
		for(FunctionMap::const_iterator fi = stFuncs.begin(); fi != stFuncs.end(); ++fi) {
		    AddressRange frange(fi->first.getBegin(),fi->first.getEnd());
	 	    for (aci=ac.equal_range(frange.getBegin()).first;
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
#ifndef NDEBUG
	    if (is_debug_symbol_events_enabled) {
	        std::cerr << output.str();
	    }
#endif
	}

	FuncStatsVec::iterator fit;
	std::set<std::string> functions;
	FunctionAvgMap functionscounts;
	for(fit = fstatvec.begin(); fit != fstatvec.end(); ++fit) {
#ifndef NDEBUG
	    if (is_debug_symbol_events_enabled) {
	        std::cerr << "FuncStatsVec: function:" << (*fit).funcname
		<< " thread:" << (*fit).tname
		<< " count:" << (*fit).value
		<< std::endl;
	    }
#endif
	    functions.insert((*fit).funcname);
	    FunctionAvgMap::iterator it;
	    it = functionscounts.find(std::string((*fit).funcname));
	    if ( it == functionscounts.end() ) {
		functionscounts.insert(std::make_pair(std::string((*fit).funcname),
				       std::make_pair((*fit).value,1)));
#ifndef NDEBUG
		if (is_debug_symbol_events_enabled) {
		std::cerr << "FuncStatsVec: function:" << (*fit).funcname
		<< " total counts:" << (*fit).value << " num threads:" << 1
		<< " avg:" << (*fit).value << std::endl;
		}
#endif
	    } else {
		(*it).second.first += (*fit).value;
		++(*it).second.second;
#ifndef NDEBUG
		if (is_debug_symbol_events_enabled) {
		std::cerr << "FuncStatsVec: function:" << (*fit).funcname
		<< " UPDATE total counts:" << (*it).second.first << " num threads:" << (*it).second.second
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
	    FunctionAvgMap::iterator it;
	    int i = 0;
	    for(it = functionscounts.begin(); it != functionscounts.end(); ++it) {
#ifndef NDEBUG
		if (is_debug_symbol_events_enabled) {
		std::cerr << "FuncAvgMap: function:" << (*it).first
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
	for (f = functions.begin(); f != functions.end(); ++f) {
	    for(fit = fstatvec.begin(); fit != fstatvec.end(); ++fit) {
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
	FunctionThreadCount::iterator mfi;
	int i = 0;
	for(mfi = maxfuncs.begin(); mfi != maxfuncs.end(); ++mfi) {
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
	for(mfi = minfuncs.begin(); mfi != minfuncs.end(); ++mfi) {
	   CBTF_Protocol_FunctionThreadValue entry;
	   entry.function = strdup((*mfi).first.c_str());
	   entry.thread = (*mfi).second.first;
	   entry.value = (*mfi).second.second;
	   minVals.values.values_val[i] = entry;
	   ++i;
	}

	boost::shared_ptr<CBTF_Protocol_FunctionThreadValues> minvals_xdr =
                boost::make_shared<CBTF_Protocol_FunctionThreadValues>(minVals);

	//std::cerr << "EMIT maxfunctionvalues_xdr_out" << std::endl;
	emitOutput<boost::shared_ptr<CBTF_Protocol_FunctionThreadValues> >("maxfunctionvalues_xdr_out",maxvals_xdr);
	//std::cerr << "EMIT minfunctionvalues_xdr_out" << std::endl;
	emitOutput<boost::shared_ptr<CBTF_Protocol_FunctionThreadValues> >("minfunctionvalues_xdr_out",minvals_xdr);
	//std::cerr << "EMIT avgfunctionvalues_xdr_out" << std::endl;
	emitOutput<boost::shared_ptr<CBTF_Protocol_FunctionAvgValues> >("avgfunctionvalues_xdr_out",avgvals_xdr);
	
	//emitOutput<SymbolTableMap>("symbtabmapout",symtabmap);
	
    }
    
    SymbolTableMap symtabmap;
    LinkedObjectEntryVec linkedobjectvec;
    AddressBuffer abuffer;
    FuncStatsVec fstatvec;
    ThreadAddrBufMap threadAddrBufMap;
    FunctionThreadCount maxvals;
    FunctionThreadCount minvals;
    FunctionAvgMap avgvals;

}; // class ResolveSymbols

KRELL_INSTITUTE_CBTF_REGISTER_FACTORY_FUNCTION(ResolveSymbols)
