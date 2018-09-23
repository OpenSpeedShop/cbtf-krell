/*******************************************************************************
** Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
** Copyright (c) 2007,2008 William Hachfeld. All Rights Reserved.
** Copyright (c) 2007-2018 Krell Institute.  All Rights Reserved.
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
#include <sys/types.h>

#include "KrellInstitute/Services/Common.h"
#include "KrellInstitute/Services/Collector.h"
#include "KrellInstitute/Services/TLS.h"

#include "KrellInstitute/Messages/DataHeader.h"
#include "KrellInstitute/Messages/EventHeader.h"
#include "KrellInstitute/Messages/LinkedObjectEvents.h"
#include "KrellInstitute/Messages/OfflineEvents.h"
#include "KrellInstitute/Messages/ToolMessageTags.h"
#include "KrellInstitute/Messages/Thread.h"
#include "KrellInstitute/Messages/ThreadEvents.h"

#include "KrellInstitute/Services/Common.h"
#include "KrellInstitute/Services/Collector.h"
#include "KrellInstitute/Services/Data.h"
#include "KrellInstitute/Services/Monitor.h"
#if defined(CBTF_SERVICE_USE_MRNET)
#include "KrellInstitute/Services/MRNet.h"
#endif
#include "KrellInstitute/Services/Offline.h"
#include "KrellInstitute/Services/Path.h"
#if defined(CBTF_SERVICE_USE_FILEIO)
#include "KrellInstitute/Services/Fileio.h"
#include "KrellInstitute/Services/Send.h"
#endif
#include "KrellInstitute/Services/Time.h"
#include "KrellInstitute/Services/TLS.h"
#include "KrellInstitute/Services/Monitor.h"
#include "monitor.h" // monitor_get_thread_num

/* forward declaration */
void cbtf_reset_header_begin_time();

extern void cbtf_offline_finish();

#if defined(CBTF_SERVICE_USE_MRNET)
#include <pthread.h>
/**
 * Checks that the given pthread function call returns the value "0". If the
 * call was unsuccessful, the returned error is reported on the standard error
 * stream and the application is aborted.
 *
 * @param x    Pthread function call to be checked.
 **/
