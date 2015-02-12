////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2007 William Hachfeld. All Rights Reserved.
// Copyright (c) 2011 The Krell Institute. All Rights Reserved.
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
 * Definition of the ThreadName class.
 *
 */

#include "KrellInstitute/Core/Path.hpp"
#include "KrellInstitute/Core/ThreadName.hpp"
#include "KrellInstitute/Messages/Thread.h"

#include "unistd.h"

using namespace KrellInstitute::Core;


ThreadName::ThreadName()
{
}

/**
 * Constructor from thread.
 *
 * Constructs a new ThreadName for the specified thread.
 *
 * @param thread    Thread for which to construct a repeatable name.
 */
ThreadName::ThreadName(const std::string& command,
		       const std::string& host,
		       const int64_t& pid,
		       const int64_t& posixtid,
		       const int32_t& rank,
		       const Path& executable
		       ) :
    dm_command(true, command),
    dm_host(host),
    dm_pid(pid),
    dm_posixtid(true, posixtid),
    dm_rank(rank),
    dm_omptid(-1),
    dm_executable(true, executable)
{
}

ThreadName::ThreadName(const std::string& command,
		       const std::string& host,
		       const int64_t& pid,
		       const int64_t& posixtid,
		       const int32_t& rank,
		       const int32_t& omptid,
		       const Path& executable
		       ) :
    dm_command(true, command),
    dm_host(host),
    dm_pid(pid),
    dm_posixtid(true, posixtid),
    dm_rank(rank),
    dm_omptid(omptid),
    dm_executable(true, executable)
{
}

ThreadName::ThreadName(const std::string& host,
		       const int64_t& pid,
		       const int64_t& posixtid,
		       const int32_t& rank
		       ) :
    dm_command(false,"empty command"),
    dm_host(host),
    dm_pid(pid),
    dm_posixtid(true, posixtid),
    dm_rank(rank),
    dm_omptid(-1),
    dm_executable(false,"no executable provided")
{
}

ThreadName::ThreadName(const std::string& host,
		       const int64_t& pid,
		       const int64_t& posixtid,
		       const int32_t& rank,
		       const int32_t& omptid
		       ) :
    dm_command(false,"empty command"),
    dm_host(host),
    dm_pid(pid),
    dm_posixtid(true, posixtid),
    dm_rank(rank),
    dm_omptid(omptid),
    dm_executable(false,"no executable provided")
{
}

ThreadName::ThreadName(const CBTF_Protocol_ThreadName& object) :
    dm_command(false,"empty command"),
    dm_host(object.host),
    dm_pid(object.pid),
    dm_posixtid(object.has_posix_tid, object.posix_tid),
    dm_rank(object.rank),
    dm_omptid(object.omp_tid),
    dm_executable(false,"no executable provided")
{
}


/**
 * Equality operator.
 *
 * Operator "==" defined for two ThreadName objects. Allows data collectors
 * applied to the new threads to mimic those applied to the original threads.
 *
 * @param other    Thread name to compare against.
 * @return         Boolean "true" if we are equal to the other entry, "false"
 *                 otherwise.
 */
bool ThreadName::operator==(const ThreadName& other) const
{

    if (dm_host == other.dm_host &&
      dm_pid == other.dm_pid &&
      dm_rank == other.dm_rank &&
      dm_omptid == other.dm_omptid &&
      dm_posixtid.first == other.dm_posixtid.first &&
      dm_posixtid.second == other.dm_posixtid.second) {
      return true;
    }

    // Otherwise the two names are not equal
    return false;
}
