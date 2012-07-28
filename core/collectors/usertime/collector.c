/*******************************************************************************
** Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
** Copyright (c) 2007,2008 William Hachfeld. All Rights Reserved.
** Copyright (c) 2007-2012 Krell Institute.  All Rights Reserved.
**
** This library is free software; you can redistribute it and/or modify it under
** the terms of the GNU Lesser General Public License as published by the Free
** Software Foundation; either version 2.1 of the License, or (at your option)
** any later version.
**
** This library is distributed in the hope that it will be useful, but WITHOUT
** ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
** FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
** details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this library; if not, write to the Free Software Foundation, Inc.,
** 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*******************************************************************************/

/** @file
 *
 * Declaration and definition of the UserTime collector's runtime.
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

#include "KrellInstitute/Messages/DataHeader.h"
#include "KrellInstitute/Messages/Usertime.h"
#include "KrellInstitute/Messages/Usertime_data.h"
#include "KrellInstitute/Messages/ToolMessageTags.h"
#include "KrellInstitute/Messages/Thread.h"
#include "KrellInstitute/Messages/ThreadEvents.h"
#include "KrellInstitute/Services/Assert.h"
#include "KrellInstitute/Services/Collector.h"
#include "KrellInstitute/Services/Common.h"
#include "KrellInstitute/Services/Context.h"
#include "KrellInstitute/Services/Data.h"
#include "KrellInstitute/Services/Time.h"
#include "KrellInstitute/Services/Timer.h"
#include "KrellInstitute/Services/Unwind.h"
#include "KrellInstitute/Services/TLS.h"

#define true 1
#define false 0

#if !defined (TRUE)
#define TRUE true
#endif

#if !defined (FALSE)
#define FALSE false
#endif

/** String uniquely identifying this collector. */
const char* const cbtf_collector_unique_id = "usertime";


/** Type defining the items stored in thread-local storage. */
typedef struct {

    CBTF_DataHeader header;  /**< Header for following data blob. */
    CBTF_usertime_data data;        /**< Actual data blob. */

    CBTF_StackTraceData buffer;      /**< Stacktrace sampling data buffer. */

    bool_t defer_sampling;
} TLS;

#if defined(USE_EXPLICIT_TLS)

/**
 * Key used to look up our thread-local storage. This key <em>must</em> be
 * unique from any other key used by any of the CBTF services.
 */
static const uint32_t TLSKey = 0x00001EF4;

#else

/** Thread-local storage. */
static __thread TLS the_tls;

#endif

static void send_samples ()
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif

    Assert(tls != NULL);

    tls->header.id = strdup("usertime");
    tls->header.time_end = CBTF_GetTime();
    tls->header.addr_begin = tls->buffer.addr_begin;
    tls->header.addr_end = tls->buffer.addr_end;

#ifndef NDEBUG
	if (getenv("CBTF_DEBUG_COLLECTOR") != NULL) {
	    fprintf(stderr, "cbtf_timer_service_stop_sampling:\n");
	    fprintf(stderr, "time_end(%#lu) addr range[%#lx, %#lx] stacktraces_len(%d) count_len(%d)\n",
		tls->header.time_end,tls->header.addr_begin,
		tls->header.addr_end,tls->data.stacktraces.stacktraces_len,
		tls->data.count.count_len);
	}
#endif

    cbtf_collector_send(&tls->header, (xdrproc_t)xdr_CBTF_usertime_data, &tls->data);

    /* Re-initialize the data blob's header */
    tls->header.time_begin = tls->header.time_end;
    tls->header.time_end = 0;
    tls->header.addr_begin = ~0;
    tls->header.addr_end = 0;

    /* Re-initialize the actual data blob */
    tls->data.stacktraces.stacktraces_len = 0;
    tls->data.count.count_len = 0;

    /* Re-initialize the sampling buffer */
    memset(tls->buffer.bt, 0, sizeof(tls->buffer.bt));
    memset(tls->buffer.count, 0, sizeof(tls->buffer.count));
}


/**
 * Timer event handler.
 *
 * Called by the timer handler each time a sample is to be taken. Extracts a
 * stacktrace from the signal context and places it into the
 * sample buffer. When the sample buffer is full, it is sent.
 *
 * @note    
 * 
 * @param context    Thread context at timer interrupt.
 */
