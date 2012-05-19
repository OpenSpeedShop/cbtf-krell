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

/** @file Plugin for Symbols. */

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
#include "KrellInstitute/Core/BFDSymbols.hpp"
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

    SymbolTableMap symtabmap;

    LinkedObjectEntryVec linkedobjectvec;

    AddressBuffer abuffer;
    AddressCounts ac;

}


/**
 * Component that resolves addresses in an AddressBuffer
 * to symbols found in corresponding  linkedobjects.
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
        declareInput<AddressBuffer>(
            "abufferin", boost::bind(&ResolveSymbols::AddressBufferHandler, this, _1)
            );
        declareInput<LinkedObjectEntryVec>(
            "linkedobjectvecin", boost::bind(&ResolveSymbols::LinkedObjectVecHandler, this, _1)
            );
        declareInput<SymtabAPISymbols>(
            "symtabapisymsin", boost::bind(&ResolveSymbols::symtabAPISymbolHandler, this, _1)
            );
        declareInput<BFDSymbols>(
            "bfdsymsin", boost::bind(&ResolveSymbols::BFDSymbolHandler, this, _1)
            );
        declareOutput<SymtabAPISymbols>("symtabapisymsout");
        declareOutput<BFDSymbols>("bfdsymbolsout");
    }

    /** Handler for the "abuffer" input.*/
    void AddressBufferHandler(const AddressBuffer& in)
    {

	abuffer = in;
	ac = in.addresscounts;
    }

    /** Handler for the "linkedobjectvecin" input.*/
    void LinkedObjectVecHandler(const LinkedObjectEntryVec& in)
    {
	linkedobjectvec = in;
    }

    /** Handler for the "symtabapisymsin" input.*/
    void symtabAPISymbolHandler(const SymtabAPISymbols& in)
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
	SymtabAPISymbols stapi_symbols;

	for(SymbolTableMap::iterator i = symtabmap.begin(); i != symtabmap.end(); ++i) {
		LinkedObjectEntry le = i->second.second;
		SymbolTable st = i->first;
		stapi_symbols.getSymbols(abuffer,le,st);
	}
	emitOutput<SymtabAPISymbols>("symtabapisymsout",stapi_symbols);
    }

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
    
}; // class ResolveSymbols

KRELL_INSTITUTE_CBTF_REGISTER_FACTORY_FUNCTION(ResolveSymbols)
