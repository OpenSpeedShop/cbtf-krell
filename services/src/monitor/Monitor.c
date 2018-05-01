/*******************************************************************************
** Copyright (c) 2007-2018 The Krell Institue. All Rights Reserved.
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
 * Declaration and definition of the offline libmonitor callbacks
 * that we will override.
 *
 */

/*
 * The Rice libmonitor package defines _MONITOR_H_
 * and these callbacks we can use to monitor a process.
 *
 * monitor_init_library(void)
 * monitor_fini_library(void)
 * monitor_pre_fork(void)
 * monitor_post_fork(pid_t child, void *data)
 * monitor_init_process(int *argc, char **argv, void *data)  SERVICE callback.
 * monitor_fini_process(int how, void *data)  SERVICE callback.
 * monitor_thread_pre_create(void)
 * monitor_thread_post_create(void *data)
 * monitor_init_thread_support(void)
 * monitor_init_thread(int tid, void *data)
 * monitor_fini_thread(void *data)  SERVICE callback.
 * monitor_dlopen(const char *path, int flags, void *handle)  SERVICE callback.
 * monitor_dlclose(void *handle)
 * monitor_init_mpi(int *argc, char ***argv)
                  monitor_mpi_comm_size(), monitor_mpi_comm_rank(), *argc);
 * monitor_fini_mpi(void)
 * monitor_mpi_pcontrol(int level)
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <string.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdbool.h>
#include <sys/types.h>
#include <unistd.h>
#include "monitor.h"

#include "KrellInstitute/Services/Monitor.h"
#include "KrellInstitute/Services/Offline.h"
#include "KrellInstitute/Services/Time.h"
#include "KrellInstitute/Services/TLS.h"

/** Type defining the items stored in thread-local storage. */
typedef struct {
    int debug;
    bool in_mpi_pre_init;
    int mpi_pcontrol, start_enabled;
    CBTF_Monitor_Status sampling_status;
    int process_is_terminating;
    int thread_is_terminating;
    bool  collector_started;
    pthread_t tid;
    pid_t pid;
    CBTF_Monitor_Type CBTF_monitor_type;

    // FIXME: MOVE THIS to collector services.
    cbtf_dlinfoList *cbtf_dllist_curr, *cbtf_dllist_head;
} TLS;

/* This global controls defering of an monitor callbacks while
 * in mpi startup.  In particular we do not monitor any threads
 * started by the mpi library for it's own use.  The cuda collector
 * seems to have a dependency on cupti callback threads that are
 * started by some mpi libraries. So we need an override for this.
 */
bool CBTF_in_mpi_startup = false;
bool  init_mpi_comm_rank = false;

#ifdef USE_EXPLICIT_TLS

/**
 * Thread-local storage key.
 *
 * Key used for looking up our thread-local storage. This key <em>must</em>
 * be globally unique across the entire Open|SpeedShop code base.
 */
static const uint32_t TLSKey = 0x0000FAB1;

#else

/** Thread-local storage. */
static __thread TLS the_tls;

#endif


/*
 * callbacks for handling of PROCESS
 */
void monitor_fini_process(int how, void *data)
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif

    /* An assert here can cause libmonitor to hang with mpt.
     * We may have forked a process which then did not call
     * monitor_init_process and allocate TLS.
     */
    if (tls == NULL) {
	fprintf(stderr,"Warning. monitor_fini_process called with no TLS.\n");
	return;
    }

    if (tls->sampling_status == CBTF_Monitor_Finished) {
        if (tls->debug) {
	    fprintf(stderr,"[%d,%lu] monitor_fini_process ALREADY FINISHED\n",tls->pid,tls->tid);
	}
	return;
    }

    if (tls->debug) {
	fprintf(stderr,"[%d,%lu] monitor_fini_process FINISHED SAMPLING\n",
		tls->pid,tls->tid);
    }

    static int f = 0;
    if (f > 0)
      raise(SIGSEGV);
    f++;

    /* collector stop_sampling does not use the arguments param */
    tls->sampling_status = CBTF_Monitor_Finished;
    if(how == MONITOR_EXIT_EXEC) {
	tls->process_is_terminating = 1;
	cbtf_offline_stop_sampling(NULL, true);
    } else {
	tls->process_is_terminating = 1;
	cbtf_offline_stop_sampling(NULL, true);
    }

    if (tls->debug) {
	fprintf(stderr,"[%d,%lu] monitor_fini_process process_is_terminating %d\n",
		tls->pid,tls->tid,tls->process_is_terminating);
    }

    // The following call handles mrnet specific activity.
    cbtf_offline_notify_event(CBTF_Monitor_fini_process_event);
}

