////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2007 William Hachfeld. All Rights Reserved.
// Copyright (c) 2011 The Krell Institute All Rights Reserved.
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
 * Declaration of the ThreadName class.
 *
 */

#ifndef _KrellInstitute_Core_ThreadName_
#define _KrellInstitute_Core_ThreadName_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string>
#include <utility>



namespace KrellInstitute { namespace Core {

    class Path;

    /**
     * Repeatable thread name.
     *
     * Container holding those attributes of a thread that are repeatable across
     * multiple instantiations of the thread. Also defines when two such names
     * are to be considered equal. When reruning an experiment this allows the
     * data collectors applied to the new threads to mimic those applied to the
     * original threads.
     *
     * @ingroup Implementation
     */
    class ThreadName
    {

    public:

	ThreadName(const std::string&, const std::string&,
		   const int&, const int&, const int&,
		   const int&, const Path&);

	bool operator==(const ThreadName&) const;

	/** Read-only data member accessor function. */
	const std::pair<bool, std::string>& getCommand() const
	{
	    return dm_command;
	}

	/** Read-only data member accessor function. */
	const std::string& getHost() const
	{
	    return dm_host;
	}

	/** Read-only data member accessor function. */
	const std::pair<bool, int>& getPid() const
	{
	    return dm_pid;
	}

	/** Read-only data member accessor function. */
	const std::pair<bool, int>& getPosixThreadId() const
	{
	    return dm_posixtid;
	}

	/** Read-only data member accessor function. */
	const std::pair<bool, int>& getOpenMPThreadId() const
	{
	    return dm_tid;
	}

	/** Read-only data member accessor function. */
	const std::pair<bool, int>& getMPIRank() const
	{
	    return dm_rank;
	}

	/** Read-only data member accessor function. */
	const std::pair<bool, Path>& getExecutable() const
	{
	    return dm_executable;
	}

    private:

	/** Command used to create this thread. */
	std::pair<bool, std::string> dm_command;

	/** Name of the host on which this thread is located. */
	std::string dm_host;

	/** Process Id identifier of this thread. */
	std::pair<bool, int> dm_pid;

	/** Posix thread Id identifier of this thread. */
	std::pair<bool, int> dm_posixtid;

	/** OpenMP identifier of this thread. */
	std::pair<bool, int> dm_tid;

	/** MPI rank of this thread. */
	std::pair<bool, int> dm_rank;

	/** Path of this thread's executable. */
	std::pair<bool, Path> dm_executable;

    };
    
} }



#endif
