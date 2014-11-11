/*******************************************************************************
** Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
** Copyright (c) 2007,2008 William Hachfeld. All Rights Reserved.
** Copyright (c) 2007-2011 Krell Institute.  All Rights Reserved.
** Copyright (c) 2012 Argo Navis Technologies. All Rights Reserved.
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

/** @file One half of the CBTF collector service API implementation. */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdbool.h>

#include "KrellInstitute/Messages/DataHeader.h"
#include "KrellInstitute/Messages/ToolMessageTags.h"
#include "KrellInstitute/Messages/Thread.h"
#include "KrellInstitute/Messages/ThreadEvents.h"

#include "KrellInstitute/Services/Common.h"
#include "KrellInstitute/Services/Collector.h"
#include "KrellInstitute/Services/TLS.h"

/** Type defining the items stored in thread-local storage. */
typedef struct {

    CBTF_DataHeader header;  /**< Header for data blobs. */

    bool_t defer_sampling;

#if defined(CBTF_SERVICE_USE_MRNET)
    CBTF_Protocol_ThreadNameGroup tgrp;
    CBTF_Protocol_ThreadName tname;
    CBTF_Protocol_AttachedToThreads attached_to_threads_message;
    CBTF_Protocol_ThreadsStateChanged thread_state_changed_message;

    bool connected_to_mrnet;
    bool is_mpi_job;
    bool is_threaded_job;
    bool has_ompt;
    bool sent_attached_to_threads;

    struct {
        CBTF_Protocol_ThreadName tnames[4096];
    } tgrpbuf;


#endif

#ifndef NDEBUG
    bool debug_mrnet;
    bool debug_collector;
#endif

} TLS;

#if defined (CBTF_SERVICE_USE_OFFLINE)
extern void cbtf_offline_sent_data(int);
extern void cbtf_send_info();
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

void init_process_thread()
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    if (tls == NULL)
	return;

    tls->tname.rank = monitor_mpi_comm_rank();
    tls->header.rank = monitor_mpi_comm_rank();

    // removed CBTF_Protocol_CreatedProcess message;

    tls->tgrp.names.names_len = 0;
    tls->tgrp.names.names_val = tls->tgrpbuf.tnames;
    memset(tls->tgrpbuf.tnames, 0, sizeof(tls->tgrpbuf.tnames));

    memcpy(&(tls->tgrpbuf.tnames[tls->tgrp.names.names_len]),
           &tls->tname, sizeof(tls->tname));
    tls->tgrp.names.names_len++;

    //CBTF_Protocol_AttachedToThreads tmessage;
    tls->attached_to_threads_message.threads = tls->tgrp;
}

void send_attached_to_threads_message()
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif

    if (tls == NULL)
	return;

    if (tls->connected_to_mrnet && ! tls->sent_attached_to_threads) {
	init_process_thread();
#ifndef NDEBUG
        if (tls->debug_mrnet) {
    	     fprintf(stderr,
	   "SEND CBTF_PROTOCOL_TAG_ATTACHED_TO_THREADS, for %s:%lld:%lld:%d\n",
                     tls->header.host, (long long)tls->header.pid,
                     (long long)tls->header.posix_tid, tls->header.rank);
        }
#endif
	CBTF_MRNet_Send( CBTF_PROTOCOL_TAG_ATTACHED_TO_THREADS,
			(xdrproc_t) xdr_CBTF_Protocol_AttachedToThreads,
			&tls->attached_to_threads_message);
	tls->sent_attached_to_threads = true;
    }
}

void set_mpi_flag(bool flag)
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

void set_threaded_flag(bool flag)
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    if (tls == NULL)
	return;

    tls->is_threaded_job = flag;
}

void set_ompt_flag(bool flag)
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    if (tls == NULL)
	return;

    tls->has_ompt = flag;
}

void set_threaded_mrnet_connection()
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    if (tls == NULL)
	return;

    tls->connected_to_mrnet = true;
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
#ifndef NDEBUG
    if (tls->debug_mrnet) {
        fprintf(stderr,"ALREADY connected  connect_to_mrnet \n");
    }
#endif
	return;
    }

#ifndef NDEBUG
    if (tls->debug_mrnet) {
	 fprintf(stderr,"connect_to_mrnet() calling CBTF_MRNet_LW_connect for rank %d\n",
	monitor_mpi_comm_rank());
    }
#endif

    CBTF_MRNet_LW_connect( monitor_mpi_comm_rank() );
    tls->header.rank = monitor_mpi_comm_rank();
    tls->connected_to_mrnet = true;

#ifndef NDEBUG
    if (tls->debug_mrnet) {
	 fprintf(stderr,
        "connect_to_mrnet reports connection successful for %s:%lld rank %d\n",
             tls->header.host, (long long)tls->header.pid, tls->header.rank);
    }
#endif

}
#endif

// noop for non mrnet collection.
void send_thread_state_changed_message()
{
#if defined(CBTF_SERVICE_USE_MRNET)
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif

    if (tls == NULL) {
#ifndef NDEBUG
	if (tls->debug_mrnet) {
	    fprintf(stderr,"EARLY EXIT send_thread_state_changed_message NO TLS for rank %d\n",
		monitor_mpi_comm_rank());
	}
#endif
	return;
    }

    //CBTF_Protocol_ThreadsStateChanged message;
    tls->thread_state_changed_message.threads = tls->tgrp;
    tls->thread_state_changed_message.state = Terminated; 
    if (tls->connected_to_mrnet) {
#ifndef NDEBUG
	if (tls->debug_mrnet) {
            fprintf(stderr,
	    "SEND CBTF_PROTOCOL_TAG_THREADS_STATE_CHANGED for %s:%lld:%lld:%d\n",
                     tls->header.host, (long long)tls->header.pid,
                     (long long)tls->header.posix_tid, tls->header.rank);
	}
#endif
	CBTF_MRNet_Send( CBTF_PROTOCOL_TAG_THREADS_STATE_CHANGED,
                  (xdrproc_t) xdr_CBTF_Protocol_ThreadsStateChanged,
		  &tls->thread_state_changed_message);
    }
#endif
}

