/*******************************************************************************
** Copyright (c) The Krell Institute 2007-2018. All Rights Reserved.
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
#include <sys/types.h>
#include <unistd.h>

#include "KrellInstitute/Messages/EventHeader.h"
#include "KrellInstitute/Messages/LinkedObjectEvents.h"
#include "KrellInstitute/Messages/OfflineEvents.h"
#include "KrellInstitute/Messages/ToolMessageTags.h"
#include "KrellInstitute/Messages/Thread.h"
#include "KrellInstitute/Messages/ThreadEvents.h"

#include "KrellInstitute/Services/Common.h"
#include "KrellInstitute/Services/Collector.h"
#include "KrellInstitute/Services/Monitor.h"

#if defined(CBTF_SERVICE_USE_MRNET_MPI) || defined(CBTF_SERVICE_USE_MRNET)
#include "KrellInstitute/Services/MRNet.h"
#endif

#include "KrellInstitute/Services/Offline.h"

#if defined(CBTF_SERVICE_USE_FILEIO)
#include "KrellInstitute/Services/Path.h"
#endif

#include "KrellInstitute/Services/Send.h"
#include "KrellInstitute/Services/Time.h"
#include "KrellInstitute/Services/TLS.h"

#if defined(CBTF_SERVICE_USE_MRNET_MPI) || defined(CBTF_SERVICE_USE_MRNET)
extern void send_attached_to_threads_message();
#endif

// forward declarations.
void cbtf_offline_finish();

/** Type defining the items stored in thread-local storage. */
typedef struct {

    bool started;
    int  finished; // is this needed.
    int  sent_data; // is this needed.

    // marker if ANY collector is connected to mrnet.
    // this applies to the non mrnet builds.

} TLS;

static bool mpi_init_done;
static bool connected_to_mrnet;

/* debug flags */
#ifndef NDEBUG
static bool IsCollectorDebugEnabled = false;
#endif

#ifdef USE_EXPLICIT_TLS

/**
 * Thread-local storage key.
 *
 * Key used for looking up our thread-local storage. This key <em>must</em>
 * be globally unique across the entire Open|SpeedShop code base.
  */

static const uint32_t TLSKey = 0x0000FEF3;

#else

/** Thread-local storage. */
static __thread TLS the_tls;

#endif

// non mrnet builds just refurn true here..
bool cbtf_connected_to_mrnet() {
#if defined(CBTF_SERVICE_USE_MRNET)
    /* Access our thread-local storage */
    return connected_to_mrnet;
#else
    return true;
#endif
}

// non mrnet builds just refurn true here..
bool cbtf_mpi_init_done() {
    /* Access our thread-local storage */
    return mpi_init_done;
}

void cbtf_set_connected_to_mrnet( bool flag)
{
    connected_to_mrnet = flag;
}


/**
 * This is visable to offline (fileio) and cbtf based collection.
 */
void cbtf_offline_sampling_status(CBTF_Monitor_Event_Type event, CBTF_Monitor_Status status)
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    //Assert(tls != NULL);


#ifndef NDEBUG
    char* statusstr = "UNKNOWNSTATUS";
    if (IsCollectorDebugEnabled) {
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
    }
