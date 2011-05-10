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
 * Specification of the Thread events communication protocol.
 *
 */

%#include "Blob.h"
%#include "Thread.h"



/**
 * Attached to threads.
 *
 * Issued by a backend to indicate the specified threads were attached.
 *
 * @note    This message is always sent in response to an AttachToThreads
 *          request. But it also sent by a backend when a thread has been
 *          automatically attached, e.g. to a fork(), pthread_create(),
 *          etc.
 */
struct CBTF_Protocol_AttachedToThreads
{
    /** Threads that were attached. */
    CBTF_Protocol_ThreadNameGroup threads;
};



/**
 * Created a process.
 *
 * Issued by a backend to indicate the specified process was created.
 *
 * @note    Sent only in response to a CreateProcess request.
 */
struct CBTF_Protocol_CreatedProcess
{
    /** Original thread as specfied in the CreateProcess request. */
    CBTF_Protocol_ThreadName original_thread;

    /** Process that was created. */
    CBTF_Protocol_ThreadName created_thread;
};



/**
 * Thread's state has changed.
 *
 * Issued by a backend to indicate that the current state of every thread
 * in the specified group has changed to the specified value.
 *
 * @note    While this message is sent in response to a ChangeThreadState
 *          request, it is also sent by a backend when thread states change
 *          asynchronously due to thread termination, etc.
 */
struct CBTF_Protocol_ThreadsStateChanged
{
    /** Threads whose state has changed. */
    CBTF_Protocol_ThreadNameGroup threads;

    /** State to which these threads have changed. */
    CBTF_Protocol_ThreadState state;
};
