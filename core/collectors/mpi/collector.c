/*******************************************************************************
** Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
** Copyright (c) 2007,2008 William Hachfeld. All Rights Reserved.
** Copyright (c) 2006-2014 Krell Institute.  All Rights Reserved.
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
 * Definition of the MPI event tracing collector's runtime.
 *
 */

/* #define DEBUG 1 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "KrellInstitute/Messages/DataHeader.h"
#include "KrellInstitute/Messages/Mpi.h"
#include "KrellInstitute/Messages/Mpi_data.h"
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
#include "MPITraceableFunctions.h"
#include "monitor.h"


/** String uniquely identifying this collector. */
#if defined(PROFILE)
const char* const cbtf_collector_unique_id = "mpip";
#else
#if defined(EXTENDEDTRACE)
const char* const cbtf_collector_unique_id = "mpit";
#else
const char* const cbtf_collector_unique_id = "mpi";
#endif
#endif


/** Number of overhead frames in each stack frame to be skipped. */
#if defined(PROFILE)
const unsigned OverheadFrameCount = 2;
#else
#if defined(CBTF_SERVICE_USE_OFFLINE)
//const unsigned OverheadFrameCount = 1;
const unsigned OverheadFrameCount = 2;
#else
#if defined(__linux) && defined(__ia64)
const unsigned OverheadFrameCount = 2 /*3*/;
#else
const unsigned OverheadFrameCount = 2;
#endif
#endif
#endif

/*
 * NOTE: For some reason GCC doesn't like it when the following three macros are
 *       replaced with constant unsigned integers. It complains about the arrays
 *       in the tls structure being "variable-size type declared outside of any
 *       function" even though the size IS constant... Maybe this can be fixed?
 */

#if defined(PROFILE)
#define MaxFramesPerStackTrace 100
#else
/** Maximum number of frames to allow in each stack trace. */
/* what is a reasonable default here. 32? */
#define MaxFramesPerStackTrace 64
#endif

#if defined(PROFILE)
#define StackTraceBufferSize (CBTF_BlobSizeFactor * 512)
#else
/** Number of stack trace entries in the tracing buffer. */
/** event.stacktrace buffer is 64*8=512 bytes */
/** allows for 6 unique stacktraces (384*8/512) */
#define StackTraceBufferSize (CBTF_BlobSizeFactor * 384)
#endif


/** Number of event entries in the tracing buffer. */
/** CBTF_mpi_event is 32 bytes */
#if defined(EXTENDEDTRACE)
#define EventBufferSize (CBTF_BlobSizeFactor * 215)
#elif !defined(PROFILE)
#define EventBufferSize (CBTF_BlobSizeFactor * 415)
#endif

/** Type defining the items stored in thread-local storage. */
typedef struct {

    CBTF_DataHeader header;  /**< Header for following data blob. */
#if defined(PROFILE)
    CBTF_mpi_profile_data data;
#else
#if defined(EXTENDEDTRACE)
    CBTF_mpi_exttrace_data data;              /**< Actual data blob. */
#else
    CBTF_mpi_trace_data data;              /**< Actual data blob. */
#endif
#endif

    /** Tracing buffer. */
#if defined(PROFILE)
    struct {
        uint64_t stacktraces[StackTraceBufferSize];  /**< Stack traces. */
        uint64_t time[StackTraceBufferSize];  /**< Stack traces. */
        uint8_t count[StackTraceBufferSize];  /**< Stack traces. */
    } buffer;
#else
    struct {
        uint64_t stacktraces[StackTraceBufferSize]; /**< Stack traces. */
#if defined(EXTENDEDTRACE)
        CBTF_mpit_event events[EventBufferSize];    /**< MPI call events with details. */
#else
        CBTF_mpi_event events[EventBufferSize];     /**< MPI call events. */
#endif
    } buffer;
#endif
    
#if defined (CBTF_SERVICE_USE_OFFLINE)
    char CBTF_mpi_traced[PATH_MAX];
#endif
    
    /** Nesting depth within the MPI function wrappers. */
    unsigned nesting_depth;
    bool_t do_trace;
    bool_t defer_sampling;
} TLS;

#if defined(USE_EXPLICIT_TLS)

/**
 * Key used to look up our thread-local storage. This key <em>must</em> be
 * unique from any other key used by any of the CBTF services.
 */
