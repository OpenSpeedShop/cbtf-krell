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


    typedef std::vector<LinkedObjectEntry > LinkedObjectEntryVec;
    LinkedObjectEntryVec linkedobjectvec;

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
        declareInput<boost::shared_ptr<CBTF_Protocol_LoadedLinkedObject> >(
            "loaded", boost::bind(&LinkedObject::loadedHandler, this, _1)
            );
        declareInput<boost::shared_ptr<CBTF_Protocol_UnloadedLinkedObject> >(
            "unloaded", boost::bind(&LinkedObject::unloadedHandler, this, _1)
            );
    }

    /** Handlers for the inputs.*/
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

	    linkedobjectvec.push_back(entry);

// used to show the linkedobject information sent.
#if 0
	    std::cerr << "path " << entry.path
	    << " loaded at time " << entry.time_loaded
	    << " at " << AddressRange(entry.addr_begin,entry.addr_end)
	    << " in thread " << entry.tname.getHost()
	    << ":" << entry.tname.getPid().second
	    << ":" <<  entry.tname.getPosixThreadId().second
	    << std::endl;
#endif
	}
    }

    void unloadedHandler(const boost::shared_ptr<CBTF_Protocol_UnloadedLinkedObject>& in)
    {
    }

}; // class LinkedObject

KRELL_INSTITUTE_CBTF_REGISTER_FACTORY_FUNCTION(LinkedObject)