#endif

    if (tls == NULL) {
#ifndef NDEBUG
        if (IsCollectorDebugEnabled) {
	   fprintf(stderr,"[%d,%d] cbtf_offline_sampling_status NO TLS passed status:%s\n",
			getpid(),monitor_get_thread_num(),statusstr);
	}
#endif
	return;
    }

    switch( event ) {
	case CBTF_Monitor_MPI_pre_init_event:
#ifndef NDEBUG
	    if (IsCollectorDebugEnabled) {
	        fprintf(stderr,"[%d,%d] cbtf_offline_sampling_status CBTF_Monitor_MPI_pre_init_event status:%s\n",
			getpid(),monitor_get_thread_num(),statusstr);
	    }
#endif
	    if (status == CBTF_Monitor_Paused && tls->started) {
		cbtf_offline_service_sampling_control(CBTF_Monitor_Paused);
	    } else if (status == CBTF_Monitor_Resumed && tls->started) {
		cbtf_offline_service_sampling_control(CBTF_Monitor_Resumed);
	    }
	    break;
	case CBTF_Monitor_MPI_init_event:
#ifndef NDEBUG
	    if (IsCollectorDebugEnabled) {
	        fprintf(stderr,"[%d,%d] cbtf_offline_sampling_status CBTF_Monitor_MPI_init_event status:%s\n",
			getpid(),monitor_get_thread_num(),statusstr);
	    }
#endif
	    if (status == CBTF_Monitor_Paused && tls->started) {
		cbtf_offline_service_sampling_control(CBTF_Monitor_Paused);
	    } else if (status == CBTF_Monitor_Resumed && tls->started) {
		mpi_init_done = true;
		cbtf_offline_service_sampling_control(CBTF_Monitor_Resumed);
	    }
	    break;
	case CBTF_Monitor_MPI_post_comm_rank_event:
#ifndef NDEBUG
	    if (IsCollectorDebugEnabled) {
	        fprintf(stderr,
		"[%d,%d] cbtf_offline_sampling_status CBTF_Monitor_MPI_post_comm_rank_event status:%s tls->started:%d\n"
		,getpid(),monitor_get_thread_num(),statusstr,tls->started);
	    }
#endif
	    if ( !cbtf_connected_to_mrnet() && ( monitor_mpi_comm_rank() >= 0 || mpi_init_done)) {
	        if (tls->started) {
		cbtf_offline_service_sampling_control(CBTF_Monitor_Paused);
		}
#ifndef NDEBUG
		if (IsCollectorDebugEnabled) {
	            fprintf(stderr,
		    "[%d,%d] cbtf_offline_sampling_status CBTF_Monitor_MPI_post_comm_rank_event calls connect_to_mrnet\n",
		    getpid(),monitor_get_thread_num());
		}
#endif
#if defined(CBTF_SERVICE_USE_MRNET_MPI)
	        bool connect_success = connect_to_mrnet();
		// The sending of attached threads was previously
		// defered until mpi job was terminating. For large
		// mpi jobs it is more efficient to send this message
		// as soon as mpi init has provided a rank and there
		// is not as much message traffic over mrnet.
		// We do not send the dso list (addressspace) until
		// the job has terminated since that list will be pruned
		// of dsos for which no sample or callstack addresses
		// are found.
		if (connect_success) {
		    cbtf_set_connected_to_mrnet(true);
		    send_attached_to_threads_message();
		} else {
		    cbtf_set_connected_to_mrnet(false);
		}
#endif
		// FIXME: verify pcontrol...
		// we really do not want to resume here IFF not start_enabled.
	        if (tls->started) {
		    cbtf_offline_service_sampling_control(CBTF_Monitor_Resumed);
		}
	    }
	    break;
	case CBTF_Monitor_MPI_fini_event:
#ifndef NDEBUG
	    if (IsCollectorDebugEnabled) {
	        fprintf(stderr,"[%d,%d] cbtf_offline_sampling_status CBTF_Monitor_MPI_fini_event status:%s\n",
			getpid(),monitor_get_thread_num(),statusstr);
	    }
#endif
	    if (status == CBTF_Monitor_Paused && tls->started) {
		cbtf_offline_service_sampling_control(CBTF_Monitor_Paused);
	    } else if (status == CBTF_Monitor_Paused && tls->started) {
		cbtf_offline_service_sampling_control(CBTF_Monitor_Resumed);
	    }
	    break;
	case CBTF_Monitor_MPI_post_fini_event:
#ifndef NDEBUG
	    if (IsCollectorDebugEnabled) {
	        fprintf(stderr, "[%d,%d] cbtf_offline_sampling_status CBTF_Monitor_MPI_post_fini_event status:%s\n",
		getpid(),monitor_get_thread_num(),statusstr);
	    }
#endif
	    if (status == CBTF_Monitor_Paused && tls->started) {
		cbtf_offline_service_sampling_control(CBTF_Monitor_Paused);
	    } else if (status == CBTF_Monitor_Paused && tls->started) {
		cbtf_offline_service_sampling_control(CBTF_Monitor_Resumed);
	    }
	    break;
	case CBTF_Monitor_init_thread_event:
#ifndef NDEBUG
	    if (IsCollectorDebugEnabled) {
	        fprintf(stderr,"[%d,%d] cbtf_offline_sampling_status CBTF_Monitor_init_thread_event status:%s\n",
		getpid(),monitor_get_thread_num(),statusstr);
	    }
#endif
	    if (status == CBTF_Monitor_Paused && tls->started) {
		cbtf_offline_service_sampling_control(CBTF_Monitor_Paused);
	    } else if (status == CBTF_Monitor_Paused && tls->started) {
		cbtf_offline_service_sampling_control(CBTF_Monitor_Resumed);
	    }
	    break;
	case CBTF_Monitor_mpi_pcontrol_event:
#ifndef NDEBUG
	    if (IsCollectorDebugEnabled) {
	        fprintf(stderr,"[%d,%d] cbtf_offline_sampling_status CBTF_Monitor_mpi_pcontrol_event status:%s\n",
		getpid(),monitor_get_thread_num(),statusstr);
	    }
#endif
	    if (status == CBTF_Monitor_Paused && tls->started) {
		cbtf_offline_service_sampling_control(CBTF_Monitor_Paused);
	    } else if (status == CBTF_Monitor_Paused && tls->started) {
		cbtf_offline_service_sampling_control(CBTF_Monitor_Resumed);
	    }
	    break;
	case CBTF_Monitor_pre_dlopen_event:
#ifndef NDEBUG
	    if (IsCollectorDebugEnabled) {
	        fprintf(stderr,"[%d,%d] cbtf_offline_sampling_status CBTF_Monitor_pre_dlopen_event NOOP status:%s\n",
		getpid(),monitor_get_thread_num(),statusstr);
	    }
#endif
	    break;
	case CBTF_Monitor_dlopen_event:
#ifndef NDEBUG
	    if (IsCollectorDebugEnabled) {
	        fprintf(stderr,"[%d,%d] cbtf_offline_sampling_status CBTF_Monitor_dlopen_event NOOP status:%s\n",
		getpid(),monitor_get_thread_num(),statusstr);
	    }
#endif
	    break;
	case CBTF_Monitor_dlclose_event:
#ifndef NDEBUG
	    if (IsCollectorDebugEnabled) {
	        fprintf(stderr,"[%d,%d] cbtf_offline_sampling_status CBTF_Monitor_dlclose_event NOOP status:%s\n",
		getpid(),monitor_get_thread_num(),statusstr);
	    }
#endif
	    break;
	case CBTF_Monitor_post_dlclose_event:
#ifndef NDEBUG
	    if (IsCollectorDebugEnabled) {
	        fprintf(stderr,"[%d,%d] cbtf_offline_sampling_status CBTF_Monitor_post_dlclose_event NOOP status:%s\n",
		getpid(),monitor_get_thread_num(),statusstr);
	    }
#endif
	    break;
	case CBTF_Monitor_post_fork_event:
#ifndef NDEBUG
	    if (IsCollectorDebugEnabled) {
	        fprintf(stderr,"[%d,%d] cbtf_offline_sampling_status CBTF_Monitor_post_fork_event status:%s\n",
		getpid(),monitor_get_thread_num(),statusstr);
	    }
#endif
	    if (status == CBTF_Monitor_Paused && tls->started) {
		cbtf_offline_service_sampling_control(CBTF_Monitor_Paused);
	    } else if (status == CBTF_Monitor_Paused && tls->started) {
		cbtf_offline_service_sampling_control(CBTF_Monitor_Resumed);
	    }
	    break;

// pause and resume from CBTF_Monitor_thread_pre_create_event and
// CBTF_Monitor_thread_post_create_event callbacks are preventing the
// created threads from collecting data.
//
	case CBTF_Monitor_Default_event:
#ifndef NDEBUG
	    if (IsCollectorDebugEnabled) {
	        fprintf(stderr,"[%d,%d] cbtf_offline_sampling_status CBTF_Monitor_Default_event status:%s\n",
		getpid(),monitor_get_thread_num(),statusstr);
	    }
#endif
	    if (status == CBTF_Monitor_Paused && tls->started) {
		cbtf_offline_service_sampling_control(CBTF_Monitor_Paused);
	    } else if (status == CBTF_Monitor_Paused && tls->started) {
		cbtf_offline_service_sampling_control(CBTF_Monitor_Resumed);
	    }
	    break;
	default:
#ifndef NDEBUG
	    if (IsCollectorDebugEnabled) {
	        fprintf(stderr,"[%d,%d] cbtf_offline_sampling_status passed unknown event status:%s\n",
		getpid(),monitor_get_thread_num(),statusstr);
	    }
#endif
	    break;
    }
}