#if defined(PROFILE)
static const uint32_t TLSKey = 0x00001EFD;
#else
#if defined(EXTENDEDTRACE)
static const uint32_t TLSKey = 0x00001EFC;
#else
static const uint32_t TLSKey = 0x00001EFB;
#endif
#endif

int mpi_init_tls_done = 0;

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
    tls->data.stacktraces.stacktraces_val = tls->buffer.stacktraces;
    tls->data.stacktraces.stacktraces_len = 0;
#if defined(PROFILE)
    tls->data.count.count_val = tls->buffer.count;
    tls->data.count.count_len = 0;
    tls->data.time.time_val = tls->buffer.time;
    tls->data.time.time_len = 0;
#else
    tls->data.events.events_len = 0;
    tls->data.events.events_val = tls->buffer.events;
#endif

    /* Re-initialize the sampling buffer */
    memset(tls->buffer.stacktraces, 0, sizeof(tls->buffer.stacktraces));
#if defined(PROFILE)
    memset(tls->buffer.count, 0, sizeof(tls->buffer.count));
    memset(tls->buffer.time, 0, sizeof(tls->buffer.time));
#else
    memset(tls->buffer.events, 0, sizeof(tls->buffer.events));
#endif
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

    if (time < tls->header.time_begin)
    {
        tls->header.time_begin = time;
    }
    if (time >= tls->header.time_end)
    {
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

    if (addr < tls->header.addr_begin)
    {
        tls->header.addr_begin = addr;
    }
    if (addr >= tls->header.addr_end)
    {
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
    Assert(tls != NULL);

    tls->header.id = strdup(cbtf_collector_unique_id);
    tls->header.time_end = CBTF_GetTime();
    tls->header.rank = monitor_mpi_comm_rank();

#ifndef NDEBUG
	if (getenv("CBTF_DEBUG_COLLECTOR") != NULL) {
	    fprintf(stderr, "mpi send_samples:\n");
	    fprintf(stderr, "time_range(%#lu,%#lu) addr range[%#lx, %#lx] stacktraces_len(%u) events_len(%u)\n",
		tls->header.time_begin,tls->header.time_end,
		tls->header.addr_begin,tls->header.addr_end,
#if defined(PROFILE)
		tls->data.stacktraces.stacktraces_len
#else
		tls->data.stacktraces.stacktraces_len,
		tls->data.events.events_len
#endif
	    );
	}
#endif

#if defined(PROFILE)
    cbtf_collector_send(&(tls->header), (xdrproc_t)xdr_CBTF_mpi_profile_data, &(tls->data));
#else
#if defined(EXTENDEDTRACE)
    cbtf_collector_send(&(tls->header), (xdrproc_t)xdr_CBTF_mpi_exttrace_data, &(tls->data));
#else
    cbtf_collector_send(&(tls->header), (xdrproc_t)xdr_CBTF_mpi_trace_data, &(tls->data));
#endif
#endif

    /* Re-initialize the data blob's header */
    initialize_data(tls);
}

/**
 * Start an event.
 *
 * Called by the MPI function wrappers each time an event is to be started.
 * Initializes the event record and increments the wrappers nesting depth.
 *
 * @param event    Event to be started.
 */
#if defined(PROFILE)
void mpi_start_event(CBTF_mpip_event* event)
#else
#if defined(EXTENDEDTRACE)
void mpi_start_event(CBTF_mpit_event* event)
#else
void mpi_start_event(CBTF_mpi_event* event)
#endif
#endif
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);

    /* Increment the MPI function wrapper nesting depth */
    ++tls->nesting_depth;

    /* Initialize the event record. */

#if defined(PROFILE)
    memset(event, 0, sizeof(CBTF_mpip_event));
#else
#if defined(EXTENDEDTRACE)
    memset(event, 0, sizeof(CBTF_mpit_event));
#else
    memset(event, 0, sizeof(CBTF_mpi_event));
#endif
#endif
}


    
/**
 * Record an event.
 *
 * Called by the MPI function wrappers each time an event is to be recorded.
 * Extracts the stack trace from the current thread context and places it, along
 * with the specified event record, into the tracing buffer. When the tracing
 * buffer is full, it is sent to the framework for storage in the experiment's
 * database.
 *
 * @param event       Event to be recorded.
 * @param function    Address of the MPI function for which the event is being
 *                    recorded.
 */
