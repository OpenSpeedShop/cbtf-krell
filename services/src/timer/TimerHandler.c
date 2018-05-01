/*******************************************************************************
** Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
** Copyright (c) 2008 William Hachfeld. All Rights Reserved.
** Copyright (c) 2006-2011 The Krell Institute. All Rights Reserved.
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
 * Definition of the CBTF_Timer() function.
 *
 */

#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <stdbool.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

#include "KrellInstitute/Services/Common.h"
#include "KrellInstitute/Services/Timer.h"
#include "KrellInstitute/Services/TLS.h"

//extern int monitor_get_thread_num();

#define CBTF_ITIMER_SIGNAL   (SIGPROF)
#ifdef HAVE_POSIX_TIMERS
#define CBTF_REALTIME_SIGNAL (SIGRTMIN+3)
#ifndef sigev_notify_thread_id
/* manpages claim sigev_notify_thread_id. but this appears hidden
 * on some systems
 */
#define sigev_notify_thread_id  _sigev_un._tid
#endif
#endif

/** Mutual exclusion lock for accessing shared state. */
static pthread_mutex_t mutex_lock = PTHREAD_MUTEX_INITIALIZER;

/** Number of threads currently using timers (shared state). */
static unsigned num_threads = 0;

/** Old SIGPROF signal handler action (shared state). */
struct sigaction original_sigprof_action;

static int cbtf_timer_signal = 0;

static bool use_posix_timer = true;
static bool init_timer_signal = false;

/** Type defining the items stored in thread-local storage. */
typedef struct {

    /** Timer event handling function. */
    CBTF_TimerEventHandler timer_handler;
#ifdef HAVE_POSIX_TIMERS
    struct sigevent sig_event;
    timer_t	    timerid;
    bool	    posix_timer_initialized;
#endif

} TLS;

#ifdef USE_EXPLICIT_TLS

/**
 * Thread-local storage key.
 *
 * Key used for looking up our thread-local storage. This key <em>must</em>
 * be globally unique across the entire Open|SpeedShop code base.
 */
static const uint32_t TLSKey = 0xBAD0BEEF;

#else

/** Thread-local storage. */
static __thread TLS the_tls;

#endif



/**
 * Signal handler.
 *
 * Called by the operating system's signal handling system each time the
 * running thread is interrupted by our timer. Passes the signal context
 * to the per-thread timer event handling function.
 *
 * @param signal    Signal number.
 * @param info      Signal information.
 * @param ptr       Untyped pointer to thread context.
 *
 * @ingroup Implementation
 */
static void signalHandler(int signal, siginfo_t* info, void* ptr)
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
    if(tls == NULL) {
	tls = malloc(sizeof(TLS));
	Assert(tls != NULL);
	CBTF_SetTLS(TLSKey, tls);
    }
#else
    TLS* tls = &the_tls;
#endif

/* FIXME:NOTE
 * With explicit TLS and dyninst instrumentation this signalhandler
 * is being called in new pthreads as they are created BUT before
 * the start_samping call in the collector runtime has been called.
 * It is the start_sampling functions with allocate the timer tls.
 * Therefore the assert was causing premature aborts for online.
 * Once the start_sampling function is called then we will get the
 * tls for this thread and can continue to set up this threads timer.
 * This does not miss any samples AFAICT since dyninst has not yet
 * called our start_sampling functions via executeNow in the collector
 * runtimes.  Tested with test/executables/threads.
 */
    if (tls == NULL) {
	/* we have not yet officially started the timer from
	 * the collector runtime. Just return.
	 */
	return;
    }

    /* Call this thread's timer event handler */
    if(tls->timer_handler != NULL)
	(*tls->timer_handler)((ucontext_t*)ptr);
}



