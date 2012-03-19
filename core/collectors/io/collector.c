/*******************************************************************************
** Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
** Copyright (c) 2007,2008 William Hachfeld. All Rights Reserved.
** Copyright (c) 2007-2011 Krell Institute.  All Rights Reserved.
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
 * Definition of the IO event tracing collector's runtime.
 *
 */

/* #define DEBUG 1 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "KrellInstitute/Messages/DataHeader.h"
#include "KrellInstitute/Messages/IO.h"
#include "KrellInstitute/Messages/IO_data.h"
#include "KrellInstitute/Messages/ToolMessageTags.h"
#include "KrellInstitute/Messages/Thread.h"
#include "KrellInstitute/Messages/ThreadEvents.h"
#include "KrellInstitute/Services/Common.h"
#include "KrellInstitute/Services/Context.h"
#include "KrellInstitute/Services/Data.h"
#include "KrellInstitute/Services/Unwind.h"
#include "KrellInstitute/Services/TLS.h"
#include "IOTraceableFunctions.h"

#define true 1
#define false 0

#if !defined (TRUE)
#define TRUE true
#endif

#if !defined (FALSE)
#define FALSE false
#endif


/** Number of overhead frames in each stack frame to be skipped. */
#if defined(CBTF_SERVICE_USE_OFFLINE)
const unsigned OverheadFrameCount = 2;
#else
#if defined(__linux) && defined(__ia64)
const unsigned OverheadFrameCount = 2 /*3*/;
#else
const unsigned OverheadFrameCount = 2;
#endif
#endif

/*
 * NOTE: For some reason GCC doesn't like it when the following three macros are
 *       replaced with constant unsigned integers. It complains about the arrays
 *       in the tls structure being "variable-size type declared outside of any
 *       function" even though the size IS constant... Maybe this can be fixed?
 */

/** Maximum number of frames to allow in each stack trace. */
/* what is a reasonable default here. 32? */
#define MaxFramesPerStackTrace 48

/** Number of stack trace entries in the tracing buffer. */
/** event.stacktrace buffer is 64*8=512 bytes */
/** allows for 6 unique stacktraces (384*8/512) */
#define StackTraceBufferSize (CBTF_BlobSizeFactor * 384)


/** Number of event entries in the tracing buffer. */
/** CBTF_io_event is 32 bytes */
#define EventBufferSize (CBTF_BlobSizeFactor * 415)

/** Thread-local storage. */
typedef struct {
    
    /** Nesting depth within the IO function wrappers. */
    unsigned nesting_depth;
    
    CBTF_DataHeader header;  /**< Header for following data blob. */
    CBTF_io_trace_data data;              /**< Actual data blob. */
    
    /** Tracing buffer. */
    struct {
	uint64_t stacktraces[StackTraceBufferSize];  /**< Stack traces. */
	CBTF_io_event events[EventBufferSize];          /**< IO call events. */
    } buffer;    
    
#if defined (CBTF_SERVICE_USE_OFFLINE)
    char CBTF_io_traced[PATH_MAX];
#endif
    int defer_sampling;
    int do_trace;

#if defined(CBTF_SERVICE_USE_MRNET)
    CBTF_Protocol_ThreadNameGroup tgrp;
    CBTF_Protocol_ThreadName tname;
    CBTF_Protocol_CreatedProcess created_process_message;
    CBTF_Protocol_AttachedToThreads attached_to_threads_message;
    CBTF_Protocol_ThreadsStateChanged thread_state_changed_message;

    int connected_to_mrnet;
    int is_mpi_job;
    int sent_process_thread_info;
    int process_created;

    struct {
        CBTF_Protocol_ThreadName tnames[4096];
    } tgrpbuf;
#endif

} TLS;


#if defined (CBTF_SERVICE_USE_OFFLINE)
extern void cbtf_offline_sent_data(int);
#endif

#ifdef USE_EXPLICIT_TLS

/**
 * Thread-local storage key.
 *
 * Key used for looking up our thread-local storage. This key <em>must</em>
 * be globally unique across the entire Open|SpeedShop code base.
 */
static const uint32_t TLSKey = 0x00001EF7;
int io_init_tls_done = 0;

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


#if defined(CBTF_SERVICE_USE_MRNET)