void *monitor_init_process(int *argc, char **argv, void *data)
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = malloc(sizeof(TLS));
    Assert(tls != NULL);
    CBTF_SetTLS(TLSKey, tls);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);

    if (monitor_is_threaded()) {
	tls->tid = monitor_get_thread_num();
    } else {
	tls->tid = 0;
    }

    if ( (getenv("CBTF_DEBUG_MONITOR_SERVICE") != NULL)) {
	tls->debug=1;
    } else {
	tls->debug=0;
    }

    short debug_mpi_pcontrol=0;
    if ( (getenv("CBTF_DEBUG_MPI_PCONTROL") != NULL)) {
	debug_mpi_pcontrol=1;
    }

    tls->pid = getpid();
    tls->cbtf_dllist_head = NULL;
    tls->cbtf_dllist_curr = NULL;
    tls->collector_started = false;

    if (tls->debug) {
	fprintf(stderr,"[%d,%lu] monitor_init_process ENTERED\n",
		tls->pid,tls->tid);
    }

    if (CBTF_in_mpi_startup || tls->in_mpi_pre_init) {
        if (tls->debug) {
	    fprintf(stderr,"[%d,%lu] monitor_init_process returns early due to in mpi init\n",tls->pid,tls->tid);
	}
	return NULL;
    }

    tls->in_mpi_pre_init = false;
    tls->CBTF_monitor_type = CBTF_Monitor_Proc;

    tls->mpi_pcontrol = 0, tls->start_enabled = 0;
    if (getenv("OPENSS_ENABLE_MPI_PCONTROL") != NULL) tls->mpi_pcontrol = 1;
    if (getenv("OPENSS_START_ENABLED") != NULL) tls->start_enabled = 1;
    if (getenv("CBTF_ENABLE_MPI_PCONTROL") != NULL) tls->mpi_pcontrol = 1;
    if (getenv("CBTF_START_ENABLED") != NULL) tls->start_enabled = 1;

    /* 
     * Always start the collector via cbtf_offline_start_sampling.
     * In the presence of active mpi_pcontrol and start with the
     * collector disabled, we issue a cbtf_offline_sampling_statuspaused call.
     */
    tls->sampling_status = CBTF_Monitor_Started;
    tls->collector_started = true;
    cbtf_offline_start_sampling(NULL);
    cbtf_offline_notify_event(CBTF_Monitor_init_process_event);
    if ( tls->mpi_pcontrol && !tls->start_enabled) {
	if (debug_mpi_pcontrol) {
            fprintf(stderr,"[%d,%lu] monitor_init_process CBTF_START_ENABLED NOT SET. Defer sampling at start-up time.\n",
		tls->pid,tls->tid);
	}
	tls->sampling_status = CBTF_Monitor_Paused;
	cbtf_offline_sampling_status(CBTF_Monitor_init_process_event,CBTF_Monitor_Paused);
    } else if ( tls->mpi_pcontrol && tls->start_enabled) {
	if (debug_mpi_pcontrol) {
            fprintf(stderr,"[%d,%lu] monitor_init_process CBTF_START_ENABLED SET. START SAMPLING\n",
                tls->pid,tls->tid);
	}
    } else {
	if (tls->debug) {
	    fprintf(stderr,"[%d,%lu] monitor_init_process START SAMPLING\n",
		tls->pid,tls->tid);
	}
    }
    return (data);
}

/*
 * callbacks for handling of monitor init
 */
void monitor_init_library(void)
{
#if 0
    if ( (getenv("CBTF_DEBUG_MONITOR_SERVICE") != NULL)) {
	fprintf(stderr,"cbtf callback monitor_init_library entered\n");
    }
#endif
}

void monitor_fini_library(void)
{
#if 0
    if ( (getenv("CBTF_DEBUG_MONITOR_SERVICE") != NULL)) {
	fprintf(stderr,"cbtf callback monitor_fini_library entered\n");
    }
#endif
    static int f = 0;
    if (f > 0)
      raise(SIGSEGV);
    f++;
}

#ifdef HAVE_TARGET_POSIX_THREADS
/*
 * callbacks for handling of THREADS
 */
