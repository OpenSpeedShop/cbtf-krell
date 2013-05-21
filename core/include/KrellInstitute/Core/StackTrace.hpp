////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2012-2013 The Krell Institute. All Rights Reserved.
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

/** @file
 *
 * Declaration of the StackTrace class.
 *
 */

#ifndef _StacktraceData_
#define _StacktraceData_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "KrellInstitute/Core/Address.hpp"
#include "KrellInstitute/Core/AddressBuffer.hpp"
#include "KrellInstitute/Core/ThreadName.hpp"
#include "KrellInstitute/Core/Time.hpp"

#include <set>
#include <vector>
#include <utility>



namespace KrellInstitute { namespace Core {

    /**
     * Stack trace.
     *
     * Ordered list of addresses representing the function call sequence at a
     * particular moment in a thread's execution. The first (zero index) entry
     * in the list is the address of the currently executing instruction. The
     * last (highest index) entry is in the start routine (e.g. "_start") of
     * the thread. Entries in between contain caller addresses representing
     * the sequence of function calls that led to the current instruction.
     *
     * @sa    http://en.wikipedia.org/wiki/Stack_trace
     *
     * @ingroup CollectorAPI ToolAPI
     */
    class StackTrace :
	public std::vector<Address>
    {

    public:

	/** Constructor from thread and time. */
	StackTrace(const ThreadName& thread, const Time& time) :
	    std::vector<Address>(),
	    dm_thread(thread),
	    dm_time(time)
	{
	}

	StackTrace() :
	    std::vector<Address>(),
	    dm_thread(NULL),
	    dm_time(Time::TheBeginning())
	{
	}

	/** Read-only data member accessor function. */
	ThreadName getThread() const
	{
	    return dm_thread;
	}

	/** Read-only data member accessor function. */
	Time getTime() const
	{
	    return dm_time;
	}


    private:

	/** Thread in which this stack trace was recorded. */
	ThreadName dm_thread;
	
	/** Time at which this stack trace was recorded. */
	Time dm_time;
	
    };
    
} }



#endif
