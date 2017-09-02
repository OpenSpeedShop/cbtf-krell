/*******************************************************************************
** Copyright (c) The Krell Institute 2007-2013. All Rights Reserved.
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
#include "KrellInstitute/Services/Offline.h"
#include "KrellInstitute/Services/Path.h"
#include "KrellInstitute/Services/Time.h"
#include "KrellInstitute/Services/TLS.h"

/* FIXME: defined in services/src/data/InitializeEventHeader.c. need include. */
extern void CBTF_InitializeEventHeader( CBTF_EventHeader* header);

/* FIXME: defined in services/collector/collector.c. need include. */
extern void cbtf_timer_service_start_sampling(const char* arguments);
extern void cbtf_timer_service_stop_sampling(const char* arguments);

#if defined(CBTF_SERVICE_USE_FILEIO)
/* FIXME: defined in services/src/xdr/Send.c. need include. */
extern void CBTF_Event_Send(const CBTF_EventHeader* header,
                 const xdrproc_t xdrproc, const void* data);
#endif

/* FIXME: defined in services/src/mrnet/MRNet_Send.c. need include. */
#if defined(CBTF_SERVICE_USE_MRNET)
extern void CBTF_Waitfor_MRNet_Shutdown();
#endif
#if defined(CBTF_SERVICE_USE_MRNET_MPI) || defined(CBTF_SERVICE_USE_MRNET)
extern void CBTF_MRNet_Send(const int tag,
                 const xdrproc_t xdrproc, const void* data);
#endif

/** Type defining the items stored in thread-local storage. */
typedef struct {

    uint64_t time_started;

    CBTF_EventHeader dso_header;   /**< Header for following dso blob. */
    CBTF_EventHeader info_header;  /**< Header for following info blob. */

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

    CBTF_Protocol_ThreadNameGroup tgrp;
    CBTF_Protocol_ThreadName tname;

    struct {
        CBTF_Protocol_ThreadName tnames[4096];
    } tgrpbuf;
#endif



    int  dsoname_len;
    bool  started;
    int  finished;
    int  sent_data;

    // marker if ANY collector is connected to mrnet.
    // this applies to the non mrnet builds.
    bool  connected_to_mrnet;

    bool  mpi_init_done;

} TLS;

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

// external functions for all collection types.
extern void cbtf_offline_service_start_timer();
extern void cbtf_offline_service_stop_timer();
extern void set_mpi_flag(bool);
extern void set_threaded_flag(bool);
extern void set_threaded_mrnet_connection();
// omp,ompt
extern int32_t cbtf_collector_get_openmp_threadid();

// external functions for using mrnet.
#if defined(CBTF_SERVICE_USE_MRNET)
extern void connect_to_mrnet();
extern void send_thread_state_changed_message();
extern void send_process_created_message();
extern void send_attached_to_threads_message();
#endif

// No-op for non mrnet builds.
void cbtf_offline_waitforshutdown() {
#if defined(CBTF_SERVICE_USE_MRNET)
	CBTF_Waitfor_MRNet_Shutdown();
#endif
}

// non mrnet builds just refurn false here..
bool_t cbtf_connected_to_mrnet() {
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);
    return tls->connected_to_mrnet;
}


// forward declarations.
void cbtf_offline_finish();
void cbtf_record_dsos();