void monitor_fini_thread(void *ptr)
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);

    if (!tls->collector_started) {
	if (tls->debug) {
	    fprintf(stderr,"[%d,%lu] monitor_fini_thread collector never started.\n",
		tls->pid,tls->tid);
	}
	return;
    }

    if (tls->debug) {
	fprintf(stderr,"[%d,%lu] monitor_fini_thread STOP SAMPLING\n",
		tls->pid,tls->tid);
    }
    cbtf_offline_stop_sampling(NULL,true);

    tls->sampling_status = CBTF_Monitor_Finished;
    tls->thread_is_terminating = 1;
    // The following call will handle any mrnet specific activity.
    cbtf_offline_notify_event(CBTF_Monitor_fini_thread_event);
}

void *monitor_init_thread(int tid, void *data)
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = malloc(sizeof(TLS));
    Assert(tls != NULL);
    CBTF_SetTLS(TLSKey, tls);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);

    tls->pid = getpid();
    if (monitor_is_threaded()) {
	tls->tid = monitor_get_thread_num();
    } else {
	tls->tid = 0;
    }

    if ( (getenv("CBTF_DEBUG_MONITOR_SERVICE") != NULL)) {
	tls->debug=1;
	fprintf(stderr,"[%d,%lu] monitor_init_thread ENTERED\n",tls->pid,tls->tid);
    } else {
	tls->debug=0;
    }

    short debug_mpi_pcontrol=0;
    if ( (getenv("CBTF_DEBUG_MPI_PCONTROL") != NULL)) {
	debug_mpi_pcontrol=1;
    }

    tls->collector_started = false;
    tls->cbtf_dllist_head = NULL;
    tls->cbtf_dllist_curr = NULL;

    if (tls->debug) {
	fprintf(stderr,"[%d,%lu] monitor_init_thread BEGIN SAMPLING\n",
		tls->pid,tls->tid);
    }

    tls->in_mpi_pre_init = false;
    tls->CBTF_monitor_type = CBTF_Monitor_Thread;

    tls->mpi_pcontrol = 0, tls->start_enabled = 0;
    if (getenv("OPENSS_ENABLE_MPI_PCONTROL") != NULL) tls->mpi_pcontrol = 1;
    if (getenv("OPENSS_START_ENABLED") != NULL) tls->start_enabled = 1;
    if (getenv("CBTF_ENABLE_MPI_PCONTROL") != NULL) tls->mpi_pcontrol = 1;
    if (getenv("CBTF_START_ENABLED") != NULL) tls->start_enabled = 1;

    /* 
     * Always start the collector via cbtf_offline_start_sampling.
     * In the presence of active mpi_pcontrol and start with the
     * collector disabled, we issue a cbtf_offline_sampling_status paused call.
     */

    if (CBTF_in_mpi_startup) {
	if (tls->debug) {
	    fprintf(stderr,"[%d,%lu] monitor_init_thread IN MPI STARTUP - DEFER SAMPLING\n",
                tls->pid,tls->tid);
	}
	return (data);
    }

    tls->collector_started = true;
    tls->sampling_status = CBTF_Monitor_Started;
    cbtf_offline_start_sampling(NULL);
    cbtf_offline_notify_event(CBTF_Monitor_init_thread_event);
    if ( tls->mpi_pcontrol && !tls->start_enabled) {
	if (debug_mpi_pcontrol) {
            fprintf(stderr,"[%d,%lu] monitor_init_thread CBTF_START_ENABLED NOT SET. Defer sampling at start-up time.\n",
		tls->pid,tls->tid);
	}
	tls->sampling_status = CBTF_Monitor_Paused;
	cbtf_offline_sampling_status(CBTF_Monitor_init_thread_event,CBTF_Monitor_Paused);
    } else if ( tls->mpi_pcontrol && tls->start_enabled) {
	if (debug_mpi_pcontrol) {
            fprintf(stderr,"[%d,%lu] monitor_init_thread CBTF_START_ENABLED SET. START SAMPLING\n",
                tls->pid,tls->tid);
	}
    } else {
	if (tls->debug) {
	    fprintf(stderr,"[%d,%lu] monitor_init_thread START SAMPLING\n",
		tls->pid,tls->tid);
	}
    }

    return(data);
}

void monitor_init_thread_support(void)
{
#if 0
    if ( (getenv("CBTF_DEBUG_MONITOR_SERVICE") != NULL)) {
	fprintf(stderr,"Entered cbtf monitor_init_thread_support callback\n");
    }
#endif
}

