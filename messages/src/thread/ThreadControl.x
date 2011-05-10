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
 * Specification of the Thread Control communication protocol.
 *
 */

%#include "Blob.h"
%#include "Thread.h"



/**
 * Attach to threads.
 *
 * Issued by the frontend to request the specified threads be attached.
 */
struct CBTF_Protocol_AttachToThreads
{
    /** Threads to be attached. */
    CBTF_Protocol_ThreadNameGroup threads;
};



/**
 * Change state of threads.
 *
 * Issued by the frontend to request that the current state of every thread
 * in the specified group be changed to the specified value. Used to, for
 * example, suspend threads that were previously running.
 */
struct CBTF_Protocol_ChangeThreadsState
{
    /** Threads whose state should be changed. */
    CBTF_Protocol_ThreadNameGroup threads;

    /** Change the threads to this state. */
    CBTF_Protocol_ThreadState state;
};



/**
 * Create a thread.
 *
 * Issued by the frontend to request the specified thread be created as a new
 * process to execute the specified command. The command is created with the
 * specified initial environment and the process is created in a suspended
 * state.
 */
struct CBTF_Protocol_CreateProcess
{
    /** Thread to be created (only the host name is actually used). */
    CBTF_Protocol_ThreadName thread;

    /** Command to be executed. */
    string command<>;

    /** Environment in which to execute the command. */
    CBTF_Protocol_Blob environment;
};



/**
 * Detach from threads.
 *
 * Issued by the frontend to request the specified threads be detached.
 */
struct CBTF_Protocol_DetachFromThreads
{
    /** Threads to be detached. */
    CBTF_Protocol_ThreadNameGroup threads;
};