void started_process_thread()
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    if (tls == NULL)
	return;

    CBTF_Protocol_ThreadName origtname;
    origtname.host = strdup(tls->tname.host);
    origtname.pid = -1;
    origtname.has_posix_tid = false;
    origtname.posix_tid = 0;
    origtname.rank = -1;

    tls->tname.rank = monitor_mpi_comm_rank();

    //CBTF_Protocol_CreatedProcess message;
    tls->created_process_message.original_thread = origtname;
    tls->created_process_message.created_thread = tls->tname;

    tls->tgrp.names.names_len = 0;
    tls->tgrp.names.names_val = tls->tgrpbuf.tnames;
    memset(tls->tgrpbuf.tnames, 0, sizeof(tls->tgrpbuf.tnames));

    memcpy(&(tls->tgrpbuf.tnames[tls->tgrp.names.names_len]),
           &tls->tname, sizeof(tls->tname));
    tls->tgrp.names.names_len++;

    //CBTF_Protocol_AttachedToThreads tmessage;
    tls->attached_to_threads_message.threads = tls->tgrp;
    tls->process_created = 0;
}

void send_process_thread_message()
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    if (tls == NULL)
	return;

    started_process_thread();

    if (!tls->process_created) {
	if (tls->connected_to_mrnet) {
	    CBTF_MRNet_Send( CBTF_PROTOCOL_TAG_CREATED_PROCESS,
                           (xdrproc_t) xdr_CBTF_Protocol_CreatedProcess,
			   &tls->created_process_message);
	}
	tls->process_created = true;
    }

    if (tls->connected_to_mrnet) {
	CBTF_MRNet_Send( CBTF_PROTOCOL_TAG_ATTACHED_TO_THREADS,
			(xdrproc_t) xdr_CBTF_Protocol_AttachedToThreads,
			&tls->attached_to_threads_message);
    }
}

void set_mpi_flag(int flag)
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    if (tls == NULL)
	return;

    tls->is_mpi_job = flag;
}

void connect_to_mrnet()
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    if (tls == NULL)
	return;

    if (tls->connected_to_mrnet) {
        fprintf(stderr,"ALREADY connected  connect_to_mrnet \n");
	return;
    }

#ifndef NDEBUG
    if (getenv("CBTF_DEBUG_LW_MRNET") != NULL) {
	 fprintf(stderr,"connect_to_mrnet() calling CBTF_MRNet_LW_connect for rank %d\n",
	monitor_mpi_comm_rank());
    }
#endif

    CBTF_MRNet_LW_connect( monitor_mpi_comm_rank() );
    tls->header.rank = monitor_mpi_comm_rank();
    tls->connected_to_mrnet = 1;

#ifndef NDEBUG
    if (getenv("CBTF_DEBUG_LW_MRNET") != NULL) {
	 fprintf(stderr,"connect_to_mrnet reports connection successful for %s:%d rank %d\n",
		tls->header.host, tls->header.pid, tls->header.rank);
    }
#endif

}

void send_thread_state_changed_message()
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif

    if (tls == NULL) {
	fprintf(stderr,"EARLY EXIT send_thread_state_changed_message NO TLS for rank %d\n",
		monitor_mpi_comm_rank());
	return;
    }

#ifndef NDEBUG
    if (getenv("CBTF_DEBUG_LW_MRNET") != NULL) {
        fprintf(stderr,"ENTERED send_thread_state_changed_message for rank %d\n", monitor_mpi_comm_rank());
    }
#endif

    CBTF_Protocol_ThreadName tname;
    tname.experiment = 0;
    tname.host = strdup(tls->header.host);
    tname.pid = tls->header.pid;
    tname.has_posix_tid = true;
    tname.posix_tid = tls->header.posix_tid;
    tname.rank = tls->header.rank;

    tls->tgrp.names.names_len = 0;
    tls->tgrp.names.names_val = tls->tgrpbuf.tnames;
    memset(tls->tgrpbuf.tnames, 0, sizeof(tls->tgrpbuf.tnames));

    memcpy(&(tls->tgrpbuf.tnames[tls->tgrp.names.names_len]),
           &tname, sizeof(tname));
    tls->tgrp.names.names_len++;

    //CBTF_Protocol_ThreadsStateChanged message;
    tls->thread_state_changed_message.threads = tls->tgrp;
    tls->thread_state_changed_message.state = Terminated;

    if (tls->connected_to_mrnet) {
	CBTF_MRNet_Send( CBTF_PROTOCOL_TAG_THREADS_STATE_CHANGED,
                  (xdrproc_t) xdr_CBTF_Protocol_ThreadsStateChanged,
		  &tls->thread_state_changed_message);
    }
}
#endif