// these are preventing spawned threads from collecting if the
// cbtf_offline_sampling_status pause,resume calls are used.
#if 0
void*
monitor_thread_pre_create(void)
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);
    cbtf_offline_notify_event(CBTF_Monitor_thread_pre_create_event);
    /* Stop sampling prior to real thread_create. */
    if (tls->sampling_status != CBTF_Monitor_Paused) {
        if (tls->debug) {
	    fprintf(stderr,"[%d,%lu] monitor_thread_pre_create PAUSE SAMPLING\n",
		    tls->pid,tls->tid);
        }
	tls->sampling_status = CBTF_Monitor_Paused;
	cbtf_offline_sampling_status(CBTF_Monitor_thread_pre_create_event,CBTF_Monitor_Paused);
    } else if (tls->sampling_status == CBTF_Monitor_Paused) {
        if (tls->debug) {
	    fprintf(stderr,"[%d,%lu] monitor_thread_pre_create ALREADY PAUSED\n",
		    tls->pid,tls->tid);
        }
    }
    return (NULL);
}

void
monitor_thread_post_create(void* data)
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);
    if (tls->debug) {
	fprintf(stderr,"[%d,%lu] Entered cbtf monitor_thread_post_create callback sampling_status:%d\n",
		tls->pid,tls->tid,tls->sampling_status);
    }

    if (tls->sampling_status != CBTF_Monitor_Resumed) {
        if (tls->debug) {
	    fprintf(stderr,"[%d,%lu] monitor_thread_post_create RESUME SAMPLING\n",
		    tls->pid,tls->tid);
        }
	tls->sampling_status = CBTF_Monitor_Resumed;
	cbtf_offline_sampling_status(CBTF_Monitor_thread_post_create_event,CBTF_Monitor_Resumed);
    } else if (tls->sampling_status == CBTF_Monitor_Paused) {
        if (tls->debug) {
	    fprintf(stderr,"[%d,%lu] monitor_thread_pre_create ALREADY RESUMED\n",
		    tls->pid,tls->tid);
        }
    }
}
#endif // if 0

#endif

#ifdef HAVE_TARGET_DLOPEN
/*
 * callbacks for handling of DLOPEN/DLCLOSE.
 */
void monitor_dlopen(const char *library, int flags, void *handle)
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif

    // FIXME: sampling_status is an enum.
    if (tls == NULL || (tls && tls->sampling_status == CBTF_Monitor_Finished) ) {
	return;
    }

    if (CBTF_in_mpi_startup || tls->in_mpi_pre_init) {
        if (tls->debug) {
	    fprintf(stderr,"[%d,%lu] monitor_dlopen returns early due to in mpi init\n",tls->pid,tls->tid);
	}
	return;
    }

    if (library == NULL) {
	if (tls->debug) {
	    fprintf(stderr,"[%d,%lu] monitor_dlopen ignores null library name\n",tls->pid,tls->tid);
	}
	return;
    }

    /* TODO:
     * if CBTF_GetDLInfo does not handle errors do so here.
     */
    if (tls->debug) {
	fprintf(stderr,"[%d,%lu] monitor_dlopen called with %s\n",
	    tls->pid,tls->tid,library);
    }

    tls->cbtf_dllist_curr = (cbtf_dlinfoList*)calloc(1,sizeof(cbtf_dlinfoList));
    tls->cbtf_dllist_curr->cbtf_dlinfo_entry.load_time = CBTF_GetTime();
    tls->cbtf_dllist_curr->cbtf_dlinfo_entry.unload_time = CBTF_GetTime() + 1;
    tls->cbtf_dllist_curr->cbtf_dlinfo_entry.name = strdup(library);
    tls->cbtf_dllist_curr->cbtf_dlinfo_entry.handle = handle;
    tls->cbtf_dllist_curr->cbtf_dlinfo_next = tls->cbtf_dllist_head;
    tls->cbtf_dllist_head = tls->cbtf_dllist_curr;

    if ((tls->sampling_status == CBTF_Monitor_Paused) && !tls->in_mpi_pre_init) {
        if (tls->debug) {
	    fprintf(stderr,"[%d,%lu] monitor_dlopen RESUME SAMPLING\n",
		tls->pid,tls->tid);
        }
	tls->sampling_status = CBTF_Monitor_Resumed;
	cbtf_offline_sampling_status(CBTF_Monitor_dlopen_event,CBTF_Monitor_Resumed);
    }
}

