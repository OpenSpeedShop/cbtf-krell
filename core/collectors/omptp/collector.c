/*******************************************************************************
** Copyright (c) 2015-2016 Krell Institute.  All Rights Reserved.
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
 * Definition of the OMPTP collector's runtime.
 *
 */

/* #define DEBUG 1 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "KrellInstitute/Messages/DataHeader.h"
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
#include "collector.h"
#include "monitor.h"

/** String uniquely identifying this collector. */
const char* const cbtf_collector_unique_id = "omptp";


/** Number of overhead frames in each stack frame to be skipped. */
/** These two frames are the start_event and OpenSSGetStackFromConext */
const unsigned OverheadFrameCount = 2;

/** Maximum number of frames to allow in each stack trace. */
//#define MaxFramesPerStackTrace 48

/** Number of stack trace entries in the tracing buffer. */
/** event.stacktrace buffer is 64*8=512 bytes */
/** allows for 6 unique stacktraces (384*8/512) */
#define StackTraceBufferSize (CBTF_BlobSizeFactor * 384)


/** Number of event entries in the tracing buffer. */
// and the size of a ompt event.  Should find the best fit going forward.
#define EventBufferSize (CBTF_BlobSizeFactor * 200)

extern void CBTF_ompt_set_collector_active(bool);

/** Type defining the items stored in thread-local storage. */
typedef struct {

    /** Nesting depth within the Mem function wrappers. */
    unsigned nesting_depth;

    CBTF_DataHeader header;  /**< Header for following data blob. */
    CBTF_ompt_profile_data data;        /**< Actual data blob. */

    /** profiling buffer.*/
    struct {
	uint64_t stacktraces[StackTraceBufferSize];  /**< Stack traces. */
	uint64_t time[StackTraceBufferSize];  /**< Stack traces. */
	uint8_t count[StackTraceBufferSize];  /**< Stack traces. */
    } buffer;

    int defer_sampling;
    bool do_trace;
    bool in_parallel_region;

} TLS;

#ifndef NDEBUG
static bool IsCollectorDebugEnabled = false;
#endif

#if defined(USE_EXPLICIT_TLS)

/**
 * Key used to look up our thread-local storage. This key <em>must</em> be
 * unique from any other key used by any of the CBTF services.
 */
static const uint32_t TLSKey = 0x00007EFA;
int omptp_init_tls_done = 0;

#else

/** Thread-local storage. */
static __thread TLS the_tls;

#endif

void defer_trace(int defer_tracing) {
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);
    tls->do_trace = defer_tracing;
}


/**
 * Initialize the performance data header and blob contained within the given
 * thread-local storage. This function <em>must</em> be called before any of
 * the collection routines attempts to add a message.
 *
 * @param tls    Thread-local storage to be initialized.
 */
static void initialize_data(TLS* tls)
{
    Assert(tls != NULL);

    tls->nesting_depth = 0;

    tls->header.time_begin = CBTF_GetTime();
    tls->header.time_end = 0;
    tls->header.addr_begin = ~0;
    tls->header.addr_end = 0;
    
    /* Re-initialize the actual data blob */
    tls->data.stacktraces.stacktraces_len = 0;
    tls->data.stacktraces.stacktraces_val = tls->buffer.stacktraces;

    tls->data.count.count_val = tls->buffer.count;
    tls->data.count.count_len = 0;
    tls->data.time.time_val = tls->buffer.time;
    tls->data.time.time_len = 0;

    /* Re-initialize the sampling buffer */
    memset(tls->buffer.stacktraces, 0, sizeof(tls->buffer.stacktraces));
    memset(tls->buffer.count, 0, sizeof(tls->buffer.count));
    memset(tls->buffer.time, 0, sizeof(tls->buffer.time));
}


/**
 * Update the performance data header contained within the given thread-local
 * storage with the specified time. Insures that the time interval defined by
 * time_begin and time_end contain the specified time.
 *
 * @param tls     Thread-local storage to be updated.
 * @param time    Time with which to update.
 */
inline void update_header_with_time(TLS* tls, uint64_t time)
{
    Assert(tls != NULL);

    if (time < tls->header.time_begin) {
        tls->header.time_begin = time;
    }
    if (time >= tls->header.time_end) {
        tls->header.time_end = time + 1;
    }
}



/**
 * Update the performance data header contained within the given thread-local
 * storage with the specified address. Insures that the address range defined
 * by addr_begin and addr_end contain the specified address.
 *
 * @param tls     Thread-local storage to be updated.
 * @param addr    Address with which to update.
 */
inline void update_header_with_address(TLS* tls, uint64_t addr)
{
    Assert(tls != NULL);

    if (addr < tls->header.addr_begin) {
        tls->header.addr_begin = addr;
    }
    if (addr >= tls->header.addr_end) {
        tls->header.addr_end = addr + 1;
    }
}


