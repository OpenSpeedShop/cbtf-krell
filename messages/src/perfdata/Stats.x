/*******************************************************************************
** Copyright (c) 2014 Krell Institute. All Rights Reserved.
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
 * Specification of the Stats communication protocol.
 *
 */

%#include "Blob.h"
%#include "Thread.h"


/**
 * Function, thread, uint64_t value in thread.
 *
 * Describes a single function and a uint64_t value in a specific thread.
 * The value can represent counts or represent total time.
 */
struct CBTF_Protocol_FunctionThreadValue
{
    /** Name of this function. */
    string function<>;
    
    /** thread name. */
    CBTF_Protocol_ThreadName thread;

    /**  counts. */
    uint64_t value;
};

struct CBTF_Protocol_FunctionThreadValues
{
    CBTF_Protocol_FunctionThreadValue values<>;
};

/**
 * Function, uint64_t value in num threads.
 *
 * Describes a single function, a uint64_t value seen in num threads.
 * The value can represent counts or represent total time.
 */
struct CBTF_Protocol_FunctionAvgValue
{
    /** Name of this function. */
    string function<>;
    
    /** total value. */
    uint64_t value;

    /**  number of threads . */
    uint64_t num;
};

struct CBTF_Protocol_FunctionAvgValues
{
    CBTF_Protocol_FunctionAvgValue values<>;
};