void cbtf_offline_pause_sampling(CBTF_Monitor_Event_Type event)
{
    switch( event ) {
	case CBTF_Monitor_MPI_pre_init_event:
#ifndef NDEBUG
	    if (getenv("CBTF_DEBUG_MONITOR_SERVICE") != NULL) {
	        fprintf(stderr,"[%d,%d] cbtf_offline_pause_sampling passed event CBTF_Monitor_MPI_pre_init_event\n",getpid(),monitor_get_thread_num());
	    }
#endif
	    set_mpi_flag(true);
	    cbtf_offline_service_stop_timer();
	    break;
	case CBTF_Monitor_MPI_init_event:
#ifndef NDEBUG
	    if (getenv("CBTF_DEBUG_MONITOR_SERVICE") != NULL) {
	        fprintf(stderr,"[%d,%d] cbtf_offline_pause_sampling passed event CBTF_Monitor_MPI_init_event\n",getpid(),monitor_get_thread_num());
	    }
#endif
	    set_mpi_flag(true);
	    break;
	case CBTF_Monitor_MPI_post_comm_rank_event:
#ifndef NDEBUG
	    if (getenv("CBTF_DEBUG_MONITOR_SERVICE") != NULL) {
	        fprintf(stderr,"[%d,%d] cbtf_offline_pause_sampling passed event CBTF_Monitor_MPI_post_com_rank_event\n",getpid(),monitor_get_thread_num());
	    }
#endif
	    set_mpi_flag(true);
	    break;
	case CBTF_Monitor_MPI_fini_event:
#ifndef NDEBUG
	    if (getenv("CBTF_DEBUG_MONITOR_SERVICE") != NULL) {
	        fprintf(stderr,"[%d,%d] cbtf_offline_pause_sampling passed event CBTF_Monitor_MPI_fini_event\n",getpid(),monitor_get_thread_num());
	    }
#endif
	    cbtf_offline_service_stop_timer();
	    break;
	default:
	    break;
    }
}

