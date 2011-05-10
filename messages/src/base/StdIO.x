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
 * Specification of the Std IO communication protocol.
 *
 */

%#include "Blob.h"
%#include "Thread.h"


/**
 * Standard error stream.
 *
 * Issued by a backend to indicate that data was received on a created process'
 * standard error stream. Includes the data and the thread that generated that
 * data.
 */
struct CBTF_Protocol_StdErr
{
    /** Thread which generated the data on its standard error stream. */
    CBTF_Protocol_ThreadName thread;

    /** Data that was generated. */
    CBTF_Protocol_Blob data;
};



/**
 * Standard input stream.
 *
 * Issued by the frontend to request that data be written to the standard input
 * stream of a created process. Includes the data and the thread that should
 * receive that data.
 */
struct CBTF_Protocol_StdIn
{
    /** Thread whose standard input stream should be written. */
    CBTF_Protocol_ThreadName thread;

    /** Data to be written. */
    CBTF_Protocol_Blob data;
};



/**
 * Standard output stream.
 *
 * Issued by a backend to indicate that data was received on a created process'
 * standard output stream. Includes the data and the thread that generated that
 * data.
 */
struct CBTF_Protocol_StdOut
{
    /** Thread which generated the data on its standard output stream. */
    CBTF_Protocol_ThreadName thread;

    /** Data that was generated. */
    CBTF_Protocol_Blob data;
};