void
monitor_pre_dlopen(const char *path, int flags)
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif

    // FIXME: sampling_status is an enum.
    if (tls == NULL || (tls && tls->sampling_status == CBTF_Monitor_Finished) ) {
	return;
    }

    if (tls->in_mpi_pre_init) {
        if (tls->debug) {
	    fprintf(stderr,"[%d,%lu] monitor_pre_dlopen returns early due to in mpi init\n",tls->pid,tls->tid);
	}
	return;
    }

    if (path == NULL) {
	if (tls->debug) {
	    fprintf(stderr,"[%d,%lu] monitor_pre_dlopen ignores null path\n",tls->pid,tls->tid);
	}
	return;
    }

    if (tls->debug) {
	fprintf(stderr,"[%d,%lu] monitor_pre_dlopen %s\n",tls->pid,tls->tid,path);
    }

    if ((tls->sampling_status == CBTF_Monitor_Started ||
	 tls->sampling_status == CBTF_Monitor_Resumed) && !tls->in_mpi_pre_init) {
        if (tls->debug) {
	    fprintf(stderr,"[%d,%lu] monitor_pre_dlopen PAUSE SAMPLING\n",
		tls->pid,tls->tid);
        }
	tls->sampling_status = CBTF_Monitor_Paused;
	cbtf_offline_sampling_status(CBTF_Monitor_pre_dlopen_event,CBTF_Monitor_Paused);
    }
}

void
monitor_dlclose(void *handle)
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif


    // FIXME: sampling_status is an enum.
    if (tls == NULL || (tls && tls->sampling_status == CBTF_Monitor_Finished) ) {
	return;
    }

    if (tls->in_mpi_pre_init) {
        if (tls->debug) {
	    fprintf(stderr,"[%d,%lu] monitor_dlclose returns early due to in mpi init\n",tls->pid,tls->tid);
	}
	return;
    }

    while (tls->cbtf_dllist_curr) {
	if (tls->cbtf_dllist_curr->cbtf_dlinfo_entry.handle == handle) {
	   tls->cbtf_dllist_curr->cbtf_dlinfo_entry.unload_time = CBTF_GetTime();

            if (tls->debug) {
	        fprintf(stderr,"FOUND %p %s\n",handle, tls->cbtf_dllist_curr->cbtf_dlinfo_entry.name);
	        fprintf(stderr,"loaded at %lu, unloaded at %lu\n",
		               tls->cbtf_dllist_curr->cbtf_dlinfo_entry.load_time,
		               tls->cbtf_dllist_curr->cbtf_dlinfo_entry.unload_time);
	    }

	   /* On some systems (NASA) it appears that dlopen can be called
	    * before monitor_init_process (or even monitor_early_init).
	    * So we need to use getpid() directly here.
	    */ 

	   /* FIXME: Handle return value? */
	   int retval = CBTF_GetDLInfo(getpid(),
					 tls->cbtf_dllist_curr->cbtf_dlinfo_entry.name,
					 tls->cbtf_dllist_curr->cbtf_dlinfo_entry.load_time,
					 tls->cbtf_dllist_curr->cbtf_dlinfo_entry.unload_time
					);
	   if (retval) {
	   }
	   break;
	}
	tls->cbtf_dllist_curr = tls->cbtf_dllist_curr->cbtf_dlinfo_next;
    }

    if (!tls->thread_is_terminating || !tls->process_is_terminating) {
	if ((tls->sampling_status == CBTF_Monitor_Started ||
	     tls->sampling_status == CBTF_Monitor_Resumed) && !tls->in_mpi_pre_init) {
            if (tls->debug) {
	        fprintf(stderr,"[%d,%lu] monitor_dlclose PAUSE SAMPLING\n",
		    tls->pid,tls->tid);
            }
	    tls->sampling_status = CBTF_Monitor_Paused;
	    cbtf_offline_sampling_status(CBTF_Monitor_dlclose_event,CBTF_Monitor_Paused);
	}
    }
}

void
monitor_post_dlclose(void *handle, int ret)
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif

    // FIXME: sampling_status is an enum.
    if (tls == NULL || (tls && tls->sampling_status == CBTF_Monitor_Finished) ) {
	return;
    }

    if (tls->in_mpi_pre_init) {
        if (tls->debug) {
	    fprintf(stderr,"[%d,%lu] monitor_post_dlclose returns early due to in mpi init\n",tls->pid,tls->tid);
	}
	return;
    }

    if (!tls->thread_is_terminating || !tls->process_is_terminating) {
	if (tls->sampling_status == CBTF_Monitor_Paused && !tls->in_mpi_pre_init) {
            if (tls->debug) {
	        fprintf(stderr,"[%d,%lu] monitor_post_dlclose RESUME SAMPLING\n",
		    tls->pid,tls->tid);
            }
	    tls->sampling_status = CBTF_Monitor_Resumed;
	    cbtf_offline_sampling_status(CBTF_Monitor_post_dlclose_event,CBTF_Monitor_Resumed);
	}
    }
}

#endif

#ifdef HAVE_TARGET_FORK
/* 
 * callbacks for handling of FORK.
 * NOTE that this callback can return a void pointer if desired.
 */