void cbtf_offline_resume_sampling(CBTF_Monitor_Event_Type event)
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);

    switch( event ) {
	case CBTF_Monitor_MPI_pre_init_event:
#ifndef NDEBUG
	    if (getenv("CBTF_DEBUG_MONITOR_SERVICE") != NULL) {
	        fprintf(stderr,"[%d,%d] cbtf_offline_resume_sampling passed event CBTF_Monitor_MPI_pre_init_event\n",getpid(),monitor_get_thread_num());
	    }
#endif
	    break;
	case CBTF_Monitor_MPI_init_event:
#ifndef NDEBUG
	    if (getenv("CBTF_DEBUG_MONITOR_SERVICE") != NULL) {
	        fprintf(stderr,"[%d,%d] cbtf_offline_resume_sampling passed event CBTF_Monitor_MPI_init_event\n",getpid(),monitor_get_thread_num());
	    }
#endif
	    tls->mpi_init_done = true;
	    break;
	case CBTF_Monitor_MPI_post_comm_rank_event:
#ifndef NDEBUG
	    if (getenv("CBTF_DEBUG_MONITOR_SERVICE") != NULL) {
	        fprintf(stderr,
		"[%d,%d] cbtf_offline_resume_sampling passed event CBTF_Monitor_MPI_post_com_rank_event\n",getpid(),monitor_get_thread_num());
	    }
#endif
	    if (!tls->connected_to_mrnet && ( monitor_mpi_comm_rank() >= 0 || tls->mpi_init_done)) {
#ifndef NDEBUG
		if (getenv("CBTF_DEBUG_MRNET_MPI") != NULL) {
	            fprintf(stderr,
		    "[%d,%d] cbtf_offline_resume_sampling CBTF_Monitor_MPI_post_com_rank_event calls connect_to_mrnet\n",getpid(),monitor_get_thread_num());
		}
#endif
		tls->connected_to_mrnet = true;
#if defined(CBTF_SERVICE_USE_MRNET_MPI)
	        connect_to_mrnet();
		// The sending of attached threads was previously
		// defered until mpi job was terminating. For large
		// mpi jobs it is more efficient to send this message
		// as soon as mpi init has provided a rank and there
		// is not as much message traffic over mrnet.
		// We do not send the dso list (addressspace) until
		// the job has terminated since that list will be pruned
		// of dsos for which no sample or callstack addresses
		// are found.
		send_attached_to_threads_message();
#endif
		cbtf_offline_service_start_timer();
	    }
	    break;
	case CBTF_Monitor_MPI_post_fini_event:
#ifndef NDEBUG
	    if (getenv("CBTF_DEBUG_MONITOR_SERVICE") != NULL) {
	        fprintf(stderr,
		"[%d,%d] cbtf_offline_resume_sampling passed event CBTF_Monitor_MPI_post_fini_event\n",getpid(),monitor_get_thread_num());
	    }
#endif
	    cbtf_offline_service_start_timer();
	    break;
	default:
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

void cbtf_offline_send_dsos(TLS *tls)
{
    /* Send the offline "dsos" blob or message */
#ifndef NDEBUG
    if (getenv("CBTF_DEBUG_COLLECTOR_DSOS") != NULL) {
        fprintf(stderr,
    "cbtf_offline_send_dsos SENDS DSOS for %s:%lld:%lld:%d:%d\n",
                tls->dso_header.host, (long long)tls->dso_header.pid, 
                (long long)tls->dso_header.posix_tid, tls->dso_header.rank,tls->dso_header.omp_tid);
    }
#endif

#if defined(CBTF_SERVICE_USE_FILEIO)
    //CBTF_EventSetSendToFile(&(tls->dso_header),
    //                        cbtf_collector_unique_id, "openss-dsos");
    CBTF_Event_Send(&(tls->dso_header),
		(xdrproc_t)xdr_CBTF_Protocol_Offline_LinkedObjectGroup,
		&(tls->data));
#endif

#if defined(CBTF_SERVICE_USE_MRNET_MPI)
    tls->data.thread = tls->tname;
    if (tls->connected_to_mrnet) {
        CBTF_MRNet_Send( CBTF_PROTOCOL_TAG_LINKED_OBJECT_GROUP,
                  (xdrproc_t) xdr_CBTF_Protocol_LinkedObjectGroup,&(tls->data));
    }
#elif defined(CBTF_SERVICE_USE_MRNET)
    tls->data.thread = tls->tname;
    if (tls->connected_to_mrnet) {
        CBTF_MRNet_Send( CBTF_PROTOCOL_TAG_LINKED_OBJECT_GROUP,
                  (xdrproc_t) xdr_CBTF_Protocol_LinkedObjectGroup,&(tls->data));
    }
#endif

    tls->data.linkedobjects.linkedobjects_len = 0;
    tls->dsoname_len = 0;
    memset(tls->buffer.objs, 0, sizeof(tls->buffer.objs));
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
    TLS* tls = malloc(sizeof(TLS));
    Assert(tls != NULL);
    CBTF_SetTLS(TLSKey, tls);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);
#ifndef NDEBUG
    if (getenv("CBTF_DEBUG_COLLECTOR") != NULL) {
	fprintf(stderr,"[%d,%d] ENTER cbtf_offline_start_sampling\n",getpid(),monitor_get_thread_num());
    }
#endif

    tls->connected_to_mrnet = false;
    tls->mpi_init_done = false;

    tls->time_started = CBTF_GetTime();

    tls->dsoname_len = 0;
    tls->data.linkedobjects.linkedobjects_len = 0;
    tls->data.linkedobjects.linkedobjects_val = tls->buffer.objs;
    memset(tls->buffer.objs, 0, sizeof(tls->buffer.objs));

    /* Start sampling */
    cbtf_offline_sent_data(0);
    tls->finished = 0;
    tls->started = false;

/* SEQUENTIAL cbtf mrnet collection */
#if defined(CBTF_SERVICE_USE_MRNET) && !defined(CBTF_SERVICE_USE_MRNET_MPI)
 /* FIXME: should query this from collector.c where connection
 * is made.
 */
#ifndef NDEBUG
    if (getenv("CBTF_DEBUG_COLLECTOR") != NULL) {
	fprintf(stderr,"[%d,%d] cbtf_offline_start_sampling calls connect_to_mrnet for NON MPI program\n",getpid(),monitor_get_thread_num());
    }
#endif

    connect_to_mrnet();
    tls->connected_to_mrnet = true;

    /* Initialize the offline "dso" blob's header */
    CBTF_EventHeader local_header;
    CBTF_InitializeEventHeader(&local_header);
    local_header.rank = monitor_mpi_comm_rank();
    local_header.omp_tid = monitor_get_thread_num();
    memcpy(&tls->dso_header, &local_header, sizeof(CBTF_EventHeader));

#ifndef NDEBUG
    if (getenv("CBTF_DEBUG_COLLECTOR") != NULL) {
	    fprintf(stderr,"cbtf_offline_start_sampling BEGINS for %s:%ld:%ld:%d:%d\n",
                tls->dso_header.host,
                tls->dso_header.pid,
                tls->dso_header.posix_tid,
                tls->dso_header.omp_tid,
                tls->dso_header.rank);
    }
#endif

/* MPI cbtf mrnet collection */
#elif defined(CBTF_SERVICE_USE_MRNET) && defined(CBTF_SERVICE_USE_MRNET_MPI)
#ifndef NDEBUG
    if (getenv("CBTF_DEBUG_COLLECTOR") != NULL) {
	fprintf(stderr,"[%d,%d] cbtf_offline_start_sampling BEGINS for mpi program. defer init of event header.\n",getpid(),monitor_get_thread_num());
    }
#endif
/* OFFLINE collection */
#else
#ifndef NDEBUG
    if (getenv("CBTF_DEBUG_COLLECTOR") != NULL) {
	fprintf(stderr,"[%d,%d] cbtf_offline_start_sampling BEGINS non mrnet collection. defer init of event header.\n",getpid(),monitor_get_thread_num());
    }
#endif
#endif

#ifndef NDEBUG
    if (getenv("CBTF_DEBUG_COLLECTOR") != NULL) {
	fprintf(stderr,"[%d,%d] cbtf_offline_start_sampling calls cbtf_timer_service_start_sampling\n",getpid(),monitor_get_thread_num());
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
void cbtf_offline_stop_sampling(const char* in_arguments, const int finished)
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif

    if (!tls) {
#ifndef NDEBUG
	if (getenv("CBTF_DEBUG_COLLECTOR") != NULL) {
	    fprintf(stderr,"[%d,%d] warn: cbtf_offline_stop_sampling has no TLS\n",getpid(),monitor_get_thread_num());
	}
#endif
	return;
    }

    if (!tls->started) {
	return;
    }

    /* Stop sampling */
    cbtf_timer_service_stop_sampling(NULL);

    // DSOS: This is the only time we record the addressspace.
#if defined(CBTF_SERVICE_USE_FILEIO)
    cbtf_offline_finish();
#else
    cbtf_record_dsos();
#endif


    tls->finished = finished;

    if (finished && tls->sent_data) {
#ifndef NDEBUG
	if (getenv("CBTF_DEBUG_COLLECTOR") != NULL) {
	    fprintf(stderr,"[%d,%d] cbtf_offline_stop_sampling FINISHED for %s:%lld:%lld:%d:%d\n",
                getpid(),
                monitor_get_thread_num(),
                tls->dso_header.host,
                (long long)tls->dso_header.pid,
                (long long)tls->dso_header.posix_tid,
                tls->dso_header.rank,
                tls->dso_header.omp_tid);
	}
#endif
    }
}

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
	case CBTF_Monitor_MPI_pre_init_event:
#ifndef NDEBUG
	    if (getenv("CBTF_DEBUG_COLLECTOR") != NULL) {
	        fprintf(stderr,"[%d,%d] cbtf_offline_notify_event passed event CBTF_Monitor_MPI_pre_init_event\n",getpid(),monitor_get_thread_num());
	    }
#endif
	    set_mpi_flag(true);
	    break;
	case CBTF_Monitor_MPI_init_event:
#ifndef NDEBUG
	    if (getenv("CBTF_DEBUG_COLLECTOR") != NULL) {
	         fprintf(stderr,"[%d,%d] cbtf_offline_notify_event passed event CBTF_Monitor_MPI_init_event\n",getpid(),monitor_get_thread_num());
	    }
#endif
	    set_mpi_flag(true);
	    break;
	case CBTF_Monitor_MPI_post_comm_rank_event:
#ifndef NDEBUG
	    if (getenv("CBTF_DEBUG_COLLECTOR") != NULL) {
	        fprintf(stderr,"[%d,%d] cbtf_offline_notify_event CBTF_Monitor_MPI_post_comm_rank_event for rank %d\n",
			getpid(),monitor_get_thread_num(),monitor_mpi_comm_rank());
	    }
	    set_mpi_flag(true);
#endif
	    break;
	case CBTF_Monitor_init_process_event:
#ifndef NDEBUG
	    if (getenv("CBTF_DEBUG_MONITOR_SERVICE") != NULL) {
	        fprintf(stderr,"[%d,%d] cbtf_offline_notify_event CBTF_Monitor_init_process_event\n",
			getpid(),monitor_get_thread_num());
	    }
#endif
	    break;
	case CBTF_Monitor_init_thread_event:
#ifndef NDEBUG
	    if (getenv("CBTF_DEBUG_MONITOR_SERVICE") != NULL) {
	        fprintf(stderr,"[%d,%d] cbtf_offline_notify_event CBTF_Monitor_init_thread_event\n",
			getpid(),monitor_get_thread_num());
	    }
#endif
	    // threads share process wide mrnet connection.
	    set_threaded_flag(true);
#if defined(CBTF_SERVICE_USE_MRNET) || defined(CBTF_SERVICE_USE_MRNET_MPI)
	    tls->connected_to_mrnet = true;
	    set_threaded_mrnet_connection();
	    // no longer attempting to send thread attached at init thread.
	    // We are potentially called early in an mpi program and
	    // there is only an mrnet connection AFTER the mpi rank is
	    // set.  Therefore the send of thread attached needs to
	    // wait until there is collected data to send or the thread
	    // is finished.
#endif
	    break;
	default:
	    break;
    }
}