static void serviceTimerHandler(const ucontext_t* context)
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);

    if(tls->defer_sampling == TRUE) {
        return;
    }

    int framecount = 0;
    int stackindex = 0;
    uint64_t framebuf[CBTF_ST_MAXFRAMES];

    memset(framebuf,0, sizeof(framebuf));

    /* get stack address for current context and store them into framebuf. */

    CBTF_GetStackTraceFromContext (context, TRUE, 0,
                        CBTF_ST_MAXFRAMES /* maxframes*/, &framecount, framebuf) ;

    bool_t stack_already_exists = FALSE;

    int i, j;
    /* search individual stacks via count/indexing array */
    for (i = 0; i < tls->data.count.count_len ; i++ )
    {
	/* a count > 0 indexes the top of stack in the data buffer. */
	/* a count == 255 indicates this stack is at the count limit. */
	if (tls->buffer.count[i] == 0) {
	    continue;
	}
	if (tls->buffer.count[i] == 255) {
	    continue;
	}

	/* see if the stack addresses match */
	for (j = 0; j < framecount ; j++ )
	{
	    if ( tls->buffer.bt[i+j] != framebuf[j] ) {
		   break;
	    }
	}

	if ( j == framecount) {
	    stack_already_exists = TRUE;
	    stackindex = i;
	}
    }

    /* if the stack already exisits in the buffer, update its count
     * and return. If the stack is already at the count limit.
    */
    if (stack_already_exists && tls->buffer.count[stackindex] < 255 ) {
	/* update count for this stack */
	tls->buffer.count[stackindex] = tls->buffer.count[stackindex] + 1;
	return;
    }

    /* sample buffer has no room for these stack frames.*/
    int buflen = tls->data.stacktraces.stacktraces_len + framecount;
    if ( buflen > CBTF_ST_BufferSize) {
	/* send the current sample buffer. (will init a new buffer) */
	send_samples(tls);
    }

    /* add frames to sample buffer, compute addresss range */
    for (i = 0; i < framecount ; i++)
    {
	/* always add address to buffer bt */
	tls->buffer.bt[tls->data.stacktraces.stacktraces_len] = framebuf[i];

	/* top of stack indicated by a positive count. */
	/* all other elements are 0 */
	if (i > 0 ) {
	    tls->buffer.count[tls->data.count.count_len] = 0;
	} else {
	    tls->buffer.count[tls->data.count.count_len] = 1;
	}

	if (framebuf[i] < tls->buffer.addr_begin ) {
	    tls->buffer.addr_begin = framebuf[i];
	}
	if (framebuf[i] > tls->buffer.addr_end ) {
	    tls->buffer.addr_end = framebuf[i];
	}
	tls->data.stacktraces.stacktraces_len++;
	tls->data.count.count_len++;
    }
}


/**
 * Called by the CBTF collector service in order to start data collection.
 */
void cbtf_collector_start(const CBTF_DataHeader* const header)
{
/**
 * Start sampling.
 *
 * Starts user time sampling for the thread executing this function.
 * Initializes the appropriate thread-local data structures and then enables the
 * sampling timer.
 *
 * @param arguments    Encoded function arguments.
 */
    /* Create and access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = malloc(sizeof(TLS));
    Assert(tls != NULL);
    CBTF_SetTLS(TLSKey, tls);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);

    tls->defer_sampling=FALSE;

    /* Decode the passed function arguments */
    // Need to handle the arguments...
    CBTF_usertime_start_sampling_args args;
    memset(&args, 0, sizeof(args));
    args.sampling_rate = 35;
#if 0
    CBTF_DecodeParameters(arguments,
			    (xdrproc_t)xdr_CBTF_usertime_start_sampling_args,
			    &args);
#endif
    
#if defined(CBTF_SERVICE_USE_FILEIO)
    CBTF_SetSendToFile("usertime", "cbtf-data");
#endif

    /* Initialize the actual data blob */
    tls->data.interval = 
	(uint64_t)(1000000000) / (uint64_t)(args.sampling_rate);
    tls->data.stacktraces.stacktraces_val = tls->buffer.bt;
    tls->data.count.count_val = tls->buffer.count;

    /* Initialize the sampling buffer */
    tls->buffer.addr_begin = ~0;
    tls->buffer.addr_end = 0;
    memset(tls->buffer.bt, 0, sizeof(tls->buffer.bt));
    memset(tls->buffer.count, 0, sizeof(tls->buffer.count));
 
    /* Begin sampling */
    memcpy(&tls->header, header, sizeof(CBTF_DataHeader));
    tls->header.time_begin = CBTF_GetTime();
    CBTF_Timer(tls->data.interval, serviceTimerHandler);
}



/**
 * Called by the CBTF collector service in order to pause data collection.
 */
void cbtf_collector_pause()
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    if (tls == NULL)
	return;

    tls->defer_sampling=TRUE;
}



/**
 * Called by the CBTF collector service in order to resume data collection.
 */
void cbtf_collector_resume()
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    if (tls == NULL)
	return;

    tls->defer_sampling=FALSE;
}



/**
 * Called by the CBTF collector service in order to stop data collection.
 */
void cbtf_collector_stop()
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);

    /* Stop sampling */
    CBTF_Timer(0, NULL);

    tls->header.time_end = CBTF_GetTime();

    /* Are there any unsent samples? */
    if(tls->data.stacktraces.stacktraces_len > 0) {
	/* Send these samples */
	send_samples();
    }

    /* Destroy our thread-local storage */
#ifdef CBTF_SERVICE_USE_EXPLICIT_TLS
    free(tls);
    CBTF_SetTLS(TLSKey, NULL);
#endif
}



#if defined (CBTF_SERVICE_USE_OFFLINE)
void cbtf_offline_service_start_timer()
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    if (tls == NULL)
	return;

    CBTF_Timer(tls->data.interval, serviceTimerHandler);
}

void cbtf_offline_service_stop_timer()
{
    CBTF_Timer(0, NULL);
}
#endif

#ifdef USE_EXPLICIT_TLS
void destroy_explicit_tls() {
    TLS* tls = CBTF_GetTLS(TLSKey);
    /* Destroy our thread-local storage */
    if (tls) {
        free(tls);
    }
    CBTF_SetTLS(TLSKey, NULL);
}
#endif