void * monitor_pre_fork(void)
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    /* The sgi MPT mpi startup apparently can fork
     * a process such that monitor does not go
     * through its normal path and our tls is not
     * allocated as we expect.
     */
    TLS* tls = CBTF_GetTLS(TLSKey);
    if (tls == NULL) {
	tls = malloc(sizeof(TLS));
    }
    Assert(tls != NULL);
    CBTF_SetTLS(TLSKey, tls);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);


    if (CBTF_in_mpi_startup || tls->in_mpi_pre_init) {
        if (tls->debug) {
	    fprintf(stderr,"[%d,%lu] monitor_pre_fork returns early due to in mpi init\n",tls->pid,tls->tid);
	}
	return (NULL);
    }

    /* Stop sampling prior to real fork. */
    if (tls->sampling_status == CBTF_Monitor_Paused ||
	tls->sampling_status == CBTF_Monitor_Started) {
        if (tls->debug) {
	    fprintf(stderr,"[%d,%lu] monitor_pre_fork PAUSE SAMPLING\n",
		    tls->pid,tls->tid);
        }
	tls->sampling_status = CBTF_Monitor_Paused;
	cbtf_offline_sampling_status(CBTF_Monitor_pre_fork_event,CBTF_Monitor_Paused);
    }
    return (NULL);
}

void monitor_post_fork(pid_t child, void *data)
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);

    if (CBTF_in_mpi_startup || tls->in_mpi_pre_init) {
        if (tls->debug) {
	    fprintf(stderr,"[%d,%lu] monitor_post_fork returns early due to in mpi init\n",tls->pid,tls->tid);
	}
	return;
    }

    /* Resume/start sampling forked process. */
    if (tls->sampling_status == CBTF_Monitor_Paused ||
	tls->sampling_status == CBTF_Monitor_Finished) {
        if (tls->debug) {
	    fprintf(stderr,"[%d,%lu] monitor_post_fork RESUME SAMPLING\n",
		    tls->pid,tls->tid);
        }
	tls->CBTF_monitor_type = CBTF_Monitor_Proc;
	tls->sampling_status = CBTF_Monitor_Resumed;
	cbtf_offline_sampling_status(CBTF_Monitor_post_fork_event,CBTF_Monitor_Resumed);
    }
}
#endif

/*
 * callbacks for handling of MPI programs.
 */

void monitor_mpi_pre_init(void)
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);

    tls->in_mpi_pre_init = true;
    CBTF_in_mpi_startup = true;
    init_mpi_comm_rank = false;

    cbtf_offline_notify_event(CBTF_Monitor_MPI_pre_init_event);
    if (tls->sampling_status == CBTF_Monitor_Started) {
        if (tls->debug) {
	    fprintf(stderr,"[%d,%lu] monitor_mpi_pre_init PAUSE SAMPLING\n",
		    tls->pid,tls->tid);
        }
	tls->sampling_status = CBTF_Monitor_Paused;
	cbtf_offline_sampling_status(CBTF_Monitor_MPI_pre_init_event,CBTF_Monitor_Paused);
    } else {
        if (tls->debug) {
	    fprintf(stderr,"[%d,%lu] monitor_mpi_pre_init IS PAUSED\n",
		    tls->pid,tls->tid);
        }
    }
}