#if defined(PROFILE)
void mpi_record_event(const CBTF_mpip_event* event, uint64_t function)
#else
#if defined(EXTENDEDTRACE)
void mpi_record_event(const CBTF_mpit_event* event, uint64_t function)
#else
void mpi_record_event(const CBTF_mpi_event* event, uint64_t function)
#endif
#endif
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);

    //if (tls->defer_sampling) return;

    tls->do_trace = FALSE;

    uint64_t stacktrace[MaxFramesPerStackTrace];
    unsigned stacktrace_size = 0;
    unsigned entry = 0, start, i;
    unsigned pathindex = 0;

#ifndef NDEBUG
	if (getenv("CBTF_DEBUG_COLLECTOR_DETAILED") != NULL) {
#if defined(EXTENDEDTRACE)
fprintf(stderr,"ENTERED mpi_record_event, sizeof event=%d, sizeof stacktrace=%d, NESTING=%d\n",sizeof(CBTF_mpit_event),sizeof(stacktrace),tls->nesting_depth);
#else
fprintf(stderr,"ENTERED mpi_record_event, sizeof event=%d, sizeof stacktrace=%d, NESTING=%d\n",sizeof(CBTF_mpi_event),sizeof(stacktrace),tls->nesting_depth);
#endif
	}
#endif

    /* Decrement the MPI function wrapper nesting depth */
    --tls->nesting_depth;

    /*
     * Don't record events for any recursive calls to our MPI function wrappers.
     * The place where this occurs is when the MPI implemetnation calls itself.
     * We don't record that data here because we are primarily interested in
     * direct calls by the application to the MPI library - not in the internal
     * implementation details of that library.
     */
    if(tls->nesting_depth > 0) {
#ifndef NDEBUG
	if (getenv("CBTF_DEBUG_COLLECTOR") != NULL) {
	    fprintf(stderr,"mpi_record_event RETURNS EARLY DUE TO NESTING\n");
	}
#endif
	return;
    }
    
    /* Newer versions of libunwind now make io calls (open a file in /proc/<self>/maps)
     * that cause a thread lock in the libunwind dwarf parser. We are not interested in
     * any io done by libunwind while we get the stacktrace for the current context.
     * So we need to bump the nesting_depth before requesting the stacktrace and
     * then decrement nesting_depth after aquiring the stacktrace
     */

    /* Obtain the stack trace from the current thread context */
    CBTF_GetStackTraceFromContext(NULL, FALSE, OverheadFrameCount,
				    MaxFramesPerStackTrace,
				    &stacktrace_size, stacktrace);

#ifndef NDEBUG
	if (getenv("CBTF_DEBUG_COLLECTOR_DETAILED") != NULL) {
	    fprintf(stderr,"mpi_record_event gets stacktrace of size:%d\n",stacktrace_size);
	}
#endif