/**
 * Send events.
 *
 * Sends all the events in the tracing buffer to the framework for storage in 
 * the experiment's database. Then resets the tracing buffer to the empty state.
 * This is done regardless of whether or not the buffer is actually full.
 */
static void send_samples(TLS *tls)
{
    int saved_do_trace = tls->do_trace;
    tls->do_trace = false;

    tls->header.omp_tid = monitor_get_thread_num();
    tls->header.id = strdup(cbtf_collector_unique_id);
    tls->header.time_end = CBTF_GetTime();
    /* rank is not filled until mpi_init finished. safe to set here*/
    tls->header.rank = monitor_mpi_comm_rank();

#ifndef NDEBUG
	if (IsCollectorDebugEnabled) {
	    fprintf(stderr, "[%ld,%d] omptp send_samples: time_range(%lu,%lu) addr range[%lx, %lx] stacktraces_len(%u) events_len(%u)\n",
		tls->header.pid,
		tls->header.omp_tid,
		tls->header.time_begin,tls->header.time_end,
		tls->header.addr_begin,tls->header.addr_end,
		tls->data.stacktraces.stacktraces_len,
		tls->data.count.count_len);
	}
#endif

    cbtf_collector_send(&(tls->header), (xdrproc_t)xdr_CBTF_ompt_profile_data, &(tls->data));

    /* Re-initialize the data blob's header */
    initialize_data(tls);
}

/**
 * Start an event.
 *
 * Called by certain omptp callbacks each time an event is to be started.
 * Initializes the event record and increments the wrappers nesting depth.
 * For omptp we access the callstack when the event is started.
 *
 * @param event    Event to be started.
 */
void omptp_start_event(CBTF_omptp_event* event, uint64_t function, uint64_t* stacktrace, unsigned* stacktrace_size)
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    //Assert(tls != NULL);
    if (tls == NULL) {
#ifndef NDEBUG
    if (IsCollectorDebugEnabled) {
	fprintf(stderr, "[%d,%d] omptp_start_event retuns NO TLS\n",getpid(),monitor_get_thread_num());
    }
#endif
	return;
    }

    int saved_do_trace = tls->do_trace;
    tls->do_trace = false;

    /* Increment the nesting depth */
    ++tls->nesting_depth;

    /* Initialize the event record. */
    memset(event, 0, sizeof(CBTF_omptp_event));

    unsigned st_size = 0;
    CBTF_GetStackTraceFromContext(NULL, FALSE, OverheadFrameCount,
				    MaxFramesPerStackTrace,
				    &st_size, stacktrace);

    *stacktrace_size = st_size;

    if(st_size > 0 && function != 0) {
	stacktrace[0] = function;
    }
    tls->do_trace = saved_do_trace;
}


    
/**
 * Record an event.
 *
 * Called by the omptp callbacks each time an event is to be recorded.
 * Extracts the stack trace from the current thread context and places it, along
 * with the specified event record, into the tracing buffer. When the tracing
 * buffer is full, it is sent to the framework for storage in the experiment's
 * database.
 *
 * @param event       Event to be recorded.
 * @param function    Address of the ompt function for which the event is being
 *                    recorded.
 */
void omptp_record_event(const CBTF_omptp_event* event, uint64_t* stacktrace, unsigned stacktrace_size)
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    //Assert(tls != NULL);
    if (tls == NULL) {
#ifndef NDEBUG
    if (IsCollectorDebugEnabled) {
	fprintf(stderr, "[%d,%d] omptp_record_event retuns NO TLS\n",getpid(),monitor_get_thread_num());
    }
#endif
	return;
    }

#ifndef NDEBUG
    if (IsCollectorDebugEnabled) {
	fprintf(stderr, "[%d,%d] omptp_record_event stacktrace_size:%d\n",getpid(),monitor_get_thread_num(),stacktrace_size);
    }
#endif

    int saved_do_trace = tls->do_trace;
    tls->do_trace = false;

    unsigned entry = 0, start, i;
    bool_t stack_already_exists = FALSE;

    int j;
    int stackindex = 0;
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
	for (j = 0; j < stacktrace_size ; j++ )
	{
	    if ( tls->buffer.stacktraces[i+j] != stacktrace[j] ) {
		   break;
	    }
	}

	if ( j == stacktrace_size) {
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
	tls->buffer.time[stackindex] += event->time;
	// reset do_trace to true.
	tls->do_trace = true;
	return;
    }

    /* sample buffer has no room for these stack frames.*/
    int buflen = tls->data.stacktraces.stacktraces_len + stacktrace_size;
    if ( buflen > StackTraceBufferSize) {
	/* send the current sample buffer. (will init a new buffer) */
	send_samples(tls);
    }

    /* add frames to sample buffer, compute addresss range */
    for (i = 0; i < stacktrace_size ; i++)
    {
	/* always add address to buffer bt */
	tls->buffer.stacktraces[tls->data.stacktraces.stacktraces_len] = stacktrace[i];

	/* top of stack indicated by a positive count. */
	/* all other elements are 0 */
	if (i > 0 ) {
	    tls->buffer.count[tls->data.count.count_len] = 0;
	} else {
	    tls->buffer.count[tls->data.count.count_len] = 1;
	    tls->buffer.time[tls->data.time.time_len] = event->time;
	}

	if (stacktrace[i] < tls->header.addr_begin ) {
	    tls->header.addr_begin = stacktrace[i];
	}
	if (stacktrace[i] > tls->header.addr_end ) {
	    tls->header.addr_end = stacktrace[i];
	}
	tls->data.stacktraces.stacktraces_len++;
	tls->data.count.count_len++;
	tls->data.time.time_len++;
    }

    tls->do_trace = saved_do_trace;
}