void
monitor_init_mpi(int *argc, char ***argv)
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);

    bool resume_sampling = false;
    if (tls->sampling_status == CBTF_Monitor_Paused) {
	if (tls->mpi_pcontrol && tls->start_enabled) {
	    if (tls->debug) {
		fprintf(stderr,"[%d,%lu] monitor_init_mpi SAMPLING pcontrol start enabled rank:%d\n",
		    tls->pid,tls->tid, monitor_mpi_comm_rank());
	    }
	    resume_sampling = true;
	} else if(tls->mpi_pcontrol && !tls->start_enabled) {
	    if (tls->debug) {
		fprintf(stderr,"[%d,%lu] monitor_init_mpi SAMPLING pcontrol start disabled rank:%d\n",
		    tls->pid,tls->tid, monitor_mpi_comm_rank());
	    }
	} else {
	    if (tls->debug) {
		fprintf(stderr,"[%d,%lu] monitor_init_mpi SAMPLING enabled rank:%d\n",
		    tls->pid,tls->tid, monitor_mpi_comm_rank());
	    }
	    resume_sampling = true;
	}
    }

    cbtf_offline_notify_event(CBTF_Monitor_MPI_init_event);
    if (resume_sampling) {
        if (tls->debug) {
	    fprintf(stderr,"[%d,%lu] monitor_init_mpi RESUME SAMPLING\n",
		    tls->pid,tls->tid);
        }
	tls->sampling_status = CBTF_Monitor_Resumed;
	//tls->CBTF_monitor_type = CBTF_Monitor_Proc;
	cbtf_offline_sampling_status(CBTF_Monitor_MPI_init_event,CBTF_Monitor_Resumed);
    } else {
        if (tls->debug) {
	    fprintf(stderr,"[%d,%lu] monitor_init_mpi is SAMPLING\n",
		    tls->pid,tls->tid);
        }
	// this is the case where start collection was disabled util at
	// the first (if any) mpi_pcontrol(1) is encountered.
	// We need to make this available to any code downstream that
	// may temporarily disable the collector via cbtf_offline_service_stop_timer
	// and later reenable the collector via cbtf_offline_service_start_timer.
	// Both of those live in services/collector/collector.c.
	// the disable and reenable is used internaly in collector.c to
	// protect one CBTF_MRNet_Send of attached threads from being
	// sampled. The other use is in the services/collector/monitor.c
	// code where we need to disable,reenable for similar reasons.
	// Could check tls->sampling_status for this from both
	// cbtf_offline_service_stop_timer and cbtf_offline_service_start_timer.
	// Provide this as util function that returns this value.
    }

    tls->in_mpi_pre_init = false;
    CBTF_in_mpi_startup = false;
}

void monitor_fini_mpi(void)
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);

    if (tls->debug) {
	fprintf(stderr,"[%d,%lu] monitor_fini_mpi CALLED\n",
		tls->pid,tls->tid);
    }

    cbtf_offline_notify_event(CBTF_Monitor_MPI_fini_event);
    if (tls->sampling_status == CBTF_Monitor_Started ||
	tls->sampling_status == CBTF_Monitor_Resumed) {
        if (tls->debug) {
	    fprintf(stderr,"[%d,%lu] monitor_fini_mpi PAUSE SAMPLING\n",
		    tls->pid,tls->tid);
        }
	tls->sampling_status = CBTF_Monitor_Paused;
	cbtf_offline_sampling_status(CBTF_Monitor_MPI_fini_event,CBTF_Monitor_Paused);
    }
}

void monitor_mpi_post_fini(void)
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);

    if (tls->debug) {
	fprintf(stderr,"[%d,%lu] monitor_mpi_post_fini CALLED\n",
		tls->pid,tls->tid);
    }

    cbtf_offline_notify_event(CBTF_Monitor_MPI_post_fini_event);
    if (1 || tls->sampling_status == CBTF_Monitor_Paused) {
        if (tls->debug) {
	    fprintf(stderr,"[%d,%lu] monitor_mpi_post_fini RESUME SAMPLING\n",
		    tls->pid,tls->tid);
        }
	tls->sampling_status = CBTF_Monitor_Resumed;
	cbtf_offline_sampling_status(CBTF_Monitor_MPI_post_fini_event,CBTF_Monitor_Resumed);
    }
}


// This callback should is only needed during mpi startup and mrnet connection.
// Once connected, this should just return as early as possible.
// Since we do not really want mrnet specific info here, we rely on the fact
// that we only use this callback the first time mpi_comm_rank is called
// so that the underlying common collector code can aquire the rank which
// is need for mrnet connections.  Therefore we should never do anything
// except in the first call.
void monitor_mpi_post_comm_rank(void)
{

    /* We do not even want to aquire the tls if mpi_comm_rank has
     * been called already. */
    if (init_mpi_comm_rank) {
	return;
    }
    init_mpi_comm_rank = true;

    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);