#define PTHREAD_CHECK(x)                                    \
    do {                                                    \
        int RETVAL = x;                                     \
        if (RETVAL != 0)                                    \
        {                                                   \
            fprintf(stderr, "[CUDA %d:%d] %s(): %s = %d\n", \
                    getpid(), monitor_get_thread_num(),     \
                    __func__, #x, RETVAL);                  \
            fflush(stderr);                                 \
            abort();                                        \
        }                                                   \
    } while (0)

/**
 * The number of threads for which are are collecting data (actively or not).
 * This value is atomically incremented in cbtf_timer_service_start_sampling,
 * decremented in cbtf_timer_service_stop_sampling, and is used by those functions
 * to determine when to call cbtf_waitfor_shutdown.
 */
static struct {
    int value;
    pthread_mutex_t mutex;
} NumThreads = { 0, PTHREAD_MUTEX_INITIALIZER };
#endif

/**
 * Only the master thread sets the connection that is shared by
 * any and all threads of a process. This is now global.
 */
bool connected_to_mrnet;

/* debug flags */
#ifndef NDEBUG
static bool IsCollectorDebugEnabled = false;
static bool IsCollectorDebugDsosEnabled = false;
static bool IsMRNetDebugEnabled = false;
#endif

/** Type defining the items stored in thread-local storage. */
typedef struct {

    CBTF_DataHeader header;  /**< Header for data blobs. */
    CBTF_EventHeader dso_header;   /**< Header for following dso blob. */
    CBTF_Monitor_Status sampling_status;

    uint64_t time_started;
    int  dsoname_len;
    int  sent_data;

    // marker if ANY collector is connected to mrnet.
    // this applies to the non mrnet builds.

    bool ompt_thread_finished;
    bool has_ompt;

#if defined(CBTF_SERVICE_USE_MRNET)

    /** These are only valid with mrnet based collection. */
    CBTF_Protocol_ThreadNameGroup tgrp;
    CBTF_Protocol_ThreadName tname;
    CBTF_Protocol_AttachedToThreads attached_to_threads_message;
    CBTF_Protocol_ThreadsStateChanged thread_state_changed_message;

    bool sent_attached_to_threads;

    struct {
        CBTF_Protocol_ThreadName tnames[4096];
    } tgrpbuf;

#endif

// TODO:  Adjust the OfflineExperiment class in OSS to use the
// same linkedobject code as the mrnet cbtf code.
#if defined(CBTF_SERVICE_USE_FILEIO)
    CBTF_Protocol_Offline_LinkedObjectGroup data; /**< Actual dso data blob. */

    struct {
	CBTF_Protocol_Offline_LinkedObject objs[CBTF_MAXLINKEDOBJECTS];
    } buffer;
#else
    CBTF_Protocol_LinkedObjectGroup data; /**< Actual dso data blob. */

    struct {
	CBTF_Protocol_LinkedObject objs[CBTF_MAXLINKEDOBJECTS];
    } buffer;
#endif

} TLS;

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

char* statusTostring (CBTF_Monitor_Status status)
{
    char* statusstr = "UNKNOWNSTATUS";
        switch(status) {
            case CBTF_Monitor_Resumed:
                statusstr = "RESUME";
                break;
            case CBTF_Monitor_Paused:
                statusstr = "PAUSE";
                break;
            case CBTF_Monitor_Started:
                statusstr = "STARTED";
                break;
            case CBTF_Monitor_Not_Started:
                statusstr = "NOTSTARTED";
                break;
            case CBTF_Monitor_Finished:
                statusstr = "FINISHED";
                break;
        }
    return statusstr;
}

void cbtf_offline_service_sampling_control(CBTF_Monitor_Status current_status)
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
    if (IsCollectorDebugEnabled) {
        fprintf(stderr,"[%d,%d] cbtf_offline_service_sampling_control passed_status:%s previous_status:%s\n",
		     getpid(),monitor_get_thread_num(),
		     statusTostring(current_status),statusTostring(tls->sampling_status));
    }
#endif

    if (current_status == CBTF_Monitor_Resumed && tls->sampling_status != CBTF_Monitor_Resumed) {
	cbtf_collector_resume();
    } else if (current_status == CBTF_Monitor_Paused && tls->sampling_status != CBTF_Monitor_Paused) {
	cbtf_collector_pause();
    } else {
    }
    tls->sampling_status = current_status;
}

#endif // defined CBTF_SERVICE_USE_OFFLINE



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

    tls->attached_to_threads_message.threads = tls->tgrp;
