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
#include <unistd.h>

#include "KrellInstitute/Messages/DataHeader.h"
#include "KrellInstitute/Messages/ToolMessageTags.h"
#include "KrellInstitute/Messages/Thread.h"
#include "KrellInstitute/Messages/ThreadEvents.h"

#include "KrellInstitute/Services/Common.h"
#include "KrellInstitute/Services/Collector.h"
#include "KrellInstitute/Services/TLS.h"
#include "monitor.h" // monitor_get_thread_num

/* FIXME: defined in services/src/data/InitializeDataHeader.c. need include. */
extern void CBTF_InitializeDataHeader(int experiment, int collector,
                               CBTF_DataHeader* header);

#if defined(CBTF_SERVICE_USE_FILEIO)
/* FIXME: defined in services/src/xdr/Send.c. need include. */
extern void CBTF_Data_Send(const CBTF_DataHeader* header,
                 const xdrproc_t xdrproc, const void* data);
/* FIXME: defined in services/src/fileio/SendToFile.c. need include. */
extern void CBTF_SetSendToFile(CBTF_DataHeader* header, const char* unique_id,
                        const char* suffix);
#endif

/* FIXME: defined in services/src/mrnet/MRNet_Send.c. need include. */
#if defined(CBTF_SERVICE_USE_MRNET)
extern void CBTF_MRNet_Send(const int tag,
                 const xdrproc_t xdrproc, const void* data);
extern void CBTF_MRNet_Send_PerfData(const CBTF_DataHeader* header,
                 const xdrproc_t xdrproc, const void* data);
extern int CBTF_MRNet_LW_connect (const int con_rank);
#endif

/** Type defining the items stored in thread-local storage. */
typedef struct {

    CBTF_DataHeader header;  /**< Header for data blobs. */

    bool_t defer_sampling;
    bool is_openmp;
    bool has_ompt;
    bool is_mpi_job;
    bool is_threaded_job;

#if defined(CBTF_SERVICE_USE_MRNET)
    /** These are only valid with mrnet based collection. */
    CBTF_Protocol_ThreadNameGroup tgrp;
    CBTF_Protocol_ThreadName tname;
    CBTF_Protocol_AttachedToThreads attached_to_threads_message;
    CBTF_Protocol_ThreadsStateChanged thread_state_changed_message;

    bool connected_to_mrnet;
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
#endif

#ifdef USE_EXPLICIT_TLS

/**
 * Thread-local storage key.
 *
 * Key used for looking up our thread-local storage. This key <em>must</em>
 * be globally unique across the entire Open|SpeedShop code base.
 */
static const uint32_t TLSKey = 0x0000EEF3;

#else

/** Thread-local storage. */
static __thread TLS the_tls;

#endif

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



/** init_process_thread only valid with mrnet based collection. */
void init_process_thread()
{
#if defined(CBTF_SERVICE_USE_MRNET)
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
    tls->header.omp_tid = monitor_get_thread_num();
    tls->tname.omp_tid = monitor_get_thread_num();

    tls->tgrp.names.names_len = 0;
    tls->tgrp.names.names_val = tls->tgrpbuf.tnames;
    memset(tls->tgrpbuf.tnames, 0, sizeof(tls->tgrpbuf.tnames));

    memcpy(&(tls->tgrpbuf.tnames[tls->tgrp.names.names_len]),
           &tls->tname, sizeof(tls->tname));
    tls->tgrp.names.names_len++;

    //CBTF_Protocol_AttachedToThreads tmessage;
    tls->attached_to_threads_message.threads = tls->tgrp;
#ifndef NDEBUG
        if (tls->debug_mrnet) {
    	     fprintf(stderr,
	   "init_process_thread [%d] INIT THREAD OR PROCESS %s:%lld:%lld:%d:%d\n",
		     tls->tgrp.names.names_len,
                     tls->header.host, (long long)tls->header.pid,
                     (long long)tls->header.posix_tid,
		     tls->header.rank,
		     tls->header.omp_tid
	    );
        }
#endif
#endif
}

/** send_attached_to_threads_message only valid with mrnet based collection. */
void send_attached_to_threads_message()
{
#if defined(CBTF_SERVICE_USE_MRNET)
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
	   "SEND CBTF_PROTOCOL_TAG_ATTACHED_TO_THREADS, for %s:%lld:%lld:%d:%d\n",
                     tls->header.host, (long long)tls->header.pid,
                     (long long)tls->header.posix_tid,
		     tls->header.rank,
		     tls->header.omp_tid
	    );
        }
#endif
	cbtf_offline_service_stop_timer();
	CBTF_MRNet_Send( CBTF_PROTOCOL_TAG_ATTACHED_TO_THREADS,
			(xdrproc_t) xdr_CBTF_Protocol_AttachedToThreads,
			&tls->attached_to_threads_message);
	tls->sent_attached_to_threads = true;
	cbtf_offline_service_start_timer();
    }
#endif
}



/** set_threaded_mrnet_connection only valid with mrnet based collection. */
void set_threaded_mrnet_connection()
{
#if defined(CBTF_SERVICE_USE_MRNET)
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    if (tls == NULL)
	return;

    tls->connected_to_mrnet = true;
#endif
}


/** connect_to_mrnet only valid with mrnet based collection. */
void connect_to_mrnet()
{
#if defined(CBTF_SERVICE_USE_MRNET)
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
        fprintf(stderr,"[%d,%d] ALREADY connected  connect_to_mrnet\n",getpid(),monitor_get_thread_num());
    }