/**
 * Send events.
 *
 * Sends all the events in the tracing buffer to the framework for storage in 
 * the experiment's database. Then resets the tracing buffer to the empty state.
 * This is done regardless of whether or not the buffer is actually full.
 */
/*
NO DEBUG PRINT STATEMENTS HERE.
*/
static void io_send_events(TLS *tls)
{
    /* Set the end time of this data blob */
    tls->header.time_end = CBTF_GetTime();
    tls->header.id = strdup("io");

#ifndef NDEBUG
    if (getenv("CBTF_DEBUG_COLLECTOR") != NULL) {
        fprintf(stderr,"IO Collector runtime sends data:\n");
        fprintf(stderr,"time_end(%#lu) addr range [%#lx, %#lx] "
		" stacktraces_len(%d) events_len(%d)\n",
            tls->header.time_end,tls->header.addr_begin,tls->header.addr_end,
	    tls->data.stacktraces.stacktraces_len,
            tls->data.events.events_len);
    }
#endif


#if defined(CBTF_SERVICE_USE_FILEIO)
	CBTF_Send(&(tls->header), (xdrproc_t)xdr_CBTF_io_trace_data, &(tls->data));
#endif

#if defined(CBTF_SERVICE_USE_MRNET)
	if (tls->connected_to_mrnet) {
	    if (!tls->sent_process_thread_info) {
	        send_process_thread_message();
	    }
	    tls->sent_process_thread_info = 1;

	    CBTF_MRNet_Send_PerfData( &tls->header,
				 (xdrproc_t)xdr_CBTF_io_trace_data,
				 &tls->data);
	}
#endif

#if defined(CBTF_SERVICE_USE_OFFLINE)
    cbtf_offline_sent_data(1);
#endif

    /* Re-initialize the data blob's header */
    tls->header.time_begin = tls->header.time_end;
    tls->header.time_end = 0;
    tls->header.addr_begin = ~0;
    tls->header.addr_end = 0;
    
    /* Re-initialize the actual data blob */
    tls->data.stacktraces.stacktraces_len = 0;
    tls->data.events.events_len = 0;    
}
    


/**
 * Start an event.
 *
 * Called by the IO function wrappers each time an event is to be started.
 * Initializes the event record and increments the wrappers nesting depth.
 *
 * @param event    Event to be started.
 */
/*
NO DEBUG PRINT STATEMENTS HERE.
*/
void io_start_event(CBTF_io_event* event)
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);

    /* Increment the IO function wrapper nesting depth */
    ++tls->nesting_depth;

    /* Initialize the event record. */
    memset(event, 0, sizeof(CBTF_io_event));
}


    
/**
 * Record an event.
 *
 * Called by the IO function wrappers each time an event is to be recorded.
 * Extracts the stack trace from the current thread context and places it, along
 * with the specified event record, into the tracing buffer. When the tracing
 * buffer is full, it is sent to the framework for storage in the experiment's
 * database.
 *
 * @param event       Event to be recorded.
 * @param function    Address of the IO function for which the event is being
 *                    recorded.
 */
/*
NO DEBUG PRINT STATEMENTS HERE IF TRACING "write, __libc_write".
*/
void io_record_event(const CBTF_io_event* event, uint64_t function)
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);

    tls->do_trace = 0;

    uint64_t stacktrace[MaxFramesPerStackTrace];
    unsigned stacktrace_size = 0;
    unsigned entry = 0, start, i;
    unsigned pathindex = 0;

