/*******************************************************************************
** Copyright (c) 2007-2011 The Krell Institue. All Rights Reserved.
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
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <pthread.h>
#include "monitor.h"

#include "KrellInstitute/Services/Monitor.h"
#include "KrellInstitute/Services/Time.h"
#include "KrellInstitute/Services/TLS.h"

extern void cbtf_offline_start_sampling(const char* arguments);
extern void cbtf_offline_stop_sampling(const char* arguments, const int finished);
extern void cbtf_offline_record_dso(const char* dsoname);
extern void cbtf_offline_defer_sampling(const int flag);
extern void cbtf_offline_pause_sampling();
extern void cbtf_offline_resume_sampling();

/** Type defining the items stored in thread-local storage. */
typedef struct {
    int debug;
    int in_mpi_pre_init;
    CBTF_Monitor_Status sampling_status;
    int process_is_terminating;
    int thread_is_terminating;
    pthread_t tid;
    pid_t pid;
    CBTF_Monitor_Type CBTF_monitor_type;
} TLS;

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

#if 0
    Assert(tls != NULL);
#else
    /* The assert above can cause libmonitor to hang with mpt.
     * We may have forked a process which then did not call
     * monitor_init_process and allocate TLS.
     */
    if (tls == NULL) {
       fprintf(stderr,"Warning. monitor_fini_process called with no TLS.\n");
       return;
    }
#endif 

    /*collector stop_sampling does not use the arguments param */
    if (tls->debug) {
	fprintf(stderr,"monitor_fini_process FINISHED SAMPLING %d,%lu\n",
		tls->pid,tls->tid);
    }

    static int f = 0;
    if (f > 0)
      raise(SIGSEGV);
    f++;

    tls->sampling_status = CBTF_Monitor_Finished;
    if(how == MONITOR_EXIT_EXEC) {
	tls->process_is_terminating = 1;
	cbtf_offline_stop_sampling(NULL, 1);
    } else {
	tls->process_is_terminating = 1;
	cbtf_offline_stop_sampling(NULL, 1);
    }
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

    if ( (getenv("CBTF_DEBUG_MONITOR_SERVICE") != NULL)) {
	tls->debug=1;

#if 0
	/* get the identifier of this thread */
	pthread_t (*f_pthread_self)();
	f_pthread_self = (pthread_t (*)())dlsym(RTLD_DEFAULT, "pthread_self");
	tls->tid = (f_pthread_self != NULL) ? (*f_pthread_self)() : 0;
#else
	tls->tid = 0;
#endif


    } else {
	tls->debug=0;
    }

    tls->pid = getpid();

    if (tls->debug) {
	fprintf(stderr,"monitor_init_process BEGIN SAMPLING %d,%lu\n",
		tls->pid,tls->tid);
    }

    tls->in_mpi_pre_init = 0;
    tls->CBTF_monitor_type = CBTF_Monitor_Proc;
    tls->sampling_status = CBTF_Monitor_Started;
    cbtf_offline_start_sampling(NULL);
    return (data);
}

/*
 * callbacks for handling of monitor init
 */
void monitor_init_library(void)
{
}

void monitor_fini_library(void)
{
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

    if (tls->debug) {
	fprintf(stderr,"monitor_fini_thread FINISHED SAMPLING %d,%lu\n",
		tls->pid,tls->tid);
    }

    tls->sampling_status = CBTF_Monitor_Finished;
    tls->thread_is_terminating = 1;
    cbtf_offline_stop_sampling(NULL,1);
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

    if ( (getenv("CBTF_DEBUG_MONITOR_SERVICE") != NULL)) {
	tls->debug=1;

#if 0
	/* get the identifier of this thread */
	pthread_t (*f_pthread_self)();
	f_pthread_self = (pthread_t (*)())dlsym(RTLD_DEFAULT, "pthread_self");
	tls->tid = (f_pthread_self != NULL) ? (*f_pthread_self)() : 0;
#else
	tls->tid = 0;
#endif


    } else {
	tls->debug=0;
    }

    tls->pid = getpid();

    if (tls->debug) {
	fprintf(stderr,"monitor_init_thread BEGIN SAMPLING %d,%lu\n",
		tls->pid,tls->tid);
    }

    tls->in_mpi_pre_init = 0;
    tls->CBTF_monitor_type = CBTF_Monitor_Thread;
    tls->sampling_status = CBTF_Monitor_Started;
    cbtf_offline_start_sampling(NULL);
    return(data);
}