void cbtf_offline_sent_data(int sent_data)
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);

    tls->sent_data = sent_data;
}

/**
 * Start offline sampling.
 *
 * Starts program counter (PC) sampling for the thread executing this function.
 * Writes descriptive information for the thread to the appropriate file and
 * calls pcsamp_start_sampling() with the environment-specified arguments.
 *
 * @param in_arguments    Encoded function arguments. Always null.
 */
void cbtf_offline_start_sampling(const char* in_arguments)
{
    /* Create and access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
    if (tls == NULL) {
	tls = malloc(sizeof(TLS));
	Assert(tls != NULL);
	CBTF_SetTLS(TLSKey, tls);
    }
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);
#ifndef NDEBUG
    IsCollectorDebugEnabled = (getenv("CBTF_DEBUG_COLLECTOR_MONITOR") != NULL);
#endif

    mpi_init_done = false;

    /* Start sampling */
    tls->sent_data = 0;
    tls->finished = false;
    tls->started = false;

/* SEQUENTIAL cbtf mrnet collection */
#if defined(CBTF_SERVICE_USE_MRNET) && !defined(CBTF_SERVICE_USE_MRNET_MPI)
 /* FIXME: should query this from collector.c where connection
 * is made.
 */
#ifndef NDEBUG
    if (IsCollectorDebugEnabled) {
	fprintf(stderr,"[%d,%d] cbtf_offline_start_sampling BEGINS mrnet collection for sequential program\n",
		getpid(),monitor_get_thread_num());
    }
#endif

/* MPI cbtf mrnet collection */
#elif defined(CBTF_SERVICE_USE_MRNET) && defined(CBTF_SERVICE_USE_MRNET_MPI)
#ifndef NDEBUG
    if (IsCollectorDebugEnabled) {
	fprintf(stderr,"[%d,%d] cbtf_offline_start_sampling BEGINS mrnet collection for MPI program\n",
		getpid(),monitor_get_thread_num());
    }
#endif
/* OFFLINE collection */
#else
#ifndef NDEBUG
    if (IsCollectorDebugEnabled) {
	fprintf(stderr,"[%d,%d] cbtf_offline_start_sampling BEGINS fileio collection.\n",
		getpid(),monitor_get_thread_num());
    }
#endif
#endif

#ifndef NDEBUG
    if (IsCollectorDebugEnabled) {
	fprintf(stderr,"[%d,%d] cbtf_offline_start_sampling calls cbtf_timer_service_start_sampling\n",
		getpid(),monitor_get_thread_num());
    }
#endif
    cbtf_timer_service_start_sampling(NULL);
    tls->started = true;
}