#if defined(PROFILE)

    bool_t stack_already_exists = FALSE;

    if(stacktrace_size > 0)
	stacktrace[0] = function;

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
	tls->do_trace = TRUE;
	return;
    }

    /* sample buffer has no room for these stack frames.*/
    int buflen = tls->data.stacktraces.stacktraces_len + stacktrace_size;
    if ( buflen > StackTraceBufferSize) {
	/* send the current sample buffer. (will init a new buffer) */
#ifndef NDEBUG
	if (getenv("CBTF_DEBUG_COLLECTOR") != NULL) {
	    fprintf(stderr,"StackTraceBufferSize SAMPLE BUFFER FULL. send samples\n");
	}
#endif
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

#else
    /*
     * Replace the first entry in the call stack with the address of the MPI
     * function that is being wrapped. On most platforms, this entry will be
     * the address of the call site of mpi_record_event() within the calling
     * wrapper. On IA64, because OverheadFrameCount is one higher, it will be
     * the mini-tramp for the wrapper that is calling mpi_record_event().
     */
    if(stacktrace_size > 0)
	stacktrace[0] = function;
    
    /*
     * Search the tracing buffer for an existing stack trace matching the stack
     * trace from the current thread context. For now do a simple linear search.
     */
    for(start = 0, i = 0;
	(i < stacktrace_size) &&
	    ((start + i) < tls->data.stacktraces.stacktraces_len);
	++i)
	
	/* Do the i'th frames differ? */
	if(stacktrace[i] != tls->buffer.stacktraces[start + i]) {
	    
	    /* Advance in the tracing buffer to the end of this stack trace */
	    for(start += i;
		(tls->buffer.stacktraces[start] != 0) &&
		    (start < tls->data.stacktraces.stacktraces_len);
		++start);
	    
	    /* Advance in the tracing buffer past the terminating zero */
	    ++start;
	    
	    /* Begin comparing at the zero'th frame again */
	    i = 0;
	    
	}
    
    /* Did we find a match in the tracing buffer? */
    if(i == stacktrace_size)
	entry = start;
    
    /* Otherwise add this stack trace to the tracing buffer */
    else {
	
	/* Send events if there is insufficient room for this stack trace */
	if((tls->data.stacktraces.stacktraces_len + stacktrace_size + 1) >=
	   StackTraceBufferSize) {
#ifndef NDEBUG
	if (getenv("CBTF_DEBUG_COLLECTOR") != NULL) {
	    fprintf(stderr,"StackTraceBufferSize FULL. send samples\n");
	}
#endif
	    send_samples(tls);
	}
	
	/* Add each frame in the stack trace to the tracing buffer. */	
	entry = tls->data.stacktraces.stacktraces_len;
	for(i = 0; i < stacktrace_size; ++i) {
	    
	    /* Add the i'th frame to the tracing buffer */
	    tls->buffer.stacktraces[entry + i] = stacktrace[i];
	    
	    /* Update the address interval in the data blob's header */
	    if(stacktrace[i] < tls->header.addr_begin)
		tls->header.addr_begin = stacktrace[i];
	    if(stacktrace[i] > tls->header.addr_end)
		tls->header.addr_end = stacktrace[i];
	    
	}
	
	/* Add a terminating zero frame to the tracing buffer */
	tls->buffer.stacktraces[entry + stacktrace_size] = 0;
	
	/* Set the new size of the tracing buffer */
	tls->data.stacktraces.stacktraces_len += (stacktrace_size + 1);
	
    }
    
    /* Add a new entry for this event to the tracing buffer. */
#if defined(EXTENDEDTRACE)
    memcpy(&(tls->buffer.events[tls->data.events.events_len]),
	   event, sizeof(CBTF_mpit_event));
#else
    memcpy(&(tls->buffer.events[tls->data.events.events_len]),
	   event, sizeof(CBTF_mpi_event));
#endif
    tls->buffer.events[tls->data.events.events_len].stacktrace = entry;
    tls->data.events.events_len++;
    
    /* Send events if the tracing buffer is now filled with events */
    if(tls->data.events.events_len == EventBufferSize) {
#ifndef NDEBUG
	if (getenv("CBTF_DEBUG_COLLECTOR") != NULL) {
	    fprintf(stderr,"EventBufferSize FULL. send samples\n");
	}
#endif
	send_samples(tls);
    }
#endif

    tls->do_trace = TRUE;
}



/**
 * Called by the CBTF collector service in order to start data collection.
 */
