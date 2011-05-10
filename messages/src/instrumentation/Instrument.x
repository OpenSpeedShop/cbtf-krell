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
 * Specification of the Instrumentation communication protocol.
 *
 */

%#include "Blob.h"
%#include "Collector.h"
%#include "Thread.h"



/**
 * Execute a library function now.
 *
 * Issued by the frontend to request the immediate execution of the specified
 * library function in the specified threads. Used by collectors to execute
 * functions in their runtime library.
 */
struct CBTF_Protocol_ExecuteNow
{
    /** Threads in which the function should be executed. */
    CBTF_Protocol_ThreadNameGroup threads;

    /** Collector requesting the execution. */
    CBTF_Protocol_Collector collector;
    
    /**
     * Boolean "true" if the floating-point registers should NOT be saved before
     * executing the library function, or "false" if they should be saved.
     */
    bool disable_save_fpr;

    /** Name of the library function to be executed. */
    string callee<>;

    /** Blob argument to the function. */
    CBTF_Protocol_Blob argument;
};



/**
 * Execute a library function at another function's entry or exit.
 *
 * Issued by the frontend to request the execution of the specified library
 * function every time another function's entry or exit is executed in the
 * specified threads. Used by collectors to execute functions in their
 * runtime library.
 */
struct CBTF_Protocol_ExecuteAtEntryOrExit
{
    /** Threads in which the function should be executed. */
    CBTF_Protocol_ThreadNameGroup threads;

    /** Collector requesting the execution. */
    CBTF_Protocol_Collector collector;

    /**
     * Name of the function at whose entry/exit
     * the library function should be executed.
     */
    string where<>;

    /**
     * Boolean "true" if instrumenting function's entry
     * point, or "false" if function's exit point.
     */
    bool at_entry;

    /** Name of the library function to be executed. */
    string callee<>;

    /** Blob argument to the function. */
    CBTF_Protocol_Blob argument;
};



/**
 * Execute a library function in place of another function.
 *
 * Issued by the frontend to request the execution of the specified library
 * function in place of another function every other time that other function
 * is called. Used by collectors to create wrappers around functions for the
 * purposes of gathering performance data on their execution.
 */
struct CBTF_Protocol_ExecuteInPlaceOf
{
    /** Threads in which the function should be executed. */
    CBTF_Protocol_ThreadNameGroup threads;

    /** Collector requesting the execution. */
    CBTF_Protocol_Collector collector;

    /** Name of the function to be replaced with the library function. */
    string where<>;

    /** Name of the library function to be executed. */
    string callee<>;
};



/**
 * Get value of an integer global variable from a thread.
 *
 * Issued by the frontend to request the current value of a signed integer
 * global variable within the specified thread. Used to extract certain
 * types of data, such as MPI job identifiers, from a process.
 */
struct CBTF_Protocol_GetGlobalInteger
{
    /** Thread from which the global variable value should be retrieved. */
    CBTF_Protocol_ThreadName thread;

    /** Name of global variable whose value is being requested. */
    string global<>;
};



/**
 * Get value of a string global variable from a thread.
 *
 * Issued by the frontend to request the current value of a character string
 * global variable within the specified thread. Used to extract certain types
 * of data, such as MPI job identifiers, from a process.
 */
struct CBTF_Protocol_GetGlobalString
{
    /** Thread from which the global variable value should be retrieved. */
    CBTF_Protocol_ThreadName thread;

    /** Name of global variable whose value is being requested. */
    string global<>;
};



/**
 * Value of an integer global variable.
 *
 * Issued by a backend to return the current value of a signed integer
 * global variable within a thread.
 *
 * @note    Sent only in response to a GetGlobalInteger request.
 */
struct CBTF_Protocol_GlobalIntegerValue
{
    /** Thread from which the global variable value was retrieved. */
    CBTF_Protocol_ThreadName thread;

    /** Name of global variable whose value is being returned. */
    string global<>;

    /** Boolean "true" if that variable was found, or "false" otherwise. */
    bool found;

    /** Current value of that variable. */
    int64_t value;
};



/**
 * Value of a string global variable.
 *
 * Issued by a backend to return the current value of a character string
 * global variable within a thread.
 *
 * @note    Sent only in response to a GetGlobalString request.
 */
struct CBTF_Protocol_GlobalStringValue
{
    /** Thread from which the global variable value was retrieved. */
    CBTF_Protocol_ThreadName thread;

    /** Name of global variable whose value is being returned. */
    string global<>;

    /** Boolean "true" if that variable was found, or "false" otherwise. */
    bool found;

    /** Current value of that variable. */
    string value<>;
};



/**
 * Threads have been instrumented.
 *
 * Issued by the backend to indicate that the specified collector has placed
 * instrumentation in the specified threads.
 *
 * @note    Sent only when the backend automatically applies instrumentation
 *          to newly created threads. Not to report completion of frontend-
 *          requested instrumentation.
 */
struct CBTF_Protocol_Instrumented
{
    /** Threads to which instrumentation was applied. */
    CBTF_Protocol_ThreadNameGroup threads;

    /** Collector which is instrumenting. */
    CBTF_Protocol_Collector collector;
};



/**
 * Set value of an integer global variable in a thread.
 *
 * Issued by the frontend to request a change to the current value of a signed
 * integer global variable within the specified thread. Used to modify certain
 * values, such as MPI debug gates, in a process.
 */
struct CBTF_Protocol_SetGlobalInteger
{
    /** Thread in which the global variable value should be set. */
    CBTF_Protocol_ThreadName thread;

    /** Name of global variable whose value is being set. */
    string global<>;

    /** New value of that variable. */
    int64_t value;
};



/**
 * Stop at a function's entry or exit.
 *
 * Issued by the frontend to request a stop every time the specified function's
 * entry or exit is executed in the specified threads. Used by the framework
 * to implement MPI job creation.
 */
struct CBTF_Protocol_StopAtEntryOrExit
{
    /** Threads which should be stopped. */
    CBTF_Protocol_ThreadNameGroup threads;

    /** Name of the function at whose entry/exit the stop should occur. */
    string where<>;

    /**
     * Boolean "true" if instrumenting function's entry
     * point, or "false" if function's exit point.
     */
    bool at_entry;
};



/**
 * Remove instrumentation from threads.
 *
 * Issued by the frontend to request the removal of all instrumentation
 * associated with the specified collector from the specified threads. Used
 * by collectors to indicate when they are done using any instrumentation
 * they have placed in threads.
 */
struct CBTF_Protocol_Uninstrument
{
    /** Threads from which instrumentation should be removed. */
    CBTF_Protocol_ThreadNameGroup threads;

    /** Collector which is removing instrumentation. */
    CBTF_Protocol_Collector collector;
};