/**
 * Stop offline sampling.
 *
 * Stops program counter (PC) sampling for the thread executing this function. 
 * Calls pcsamp_stop_sampling() and writes descriptive information for the
 * thread to the appropriate file.
 *
 * @param in_arguments    Encoded function arguments. Always null.
 */
void cbtf_offline_stop_sampling(const char* in_arguments, const bool finished)
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif

    if (!tls) {
#ifndef NDEBUG
	if (IsCollectorDebugEnabled) {
	    fprintf(stderr,"[%d,%d] warn: cbtf_offline_stop_sampling has no TLS\n",
		getpid(),monitor_get_thread_num());
	}
#endif
	return;
    }

    if (!tls->started) {
#ifndef NDEBUG
	if (IsCollectorDebugEnabled) {
	    fprintf(stderr,"[%d,%d] cbtf_offline_stop_sampling was not started. finished:%d\n",
		getpid(),monitor_get_thread_num(),finished);
	}
#endif
	return;
    }

    // fall though to stop sampling...
#ifndef NDEBUG
    if (IsCollectorDebugEnabled) {
	fprintf(stderr,"[%d,%d] cbtf_offline_stop_sampling was started. finished:%d\n",
		getpid(),monitor_get_thread_num(),finished);
    }
#endif

    /* Stop sampling */
    cbtf_timer_service_stop_sampling(NULL);

    // FIXME. is this finished flag needed anymore?
#ifndef NDEBUG
    if (IsCollectorDebugEnabled) {
	fprintf(stderr,"[%d,%d] cbtf_offline_stop_sampling finished:%d\n",
                getpid(), monitor_get_thread_num(), finished);
    }
#endif

    tls->finished = finished;
}

/**
 * This is visable to offline (fileio) and cbtf based collection.
 */