/**
 * Configure a per-thread timer.
 *
 * Configure a timer to interrupt the currently executing thread at a specified
 * interval. The specified event handler will be called at each interrupt. Any
 * previously configured timer is first removed. If the specified interval is
 * zero and/or the event handler is null, no new timer is configured.
 *
 * @note    The time measured here is CPU seconds spent executing the thread.
 *
 * @param interval   Timer interval (in nanoseconds).
 * @param handler    Timer event handler.
 *
 * @ingroup RuntimeAPI
 */
static void __CBTF_Timer(uint64_t interval, const CBTF_TimerEventHandler handler)
{
    struct sigaction action = {{0}};
#ifdef HAVE_POSIX_TIMERS
    struct itimerspec itspec = {{0}};
#endif
    struct itimerval itval = {{0}};

    /* Create and/or access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);

    /* Disable the timer for this thread */
    if (use_posix_timer) {
#ifdef HAVE_POSIX_TIMERS
	memset(&itspec, 0, sizeof(itspec));
#endif
    } else {
	memset(&itval, 0, sizeof(itval));
	Assert(setitimer(ITIMER_PROF, &itval, NULL) == 0);
    }

    /* Obtain exclusive access to shared state */
    Assert(pthread_mutex_lock(&mutex_lock) == 0);

    /* Is this thread enabling its timer? */
    if((interval > 0) && (handler != NULL)) {

	/* Is this the first thread using a timer? */
	if(num_threads == 0) {

	    /* Set the SIGPROF signal action to our signal handler */
	    memset(&action, 0, sizeof(action));
	    action.sa_sigaction = signalHandler;
	    sigfillset(&(action.sa_mask));
	    action.sa_flags = SA_SIGINFO;
	    Assert(sigaction(cbtf_timer_signal, &action, &original_sigprof_action) == 0);

	}

	/* If using posix timers we need to set a timer for each thread */
	if (use_posix_timer) {
#ifdef HAVE_POSIX_TIMERS
	    if(tls->posix_timer_initialized == false) {
		memset(&tls->sig_event, 0, sizeof(tls->sig_event));
		tls->sig_event.sigev_notify = SIGEV_THREAD_ID;
		tls->sig_event.sigev_signo = (CBTF_REALTIME_SIGNAL);
		tls->sig_event.sigev_value.sival_ptr = &tls->timerid;
		tls->sig_event.sigev_notify_thread_id = syscall(SYS_gettid);

		clockid_t clock = CLOCK_REALTIME;
		int ret = timer_create(clock, &tls->sig_event, &tls->timerid);
		if (ret == 0) {
		    tls->posix_timer_initialized = true;
		} else if (ret < 0) {
		    fprintf(stderr,"timer_create failed!\n");
		}
	    }
#endif
	}
	
	/* Is this thread using a timer now were it wasn't previously? */
	if(tls->timer_handler == NULL) {
	    
	    /* Increment the timer usage thread count */
	    ++num_threads;
	    
	}

    }
    
    /* Is this thread disabling its timer? */
    if((interval == 0) || (handler == NULL)) {
	
	if (use_posix_timer) {
#ifdef HAVE_POSIX_TIMERS
	    struct itimerspec stop_spec = {{0}};
	    memset(&stop_spec, 0, sizeof(stop_spec));
	    if (tls->posix_timer_initialized) {
		int ret = timer_delete(tls->timerid);
		if (ret < 0) {
		    fprintf(stderr,"timer_delete failed!\n");
		}
	    }
	    tls->posix_timer_initialized = false;
#endif
	}

	/* Decrement the timer usage thread count */
	--num_threads;

	/* Was this the last thread using the timer? */
	if(num_threads == 0) {
	    
	    /* Return the signal action to its original value */
	    Assert(sigaction(cbtf_timer_signal, &original_sigprof_action, NULL) == 0);
	    
	}	
	
    }
    
    /** Release exclusive access to shared state */
    Assert(pthread_mutex_unlock(&mutex_lock) == 0);
    
    /* Is this thread enabling a new timer? */
    if((interval > 0) && (handler != NULL)) {

	/* Configure the new timer event handler for this thread */
	tls->timer_handler = handler;
	
	/* Enable a timer for this thread */
	if (use_posix_timer) {
#ifdef HAVE_POSIX_TIMERS
	    tls->timer_handler = handler;
	    itspec.it_interval.tv_sec = interval / (uint64_t)(1000000000);
	    itspec.it_interval.tv_nsec =
	    (1000 * (interval % (uint64_t)(1000000000))) / (uint64_t)(1000);
	    itspec.it_value.tv_sec = itspec.it_interval.tv_sec;
	    itspec.it_value.tv_nsec = itspec.it_interval.tv_nsec;
	    int rval = timer_settime(tls->timerid, 0, &itspec, NULL);
	    if (rval) {
		fprintf(stderr,"timer_settime FAILED!\n");
	    }
#endif
	} else {
	    itval.it_interval.tv_sec = interval / (uint64_t)(1000000000);
	    itval.it_interval.tv_usec =
	    (interval % (uint64_t)(1000000000)) / (uint64_t)(1000);
	    itval.it_value.tv_sec = itval.it_interval.tv_sec;
	    itval.it_value.tv_usec = itval.it_interval.tv_usec;
	    Assert(setitimer(ITIMER_PROF, &itval, NULL) == 0);
	}
	
    }    
    else {

	/* Remove the timer event handler for this thread */
	tls->timer_handler = NULL;
	
    }
}