#ifndef NDEBUG
        if (IsMRNetDebugEnabled) {
	    fprintf(stderr,
	    "[%d,%d] init_process_thread [%d] INIT THREAD OR PROCESS %s:%lld:%lld:%d:%d\n",
		     getpid(),monitor_get_thread_num(),
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

    if (connected_to_mrnet && ! tls->sent_attached_to_threads) {
	init_process_thread();
#ifndef NDEBUG
        if (IsMRNetDebugEnabled) {
    	     fprintf(stderr,
	   "[%d,%d] SEND CBTF_PROTOCOL_TAG_ATTACHED_TO_THREADS, for %s:%lld:%lld:%d:%d\n",
		     getpid(),monitor_get_thread_num(),
                     tls->header.host, (long long)tls->header.pid,
                     (long long)tls->header.posix_tid,
		     tls->header.rank,
		     tls->header.omp_tid
	    );
        }
#endif
	// disable collector while doing internal work
	// Due to mpi_pcontrol we already may be stopped.
	// No harm to issue this again
	cbtf_offline_service_sampling_control(CBTF_Monitor_Paused);
	CBTF_MRNet_Send( CBTF_PROTOCOL_TAG_ATTACHED_TO_THREADS,
			(xdrproc_t) xdr_CBTF_Protocol_AttachedToThreads,
			&tls->attached_to_threads_message);
	tls->sent_attached_to_threads = true;
	// reenable collector
	// Due to mpi_pcontrol we already may already be stopped
	// due to the env var CBTF_START_ENABLE and need to check the 
	// the value set by the monitor service code.
	cbtf_offline_service_sampling_control(CBTF_Monitor_Resumed);
    }
#endif
}

/** connect_to_mrnet only valid with mrnet based collection.
 *  Since the master thread is the only connection to mrnet and
 *  and subsequent threads share that connection we use a global
 *  to maintain connection status.
 */
bool connect_to_mrnet()
{
#if defined(CBTF_SERVICE_USE_MRNET)
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    if (tls == NULL) {
	fprintf(stderr,"[%d,%d] Entered connect_to_mrnet with NO TLS! returning...\n",getpid(),monitor_get_thread_num());
	return false;
    }

    if (connected_to_mrnet) {
#ifndef NDEBUG
    if (IsMRNetDebugEnabled) {
        fprintf(stderr,"[%d,%d] ALREADY connected  connect_to_mrnet\n",getpid(),monitor_get_thread_num());
    }
#endif
	return true;
    }

#ifndef NDEBUG
    if (IsMRNetDebugEnabled) {
	 fprintf(stderr,"[%d,%d] connect_to_mrnet() calling CBTF_MRNet_LW_connect for rank %d\n",
	getpid(),monitor_get_thread_num(),monitor_mpi_comm_rank());
    }
#endif

    // FIXME: CBTF_MRNet_LW_connect really should return a bool connect status
    // and connected_to_mrnet should be set based on that.
    CBTF_MRNet_LW_connect( monitor_mpi_comm_rank() );
    tls->header.rank = monitor_mpi_comm_rank();
    tls->header.omp_tid = monitor_get_thread_num();
    connected_to_mrnet = true;

#ifndef NDEBUG
    if (IsMRNetDebugEnabled) {
	 fprintf(stderr,
        "connect_to_mrnet reports connection successful for %s:%ld:%ld:%d:%d\n",
             tls->header.host, tls->header.pid,tls->header.posix_tid,tls->header.rank,tls->header.omp_tid);
    }
#endif
    cbtf_reset_header_begin_time();
    return true;
#endif
    return true;
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
    tls->ompt_thread_finished = false;
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

bool get_ompt_flag()
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    if (tls == NULL)
	return false;

    return tls->has_ompt;
}

void set_ompt_thread_finished(bool flag)
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    if (tls == NULL)
	return;

    tls->ompt_thread_finished = flag;
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
    if (connected_to_mrnet) {
#ifndef NDEBUG
	if (IsMRNetDebugEnabled) {
            fprintf(stderr,
	    "[%d,%d] SEND CBTF_PROTOCOL_TAG_THREADS_STATE_CHANGED for %s:%lld:%lld:%d:%d\n",
		     getpid(),monitor_get_thread_num(),
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
    if (IsCollectorDebugEnabled) {
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
	if (connected_to_mrnet) {
	    if (!tls->sent_attached_to_threads) {
	        send_attached_to_threads_message();
	        tls->sent_attached_to_threads = true;
	    }

	    CBTF_MRNet_Send_PerfData(header, xdrproc, data);
	} else {
#ifndef NDEBUG
	    /** There are cases where a collector (eg. mem) can
             *  collect data from the mrnet connection code itself.
             *  We are not interested in such data.
             */
	    if (IsMRNetDebugEnabled) {
		fprintf(stderr,"[%d,%d] cbtf_collector_send called with no mrnet connection!\n"
		,getpid(),monitor_get_thread_num());
	    }
#endif
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

    /* All collectors start with collection enabled.  For mpi programs
     * this flag can be changed to reflect mpi_pcontrol calls and the
     * related mpi_pcontrol environment variables.
     */
    tls->sampling_status=CBTF_Monitor_Not_Started;
    tls->time_started=CBTF_GetTime();

#ifndef NDEBUG
    IsCollectorDebugEnabled = (getenv("CBTF_DEBUG_COLLECTOR") != NULL);
    IsCollectorDebugDsosEnabled = (getenv("CBTF_DEBUG_COLLECTOR_DSOS") != NULL);
    IsMRNetDebugEnabled = (getenv("CBTF_DEBUG_LW_MRNET") != NULL);
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

    // init DSOS data
    tls->data.linkedobjects.linkedobjects_len = 0;
    tls->data.linkedobjects.linkedobjects_val = tls->buffer.objs;
    tls->dsoname_len = 0;
    memset(tls->buffer.objs, 0, sizeof(tls->buffer.objs));

#if defined (CBTF_SERVICE_USE_MRNET)
    PTHREAD_CHECK(pthread_mutex_lock(&NumThreads.mutex));
    NumThreads.value++;
#ifndef NDEBUG
    if (IsMRNetDebugEnabled) {
	fprintf(stderr,"[%d,%d] cbtf_timer_service_start_sampling NumThreads:%d\n",getpid(),monitor_get_thread_num(),NumThreads.value);
    }
#endif
    PTHREAD_CHECK(pthread_mutex_unlock(&NumThreads.mutex));

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

    tls->sent_attached_to_threads = false;

#if !defined (CBTF_SERVICE_USE_MRNET_MPI)
    // Non-mpi sequential applications connect here in cbtf mode.
    // Only the master thread makes mrnet connection.
    if (NumThreads.value == 1) {
#ifndef NDEBUG
	if (IsMRNetDebugEnabled) {
	    fprintf(stderr,
		"[%d,%d] cbtf_timer_service_start_sampling calls connect_to_mrnet for sequential program\n",
		getpid(),monitor_get_thread_num());
	}
#endif
        connected_to_mrnet = false;
	connected_to_mrnet = connect_to_mrnet();
    }

    if (connected_to_mrnet) {
#ifndef NDEBUG
	if (IsMRNetDebugEnabled) {
	    fprintf(stderr,
		"[%d,%d] cbtf_timer_service_start_sampling sequential program connected_to_mrnet:%d\n",
		getpid(),monitor_get_thread_num(),connected_to_mrnet);
	}
#endif
	cbtf_set_connected_to_mrnet(true); /* inform monitor callbacks */
	if (!tls->sent_attached_to_threads) {
	    send_attached_to_threads_message();
	    tls->sent_attached_to_threads = true;
	}
    }
#endif

#endif // defined CBTF_SERVICE_USE_MRNET

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

#ifndef NDEBUG
    if (IsMRNetDebugEnabled) {
	fprintf(stderr,"[%d,%d] cbtf_timer_service_start_sampling calls cbtf_collector_start.\n",getpid(),monitor_get_thread_num());
    }
#endif
    /* Begin collection */
    tls->sampling_status = CBTF_Monitor_Started;
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

    /* used to Assert(tls != NULL); */
    if (tls == NULL) {
	return;
    }

    if (tls->has_ompt && !tls->ompt_thread_finished) {
#ifndef NDEBUG
	if (IsMRNetDebugEnabled) {
	fprintf(stderr,"[%d,%d] cbtf_timer_service_stop_sampling returns ACTIVE OMP THREAD. has_ompt:%d ompt_thread_finished:%d\n"
		,getpid(),monitor_get_thread_num(),tls->has_ompt,tls->ompt_thread_finished);
	}
#endif
	return;
    } else {
#ifndef NDEBUG
	if (IsMRNetDebugEnabled) {
	fprintf(stderr,"[%d,%d] cbtf_timer_service_stop_sampling calls cbtf_collector_stop. has_ompt:%d ompt_thread_finished:%d\n"
		,getpid(),monitor_get_thread_num(),tls->has_ompt,tls->ompt_thread_finished);
	}
#endif

    }

    /* Stop collection */
    cbtf_collector_stop();
    tls->sampling_status = CBTF_Monitor_Finished;

#if defined(CBTF_SERVICE_USE_FILEIO)
    cbtf_offline_finish();
#else

    if (tls->sent_attached_to_threads) {
    // FIXME: Only record these if we sent data!
    cbtf_record_dsos();
    // FIXME: Only send this if we sent data and sent attached message!
    // send thread_state message ONLY after recording dsos
    send_thread_state_changed_message();
    }

    PTHREAD_CHECK(pthread_mutex_lock(&NumThreads.mutex));
    NumThreads.value--;
#ifndef NDEBUG
    if (IsMRNetDebugEnabled) {
	fprintf(stderr,"[%d,%d] cbtf_timer_service_stop_sampling NumThreads:%d\n",getpid(),monitor_get_thread_num(),NumThreads.value);
    }
#endif

    if (NumThreads.value == 0) {
	CBTF_Waitfor_MRNet_Shutdown();
    }
    PTHREAD_CHECK(pthread_mutex_unlock(&NumThreads.mutex));
#endif

    /* Destroy our thread-local storage */
#ifdef CBTF_SERVICE_USE_EXPLICIT_TLS
    free(tls);
    CBTF_SetTLS(TLSKey, NULL);
#endif
}

/**
 * When running with the cbtf intrumentation we incur overhead
 * from the mrnet connection time.  Since our data headers are
 * created when monitor_init_process first calls our collector
 * we therefore have this overhead in the performance data span.
 * To eliminate this overhead we need to reset our data header
 * time_begin to the time right after the mrnet connection completes.
 * For mpi programs this is actually just after the mpi rank is
 * set for the process.  This does not affect threads with in a
 * process since they share the process wide mrnet connection.
 */
void cbtf_reset_header_begin_time()
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif

    Assert(tls != NULL);
    tls->header.time_begin = CBTF_GetTime();
}

// DSOS

void cbtf_offline_send_dsos(TLS *tls)
{
    /* Send the offline "dsos" blob or message */
#ifndef NDEBUG
    if (IsCollectorDebugEnabled) {
        fprintf(stderr,
		"[%d,%d] cbtf_offline_send_dsos SENDS DSOS for %s:%lld:%lld:%d:%d\n",
		getpid(),monitor_get_thread_num(),
                tls->dso_header.host, (long long)tls->dso_header.pid, 
                (long long)tls->dso_header.posix_tid, tls->dso_header.rank,tls->dso_header.omp_tid);
    }
#endif

#if defined(CBTF_SERVICE_USE_FILEIO)
    CBTF_Event_Send(&(tls->dso_header),
		(xdrproc_t)xdr_CBTF_Protocol_Offline_LinkedObjectGroup,
		&(tls->data));
#endif

#if defined(CBTF_SERVICE_USE_MRNET_MPI)
    tls->data.thread = tls->tname;
    if (connected_to_mrnet) {
        CBTF_MRNet_Send( CBTF_PROTOCOL_TAG_LINKED_OBJECT_GROUP,
                  (xdrproc_t) xdr_CBTF_Protocol_LinkedObjectGroup,&(tls->data));
    }
#elif defined(CBTF_SERVICE_USE_MRNET)
    tls->data.thread = tls->tname;
    if (connected_to_mrnet) {
        CBTF_MRNet_Send( CBTF_PROTOCOL_TAG_LINKED_OBJECT_GROUP,
                  (xdrproc_t) xdr_CBTF_Protocol_LinkedObjectGroup,&(tls->data));
    }
#endif

    tls->data.linkedobjects.linkedobjects_len = 0;
    tls->data.linkedobjects.linkedobjects_val = tls->buffer.objs;
    tls->dsoname_len = 0;
    memset(tls->buffer.objs, 0, sizeof(tls->buffer.objs));
}

void cbtf_record_dsos()
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);

#if defined(CBTF_SERVICE_USE_FILEIO)
    /* If this thread did not record performance data then
     * there is no need to find dsos
     */
    if (!tls->sent_data) {
	//return;
    }
#endif

#ifndef NDEBUG
    if (IsCollectorDebugDsosEnabled) {
        fprintf(stderr, "[%d,%d] cbtf_record_dsos entered.\n",getpid(),monitor_get_thread_num());
    }
#endif

    /* Initialize header */
    CBTF_EventHeader local_header;
    CBTF_InitializeEventHeader(&local_header);
    local_header.rank = monitor_mpi_comm_rank();
    local_header.omp_tid = monitor_get_thread_num();

    /* Write the thread's initial address space to the appropriate buffer */
#ifndef NDEBUG
    if (IsCollectorDebugDsosEnabled) {
	fprintf(stderr,"[%d,%d] cbtf_record_dsos calls GETDLINFO for %s:%lld:%lld:%d:%d\n",
		getpid(),monitor_get_thread_num(),
		local_header.host, (long long)local_header.pid,
		(long long)local_header.posix_tid, local_header.rank, local_header.omp_tid);
    }
#endif
    CBTF_GetDLInfo(getpid(), NULL, 0, 0);

    if(tls->data.linkedobjects.linkedobjects_len > 0) {
#ifndef NDEBUG
	if (IsCollectorDebugDsosEnabled) {
           fprintf(stderr,
            "[%d,%d] cbtf_record_dsos HAS %d OBJS for %s:%lld:%lld:%d:%d\n",
		getpid(),monitor_get_thread_num(),
		   tls->data.linkedobjects.linkedobjects_len,
                   local_header.host, (long long)local_header.pid, 
                   (long long)local_header.posix_tid, local_header.rank, local_header.omp_tid);
	}
#endif
	cbtf_offline_send_dsos(tls);
    }
}


/**
 * Record a DSO operation.
 *
 * Writes information regarding a DSO being loaded or unloaded in the thread
 * to the appropriate file.
 *
 * @param dsoname      Name of the DSO's file.
 * @param begin        Beginning address at which this DSO was loaded.
 * @param end          Ending address at which this DSO was loaded.
 * @param is_dlopen    Boolean "true" if this DSO was just opened,
 *                     or "false" if it was just closed.
 */
void cbtf_offline_record_dso(const char* dsoname,
			uint64_t begin, uint64_t end,
			uint8_t is_dlopen)
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);


    if (is_dlopen) {
	//cbtf_offline_pause_sampling(CBTF_Monitor_Default_event);
    }


    /* Initialize the offline "dso" blob's header */
    CBTF_EventHeader local_header;
    CBTF_InitializeEventHeader(&local_header);
    local_header.rank = monitor_mpi_comm_rank();
    local_header.omp_tid = monitor_get_thread_num();
    memcpy(&tls->dso_header, &local_header, sizeof(CBTF_EventHeader));

#if defined(CBTF_SERVICE_USE_FILEIO)
    CBTF_Protocol_Offline_LinkedObject objects;

    if (is_dlopen) {
	objects.time_begin = CBTF_GetTime();
    } else {
	objects.time_begin = tls->time_started;
    }

    /* Initialize the offline "dso" blob */
    objects.objname = strdup(dsoname);
    objects.addr_begin = begin;
    objects.addr_end = end;
    objects.is_open = is_dlopen;
    if (strcmp(CBTF_GetExecutablePath(),dsoname)) {
	objects.is_executable = false;
    } else {
	objects.is_executable = true;
    }

    objects.time_end = is_dlopen ? -1ULL : CBTF_GetTime();

#ifndef NDEBUG
    if (IsCollectorDebugDsosEnabled) {
	fprintf(stderr,"cbtf_offline_record_dso fileio: dso:%s is_dlopen:%d time_begin:%ld time_end:%ld addr_begin:%lx addr_end:%lx is_exe:%d\n",
	dsoname, is_dlopen, objects.time_begin,objects.time_end,objects.addr_begin,objects.addr_end,objects.is_executable);
    }
#endif

#else
    /* Initialize the "dso" message */
    /* this is only for the initial dsos loaded into thread
     * and not intended for dlopen/dlclose callbacks
     */ 
    CBTF_Protocol_LinkedObject objects;

    if (is_dlopen) {
	objects.time_begin = CBTF_GetTime();
    } else {
	objects.time_begin = tls->time_started;
    }

    CBTF_Protocol_FileName dsoFilename;
    dsoFilename.path = strdup(dsoname);
    objects.linked_object = dsoFilename;

    objects.range.begin = begin;
    objects.range.end = end;

    if (strcmp(CBTF_GetExecutablePath(),dsoname)) {
	objects.is_executable = false;
    } else {
	objects.is_executable = true;
    }

    objects.time_end = is_dlopen ? -1ULL : CBTF_GetTime();

#ifndef NDEBUG
    if (IsCollectorDebugDsosEnabled) {
	fprintf(stderr,"cbtf_offline_record_dso cbtf: dso:%s is_dlopen:%d time_begin:%ld time_end:%ld addr_begin:%lx addr_end:%lx is_exe:%d\n",
	dsoname, is_dlopen,objects.time_begin,objects.time_end,objects.range.begin,objects.range.end,objects.is_executable);
    }
#endif

#endif

#if defined(CBTF_SERVICE_USE_MRNET)
    /*
     * intended ONLY for dlopen/dlclose callbacks
     */
    CBTF_Protocol_ThreadName tname;
    tname.experiment = 0;
    tname.host = strdup(local_header.host);
    tname.pid = local_header.pid;
    tname.has_posix_tid = true;
    tname.posix_tid = local_header.posix_tid;
    tname.rank = monitor_mpi_comm_rank();
    tname.omp_tid = monitor_get_thread_num();
    memcpy(&(tls->tname), &tname, sizeof(tname));

    tls->tgrp.names.names_len = 0;
    tls->tgrp.names.names_val = tls->tgrpbuf.tnames;
    memset(tls->tgrpbuf.tnames, 0, sizeof(tls->tgrpbuf.tnames));

    memcpy(&(tls->tgrpbuf.tnames[tls->tgrp.names.names_len]),
           &tname, sizeof(tname));
    tls->tgrp.names.names_len++;


    /*
     * intended ONLY for dlopen/dlclose callbacks
     * Message tag: CBTF_PROTOCOL_TAG_LOADED_LINKED_OBJECT
     * xdr_CBTF_Protocol_LoadedLinkedObject
     */
    CBTF_Protocol_LoadedLinkedObject message;
    memset(&message, 0, sizeof(message));

    message.threads = tls->tgrp;
    message.time = objects.time_begin;
    message.range.begin =  begin;
    message.range.end = end;
    message.linked_object = dsoFilename;

    if (strcmp(CBTF_GetExecutablePath(),dsoname)) {
	message.is_executable = false;
    } else {
	message.is_executable = true;
    }


//FIXME: verify that mrnet collections works for dlopen/dlclose.
#if 0
#if defined(CBTF_SERVICE_USE_MRNET)
#ifndef NDEBUG
    if (getenv("CBTF_DEBUG_OFFLINE_COLLECTOR") != NULL) {
            fprintf(stderr,
		"cbtf_offline_record_dso SENDS CBTF_PROTOCOL_TAG_LOADED_LINKED_OBJECT for dso %s from %s %lld %lld\n",
                    dsoFilename.path, tls->dso_header.host,
                    (long long)tls->dso_header.pid,
                    (long long)tls->dso_header.posix_tid);
    }
#endif
#endif

#if defined(CBTF_SERVICE_USE_MRNET_MPI)
    if (connected_to_mrnet) {
        CBTF_MRNet_Send( CBTF_PROTOCOL_TAG_LOADED_LINKED_OBJECT,
                  (xdrproc_t) xdr_CBTF_Protocol_LoadedLinkedObject, &message);
    }
#else
    CBTF_MRNet_Send( CBTF_PROTOCOL_TAG_LOADED_LINKED_OBJECT,
                  (xdrproc_t) xdr_CBTF_Protocol_LoadedLinkedObject, &message);
#endif
#endif
#endif

    int dsoname_len = strlen(dsoname);
    int newsize = (tls->data.linkedobjects.linkedobjects_len * sizeof(objects))
		  + (tls->dsoname_len + dsoname_len);

    if(newsize > CBTF_MAXLINKEDOBJECTS * sizeof(objects)) {
#ifndef NDEBUG
	if (IsCollectorDebugEnabled) {
            fprintf(stderr,"[%d,%d] cbtf_offline_record_dso SENDS OBJS for %s:%lld:%lld:%d:%d\n",
		    getpid(),monitor_get_thread_num(),
                    tls->dso_header.host, (long long)tls->dso_header.pid, 
                    (long long)tls->dso_header.posix_tid,
		    tls->dso_header.rank,
                    tls->dso_header.omp_tid);
	}
#endif
	cbtf_offline_send_dsos(tls);
    }


// DEBUG.
#if defined(CBTF_SERVICE_USE_MRNET) && 0
    fprintf(stderr,"RECORD OBJ to tls->buffer.objs INDEX %d\n",
	tls->data.linkedobjects.linkedobjects_len);
    fprintf(stderr,"RECORD OBJ objname = %s\n",objects.linked_object);
    fprintf(stderr,"RECORD OBJ addresses = %#lx,%#lx\n",objects.range.begin, objects.range.end);
    fprintf(stderr,"RECORD OBJ times = %lu,%lu\n",objects.time_begin, objects.time_end);
#endif

    memcpy(&(tls->buffer.objs[tls->data.linkedobjects.linkedobjects_len]),
           &objects, sizeof(objects));
    tls->data.linkedobjects.linkedobjects_len++;
    tls->dsoname_len += dsoname_len;

    if (is_dlopen) {
//	cbtf_offline_sampling_status(CBTF_Monitor_Default_event,CBTF_Monitor_Resumed);
    }
}