void monitor_init_thread_support(void)
{
}

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
    if (tls == NULL || tls && tls->sampling_status == 0 ) {
	return;
    }

    if (library == NULL) {
	if (tls->debug) {
	    fprintf(stderr,"monitor_dlopen ignores null library name\n");
	}
	return;
    }

    /* TODO:
     * if CBTF_GetDLInfo does not handle errors do so here.
     */
    if (tls->debug) {
	fprintf(stderr,"monitor_dlopen called with %s for %d,%lu\n",
	    library, tls->pid,tls->tid);
    }

    /* On some systems (NASA) it appears that dlopen can be called
     * before monitor_init_process (or even monitor_early_init).
     * So we need to use getpid() directly here.
     */ 
    int retval = CBTF_GetDLInfo(getpid(), library);

    if (tls->sampling_status == CBTF_Monitor_Paused && !tls->in_mpi_pre_init) {
        if (tls->debug) {
	    fprintf(stderr,"monitor_dlopen RESUME SAMPLING %d,%lu\n",
		tls->pid,tls->tid);
        }
	tls->sampling_status = CBTF_Monitor_Resumed;
	offline_resume_sampling(CBTF_Monitor_dlopen_event);
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
    if (tls == NULL || tls && tls->sampling_status == 0 ) {
	return;
    }

    if (path == NULL) {
	if (tls->debug) {
	    fprintf(stderr,"monitor_pre_dlopen ignores null path\n");
	}
	return;
    }

    if (tls->debug) {
	fprintf(stderr,"monitor_pre_dlopen %s for %d,%lu\n",
		path, tls->pid,tls->tid);
    }

    if ((tls->sampling_status == CBTF_Monitor_Started ||
	 tls->sampling_status == CBTF_Monitor_Resumed) && !tls->in_mpi_pre_init) {
        if (tls->debug) {
	    fprintf(stderr,"monitor_pre_dlopen PAUSE SAMPLING %d,%lu\n",
		tls->pid,tls->tid);
        }
	tls->sampling_status = CBTF_Monitor_Paused;
	offline_pause_sampling(CBTF_Monitor_pre_dlopen_event);
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

    if (tls == NULL || tls && tls->sampling_status == 0 ) {
	return;
    }

    if (!tls->thread_is_terminating || !tls->process_is_terminating) {
	if ((tls->sampling_status == CBTF_Monitor_Started ||
	     tls->sampling_status == CBTF_Monitor_Resumed) && !tls->in_mpi_pre_init) {
            if (tls->debug) {
	        fprintf(stderr,"monitor_dlclose PAUSE SAMPLING %d,%lu\n",
		    tls->pid,tls->tid);
            }
	    tls->sampling_status = CBTF_Monitor_Paused;
	    offline_pause_sampling(CBTF_Monitor_dlclose_event);
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

    if (tls == NULL || tls && tls->sampling_status == 0 ) {
	return;
    }

    if (!tls->thread_is_terminating || !tls->process_is_terminating) {
	if (tls->sampling_status == CBTF_Monitor_Paused && !tls->in_mpi_pre_init) {
            if (tls->debug) {
	        fprintf(stderr,"monitor_post_dlclose RESUME SAMPLING %d,%lu\n",
		    tls->pid,tls->tid);
            }
	    tls->sampling_status = CBTF_Monitor_Resumed;
	    offline_resume_sampling(CBTF_Monitor_post_dlclose_event);
	}
    }
}

#endif

#ifdef HAVE_TARGET_FORK
/* 
 * callbacks for handling of FORK.
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

    /* Stop sampling prior to real fork. */
    if (tls->sampling_status == CBTF_Monitor_Paused ||
	tls->sampling_status == CBTF_Monitor_Started) {
        if (tls->debug) {
	    fprintf(stderr,"monitor_pre_fork FINISHED SAMPLING %d,%lu\n",
		    tls->pid,tls->tid);
        }
	tls->sampling_status = CBTF_Monitor_Finished;
	cbtf_offline_stop_sampling(NULL,1);
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

    /* Resume/start sampling forked process. */
    if (tls->sampling_status == CBTF_Monitor_Paused ||
	tls->sampling_status == CBTF_Monitor_Finished) {
        if (tls->debug) {
	    fprintf(stderr,"monitor_post_fork BEGIN SAMPLING %d,%lu\n",
		    tls->pid,tls->tid);
        }
	tls->CBTF_monitor_type = CBTF_Monitor_Proc;
	tls->sampling_status = 1;
	cbtf_offline_start_sampling(NULL);
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

    tls->in_mpi_pre_init = 1;

    if (tls->sampling_status == CBTF_Monitor_Started) {
        if (tls->debug) {
	    fprintf(stderr,"monitor_mpi_pre_init PAUSE SAMPLING %d,%lu\n",
		    tls->pid,tls->tid);
        }
	tls->sampling_status = CBTF_Monitor_Paused;
	cbtf_offline_pause_sampling(CBTF_Monitor_MPI_pre_init_event);
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

    if (tls->sampling_status == CBTF_Monitor_Paused) {
        if (tls->debug) {
	    fprintf(stderr,"monitor_init_mpi RESUME SAMPLING %d,%lu\n",
		    tls->pid,tls->tid);
        }
	tls->sampling_status = CBTF_Monitor_Resumed;
	tls->CBTF_monitor_type = CBTF_Monitor_Proc;
	cbtf_offline_resume_sampling(CBTF_Monitor_MPI_init_event);
    }

    tls->in_mpi_pre_init = 0;
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
	fprintf(stderr,"monitor_fini_mpi CALLED %d,%lu\n",
		tls->pid,tls->tid);
    }

    if (tls->sampling_status == CBTF_Monitor_Started ||
	tls->sampling_status == CBTF_Monitor_Resumed) {
        if (tls->debug) {
	    fprintf(stderr,"monitor_fini_mpi PAUSE SAMPLING %d,%lu\n",
		    tls->pid,tls->tid);
        }
	tls->sampling_status = CBTF_Monitor_Paused;
	cbtf_offline_pause_sampling(CBTF_Monitor_MPI_fini_event);
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
	fprintf(stderr,"monitor_mpi_post_fini CALLED %d,%lu\n",
		tls->pid,tls->tid);
    }

    if (tls->sampling_status == CBTF_Monitor_Paused) {
        if (tls->debug) {
	    fprintf(stderr,"monitor_mpi_post_fini RESUME SAMPLING %d,%lu\n",
		    tls->pid,tls->tid);
        }
	tls->sampling_status = CBTF_Monitor_Resumed;
	cbtf_offline_resume_sampling(CBTF_Monitor_MPI_post_fini_event);
    }
}

void monitor_mpi_post_comm_rank(void)
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);

    if (tls->debug) {
	fprintf(stderr,"monitor_mpi_post_comm_rank CALLED %d,%lu at %lu\n",
		tls->pid,tls->tid, CBTF_GetTime());
    }

    if (tls->sampling_status == CBTF_Monitor_Started ||
	tls->sampling_status == CBTF_Monitor_Resumed) {
        if (tls->debug) {
	    fprintf(stderr,"monitor_mpi_post_comm_rank PAUSE SAMPLING %d,%lu\n",
		    tls->pid,tls->tid);
        }
	tls->sampling_status = CBTF_Monitor_Paused;
	cbtf_offline_pause_sampling(CBTF_Monitor_MPI_post_comm_rank_event);
    }

    cbtf_offline_notify_event(CBTF_Monitor_MPI_post_comm_rank_event);

    if (tls->sampling_status == CBTF_Monitor_Paused) {
        if (tls->debug) {
	    fprintf(stderr,"monitor_mpi_post_comm_rank RESUME SAMPLING %d,%lu\n",
		    tls->pid,tls->tid);
        }
	tls->sampling_status = CBTF_Monitor_Resumed;
	cbtf_offline_resume_sampling(CBTF_Monitor_MPI_post_comm_rank_event);
    }
}