#ifndef NDEBUG
    char* statusstr = "UNKNOWNSTATUS";
    if (tls->debug) {
        switch(tls->sampling_status) {
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


#ifndef NDEBUG
    if (tls->debug) {
	fprintf(stderr,"[%d,%lu] monitor_mpi_post_comm_rank sampling_status:%s\n",
		tls->pid,tls->tid,statusstr);
    }
#endif

    // NOTE: For post_comm_rank, we always need to resume since
    // that is where mrnet connection is made.  Therefore that
    // resume should not resume if we are not start enabled.
    bool resume_sampling = false;
    //if (tls->sampling_status == CBTF_Monitor_Paused) {
	if (tls->mpi_pcontrol && tls->start_enabled) {
	    if (tls->debug) {
		fprintf(stderr,"[%d,%lu] monitor_mpi_post_comm_rank SAMPLING pcontrol start enabled rank:%d\n",
		    tls->pid,tls->tid, monitor_mpi_comm_rank());
	    }
	    resume_sampling = true;
	    tls->sampling_status = CBTF_Monitor_Resumed;
	} else if(tls->mpi_pcontrol && !tls->start_enabled) {
	    if (tls->debug) {
		fprintf(stderr,"[%d,%lu] monitor_mpi_post_comm_rank SAMPLING pcontrol start disabled rank:%d\n",
		    tls->pid,tls->tid, monitor_mpi_comm_rank());
	    }
	    // FORCE THIS HERE FOR NOW.
	    resume_sampling = true;
	    // tell collector service that start was deferred
	    //cbtf_offline_service_start_deferred();
	} else {
	    if (tls->debug) {
		fprintf(stderr,"[%d,%lu] monitor_mpi_post_comm_rank SAMPLING enabled rank:%d\n",
		    tls->pid,tls->tid, monitor_mpi_comm_rank());
	    }
	    resume_sampling = true;
	    tls->sampling_status = CBTF_Monitor_Resumed;
	}
    //}

    if (resume_sampling) {
        if (tls->debug) {
	    fprintf(stderr,"[%d,%lu] monitor_mpi_post_comm_rank RESUME SAMPLING\n",
		    tls->pid,tls->tid);
        }
	//tls->sampling_status = CBTF_Monitor_Resumed;
	cbtf_offline_sampling_status(CBTF_Monitor_MPI_post_comm_rank_event,CBTF_Monitor_Resumed);
    }
}

/* monitor_mpi_pcontrol is reponsible for starting and stopping data
 * collection based on the user coding:
 *
 *     MPI_Pcontrol(0) to disable collection
 *     and
 *     MPI_Pcontrol(1) to reenable data collection
 *
*/
void monitor_mpi_pcontrol(int level)
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
  TLS* tls = CBTF_GetTLS(TLSKey);
#else
  TLS* tls = &the_tls;
#endif
  Assert(tls != NULL);

  if ( tls->mpi_pcontrol ) {

    if (tls->debug) {
	fprintf(stderr,"[%d,%lu] monitor_mpi_pcontrol CALLED level:%d\n", tls->pid,tls->tid,level);
    }


    if (level == 0) {
	if ((tls->sampling_status == CBTF_Monitor_Started ||
   	    tls->sampling_status == CBTF_Monitor_Resumed) && !tls->in_mpi_pre_init) {

	   tls->sampling_status = CBTF_Monitor_Paused;
	   cbtf_offline_sampling_status(CBTF_Monitor_mpi_pcontrol_event,CBTF_Monitor_Paused);

           if (tls->debug) {
	       fprintf(stderr,"monitor_mpi_pcontrol level 0 collector started, PAUSE SAMPLING %d,%lu\n", tls->pid,tls->tid);
           }
	}
    } else if (level == 1) {
	// this should not happen now.
	if (tls->sampling_status == CBTF_Monitor_Not_Started ) {

	    // should handle mrnet connection for cbtf-mrnet case.
	    cbtf_offline_notify_event(CBTF_Monitor_mpi_pcontrol_event);

	    tls->CBTF_monitor_type = CBTF_Monitor_Proc;
	    tls->sampling_status = CBTF_Monitor_Started;
	    cbtf_offline_start_sampling(NULL);

	} else if (tls->sampling_status == CBTF_Monitor_Paused && !tls->in_mpi_pre_init) {

	    if (tls->debug) {
		fprintf(stderr,"monitor_mpi_pcontrol level 1 collector started RESUME SAMPLING %d,%lu\n", tls->pid,tls->tid);
	    }
	    tls->sampling_status = CBTF_Monitor_Resumed;
	    cbtf_offline_sampling_status(CBTF_Monitor_mpi_pcontrol_event,CBTF_Monitor_Resumed);

	} else  if (tls->sampling_status == CBTF_Monitor_Resumed && !tls->in_mpi_pre_init) {
	    if (tls->debug) {
		fprintf(stderr,"monitor_mpi_pcontrol level 1 collector ALREADY RESUMED %d,%lu\n", tls->pid,tls->tid);
	    }
	}
    } else {
	fprintf(stderr,"monitor_mpi_pcontrol CALLED with unsupported level=%d\n", level);
    }
  } else {
      /* early return - do not honor mpi_pcontrol */
      if (tls->debug) {
  	fprintf(stderr,"monitor_mpi_pcontrol ENABLE_MPI_PCONTROL **NOT** SET IGNORING MPI_PCONTROL CALL %d,%lu\n", tls->pid,tls->tid);
      }
      return;
  }
}