// is this called for mrnet based collection...
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
    if (getenv("CBTF_DEBUG_COLLECTOR") != NULL) {
        fprintf(stderr, "[%d,%d] cbtf_offline_finish entered.\n",getpid(),monitor_get_thread_num());
    }
#endif

    if (!tls->started) {
	return;
    }

    cbtf_record_dsos();
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
	return;
    }
#endif

#ifndef NDEBUG
    if (getenv("CBTF_DEBUG_COLLECTOR") != NULL) {
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
    if (getenv("CBTF_DEBUG_COLLECTOR") != NULL) {
	fprintf(stderr,"cbtf_record_dsos calls GETDLINFO for %s:%lld:%lld:%d:%d\n",
		local_header.host, (long long)local_header.pid,
		(long long)local_header.posix_tid, local_header.rank, local_header.omp_tid);
    }
#endif
    CBTF_GetDLInfo(getpid(), NULL, 0, 0);

    if(tls->data.linkedobjects.linkedobjects_len > 0) {
#ifndef NDEBUG
	if (getenv("CBTF_DEBUG_COLLECTOR") != NULL) {
           fprintf(stderr,
            "cbtf_record_dsos HAS %d OBJS for %s:%lld:%lld:%d:%d\n",
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
// FIXME: arg here should be CBTF_Monitor_Event_Type.
	cbtf_offline_pause_sampling(0);
    }

    //fprintf(stderr,"cbtf_offline_record_dso called for %s, is_dlopen = %d\n",dsoname, is_dlopen);

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
    objects.time_end = is_dlopen ? -1ULL : CBTF_GetTime();
    

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
    objects.time_end = is_dlopen ? -1ULL : CBTF_GetTime();

    CBTF_Protocol_FileName dsoFilename;
    dsoFilename.path = strdup(dsoname);
    //objects.linked_object = strdup(dsoname);
    objects.linked_object = dsoFilename;

    objects.range.begin = begin;
    objects.range.end = end;

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
    if (tls->connected_to_mrnet) {
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
	if (getenv("CBTF_DEBUG_COLLECTOR") != NULL) {
            fprintf(stderr,"cbtf_offline_record_dso SENDS OBJS for %s:%lld:%lld:%d:%d\n",
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
// FIXME: argument here is not a simple int. should be a CBTF_Monitor_Event_Type.
	cbtf_offline_resume_sampling(0);
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

    cbtf_offline_pause_sampling(0);

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
	if (getenv("CBTF_DEBUG_COLLECTOR") != NULL) {
            fprintf(stderr,"cbtf_offline_record_dlopen SENDS OBJS for %s:%lld:%lld:%d:%d\n",
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

    cbtf_offline_resume_sampling(0);
}