#endif
	return;
    }

#ifndef NDEBUG
    if (tls->debug_mrnet) {
	 fprintf(stderr,"[%d,%d] connect_to_mrnet() calling CBTF_MRNet_LW_connect for rank %d\n",
	getpid(),monitor_get_thread_num(),monitor_mpi_comm_rank());
    }
#endif

    CBTF_MRNet_LW_connect( monitor_mpi_comm_rank() );
    tls->header.rank = monitor_mpi_comm_rank();
    tls->header.omp_tid = monitor_get_thread_num();
    tls->connected_to_mrnet = true;

#ifndef NDEBUG
    if (tls->debug_mrnet) {
	 fprintf(stderr,
        "connect_to_mrnet reports connection successful for %s:%ld:%ld:%d:%d\n",
             tls->header.host, tls->header.pid,tls->header.posix_tid,tls->header.rank,tls->header.omp_tid);
    }
#endif

#endif
}

void cbtf_collector_set_openmp_threadid(int32_t omptid)
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    if (tls == NULL)
	return;


#if defined(CBTF_SERVICE_USE_MRNET)
    /* mrnet collectors send a threadname message that requires
     * it's own omp identifier.
    */
    tls->tname.omp_tid = omptid;
#endif
    tls->header.omp_tid = omptid;
    tls->has_ompt = true;
    tls->is_openmp = true;
}

int32_t cbtf_collector_get_openmp_threadid()
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    if (tls == NULL)
	return -1;

    // valid openmp thread id is 0 or greater...
    if (tls->header.omp_tid >= 0)
	return tls->header.omp_tid;
    else
	return -1;
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


// noop for non mrnet collection.
/** send_thread_state_changed_message only valid with mrnet based collection. */
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
        fprintf(stderr,"[%d,%d] EARLY EXIT send_thread_state_changed_message NO TLS for rank %d\n",
		getpid(),monitor_get_thread_num(),monitor_mpi_comm_rank());
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
	    "SEND CBTF_PROTOCOL_TAG_THREADS_STATE_CHANGED for %s:%lld:%lld:%d:%d\n",
                     tls->header.host, (long long)tls->header.pid,
                     (long long)tls->header.posix_tid,
		     tls->header.rank,
		     tls->header.omp_tid
		);
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
        fprintf(stderr,"[%d,%d] cbtf_collector_send DATA for %s:%lld:%lld:%d:%d\n",
		getpid(),monitor_get_thread_num(),
                tls->header.host, (long long)tls->header.pid,
                (long long)tls->header.posix_tid, tls->header.rank,
		tls->header.omp_tid);
        fprintf(stderr,"[%d,%d] time_range[%lu, %lu) addr range [%#lx, %#lx]\n",
	    getpid(),monitor_get_thread_num(),
            (uint64_t)header->time_begin, (uint64_t)header->time_end,
	    header->addr_begin, header->addr_end);
    }
#endif

/** The data send routines for performance data are specific to
 *  the data collection method.  Only one will be defined at build time.
 */
/** Data send support for --offline (fileio) mode of collection. */
#if defined(CBTF_SERVICE_USE_FILEIO)
    CBTF_Data_Send(header, xdrproc, data);
#endif

/** Data send support for ltwt mrnet mode of collection. */
#if defined(CBTF_SERVICE_USE_MRNET)
	if (tls->connected_to_mrnet) {
	    if (!tls->sent_attached_to_threads) {
	        send_attached_to_threads_message();
	        tls->sent_attached_to_threads = true;
	    }

	    CBTF_MRNet_Send_PerfData(header, xdrproc, data);
	} else {
	    fprintf(stderr,"[%d,%d] cbtf_collector_send called but no longer connected!!!\n",getpid(),monitor_get_thread_num());
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
    if (getenv("CBTF_DEBUG_OFFLINE_COLLECTOR") != NULL) {
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


#if defined (CBTF_SERVICE_USE_MRNET)
    tls->tname.experiment = 0;
    tls->tname.host = strdup(local_data_header.host);
    tls->tname.pid = local_data_header.pid;
    tls->tname.has_posix_tid = true;
    tls->tname.posix_tid = local_data_header.posix_tid;
    tls->tname.rank = local_data_header.rank;
    tls->tname.omp_tid = monitor_get_thread_num();

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
	fprintf(stderr,"[%d,%d] cbtf_timer_service_start_sampling calls connect_to_mrnet for NON MPI program\n",getpid(),monitor_get_thread_num());
    }
#endif
    connect_to_mrnet();
    if (tls->connected_to_mrnet) {
	if (!tls->sent_attached_to_threads) {
	    send_attached_to_threads_message();
	    tls->sent_attached_to_threads = true;
	}
    }
#endif

#endif

    tls->header.posix_tid = local_data_header.posix_tid;
    tls->header.rank = local_data_header.rank;
    tls->header.omp_tid = monitor_get_thread_num();

#if defined(CBTF_SERVICE_USE_FILEIO)
/* NOTE: this happens at process startup. mpi rank is not set yet
 * so the header info for mpi rank is incorrect in this header.
 * For FILEIO this must happen here since it uses async unsafe calls
 * like malloc.
 */
    CBTF_SetSendToFile(&(tls->header), cbtf_collector_unique_id, "openss-data");
#endif

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
