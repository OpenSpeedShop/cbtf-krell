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
 * Declaration and definition of the PC sampling collector's runtime.
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "KrellInstitute/Messages/DataHeader.h"
#include "KrellInstitute/Messages/PCSamp.h"
#include "KrellInstitute/Messages/PCSamp_data.h"
#include "KrellInstitute/Messages/ToolMessageTags.h"
#include "KrellInstitute/Messages/Thread.h"
#include "KrellInstitute/Messages/ThreadEvents.h"
#include "KrellInstitute/Services/Common.h"
#include "KrellInstitute/Services/Data.h"
#include "KrellInstitute/Services/Timer.h"
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
    CBTF_pcsamp_data data;        /**< Actual data blob. */

    CBTF_PCData buffer;      /**< PC sampling data buffer. */

#if defined(CBTF_SERVICE_USE_MRNET)
    CBTF_Protocol_ThreadNameGroup tgrp;
    CBTF_Protocol_ThreadName tname;
    CBTF_Protocol_CreatedProcess created_process_message;
    CBTF_Protocol_AttachedToThreads attached_to_threads_message;
    CBTF_Protocol_ThreadsStateChanged thread_state_changed_message;

    int mrnet_started;
    int is_mpi_job;
    int sent_process_thread_info;
    int process_created;

    struct {
        CBTF_Protocol_ThreadName tnames[4096];
    } tgrpbuf;

#endif
} TLS;

#if defined(CBTF_SERVICE_USE_MRNET)
#endif

#ifdef USE_EXPLICIT_TLS

/**
 * Thread-local storage key.
 *
 * Key used for looking up our thread-local storage. This key <em>must</em>
 * be globally unique across the entire Open|SpeedShop code base.
 */
static const uint32_t TLSKey = 0x00001EF3;

#else

/** Thread-local storage. */
static __thread TLS the_tls;

#endif

#if defined(CBTF_SERVICE_USE_MRNET)
void set_mpi_flag(int flag) {
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

    if (!tls->process_created) {
	CBTF_MRNet_Send( CBTF_PROTOCOL_TAG_CREATED_PROCESS,
                           (xdrproc_t) xdr_CBTF_Protocol_CreatedProcess,
			   &tls->created_process_message);
	tls->process_created = true;
    }

    CBTF_MRNet_Send( CBTF_PROTOCOL_TAG_ATTACHED_TO_THREADS,
			(xdrproc_t) xdr_CBTF_Protocol_AttachedToThreads,
			&tls->attached_to_threads_message);
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

    if (tls->mrnet_started) {
	return;
    }

    fprintf(stderr,"CALLING CBTF_MRNet_LW_connect for rank %d\n",
	monitor_mpi_comm_rank());
    CBTF_MRNet_LW_connect( monitor_mpi_comm_rank() );
    sleep(1);
    tls->mrnet_started = 1;
    send_process_thread_message();
    tls->sent_process_thread_info = 1;
}

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

void send_thread_state_changed_message()
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    fprintf(stderr,"ENTERED send_thread_state_changed_message for rank %d\n", monitor_mpi_comm_rank());
    if (tls == NULL) {
    fprintf(stderr,"EARLY EXIT send_thread_state_changed_message NO TLS for rank %d\n", monitor_mpi_comm_rank());
	return;
    }

    CBTF_Protocol_ThreadName tname;
    tname.experiment = 0;
    tname.host = strdup(tls->header.host);
    tname.pid = tls->header.pid;
    tname.has_posix_tid = true;
    tname.posix_tid = tls->header.posix_tid;

    tls->tgrp.names.names_len = 0;
    tls->tgrp.names.names_val = tls->tgrpbuf.tnames;
    memset(tls->tgrpbuf.tnames, 0, sizeof(tls->tgrpbuf.tnames));

    memcpy(&(tls->tgrpbuf.tnames[tls->tgrp.names.names_len]),
           &tname, sizeof(tname));
    tls->tgrp.names.names_len++;

    fprintf(stderr,"CREATING thread_state_changed_message for rank %d\n", monitor_mpi_comm_rank());
    //CBTF_Protocol_ThreadsStateChanged message;
    tls->thread_state_changed_message.threads = tls->tgrp;
    tls->thread_state_changed_message.state = Terminated;

    CBTF_MRNet_Send( CBTF_PROTOCOL_TAG_THREADS_STATE_CHANGED,
                  (xdrproc_t) xdr_CBTF_Protocol_ThreadsStateChanged, &tls->thread_state_changed_message);
}
#endif

static void serviceTimerHandler(const ucontext_t* context);

#if defined (CBTF_SERVICE_USE_OFFLINE)
extern void cbtf_offline_sent_data(int);
#endif