void cbtf_collector_start(const CBTF_DataHeader* const header)
{
/**
 * Start tracing.
 *
 * Starts MPI event tracing for the thread executing this function.
 * Initializes the appropriate thread-local data structures.
 *
 * @param arguments    Encoded function arguments.
 */
    /* Create and access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = malloc(sizeof(TLS));
    Assert(tls != NULL);
    CBTF_SetTLS(TLSKey, tls);
    mpi_init_tls_done = true;
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);

#ifndef NDEBUG
    if (getenv("CBTF_DEBUG_COLLECTOR") != NULL) {
	fprintf(stderr,"ENTERED cbtf_collector_start for %d\n", getpid());
    }
#endif

    tls->defer_sampling=FALSE;

    /* Decode the passed function arguments */
    // Need to handle the arguments...
    CBTF_mpi_start_sampling_args args;
    memset(&args, 0, sizeof(args));
    
#if 0
    CBTF_DecodeParameters(arguments,
			  (xdrproc_t)xdr_CBTF_mpi_start_sampling_args,
			   &args);
#endif

#if defined(CBTF_SERVICE_USE_OFFLINE)

    /* If CBTF_MPI_TRACED is set to a valid list of mpi functions, trace only
     * those functions.
     * If CBTF_MPI_TRACED is set and is empty, trace all functions.
     * For any misspelled function name in CBTF_MPI_TRACED, silently ignore.
     * If all names in CBTF_MPI_TRACED are misspelled or not part of
     * TraceableFunctions, nothing will be traced.
     */
    const char* mpi_traced = getenv("CBTF_MPI_TRACED");

    if (mpi_traced != NULL && strcmp(mpi_traced,"") != 0) {
	strcpy(tls->CBTF_mpi_traced,mpi_traced);
    } else {
	strcpy(tls->CBTF_mpi_traced,all);
    }
#endif

    memcpy(&tls->header, header, sizeof(CBTF_DataHeader));

    /* Initialize the actual data blob */
    initialize_data(tls);

    /* Initialize the MPI function wrapper nesting depth */
    tls->nesting_depth = 0;
 
    /* Begin sampling */
    tls->header.time_begin = CBTF_GetTime();
    tls->do_trace = TRUE;
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

#ifndef NDEBUG
    if (getenv("CBTF_DEBUG_COLLECTOR") != NULL) {
	fprintf(stderr,"ENTERED cbtf_collector_pause for %d\n", getpid());
    }
#endif
    tls->defer_sampling=TRUE;
    tls->do_trace = FALSE;
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

#ifndef NDEBUG
    if (getenv("CBTF_DEBUG_COLLECTOR") != NULL) {
	fprintf(stderr,"ENTERED cbtf_collector_resume for %d\n", getpid());
    }
#endif
    tls->defer_sampling=FALSE;
    tls->do_trace = TRUE;
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
 * Stops MPI event tracing for the thread executing this function.
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

#ifndef NDEBUG
    if (getenv("CBTF_DEBUG_COLLECTOR") != NULL) {
	fprintf(stderr,"ENTERED cbtf_collector_stop for %d\n", getpid());
    }
#endif

    tls->header.time_end = CBTF_GetTime();

    /* Stop sampling */
    defer_trace(0);

    /* Are there any unsent samples? */
#if defined(PROFILE)
#ifndef NDEBUG
    if (getenv("CBTF_DEBUG_COLLECTOR") != NULL) {
	    fprintf(stderr,"cbtf_collector_stop count_len:%d stacktraces_len%d\n",tls->data.count.count_len, tls->data.stacktraces.stacktraces_len);
    }
#endif
    if(tls->data.count.count_len > 0 || tls->data.stacktraces.stacktraces_len > 0) {
	send_samples(tls);
    }
#else
#ifndef NDEBUG
    if (getenv("CBTF_DEBUG_COLLECTOR") != NULL) {
	    fprintf(stderr,"cbtf_collector_stop events_len:%d stacktraces_len%d\n",tls->data.events.events_len, tls->data.stacktraces.stacktraces_len);
    }
#endif
    if(tls->data.events.events_len > 0 || tls->data.stacktraces.stacktraces_len > 0) {
	send_samples(tls);
    }
#endif

    /* Destroy our thread-local storage */
#ifdef CBTF_SERVICE_USE_EXPLICIT_TLS
    free(tls);
    CBTF_SetTLS(TLSKey, NULL);
#endif
}

bool_t mpi_do_trace(const char* traced_func)
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls;
    if (mpi_init_tls_done) {
        tls = CBTF_GetTLS(TLSKey);
    } else {
	return FALSE;
    }
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);

#if defined (CBTF_SERVICE_USE_OFFLINE)

    if (tls->do_trace == FALSE) {
	if (tls->nesting_depth > 1) {
	    --tls->nesting_depth;
	}
	return FALSE;
    }

    /* See if this function has been selected for tracing */
    char *tfptr, *saveptr, *tf_token;
    tfptr = strdup(tls->CBTF_mpi_traced);
    int i;
    for (i = 1;  ; i++, tfptr = NULL) {
	tf_token = strtok_r(tfptr, ":,", &saveptr);
	if (tf_token == NULL)
	    break;
	if ( strcmp(tf_token,traced_func) == 0) {
	
    	    if (tfptr)
		free(tfptr);
	    return TRUE;
	}
    }

    /* Remove any nesting due to skipping mpi_start_event/mpi_record_event for
     * potentially nested iop calls that are not being traced.
     */

    if (tls->nesting_depth > 1) {
	--tls->nesting_depth;
    }

    return FALSE;
#else
    /* Always return true for dynamic instrumentors since these collectors
     * can be passed a list of traced functions for use with executeInPlaceOf.
     */

    if (tls->do_trace == FALSE) {
	if (tls->nesting_depth > 1)
	    --tls->nesting_depth;
	return FALSE;
    }
    return TRUE;
#endif
}