void cbtf_offline_notify_event(CBTF_Monitor_Event_Type event)
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);
    switch( event ) {
#if 0
	case CBTF_Monitor_MPI_pre_init_event:
#ifndef NDEBUG
	    if (IsCollectorDebugEnabled) {
	        fprintf(stderr,"[%d,%d] cbtf_offline_notify_event passed event CBTF_Monitor_MPI_pre_init_event\n",
		getpid(),monitor_get_thread_num());
	    }
#endif
	    // used to set an mpi flag.
	    break;
#endif
	case CBTF_Monitor_init_process_event:
#ifndef NDEBUG
	    if (IsCollectorDebugEnabled) {
	        fprintf(stderr,"[%d,%d] cbtf_offline_notify_event passed event CBTF_Monitor_init_process_event\n",
		getpid(),monitor_get_thread_num());
	    }
#endif
#if defined(CBTF_SERVICE_USE_MRNET_MPI)
	    // We can not connect in the MPI case until after the mpi program
	    // has call mpi_comm_rank.
	    cbtf_set_connected_to_mrnet(false);
#endif
	    break;

	case CBTF_Monitor_init_thread_event:
	    // threads share process wide mrnet connection.
#if defined(CBTF_SERVICE_USE_MRNET) || defined(CBTF_SERVICE_USE_MRNET_MPI)
#ifndef NDEBUG
	    if (IsCollectorDebugEnabled) {
	        fprintf(stderr,"[%d,%d] cbtf_offline_notify_event CBTF_Monitor_init_thread_event mrnet.\n",
			getpid(),monitor_get_thread_num());
	    }
#endif
	    if (cbtf_connected_to_mrnet()) {
		// We are potentially called early in an mpi program and
		// there is only an mrnet connection AFTER the mpi rank is
		// set in the master thread.  Therefore the send of thread attached needs to
		// really check that a connection exists.
		send_attached_to_threads_message();
	    }
#else
#ifndef NDEBUG
	    if (IsCollectorDebugEnabled) {
	        fprintf(stderr,"[%d,%d] cbtf_offline_notify_event CBTF_Monitor_init_thread_event non mrnet.\n",
			getpid(),monitor_get_thread_num());
	    }
#endif
#endif
	    break;
	case CBTF_Monitor_mpi_pcontrol_event:
#ifndef NDEBUG
	    if (IsCollectorDebugEnabled) {
	        fprintf(stderr,"[%d,%d] cbtf_offline_notify_event CBTF_Monitor_mpi_pcontrol_event.\n",
		getpid(),monitor_get_thread_num());
	    }
#endif
	    /* We must connect to mrnet if we are using mrnet collection */
	    if (!cbtf_connected_to_mrnet()) {
#ifndef NDEBUG
		if (IsCollectorDebugEnabled) {
		    fprintf(stderr,
			"[%d,%d] cbtf_offline_notify_event CBTF_Monitor_mpi_pcontrol_event. level 1 collector not started, connect...\n",
			getpid(),monitor_get_thread_num());
		}
#endif
		// Moved here from monitor services since it is not
		// mrnet aware. monitor services is never built for
		// fileio,mrnet,mrnet-mpi. Just one service for all.
#if defined(CBTF_SERVICE_USE_MRNET_MPI)
	        bool connect_success = connect_to_mrnet();
                // The sending of attached threads was previously
                // defered until mpi job was terminating. For large
                // mpi jobs it is more efficient to send this message
                // as soon as mpi init has provided a rank and there
                // is not as much message traffic over mrnet.
                // We do not send the dso list (addressspace) until
                // the job has terminated since that list will be pruned
                // of dsos for which no sample or callstack addresses
                // are found.
		if (connect_success) {
		    cbtf_set_connected_to_mrnet(true);
		    send_attached_to_threads_message();
		} else {
		    cbtf_set_connected_to_mrnet(false);
		}
#endif
	    }
	    break;
#if 0
	case CBTF_Monitor_fini_process_event:
#ifndef NDEBUG
	    if (IsCollectorDebugEnabled) {
	        fprintf(stderr,"[%d,%d] cbtf_offline_notify_event CBTF_Monitor_fini_process_event. NOOP\n",
		getpid(),monitor_get_thread_num());
	    }
#endif
	    break;
	case CBTF_Monitor_fini_thread_event:
#ifndef NDEBUG
	    if (IsCollectorDebugEnabled) {
	        fprintf(stderr,"[%d,%d] cbtf_offline_notify_event CBTF_Monitor_fini_thread_event. NOOP\n",
		getpid(),monitor_get_thread_num());
	    }
#endif
	    break;
#endif
	case CBTF_Monitor_MPI_init_event:
#ifndef NDEBUG
	    if (IsCollectorDebugEnabled) {
	        fprintf(stderr,"[%d,%d] cbtf_offline_notify_event CBTF_Monitor_MPI_init_event sets mpi_init_done true.\n",
		getpid(),monitor_get_thread_num());
	    }
#endif
	    // this is only meaningfull in the master thread for a rank.
	    mpi_init_done = true;
	    break;
	default:
	    break;
    }
}

// this is called for fileio based collection...
// FIXME: it could be factored out if tls->started is available
// in collector.c. see collector.c
void cbtf_offline_finish()
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
        fprintf(stderr, "[%d,%d] cbtf_offline_finish entered. started:%d\n",
		getpid(),monitor_get_thread_num(),tls->started);
    }
#endif

    if (!tls->started) {
	return;
    }

    cbtf_record_dsos();
}
