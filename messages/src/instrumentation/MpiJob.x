/*******************************************************************************
** Copyright (c) 2007,2008 William Hachfeld. All Rights Reserved.
** Copyright (c) 2011 Krell Institute. All Rights Reserved.
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
 * Specification of the MPI Job communication protocol.
 *
 */

%#include "Job.h"
%#include "Thread.h"

/**
 * Get value of the MPICH process table from a thread.
 *
 * Issued by the frontend to request the current value of the MPICH process
 * table within the specified thread. Used to obtain this information for
 * the purposes of attaching to an entire MPI job.
 */
struct CBTF_Protocol_GetMPICHProcTable
{
    /** Thread from which the MPICH process table should be retrieved. */
    CBTF_Protocol_ThreadName thread;

    /** Name of global variable whose value is being requested. */
    string global<>;
};



/**
 * Value of a job description variable.
 *
 * Issued by a backend to return the current value of a job description
 * global variable within a thread.
 *
 * @note    Sent only in response to a GetMPICHProcTable request.
 */
struct CBTF_Protocol_GlobalJobValue
{
    /** Thread from which the global variable value was retrieved. */
    CBTF_Protocol_ThreadName thread;

    /** Name of global variable whose value is being returned. */
    string global<>;
    
    /** Boolean "true" if that variable was found, or "false" otherwise. */
    bool found;

    /** Current value of that variable. */
    CBTF_Protocol_Job value;
};



struct CBTF_Protocol_MPIStartup
{
    /** Threads which are in MPI startup. */
    CBTF_Protocol_ThreadNameGroup threads;

    /**
     * Boolean "true" if threads are in MPI startup.
     */
    bool in_mpi_startup;
};
