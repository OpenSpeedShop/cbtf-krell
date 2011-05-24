/*******************************************************************************
** Copyright (c) The Krell Institute 2007-2011. All Rights Reserved.
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
 * Declaration and definition of the PC sampling collector's offline runtime.
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "KrellInstitute/Messages/EventHeader.h"
#include "KrellInstitute/Messages/LinkedObjectEvents.h"
#include "KrellInstitute/Messages/OfflineEvents.h"
#include "KrellInstitute/Messages/PCSamp.h"
#include "KrellInstitute/Messages/ToolMessageTags.h"
#include "KrellInstitute/Messages/Thread.h"
#include "KrellInstitute/Messages/ThreadEvents.h"
#include "KrellInstitute/Services/Common.h"
#include "KrellInstitute/Services/Monitor.h"
#include "KrellInstitute/Services/Offline.h"
#include "KrellInstitute/Services/Path.h"
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

    uint64_t time_started;

    CBTF_EventHeader dso_header;   /**< Header for following dso blob. */
    CBTF_EventHeader info_header;  /**< Header for following info blob. */
    CBTF_Protocol_Offline_LinkedObjectGroup data; /**< Actual dso data blob. */

    struct {
	CBTF_Protocol_Offline_LinkedObject objs[CBTF_OBJBufferSize];
    } buffer;

    CBTF_Protocol_ThreadNameGroup tgrp;
    struct {
        CBTF_Protocol_ThreadName tnames[4096];
    } tgrpbuf;

    int  dsoname_len;
    int  started;
    int  finished;
    int  sent_data;

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

extern void cbtf_offline_service_start_timer();
extern void cbtf_offline_service_stop_timer();
extern void set_mpi_flag(int);
#if defined(CBTF_SERVICE_USE_MRNET)
extern void connect_to_mrnet();
extern void send_thread_state_changed_message();
#endif

void cbtf_offline_finish();

void cbtf_offline_pause_sampling(CBTF_Monitor_Event_Type event)
{
    switch( event ) {
	case CBTF_Monitor_MPI_pre_init_event:
	    fprintf(stderr,"offline_pause_sampling passed event CBTF_Monitor_MPI_pre_init_event\n");
	    set_mpi_flag(1);
	    break;
	case CBTF_Monitor_MPI_init_event:
	    fprintf(stderr,"offline_pause_sampling passed event CBTF_Monitor_MPI_init_event\n");
	    set_mpi_flag(1);
	    break;
	case CBTF_Monitor_MPI_post_comm_rank_event:
	    fprintf(stderr,"offline_pause_sampling passed event CBTF_Monitor_MPI_post_com_rank_event\n");
	    set_mpi_flag(1);
	    break;
	default:
	    break;
    }
    cbtf_offline_service_stop_timer();
}

void cbtf_offline_resume_sampling(CBTF_Monitor_Event_Type event)
{
    switch( event ) {
	case CBTF_Monitor_MPI_pre_init_event:
	    fprintf(stderr,"offline_resume_sampling passed event CBTF_Monitor_MPI_pre_init_event\n");
	    break;
	case CBTF_Monitor_MPI_init_event:
	    fprintf(stderr,"offline_resume_sampling passed event CBTF_Monitor_MPI_init_event\n");
	    break;
	case CBTF_Monitor_MPI_post_comm_rank_event:
	    fprintf(stderr,"offline_resume_sampling passed event CBTF_Monitor_MPI_post_com_rank_event\n");
	    break;
	default:
	    break;
    }
    cbtf_offline_service_start_timer();
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
    fprintf(stderr,"exit cbtf_offline_sent_data sent_data = %d\n",
		tls->sent_data);
}