void CBTF_SetTimerSignal()
{
    if(!init_timer_signal) {
#ifdef HAVE_POSIX_TIMERS
	cbtf_timer_signal = CBTF_REALTIME_SIGNAL;
	use_posix_timer = true;
#else
	cbtf_timer_signal = CBTF_ITIMER_SIGNAL;
	use_posix_timer = false;
#endif
	if (getenv("CBTF_FORCE_ITIMER_SIGNAL") != NULL) {
	    cbtf_timer_signal = CBTF_ITIMER_SIGNAL;
	    use_posix_timer = false;
	}
	init_timer_signal = true;
    }
}

int CBTF_GetTimerSignal()
{
    return cbtf_timer_signal;
}

void CBTF_Timer(uint64_t interval, const CBTF_TimerEventHandler handler)
{
    /* Create and/or access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
    if(tls == NULL) {
	tls = malloc(sizeof(TLS));
	Assert(tls != NULL);
	CBTF_SetTLS(TLSKey, tls);
    }
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);

    if(!init_timer_signal) {
	tls->posix_timer_initialized = false;
	CBTF_SetTimerSignal();
    }
    __CBTF_Timer(interval, handler);
}

/* BLOCK profiling signals.
 * We need to do more than ignore samples (defer_sampling).
 * It is best to block the profiling signal. Currently that
 * is SIGPROF. When we add a posix based timer that handles
 * thread samples correctly we will be blocking one of the
 * real time signals (SIGRTMIN or SIGRTMIN+N) as well and
 * likely default to the posix based timer.
 * fixes issues seen with omnipath based mpi connects.
 */
void CBTF_BlockTimerSignal()
{
    if(!init_timer_signal) {
	CBTF_SetTimerSignal();
    }
    sigset_t signal_set;
    sigemptyset(&signal_set);
    sigaddset(&signal_set, CBTF_GetTimerSignal());
    // FIXME: should we use pthread_sigmask here?
    sigprocmask(SIG_BLOCK, &signal_set, NULL);
}

/* BLOCK profiling signals.
 */
void CBTF_UnBlockTimerSignal()
{
    if(!init_timer_signal) {
	CBTF_SetTimerSignal();
    }
    sigset_t signal_set;
    sigemptyset(&signal_set);
    sigaddset(&signal_set, CBTF_GetTimerSignal());
    // FIXME: should we use pthread_sigmask here?
    sigprocmask(SIG_UNBLOCK, &signal_set, NULL);
}