#ifdef DEBUG
fprintf(stderr,"ENTERED io_record_event, sizeof event=%d, sizeof stacktrace=%d, NESTING=%d\n",sizeof(CBTF_io_event),sizeof(stacktrace),tls->nesting_depth);
#endif

    /* Decrement the IO function wrapper nesting depth */
    --tls->nesting_depth;

    /*
     * Don't record events for any recursive calls to our IO function wrappers.
     * The place where this occurs is when the IO implemetnation calls itself.
     * We don't record that data here because we are primarily interested in
     * direct calls by the application to the IO library - not in the internal
     * implementation details of that library.
     */
    if(tls->nesting_depth > 0) {
#ifdef DEBUG
	fprintf(stderr,"io_record_event RETURNS EARLY DUE TO NESTING\n");
#endif
	return;
    }
    
    /* Newer versions of libunwind now make io calls (open a file in /proc/<self>/maps)
     * that cause a thread lock in the libunwind dwarf parser. We are not interested in
     * any io done by libunwind while we get the stacktrace for the current context.
     * So we need to bump the nesting_depth before requesting the stacktrace and
     * then decrement nesting_depth after aquiring the stacktrace
     */

    ++tls->nesting_depth;
    /* Obtain the stack trace from the current thread context */
    CBTF_GetStackTraceFromContext(NULL, FALSE, OverheadFrameCount,
				    MaxFramesPerStackTrace,
				    &stacktrace_size, stacktrace);
    --tls->nesting_depth;

    /*
     * Replace the first entry in the call stack with the address of the IO
     * function that is being wrapped. On most platforms, this entry will be
     * the address of the call site of io_record_event() within the calling
     * wrapper. On IA64, because OverheadFrameCount is one higher, it will be
     * the mini-tramp for the wrapper that is calling io_record_event().
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
#ifdef DEBUG
fprintf(stderr,"StackTraceBufferSize is full, call io_send_events\n");
#endif
	    io_send_events(tls);
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
    memcpy(&(tls->buffer.events[tls->data.events.events_len]),
	   event, sizeof(CBTF_io_event));
    tls->buffer.events[tls->data.events.events_len].stacktrace = entry;
    tls->data.events.events_len++;
    
    /* Send events if the tracing buffer is now filled with events */
    if(tls->data.events.events_len == EventBufferSize) {
#ifdef DEBUG
fprintf(stderr,"Event Buffer is full, call io_send_events\n");
#endif
	io_send_events(tls);
    }

    tls->do_trace = 1;
}



/**
 * Start tracing.
 *
 * Starts IO extended event tracing for the thread executing this function.
 * Initializes the appropriate thread-local data structures.
 *
 * @param arguments    Encoded function arguments.
 */