void cbtf_collector_send(const CBTF_DataHeader* header,
                         const xdrproc_t xdrproc, const void* data)
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif

    Assert(tls != NULL);

#ifndef NDEBUG
    if (tls->debug_collector) {
        fprintf(stderr,"cbtf_collector_send DATA for %s:%lld:%lld:%d:\n",
                     tls->header.host, (long long)tls->header.pid,
                     (long long)tls->header.posix_tid, tls->header.rank);
        fprintf(stderr,"time_range[%lu, %lu) addr range [%#lx, %#lx]\n",
            (uint64_t)header->time_begin, (uint64_t)header->time_end,
	    header->addr_begin, header->addr_end);
    }
#endif

#if defined(CBTF_SERVICE_USE_FILEIO)
    CBTF_Send(header, xdrproc, data);
#endif

#if defined(CBTF_SERVICE_USE_MRNET)
	if (tls->connected_to_mrnet) {
	    if (!tls->sent_attached_to_threads) {
		//init_process_thread();
	        send_attached_to_threads_message();
	        tls->sent_attached_to_threads = true;
	    }

	    CBTF_MRNet_Send_PerfData(header, xdrproc, data);
	} else {
	    fprintf(stderr,"cbtf_collector_send called but no longer connected!!!\n");
	}
#endif

#if defined(CBTF_SERVICE_USE_OFFLINE)
    cbtf_offline_sent_data(1);
#endif
}



/**
 * Start collection.
 *
 * Starts collection for the thread executing this function.
 * Initializes the appropriate thread-local data structures and then enables the
 * sampling timer.
 *
 * @param arguments    Encoded function arguments.
 */
void cbtf_timer_service_start_sampling(const char* arguments)
{
    /* Create and access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = malloc(sizeof(TLS));
    Assert(tls != NULL);
    CBTF_SetTLS(TLSKey, tls);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);

    tls->defer_sampling=false;
#ifndef NDEBUG
    if (getenv("CBTF_DEBUG_LW_MRNET") != NULL) {
	tls->debug_mrnet=true;
    } else {
	tls->debug_mrnet=false;
    }
    if (getenv("CBTF_DEBUG_COLLECTOR") != NULL) {
	tls->debug_collector=true;
    } else {
	tls->debug_collector=false;
    }
#endif

    /* 
     * Initialize the data blob's header
     *
     * Passing &tls->header to CBTF_InitializeDataHeader() was found
     * to not be safe on IA64 systems. Hopefully the extra copy can be
     * removed eventually.
     */
    
    CBTF_DataHeader local_data_header;
    CBTF_InitializeDataHeader(0 /* Experiment */, 1 /* Collector */,
				&local_data_header);
    memcpy(&tls->header, &local_data_header, sizeof(CBTF_DataHeader));

    tls->header.id = strdup(cbtf_collector_unique_id);

#if defined(CBTF_SERVICE_USE_FILEIO)
    CBTF_SetSendToFile(&(tls->header), cbtf_collector_unique_id, "cbtf-data");
#endif

#if defined (CBTF_SERVICE_USE_MRNET)
    //CBTF_Protocol_ThreadName tname;
    tls->tname.experiment = 0;
    tls->tname.host = strdup(local_data_header.host);
    tls->tname.pid = local_data_header.pid;
    tls->tname.has_posix_tid = true;
    tls->tname.posix_tid = local_data_header.posix_tid;
    tls->tname.rank = local_data_header.rank;

    tls->tgrp.names.names_len = 0;
    tls->tgrp.names.names_val = tls->tgrpbuf.tnames;
    memset(tls->tgrpbuf.tnames, 0, sizeof(tls->tgrpbuf.tnames));

    memcpy(&(tls->tgrpbuf.tnames[tls->tgrp.names.names_len]),
           &tls->tname, sizeof(tls->tname));
    tls->tgrp.names.names_len++;

    tls->connected_to_mrnet = false;
    tls->sent_attached_to_threads = false;

#if !defined (CBTF_SERVICE_USE_MRNET_MPI)
    // Non-mpi applications connect here.
#ifndef NDEBUG
    if (tls->debug_mrnet) {
	fprintf(stderr,"cbtf_timer_service_start_sampling calls connect_to_mrnet for NON MPI program\n");
    }
#endif
    connect_to_mrnet();
    if (tls->connected_to_mrnet) {
	if (!tls->sent_attached_to_threads) {
	    //init_process_thread();
	    send_attached_to_threads_message();
	    tls->sent_attached_to_threads = true;
	}
    }
    cbtf_send_info();
    // defer record of dsos
#endif

#endif

    tls->header.posix_tid = local_data_header.posix_tid;
    tls->header.rank = local_data_header.rank;
    /* Begin collection */
    cbtf_collector_start(&tls->header);
}



/**
 * Stop collection.
 *
 * Stops collection for the thread executing this function.
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

    /* Stop collection */
    cbtf_collector_stop();

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

    cbtf_collector_resume();
}

void cbtf_offline_service_stop_timer()
{
    cbtf_collector_pause();
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