/**
 * Called by the CBTF collector service in order to start data collection.
 */
void cbtf_collector_start(const CBTF_DataHeader* const header)
{
/**
 * Start tracing.
 *
 * Starts omptp extended event tracing for the thread executing this function.
 * Initializes the appropriate thread-local data structures.
 *
 * @param arguments    Encoded function arguments.
 */
    /* Create and access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = malloc(sizeof(TLS));
    Assert(tls != NULL);
    CBTF_SetTLS(TLSKey, tls);
    omptp_init_tls_done = 1;
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);

    tls->defer_sampling = 1;
    tls->do_trace = false;
    tls->in_parallel_region = false;

    /* Decode the passed function arguments */
    // Need to handle the arguments...
    CBTF_ompt_start_sampling_args args;
    memset(&args, 0, sizeof(args));
    
    memcpy(&tls->header, header, sizeof(CBTF_DataHeader));

    /* Initialize the actual data blob */
    initialize_data(tls);

    /* Initialize the callstack nesting depth */
    tls->nesting_depth = 0;
 
    /* Begin sampling */
    tls->header.time_begin = CBTF_GetTime();
    tls->defer_sampling = 0;
    tls->do_trace = true;
#ifndef NDEBUG
    IsCollectorDebugEnabled = (getenv("CBTF_DEBUG_COLLECTOR") != NULL);
    if (IsCollectorDebugEnabled) {
	fprintf(stderr, "[%d,%d] cbtf_collector_start SET ACTIVE\n",getpid(),monitor_get_thread_num());
    }
#endif
    CBTF_ompt_set_collector_active(true);
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

    tls->defer_sampling = 1;
    tls->do_trace = false;
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

    tls->defer_sampling = 0;
    tls->do_trace = true;
}


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


/**
 * Stop tracing.
 *
 * Stops ompt event tracing for the thread executing this function.
 * Sends any events remaining in the buffer.
 *
 * @param arguments    Encoded (unused) function arguments.
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

    tls->header.time_end = CBTF_GetTime();
#ifndef NDEBUG
    if (IsCollectorDebugEnabled) {
	fprintf(stderr, "[%d,%d] cbtf_collector_stop SET NOT ACTIVE count:%u stacktraces:%u\n",
	getpid(),monitor_get_thread_num(),tls->data.count.count_len,tls->data.stacktraces.stacktraces_len);
    }
#endif
    CBTF_ompt_set_collector_active(false);

    /* Stop sampling */
    defer_trace(0);

    /* Are there any unsent samples? */
    if(tls->data.count.count_len > 0 || tls->data.stacktraces.stacktraces_len > 0) {
	send_samples(tls);
    }

    /* Destroy our thread-local storage */
#ifdef CBTF_SERVICE_USE_EXPLICIT_TLS
    free(tls);
    CBTF_SetTLS(TLSKey, NULL);
#endif
}

/* 
 * These differ from sampling.  For a profile of idle,barrier,wait_barrier we
 * likely want to record total time here.  ie. get our context and pass
 * the time from the callback in callbacks.c or record the time here.
 * In any case, we will have a callstack to say idle event and time for that idle.
 *
 */
void IDLE(bool flag) {
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    if (tls == NULL)
	return;
//fprintf(stderr,"[%d] IDLE %d\n",monitor_get_thread_num(),flag);

    //tls->thread_idle=flag;
}

void BARRIER(bool flag) {
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    if (tls == NULL)
	return;

//fprintf(stderr,"[%d] BARRIER %d\n",monitor_get_thread_num(),flag);

    //tls->thread_barrier=flag;
}

void WAIT_BARRIER(bool flag) {
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    if (tls == NULL)
	return;

//fprintf(stderr,"[%d] WAIT_BARRIER %d\n",monitor_get_thread_num(),flag);

    //tls->thread_wait_barrier=flag;
}

void omptp_set_in_parallel_region(bool flag)
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    if (tls == NULL)
	return;

    tls->in_parallel_region = flag;
}