/**
 * Record a dlopened library.
 *
 * Writes information regarding a DSO that was dlopened/dlclosed  in the thread
 * to the appropriate file.
 *
 * @param dsoname      Name of the DSO's file.
 * @param begin        Beginning address at which this DSO was loaded.
 * @param end          Ending address at which this DSO was loaded.
 * @param b_time       Load time
 * @param e_time       Unload time
 */
void cbtf_offline_record_dlopen(const char* dsoname,
			uint64_t begin, uint64_t end,
			uint64_t b_time, uint64_t e_time)
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);

    //cbtf_offline_pause_sampling(CBTF_Monitor_Default_event);

    /* Initialize the offline "dso" blob's header */
    CBTF_EventHeader local_header;
    CBTF_InitializeEventHeader(&local_header);
    local_header.rank = monitor_mpi_comm_rank();
    local_header.omp_tid = monitor_get_thread_num();
    memcpy(&tls->dso_header, &local_header, sizeof(CBTF_EventHeader));

#if defined(CBTF_SERVICE_USE_FILEIO)
    CBTF_Protocol_Offline_LinkedObject objects;

    /* Initialize the offline "dso" blob */
    objects.objname = strdup(dsoname);
    objects.addr_begin = begin;
    objects.addr_end = end;
    objects.time_begin = b_time;
    objects.time_end = e_time;
    objects.is_open = true;