/**
 * Timer event handler.
 *
 * Called by the timer handler each time a sample is to be taken. Extracts the
 * program counter (PC) address from the signal context and places it into the
 * sample buffer. When the sample buffer is full, it is sent to the framework
 * for storage in the experiment's database.
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
    
    /* Obtain the program counter (PC) address from the thread context */
    uint64_t pc = CBTF_GetPCFromContext(context);

    /* Update the sampling buffer and check if it has been filled */
    if(CBTF_UpdatePCData(pc, &tls->buffer)) {

	/* Send these samples */
	tls->header.time_end = CBTF_GetTime();
	tls->header.addr_begin = tls->buffer.addr_begin;
	tls->header.addr_end = tls->buffer.addr_end;
	tls->data.pc.pc_len = tls->buffer.length;
	tls->data.count.count_len = tls->buffer.length;


#ifndef NDEBUG
        if (getenv("CBTF_DEBUG_SERVICE") != NULL) {
            fprintf(stderr,"serviceTimerHandler sends data:\n");
            fprintf(stderr,"time_end(%#lu) addr range [%#lx, %#lx] pc_len(%d) count_len(%d)\n",
                tls->header.time_end,tls->header.addr_begin,
		tls->header.addr_end,tls->data.pc.pc_len,
                tls->data.count.count_len);
        }
#endif

#if defined(CBTF_SERVICE_USE_FILEIO)
	CBTF_Send(&tls->header, (xdrproc_t)xdr_CBTF_pcsamp_data, &tls->data);
#endif

#if defined(CBTF_SERVICE_USE_MRNET)
	if (!tls->sent_process_thread_info) {
	    send_process_thread_message();
	}

	CBTF_MRNet_Send_PerfData(CBTF_PROTOCOL_TAG_PERFORMANCE_DATA,
				 &tls->header,
				 (xdrproc_t)xdr_CBTF_pcsamp_data,
				 &tls->data);
#endif

#if defined(CBTF_SERVICE_USE_OFFLINE)
	cbtf_offline_sent_data(1);
#endif

	/* Re-initialize the data blob's header */
	tls->header.time_begin = tls->header.time_end;

	/* Re-initialize the sampling buffer */
	tls->buffer.addr_begin = ~0;
	tls->buffer.addr_end = 0;
	tls->buffer.length = 0;
	memset(tls->buffer.hash_table, 0, sizeof(tls->buffer.hash_table));

    }
}



/**
 * Start sampling.
 *
 * Starts program counter (PC) sampling for the thread executing this function.
 * Initializes the appropriate thread-local data structures and then enables the
 * sampling timer.
 *
 * @param arguments    Encoded function arguments.
 */
void cbtf_timer_service_start_sampling(const char* arguments)
{
    CBTF_pcsamp_start_sampling_args args;

    /* Create and access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = malloc(sizeof(TLS));
    Assert(tls != NULL);
    CBTF_SetTLS(TLSKey, tls);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);

    /* Decode the passed function arguments */
    memset(&args, 0, sizeof(args));
    CBTF_DecodeParameters(arguments,
			    (xdrproc_t)xdr_CBTF_pcsamp_start_sampling_args,
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
    CBTF_SetSendToFile(&(tls->header), "pcsamp", "cbtf-data");
    
    /* Initialize the actual data blob */
    tls->data.interval = 
	(uint64_t)(1000000000) / (uint64_t)(args.sampling_rate);
    tls->data.pc.pc_val = tls->buffer.pc;
    tls->data.count.count_val = tls->buffer.count;

    /* Initialize the sampling buffer */
    tls->buffer.addr_begin = ~0;
    tls->buffer.addr_end = 0;
    tls->buffer.length = 0;
    memset(tls->buffer.hash_table, 0, sizeof(tls->buffer.hash_table));
 
#if defined (CBTF_SERVICE_USE_MRNET)
    //CBTF_Protocol_ThreadName tname;
    tls->tname.host = strdup(local_data_header.host);
    tls->tname.pid = local_data_header.pid;
    tls->tname.has_posix_tid = true;
    tls->tname.posix_tid = local_data_header.posix_tid;

    CBTF_Protocol_ThreadName origtname;
    origtname.host = strdup(tls->tname.host);
    origtname.pid = -1;
    origtname.has_posix_tid = false;
    origtname.posix_tid = 0;

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

    tls->mrnet_started = 0;
    tls->sent_process_thread_info = 0;

    tls->tgrp.names.names_len = 0;
    tls->tgrp.names.names_val = tls->tgrpbuf.tnames;
    memset(tls->tgrpbuf.tnames, 0, sizeof(tls->tgrpbuf.tnames));

    started_process_thread();
#endif

    /* Begin sampling */
    tls->header.time_begin = CBTF_GetTime();
    CBTF_Timer(tls->data.interval, serviceTimerHandler);
}



/**
 * Stop sampling.
 *
 * Stops program counter (PC) sampling for the thread executing this function.
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

    tls->header.time_end = CBTF_GetTime();

    /* Are there any unsent samples? */
    if(tls->buffer.length > 0) {

	/* Send these samples */
	tls->header.addr_begin = tls->buffer.addr_begin;
	tls->header.addr_end = tls->buffer.addr_end;
	tls->data.pc.pc_len = tls->buffer.length;
	tls->data.count.count_len = tls->buffer.length;

#ifndef NDEBUG
	if (getenv("CBTF_DEBUG_SERVICE") != NULL) {
	    fprintf(stderr, "cbtf_timer_service_stop_sampling:\n");
	    fprintf(stderr, "time_end(%#lu) addr range[%#lx, %#lx] pc_len(%d) count_len(%d)\n",
		tls->header.time_end,tls->header.addr_begin,
		tls->header.addr_end,tls->data.pc.pc_len,
		tls->data.count.count_len);
	}
#endif

#if defined(CBTF_SERVICE_USE_FILEIO)
	CBTF_Send(&(tls->header), (xdrproc_t)xdr_CBTF_pcsamp_data, &(tls->data));
#endif

#if defined(CBTF_SERVICE_USE_MRNET)
	CBTF_MRNet_Send_PerfData(CBTF_PROTOCOL_TAG_PERFORMANCE_DATA,
				 &tls->header,
				 (xdrproc_t)xdr_CBTF_pcsamp_data,
				 &tls->data);
#endif

#if defined(CBTF_SERVICE_USE_OFFLINE)
	cbtf_offline_sent_data(1);
#endif
	
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