void cbtf_offline_send_dsos(TLS *tls)
{
    CBTF_EventSetSendToFile(&(tls->dso_header), "pcsamp", "cbtf-dsos");
    CBTF_Send(&(tls->dso_header),
		(xdrproc_t)xdr_CBTF_Protocol_Offline_LinkedObjectGroup,
		&(tls->data));
    
    /* Send the offline "info" blob */
#ifndef NDEBUG
    if (getenv("CBTF_DEBUG_SERVICE") != NULL) {
        fprintf(stderr,"cbtf_offline_send_dsos SENDS DSOS for HOST %s, PID %d, POSIX_TID %lu\n",
        tls->dso_header.host, tls->dso_header.pid, tls->dso_header.posix_tid);
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

    CBTF_pcsamp_start_sampling_args args;
    char arguments[3 * sizeof(CBTF_pcsamp_start_sampling_args)];

    /* Access the environment-specified arguments */
    const char* sampling_rate = getenv("CBTF_PCSAMP_RATE");

    /* Encode those arguments for pcsamp_start_sampling() */
    args.sampling_rate = (sampling_rate != NULL) ? atoi(sampling_rate) : 100;
    args.collector = 1;
    CBTF_EncodeParameters(&args,
			    (xdrproc_t)xdr_CBTF_pcsamp_start_sampling_args,
			    arguments);
        
    tls->time_started = CBTF_GetTime();

    tls->dsoname_len = 0;
    tls->data.linkedobjects.linkedobjects_len = 0;
    tls->data.linkedobjects.linkedobjects_val = tls->buffer.objs;
    memset(tls->buffer.objs, 0, sizeof(tls->buffer.objs));

    /* Start sampling */
    cbtf_offline_sent_data(0);
    tls->finished = 0;
    tls->started = 1;
    cbtf_timer_service_start_sampling(arguments);
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
	if (getenv("CBTF_DEBUG_SERVICE") != NULL) {
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

    tls->finished = finished;

    if (finished && tls->sent_data) {
	cbtf_offline_finish();
#ifndef NDEBUG
	if (getenv("CBTF_DEBUG_SERVICE") != NULL) {
	    fprintf(stderr,"cbtf_offline_stop_sampling FINISHED for %d, %lu\n",
		tls->dso_header.pid, tls->dso_header.posix_tid);
	}
#endif

#if defined(CBTF_SERVICE_USE_MRNET)
	    fprintf(stderr,"offline_stop_sampling FINISHED for %d, %lu calling send_thread_state_changed_message\n",
		tls->dso_header.pid, tls->dso_header.posix_tid);
	send_thread_state_changed_message();
	CBTF_Waitfor_MRNet_Shutdown();
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
	case CBTF_Monitor_MPI_post_comm_rank_event:
	    fprintf(stderr,"offline_notify_event CBTF_Monitor_MPI_post_comm_rank_event for rank %d\n", monitor_mpi_comm_rank());
	    connect_to_mrnet();
	    break;
	default:
	    break;
    }
}

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

    /* Initialize the offline "info" blob's header */
    CBTF_EventHeader local_header;
    CBTF_InitializeEventHeader(&local_header);
    
    /* Initialize the offline "info" blob */
    CBTF_Protocol_Offline_Parameters info;
    CBTF_InitializeParameters(&info);
    info.collector = strdup("pcsamp");
    const char* myexe = (const char*) CBTF_GetExecutablePath();
    info.exename = strdup(myexe);
    //info.rank = monitor_mpi_comm_rank();

    /* Access the environment-specified arguments */
    const char* sampling_rate = getenv("CBTF_PCSAMP_RATE");
    info.rate = (sampling_rate != NULL) ? atoi(sampling_rate) : 100;
    
    /* Send the offline "info" blob */
#ifndef NDEBUG
    if (getenv("CBTF_DEBUG_SERVICE") != NULL) {
        fprintf(stderr,"cbtf_offline_stop_sampling SENDS INFO for HOST %s, PID %d, POSIX_TID %lu\n",
        local_header.host, local_header.pid, local_header.posix_tid);
    }
#endif

    CBTF_EventSetSendToFile(&local_header, "pcsamp", "cbtf-info");
    CBTF_Send(&local_header, (xdrproc_t)xdr_CBTF_Protocol_Offline_Parameters, &info);

    /* Write the thread's initial address space to the appropriate file */
    CBTF_GetDLInfo(getpid(), NULL);
    if(tls->data.linkedobjects.linkedobjects_len > 0) {
#ifndef NDEBUG
	if (getenv("CBTF_DEBUG_SERVICE") != NULL) {
           fprintf(stderr,"cbtf_offline_stop_sampling SENDS OBJS for HOST %s, PID %d, POSIX_TID %lu\n",
        	   local_header.host, local_header.pid, local_header.posix_tid);
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
	cbtf_offline_service_stop_timer();
    }

    //fprintf(stderr,"cbtf_offline_record_dso called for %s, is_dlopen = %d\n",dsoname, is_dlopen);

    /* Initialize the offline "dso" blob's header */
    CBTF_EventHeader local_header;
    CBTF_InitializeEventHeader(&local_header);
    memcpy(&tls->dso_header, &local_header, sizeof(CBTF_EventHeader));

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

#if defined(CBTF_SERVICE_USE_MRNET)
    CBTF_Protocol_ThreadName tname;
    tname.experiment = 0;
    tname.host = strdup(local_header.host);
    tname.pid = local_header.pid;
    tname.has_posix_tid = true;
    tname.posix_tid = local_header.posix_tid;

    tls->tgrp.names.names_len = 0;
    tls->tgrp.names.names_val = tls->tgrpbuf.tnames;
    memset(tls->tgrpbuf.tnames, 0, sizeof(tls->tgrpbuf.tnames));

    memcpy(&(tls->tgrpbuf.tnames[tls->tgrp.names.names_len]),
           &tname, sizeof(tname));
    tls->tgrp.names.names_len++;


// CBTF_PROTOCOL_TAG_LOADED_LINKED_OBJECT
// xdr_CBTF_Protocol_LoadedLinkedObject
    CBTF_Protocol_LoadedLinkedObject message;
    memset(&message, 0, sizeof(message));

    message.threads = tls->tgrp;
    message.time = objects.time_begin;
    message.range.begin =  begin;
    message.range.end = end;

    CBTF_Protocol_FileName dsoFilename;
    dsoFilename.path = strdup(dsoname);

    message.linked_object = dsoFilename;

    if (strcmp(CBTF_GetExecutablePath(),dsoname)) {
	message.is_executable = false;
    } else {
	message.is_executable = true;
    }

    CBTF_MRNet_Send( CBTF_PROTOCOL_TAG_LOADED_LINKED_OBJECT,
                  (xdrproc_t) xdr_CBTF_Protocol_LoadedLinkedObject, &message);
#endif

    int dsoname_len = strlen(dsoname);
    int newsize = (tls->data.linkedobjects.linkedobjects_len *
		   sizeof(objects)) + (tls->dsoname_len + dsoname_len);


    if(newsize > CBTF_OBJBufferSize) {
#ifndef NDEBUG
	if (getenv("CBTF_DEBUG_SERVICE") != NULL) {
            fprintf(stderr,"cbtf_offline_record_dso SENDS OBJS for HOST %s, PID %d, POSIX_TID %lu\n",
        	   tls->dso_header.host, tls->dso_header.pid, tls->dso_header.posix_tid);
	}
#endif
	cbtf_offline_send_dsos(tls);
    }

    memcpy(&(tls->buffer.objs[tls->data.linkedobjects.linkedobjects_len]),
           &objects, sizeof(objects));
    tls->data.linkedobjects.linkedobjects_len++;
    tls->dsoname_len += dsoname_len;

    if (is_dlopen) {
	cbtf_offline_service_start_timer();
    }
}
