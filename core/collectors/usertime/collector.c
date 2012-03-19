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
 * Declaration and definition of the UserTime collector's runtime.
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "KrellInstitute/Messages/DataHeader.h"
#include "KrellInstitute/Messages/Usertime.h"
#include "KrellInstitute/Messages/Usertime_data.h"
#include "KrellInstitute/Messages/ToolMessageTags.h"
#include "KrellInstitute/Messages/Thread.h"
#include "KrellInstitute/Messages/ThreadEvents.h"
#include "KrellInstitute/Services/Common.h"
#include "KrellInstitute/Services/Context.h"
#include "KrellInstitute/Services/Data.h"
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



/** Type defining the items stored in thread-local storage. */
typedef struct {

    CBTF_DataHeader header;  /**< Header for following data blob. */
    CBTF_usertime_data data;        /**< Actual data blob. */

    CBTF_StackTraceData buffer;      /**< Stacktrace sampling data buffer. */

    bool_t defer_sampling;

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
static const uint32_t TLSKey = 0x00001EF4;

#else

/** Thread-local storage. */
static __thread TLS the_tls;

#endif

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

#if defined(CBTF_SERVICE_USE_FILEIO)
	CBTF_Send(&(tls->header), (xdrproc_t)xdr_CBTF_usertime_data, &(tls->data));
#endif

#if defined(CBTF_SERVICE_USE_MRNET)
	if (tls->connected_to_mrnet) {
	    if (!tls->sent_process_thread_info) {
	        send_process_thread_message();
	    }
	    tls->sent_process_thread_info = 1;

	    CBTF_MRNet_Send_PerfData( &tls->header,
				 (xdrproc_t)xdr_CBTF_usertime_data,
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
 * Start sampling.
 *
 * Starts user time sampling for the thread executing this function.
 * Initializes the appropriate thread-local data structures and then enables the
 * sampling timer.
 *
 * @param arguments    Encoded function arguments.
 */
void cbtf_timer_service_start_sampling(const char* arguments)
{
    CBTF_usertime_start_sampling_args args;

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
    memset(&args, 0, sizeof(args));
    CBTF_DecodeParameters(arguments,
			    (xdrproc_t)xdr_CBTF_usertime_start_sampling_args,
			    &args);
    
    /* 
     * Initialize the data blob's header
     *
     * Passing &tls->header to CBTF_InitializeDataHeader() was found
     * to not be safe on IA64 systems. Hopefully the extra copy can be
     * removed eventually.
     */
    
    CBTF_DataHeader local_data_header;
    CBTF_InitializeDataHeader(0, args.collector,
				&local_data_header);
    memcpy(&tls->header, &local_data_header, sizeof(CBTF_DataHeader));

#if defined(CBTF_SERVICE_USE_FILEIO)
    CBTF_SetSendToFile(&(tls->header), "usertime", "cbtf-data");
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
    CBTF_Timer(tls->data.interval, serviceTimerHandler);
}



/**
 * Stop sampling.
 *
 * Stops Usertime sampling for the thread executing this function.
 * Disables the sampling timer and sends any samples remaining in the buffer.
 *
 * @param arguments    Encoded (unused) function arguments.
 */
void cbtf_timer_service_stop_sampling(const char* arguments)
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