#else
    /* Initialize the "dso" message */
    /* this is only for the initial dsos loaded into thread
     * and not intended for dlopen/dlclose callbacks
     */ 
    CBTF_Protocol_LinkedObject objects;


    CBTF_Protocol_FileName dsoFilename;
    dsoFilename.path = strdup(dsoname);
    objects.linked_object = dsoFilename;

    objects.range.begin = begin;
    objects.range.end = end;
    objects.time_begin = b_time;
    objects.time_end = e_time;

    if (strcmp(CBTF_GetExecutablePath(),dsoname)) {
	objects.is_executable = false;
    } else {
	objects.is_executable = true;
    }
#endif

#if defined(CBTF_SERVICE_USE_MRNET)
    /*
     * intended ONLY for dlopen/dlclose callbacks
     */
    CBTF_Protocol_ThreadName tname;
    tname.experiment = 0;
    tname.host = strdup(local_header.host);
    tname.pid = local_header.pid;
    tname.has_posix_tid = true;
    tname.posix_tid = local_header.posix_tid;
    tname.rank = monitor_mpi_comm_rank();
    tname.omp_tid = monitor_get_thread_num();
    memcpy(&(tls->tname), &tname, sizeof(tname));

    tls->tgrp.names.names_len = 0;
    tls->tgrp.names.names_val = tls->tgrpbuf.tnames;
    memset(tls->tgrpbuf.tnames, 0, sizeof(tls->tgrpbuf.tnames));

    memcpy(&(tls->tgrpbuf.tnames[tls->tgrp.names.names_len]),
           &tname, sizeof(tname));
    tls->tgrp.names.names_len++;


    /*
     * intended ONLY for dlopen/dlclose callbacks
     * Message tag: CBTF_PROTOCOL_TAG_LOADED_LINKED_OBJECT
     * xdr_CBTF_Protocol_LoadedLinkedObject
     */
    CBTF_Protocol_LoadedLinkedObject message;
    memset(&message, 0, sizeof(message));

    message.threads = tls->tgrp;
    message.time = objects.time_begin;
    message.range.begin =  begin;
    message.range.end = end;
    message.linked_object = dsoFilename;

    if (strcmp(CBTF_GetExecutablePath(),dsoname)) {
	message.is_executable = false;
    } else {
	message.is_executable = true;
    }

#endif

    int dsoname_len = strlen(dsoname);
    int newsize = (tls->data.linkedobjects.linkedobjects_len * sizeof(objects))
		  + (tls->dsoname_len + dsoname_len);

    if(newsize > CBTF_MAXLINKEDOBJECTS * sizeof(objects)) {
#ifndef NDEBUG
	if (IsCollectorDebugEnabled) {
            fprintf(stderr,"[%d,%d] cbtf_offline_record_dlopen SENDS OBJS for %s:%lld:%lld:%d:%d\n",
		    getpid(),monitor_get_thread_num(),
                    tls->dso_header.host, (long long)tls->dso_header.pid, 
                    (long long)tls->dso_header.posix_tid,
		    tls->dso_header.rank,
		    tls->dso_header.omp_tid
		    );
	}
#endif
	cbtf_offline_send_dsos(tls);
    }

    memcpy(&(tls->buffer.objs[tls->data.linkedobjects.linkedobjects_len]),
           &objects, sizeof(objects));
    tls->data.linkedobjects.linkedobjects_len++;
    tls->dsoname_len += dsoname_len;

    //cbtf_offline_sampling_status(CBTF_Monitor_Default_event,CBTF_Monitor_Resumed);
}
