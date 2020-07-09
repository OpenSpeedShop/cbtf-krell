/*******************************************************************************
** Copyright (c) 2007,2008 William Hachfeld. All Rights Reserved.
** Copyright (c) 2011 The Krell INstitute. All Rights Reserved.
**
** This program is free software; you can redistribute it and/or modify it under
** the terms of the GNU General Public License as published by the Free Software
** Foundation; either version 2 of the License, or (at your option) any later
** version.
**
** This program is distributed in the hope that it will be useful, but WITHOUT
** ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
** FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
** details.
**
** You should have received a copy of the GNU General Public License along with
** this program; if not, write to the Free Software Foundation, Inc., 59 Temple
** Place, Suite 330, Boston, MA  02111-1307  USA
*******************************************************************************/

/** @file
 *
 * Specification of the base Thread communication protocol.
 *
 */

%#include "Blob.h"


/**
 * Thread name.
 *
 * Names a single thread of code execution. To uniquely identify a thread,
 * the host name and process identifier must be specified. A POSIX thread
 * identifier must also be specified if a specific thread in a process is
 * being named.
 */
struct CBTF_Protocol_ThreadName
{
    /** Unique identifier for the experiment containing this thread. */
    int experiment;

    /** Name of the host on which this thread is located. */
    string host<>;

    /** Identifier of the process containing this thread. */
    int64_t pid;

    /**
     * Boolean "true" if this name refers to a specified thread in the
     * process and thus includes a POSIX thread identifier, or "false"
     * if the main thread of the process is being named and the POSIX
     * thread identifier should be considered invalid.
     */
    bool has_posix_tid;

    /** POSIX identifier of this thread. */
    uint64_t posix_tid;

    /** MPI rank identifier. */
    int32_t rank;

    /** OpenMP thread identifier. */
    int32_t omp_tid;
};



/**
 * Arbitrary group of thread names.
 *
 * List holding an arbitrary group of thread names. No specific relationship
 * is implied between the threads named by a given thread name group. Used to
 * provide a way of applying operations to a group of threads as a whole rather
 * than each individually.
 */
struct CBTF_Protocol_ThreadNameGroup
{
    /** Names of the threads in the group. */
    CBTF_Protocol_ThreadName names<>;
};



/**
 * Thread state enumeration.
 *
 * Enumeration defining the state of a thread. This may not enumerate
 * all the possible states in which a thread may find itself on a given
 * system. It contains only those states that are generally of interest
 * to a performance tool.
 */
enum CBTF_Protocol_ThreadState
{
    Disconnected,  /**< Thread isn't connected (may not even exist). */
    Connecting,    /**< Thread is being connected. */
    Nonexistent,   /**< Thread doesn't exist. */
    Running,       /**< Thread is active and running. */
    Suspended,     /**< Thread has been temporarily suspended. */
    Terminated     /**< Thread has terminated. */
};