void io_start_tracing(const char* arguments)
{
    CBTF_io_start_sampling_args args;

    /* Create and access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
	//fprintf(stderr,"io_start_tracing sets TLSKey %#x\n",TLSKey);
    TLS* tls = malloc(sizeof(TLS));
    Assert(tls != NULL);
    CBTF_SetTLS(TLSKey, tls);
    io_init_tls_done = 1;
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);


    /* Decode the passed function arguments */
    memset(&args, 0, sizeof(args));
    CBTF_DecodeParameters(arguments,
			  (xdrproc_t)xdr_CBTF_io_start_sampling_args,
			   &args);
    
    /* 
     * Initialize the data blob's header
     *
     * Passing &tls->header to CBTF_InitializeDataHeader() was found
     * to not be safe on IA64 systems. Hopefully the extra copy can be
     * removed eventually.
     */
    CBTF_DataHeader local_data_header;
    CBTF_InitializeDataHeader(args.experiment, args.collector, &(local_data_header));
    memcpy(&tls->header, &local_data_header, sizeof(CBTF_DataHeader));

#if defined(CBTF_SERVICE_USE_FILEIO)
    CBTF_SetSendToFile(&(tls->header), "io","openss-data");
#endif

#if defined(CBTF_SERVICE_USE_OFFLINE)

    /* If CBTF_IO_TRACED is set to a valid list of io functions, trace only
     * those functions.
     * If CBTF_IO_TRACED is set and is empty, trace all functions.
     * For any misspelled function name in CBTF_IO_TRACED, silently ignore.
     * If all names in CBTF_IO_TRACED are misspelled or not part of
     * TraceableFunctions, nothing will be traced.
     */
    const char* io_traced = getenv("CBTF_IO_TRACED");

    if (io_traced != NULL && strcmp(io_traced,"") != 0) {
	strcpy(tls->CBTF_io_traced,io_traced);
    } else {
	strcpy(tls->CBTF_io_traced,traceable);
    }
#endif

    /* Initialize the IO function wrapper nesting depth */
    tls->nesting_depth = 0;

    tls->header.time_begin = 0;
    tls->header.time_end = 0;
    tls->header.addr_begin = ~0;
    tls->header.addr_end = 0;
    
    /* Initialize the actual data blob */
    tls->data.stacktraces.stacktraces_len = 0;
    tls->data.stacktraces.stacktraces_val = tls->buffer.stacktraces;
    tls->data.events.events_len = 0;
    tls->data.events.events_val = tls->buffer.events;

 
#if defined (CBTF_SERVICE_USE_MRNET)
    //CBTF_Protocol_ThreadName tname;
    tls->tname.host = strdup(local_data_header.host);
    tls->tname.pid = local_data_header.pid;
    tls->tname.has_posix_tid = true;
    tls->tname.posix_tid = local_data_header.posix_tid;
    tls->tname.rank = local_data_header.rank;

    CBTF_Protocol_ThreadName origtname;
    origtname.host = strdup(tls->tname.host);
    origtname.pid = -1;
    origtname.has_posix_tid = false;
    origtname.posix_tid = 0;
    origtname.rank = -1;

    CBTF_Protocol_CreatedProcess message;
    message.original_thread = origtname;
    message.created_thread = tls->tname;

    tls->tgrp.names.names_len = 0;
    tls->tgrp.names.names_val = tls->tgrpbuf.tnames;
    memset(tls->tgrpbuf.tnames, 0, sizeof(tls->tgrpbuf.tnames));

    memcpy(&(tls->tgrpbuf.tnames[tls->tgrp.names.names_len]),
           &tls->tname, sizeof(tls->tname));
    tls->tgrp.names.names_len++;

    CBTF_Protocol_AttachedToThreads tmessage;
    tmessage.threads = tls->tgrp;

    tls->connected_to_mrnet = 0;
    tls->sent_process_thread_info = 0;

    tls->tgrp.names.names_len = 0;
    tls->tgrp.names.names_val = tls->tgrpbuf.tnames;
    memset(tls->tgrpbuf.tnames, 0, sizeof(tls->tgrpbuf.tnames));

#if !defined (CBTF_SERVICE_USE_MRNET_MPI)
    connect_to_mrnet();
#endif

#endif

    /* Begin sampling */
    tls->header.time_begin = CBTF_GetTime();
    tls->do_trace = 1;
}



/**
 * Stop tracing.
 *
 * Stops IO event tracing for the thread executing this function.
 * Sends any events remaining in the buffer.
 *
 * @param arguments    Encoded (unused) function arguments.
 */
void io_stop_tracing(const char* arguments)
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif

    Assert(tls != NULL);

    /* Stop sampling */
    defer_trace(0);

    /* Are there any unsent samples? */
    if(tls->data.events.events_len > 0) {
	io_send_events(tls);
    }

    /* Destroy our thread-local storage */
#ifdef CBTF_SERVICE_USE_EXPLICIT_TLS
    free(tls);
    CBTF_SetTLS(TLSKey, NULL);
#endif
}

bool_t io_do_trace(const char* traced_func)
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls;
    if (io_init_tls_done) {
        tls = CBTF_GetTLS(TLSKey);
    } else {
	return FALSE;
    }
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);

#if defined (CBTF_SERVICE_USE_OFFLINE)

    if (tls->do_trace == 0) {
	if (tls->nesting_depth > 1)
	    --tls->nesting_depth;
	return FALSE;
    }

    /* See if this function has been selected for tracing */
    char *tfptr, *saveptr, *tf_token;
    tfptr = strdup(tls->CBTF_io_traced);
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

    /* Remove any nesting due to skipping io_start_event/io_record_event for
     * potentially nested iop calls that are not being traced.
     */

    if (tls->nesting_depth > 1)
	--tls->nesting_depth;

    return FALSE;
#else
    /* Always return true for dynamic instrumentors since these collectors
     * can be passed a list of traced functions for use with executeInPlaceOf.
     */

    if (tls->do_trace == 0) {
	if (tls->nesting_depth > 1)
	    --tls->nesting_depth;
	return FALSE;
    }
    return TRUE;
#endif
}

#if defined (CBTF_SERVICE_USE_OFFLINE)

void cbtf_offline_service_resume_sampling()
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

void cbtf_offline_service_defer_sampling()
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
#endif
