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

/** Type defining the items stored in thread-local storage. */
typedef struct {

    uint64_t time_started;

    CBTF_EventHeader dso_header;   /**< Header for following dso blob. */
    CBTF_EventHeader info_header;  /**< Header for following info blob. */

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

#if defined(CBTF_SERVICE_USE_MRNET_MPI)
    bool  mpi_init_done;
#endif

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
void cbtf_send_info();
void cbtf_record_dsos();

void cbtf_offline_pause_sampling(CBTF_Monitor_Event_Type event)
{
#if defined(CBTF_SERVICE_USE_MRNET_MPI)
    switch( event ) {
	case CBTF_Monitor_MPI_pre_init_event:
#ifndef NDEBUG
	    if (getenv("CBTF_DEBUG_MRNET_MPI") != NULL) {
	        fprintf(stderr,"cbtf_offline_pause_sampling passed event CBTF_Monitor_MPI_pre_init_event\n");
	    }
#endif
	    set_mpi_flag(true);
	    cbtf_offline_service_stop_timer();
	    break;
	case CBTF_Monitor_MPI_init_event:
#ifndef NDEBUG
	    if (getenv("CBTF_DEBUG_MRNET_MPI") != NULL) {
	        fprintf(stderr,"cbtf_offline_pause_sampling passed event CBTF_Monitor_MPI_init_event\n");
	    }
#endif
	    set_mpi_flag(true);
	    break;
	case CBTF_Monitor_MPI_post_comm_rank_event:
#ifndef NDEBUG
	    if (getenv("CBTF_DEBUG_MRNET_MPI") != NULL) {
	        fprintf(stderr,"cbtf_offline_pause_sampling passed event CBTF_Monitor_MPI_post_com_rank_event\n");
	    }
#endif
	    set_mpi_flag(true);
	    break;
	case CBTF_Monitor_MPI_fini_event:
#ifndef NDEBUG
	    if (getenv("CBTF_DEBUG_MRNET_MPI") != NULL) {
	        fprintf(stderr,"cbtf_offline_pause_sampling passed event CBTF_Monitor_MPI_fini_event\n");
	    }
#endif
	    cbtf_offline_service_stop_timer();
	    break;
	default:
	    break;
    }
#endif
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

#if defined(CBTF_SERVICE_USE_MRNET_MPI)
    switch( event ) {
	case CBTF_Monitor_MPI_pre_init_event:
#ifndef NDEBUG
	    if (getenv("CBTF_DEBUG_MRNET_MPI") != NULL) {
	        fprintf(stderr,"cbtf_offline_resume_sampling passed event CBTF_Monitor_MPI_pre_init_event\n");
	    }
#endif
	    break;
	case CBTF_Monitor_MPI_init_event:
#ifndef NDEBUG
	    if (getenv("CBTF_DEBUG_MRNET_MPI") != NULL) {
	        fprintf(stderr,"cbtf_offline_resume_sampling passed event CBTF_Monitor_MPI_init_event\n");
	    }
#endif
	    tls->mpi_init_done = true;
	    break;
	case CBTF_Monitor_MPI_post_comm_rank_event:
#ifndef NDEBUG
	    if (getenv("CBTF_DEBUG_MRNET_MPI") != NULL) {
	        fprintf(stderr,
		"cbtf_offline_resume_sampling passed event CBTF_Monitor_MPI_post_com_rank_event\n");
	    }
#endif
	    if (!tls->connected_to_mrnet && ( monitor_mpi_comm_rank() >= 0 || tls->mpi_init_done)) {
#ifndef NDEBUG
		if (getenv("CBTF_DEBUG_MRNET_MPI") != NULL) {
	            fprintf(stderr,
		    "cbtf_offline_resume_sampling CBTF_Monitor_MPI_post_com_rank_event calls connect_to_mrnet\n");
		}
#endif
	        connect_to_mrnet();
		tls->connected_to_mrnet = true;
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
		cbtf_send_info();
		cbtf_offline_service_start_timer();
	    }
	    break;
	case CBTF_Monitor_MPI_post_fini_event:
#ifndef NDEBUG
	    if (getenv("CBTF_DEBUG_MRNET_MPI") != NULL) {
	        fprintf(stderr,
		"cbtf_offline_resume_sampling passed event CBTF_Monitor_MPI_post_fini_event\n");
	    }
#endif
	    cbtf_offline_service_start_timer();
	    break;
	default:
	    break;
    }
#endif
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
    if (getenv("CBTF_DEBUG_OFFLINE_COLLECTOR") != NULL) {
        fprintf(stderr,
    "cbtf_offline_send_dsos SENDS DSOS for %s:%lld:%lld:%d:%d\n",
                tls->dso_header.host, (long long)tls->dso_header.pid, 
                (long long)tls->dso_header.posix_tid, tls->dso_header.rank,tls->dso_header.omp_tid);
    }
#endif

#if defined(CBTF_SERVICE_USE_FILEIO)
    CBTF_EventSetSendToFile(&(tls->dso_header),
                            cbtf_collector_unique_id, "cbtf-dsos");
    CBTF_Send(&(tls->dso_header),
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
    if (getenv("CBTF_DEBUG_OFFLINE_COLLECTOR") != NULL) {
	fprintf(stderr,"ENTER cbtf_offline_start_sampling\n");
    }
#endif

    tls->connected_to_mrnet = false;
#if defined(CBTF_SERVICE_USE_MRNET_MPI)
    tls->mpi_init_done = false;
#endif

    tls->time_started = CBTF_GetTime();

    tls->dsoname_len = 0;
    tls->data.linkedobjects.linkedobjects_len = 0;
    tls->data.linkedobjects.linkedobjects_val = tls->buffer.objs;
    memset(tls->buffer.objs, 0, sizeof(tls->buffer.objs));

    /* Start sampling */
    cbtf_offline_sent_data(0);
    tls->finished = 0;
    tls->started = false;
#if defined(CBTF_SERVICE_USE_MRNET) && !defined(CBTF_SERVICE_USE_MRNET_MPI)
 /* FIXME: should query this from collector.c where connection
 * is made.
 */
#ifndef NDEBUG
    if (getenv("CBTF_DEBUG_OFFLINE_COLLECTOR") != NULL) {
	fprintf(stderr,"cbtf_offline_start_sampling calls connect_to_mrnet for NON MPI program\n");
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
    if (getenv("CBTF_DEBUG_OFFLINE_COLLECTOR") != NULL) {
	    fprintf(stderr,"cbtf_offline_start_sampling BEGINS for %s:%lld:%lld:%d:%d\n",
                (long long)tls->dso_header.host,
                (long long)tls->dso_header.pid,
                (long long)tls->dso_header.posix_tid,
                (long long)tls->dso_header.omp_tid,
                (long long)tls->dso_header.rank);
    }
#endif

#elif defined(CBTF_SERVICE_USE_MRNET) && defined(CBTF_SERVICE_USE_MRNET_MPI)
#ifndef NDEBUG
    if (getenv("CBTF_DEBUG_OFFLINE_COLLECTOR") != NULL) {
	fprintf(stderr,"cbtf_offline_start_sampling BEGINS for mpi program. defer init of event header.\n");
    }
#endif
#else
#ifndef NDEBUG
    if (getenv("CBTF_DEBUG_OFFLINE_COLLECTOR") != NULL) {
	fprintf(stderr,"cbtf_offline_start_sampling BEGINS non mrnet collection. defer init of event header.\n");
    }
#endif
#endif

#ifndef NDEBUG
    if (getenv("CBTF_DEBUG_OFFLINE_COLLECTOR") != NULL) {
	fprintf(stderr,"cbtf_offline_start_sampling calls cbtf_timer_service_start_sampling\n");
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
	if (getenv("CBTF_DEBUG_OFFLINE_COLLECTOR") != NULL) {
	    fprintf(stderr,"warn: cbtf_offline_stop_sampling has no TLS for %d\n",getpid());
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
    cbtf_record_dsos();

    tls->finished = finished;

    if (finished && tls->sent_data) {
#ifndef NDEBUG
	if (getenv("CBTF_DEBUG_OFFLINE_COLLECTOR") != NULL) {
	    fprintf(stderr,"cbtf_offline_stop_sampling FINISHED for %s:%lld:%lld:%d:%d\n",
                (long long)tls->dso_header.host,
                (long long)tls->dso_header.pid,
                (long long)tls->dso_header.posix_tid,
                (long long)tls->dso_header.rank,
                (long long)tls->dso_header.omp_tid);
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
#if defined(CBTF_SERVICE_USE_MRNET_MPI)
	case CBTF_Monitor_MPI_pre_init_event:
#ifndef NDEBUG
	    if (getenv("CBTF_DEBUG_MRNET_MPI") != NULL) {
	        fprintf(stderr,"cbtf_offline_notify_event passed event CBTF_Monitor_MPI_pre_init_event\n");
	    }
#endif
	    set_mpi_flag(true);
	    break;
	case CBTF_Monitor_MPI_init_event:
#ifndef NDEBUG
	    if (getenv("CBTF_DEBUG_MRNET_MPI") != NULL) {
	         fprintf(stderr,"cbtf_offline_notify_event passed event CBTF_Monitor_MPI_init_event\n");
	    }
#endif
	    set_mpi_flag(true);
	    break;
	case CBTF_Monitor_MPI_post_comm_rank_event:
#ifndef NDEBUG
	    if (getenv("CBTF_DEBUG_MRNET_MPI") != NULL) {
	        fprintf(stderr,"cbtf_offline_notify_event CBTF_Monitor_MPI_post_comm_rank_event for rank %d\n",
			monitor_mpi_comm_rank());
	    }
	    set_mpi_flag(true);
#endif
	    break;
#endif
	case CBTF_Monitor_init_process_event:
#ifndef NDEBUG
	    if (getenv("CBTF_DEBUG_MONITOR_SERVICE") != NULL) {
	        fprintf(stderr,"cbtf_offline_notify_event CBTF_Monitor_init_process_event for pid %d\n",
			getpid());
	    }
#endif
	    break;
	case CBTF_Monitor_init_thread_event:
#ifndef NDEBUG
	    if (getenv("CBTF_DEBUG_MONITOR_SERVICE") != NULL) {
	        fprintf(stderr,"cbtf_offline_notify_event CBTF_Monitor_init_thread_event for pid %d\n",
			getpid());
	    }
#endif
	    // threads share process wide mrnet connection.
#if defined(CBTF_SERVICE_USE_MRNET) || defined(CBTF_SERVICE_USE_MRNET_MPI)
	    tls->connected_to_mrnet = true;
#endif
	    set_threaded_flag(true);
	    set_threaded_mrnet_connection();
	    send_attached_to_threads_message();
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

    if (!tls->finished) {
	return;
    }

    cbtf_send_info();
// DSOS
    cbtf_record_dsos();
}

void cbtf_send_info()
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);

    /* Initialize the offline "info" blob's header */
    CBTF_EventHeader local_header;
    CBTF_InitializeEventHeader(&local_header);
    local_header.rank = monitor_mpi_comm_rank();
    local_header.omp_tid = monitor_get_thread_num();
    
    /* Initialize the offline "info" blob */
    CBTF_Protocol_Offline_Parameters info;
    CBTF_InitializeParameters(&info);
    info.collector = strdup(cbtf_collector_unique_id);
    const char* myexe = (const char*) CBTF_GetExecutablePath();
    info.exename = strdup(myexe);
    //info.rank = monitor_mpi_comm_rank();

    /* Access the environment-specified arguments */
    info.rate = 100;
    
// FIXME: does mrnet collection handle this yet?
    /* Send the offline "info" blob */
#ifndef NDEBUG
    if (getenv("CBTF_DEBUG_OFFLINE_COLLECTOR") != NULL) {
        fprintf(stderr,
            "cbtf_send_info SENDS INFO for %s:%lld:%lld:%d:%d\n",
                local_header.host, (long long)local_header.pid, 
                (long long)local_header.posix_tid,
                local_header.rank,
                local_header.omp_tid
		);
    }
#endif


#if defined(CBTF_SERVICE_USE_FILEIO)
    CBTF_EventSetSendToFile(&local_header,
                            cbtf_collector_unique_id, "cbtf-info");
    CBTF_Send(&local_header, (xdrproc_t)xdr_CBTF_Protocol_Offline_Parameters, &info);
#endif
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

    /* Initialize header */
    CBTF_EventHeader local_header;
    CBTF_InitializeEventHeader(&local_header);
    local_header.rank = monitor_mpi_comm_rank();
    local_header.omp_tid = monitor_get_thread_num();

    /* Write the thread's initial address space to the appropriate buffer */
#ifndef NDEBUG
    if (getenv("CBTF_DEBUG_OFFLINE_COLLECTOR") != NULL) {
	fprintf(stderr,"cbtf_record_dsos calls GETDLINFO for %s:%lld:%lld:%d:%d\n",
		local_header.host, (long long)local_header.pid,
		(long long)local_header.posix_tid, local_header.rank, local_header.omp_tid);
    }
#endif
    CBTF_GetDLInfo(getpid(), NULL);

    if(tls->data.linkedobjects.linkedobjects_len > 0) {
#ifndef NDEBUG
	if (getenv("CBTF_DEBUG_OFFLINE_COLLECTOR") != NULL) {
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
	if (getenv("CBTF_DEBUG_OFFLINE_COLLECTOR") != NULL) {
            fprintf(stderr,"cbtf_offline_record_dso SENDS OBJS for %s:%lld:%lld:%d:%d\n",
                    tls->dso_header.host, (long long)tls->dso_header.pid, 
                    (long long)tls->dso_header.posix_tid,
		    tls->dso_header.rank);
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
	if (getenv("CBTF_DEBUG_OFFLINE_COLLECTOR") != NULL) {
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
