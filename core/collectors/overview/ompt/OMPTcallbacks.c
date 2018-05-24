/*******************************************************************************
** Copyright (c) 2017  The Krell Institute. All Rights Reserved.
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
 * Definition of the omptp callbacks.
 *
 */

// struct CBTF_omptp_event {
//     uint64_t time;   /**< time of the call. */
//     uint16_t stacktrace;  /**< Index of the stack trace. */
// };

#include <stdbool.h> /* C99 std */
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "collector.h"
#include "ompt.h"
#include "KrellInstitute/Services/Assert.h"
#include "KrellInstitute/Services/Common.h"
#include "KrellInstitute/Services/Ompt.h"
#include "KrellInstitute/Services/Time.h"
#include "KrellInstitute/Services/TLS.h"
#include "KrellInstitute/Messages/Ompt_data.h"


ompt_enumerate_state_t ompt_enumerate_state;
ompt_set_callback_t ompt_set_callback;
ompt_get_callback_t ompt_get_callback;
ompt_get_idle_frame_t ompt_get_idle_frame;
ompt_get_task_frame_t ompt_get_task_frame;
ompt_get_state_t ompt_get_state;
ompt_get_parallel_id_t ompt_get_parallel_id;
ompt_get_task_id_t ompt_get_task_id;
ompt_get_thread_id_t ompt_get_thread_id;

#include <execinfo.h>
static void print_bt()
{
  void *array[32];
  size_t size;
  char **strings;
  size_t i;

  size = backtrace (array, 32);
  strings = backtrace_symbols (array, size);
  printf ("Obtained %zd stack frames.\n", size);
  for (i = 0; i < size; i++)
     printf ("%" PRIu64 ": [%d] %p:  %s\n", ompt_get_thread_id(), i, array[i], strings[i]);

}

static void print_ids(int level)
{
  ompt_frame_t* frame = ompt_get_task_frame(level);
  printf("[%" PRIu64 "] level %d: parallel_id=%" PRIu64 ", task_id=%" PRIu64 ", exit_frame=%p, reenter_frame=%p\n", ompt_get_thread_id(), level, ompt_get_parallel_id(level), ompt_get_task_id(level), frame->exit_runtime_frame, frame->reenter_runtime_frame);
}

#define print_frame(level)\
do {\
  printf("%" PRIu64 ": __builtin_frame_address(%d)=%p\n", ompt_get_thread_id(), level, __builtin_frame_address(level));\
} while(0)

static int cbtf_ompt_debug = 0;
static int cbtf_ompt_debug_details = 0;
static int cbtf_ompt_debug_blame = 0;
static int cbtf_ompt_debug_trace = 0;
static int cbtf_ompt_debug_wait = 0;
static int cbtf_ompt_debug_lock = 0;

/** Type defining the items stored in thread-local storage. */
typedef struct {
    bool     collector_active;
    uint64_t collector_etime;
    uint64_t region_btime;
    uint64_t region_ttime;
    uint64_t region_count;
    uint64_t idle_btime;
    uint64_t idle_ttime;
    uint64_t barrier_btime;
    uint64_t barrier_ttime;
    uint64_t wbarrier_btime;
    uint64_t wbarrier_ttime;
    uint64_t lock_btime;
    uint64_t lock_time;
    uint64_t itask_btime;
    uint64_t itask_ttime;
    uint64_t serial_btime;
    uint64_t serial_ttime;
    uint64_t thread_btime;
    uint64_t thread_ttime;
    uint64_t stacktrace[MaxFramesPerStackTrace];
    unsigned stacktrace_size;
    CBTF_omptp_event region_event;
    CBTF_omptp_event task_event;
} TLS;

static uint64_t current_region_context = NULL;

static int level = 0;
static struct {
    CBTF_omptp_region values[MAX_REGIONS];
} Regions = { { 0 } };

int CBTF_omptp_region_add(uint64_t parallelID, uint64_t *stacktrace, unsigned stacktrace_size)
{
    int i;
    for (i = 0; i < MAX_REGIONS; ++i) {
        if (Regions.values[i].id == 0) {
	    Regions.values[i].id = parallelID;
	    Regions.values[i].stacktrace_size = stacktrace_size;
	    int j;
	    for (j = 0; j < stacktrace_size; ++j) {
		Regions.values[i].stacktrace[j] = stacktrace[j];
	    }
	    break;
	}
    }
    return i;
}

void CBTF_omptp_region_clear(uint64_t id)
{
    int i;
    for (i = 0; i < MAX_REGIONS; ++i) {
        if (Regions.values[i].id == id) {
	    Regions.values[i].id = 0;
	    memset(&Regions.values[i],0,sizeof(CBTF_omptp_region));
	    break;
	}
    }
}

CBTF_omptp_region CBTF_omptp_region_with_id(uint64_t id)
{
    CBTF_omptp_region region;
    int i;
    for (i = 0; i < MAX_REGIONS; ++i) {
        if (Regions.values[i].id == id) {
	    region = Regions.values[i];
	    break;
	}
    }
    return region;
}

#if defined(USE_EXPLICIT_TLS)

/**
 *  * Key used to look up our thread-local storage. This key <em>must</em> be
 *   * unique from any other key used by any of the CBTF services.
 *    */
static const uint32_t TLSKey = 0x00008EFA;
int ompt_cb_init_tls_done = 0;

#else

/** Thread-local storage. */
static __thread TLS the_tls;

#endif

/**
  * @param tls    Thread-local storage to be initialized.
  */
static void initialize_data(TLS* tls)
{
}

// Attempt to register a callback with the ompt api.  Not all callbacks
// are currently implemented in all ompt implementation begining with
// intel 12.  No intel release currently has ompt so we  must
// supply a libiomp built from intel source that has the ompt api.
void CBTF_register_ompt_callback(ompt_event_t event, void* callback)
{
    int rval = ompt_set_callback(event, (ompt_callback_t)callback);
#ifndef NDEBUG
    if (cbtf_ompt_debug_details) {
	if (!rval) {
	    fprintf(stderr,"register_ompt_callback: FAILURE registering ompt event %d\n",event);
	} else {
	    fprintf(stderr,"register_ompt_callback: SUCCESS registering ompt event %d\n",event);
	}
    }
#endif
}

void CBTF_ompt_set_collector_active(bool flag)
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
    if (tls == NULL) {
        tls = malloc(sizeof(TLS));
        Assert(tls != NULL);
        CBTF_SetTLS(TLSKey, tls);
        ompt_cb_init_tls_done = 1;
    }
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);
    tls->collector_active=flag;
    tls->collector_etime=CBTF_GetTime();
}

// unused helper...
#if 0
level=omp_get_level()
void report_num_threads(int level) {
    #pragma omp critical
    {
        printf("Level %d: number of threads in the team - %d\n",
                  level, omp_get_num_threads());
    }
}
#endif

// ompt_event_MAY_ALWAYS
// record start time and get address or callstack of this context
// This seems to only be active in the master thread.
// increment is parallel flag. This flag indicates that a
// parallel region is active to other callbacks that may be
// interested.
void CBTF_ompt_cb_parallel_begin (
  ompt_task_id_t parent_taskID,     /* parent task id */
  ompt_frame_t *parent_taskframe,   /* parent task frame data */
  ompt_parallel_id_t parallelID,    /* parallel region ID */
  uint32_t requested_team_size,     /* number of threads in team */
  void *parallel_function,          /* pointer to outlined function */
  ompt_invoker_t invoker)           /* who invokes master task? */
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);

    // do not sample if inside our tool
    tls->region_btime = CBTF_GetTime();

 
    /* Regions are only tracked on the master thread.  We will create a
     * callstack context here that is used by all the worker threads.
     * The implicit_task callbasks will record the original context
     * from the master.
     *
     * Handle nested regions via regionMap.
     */

    current_region_context = (uint64_t)parallel_function;
    uint64_t stacktrace[MaxFramesPerStackTrace];
    unsigned stacktrace_size;
    CBTF_omptp_event event;
    omptp_start_event(&event,(uint64_t)parallel_function, stacktrace, &stacktrace_size);
    tls->region_event.time = 0;
    int nest_level = CBTF_omptp_region_add(parallelID, stacktrace, stacktrace_size);

#ifndef NDEBUG
    if (cbtf_ompt_debug) {
	fprintf(stderr,
	"[%d] CBTF_ompt_cb_parallel_begin parallelID:%lu parent_taskID:%lu req_team_size:%u invoker:%d context:%p nest_level=%d\n",
	ompt_get_thread_id(), parallelID, parent_taskID, requested_team_size, invoker, parallel_function, nest_level);
    }
#endif
}

// ompt_event_MAY_ALWAYS
void CBTF_ompt_cb_parallel_end (
  ompt_parallel_id_t parallelID,    /* parallel region ID */
  ompt_task_id_t parent_taskID,     /* parent task id */
  ompt_invoker_t invoker)           /* pointer to outlined function */
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);

    // do not sample if inside our tool
    tls->region_ttime += CBTF_GetTime() - tls->region_btime;
    tls->region_event.time = CBTF_GetTime() - tls->region_btime;

#ifndef NDEBUG
    if (cbtf_ompt_debug) {
	fprintf(stderr,
	"[%d] CBTF_ompt_cb_parallel_end parallelID:%lu parent_taskID:%lu invoker:%d context:%p total time:%f\n",
	ompt_get_thread_id(), parallelID, parent_taskID, invoker, current_region_context, (float)tls->region_ttime/1000000000);
    }
#endif

    current_region_context = (uint64_t)NULL;

#if 0
    ompt_task_id_t tsk_id = ompt_get_task_id(0);
    ompt_frame_t *rt_frame = ompt_get_task_frame(0);

    fprintf(stderr, "[%d] tsk_id:%d CBTF_ompt_cb_parallel_end: rt_frame->exit_runtime_frame=%p rt_frame->reenter_runtime_frame=%p\n",
	ompt_get_thread_id(), tsk_id, rt_frame->exit_runtime_frame,rt_frame->reenter_runtime_frame);
#endif

    CBTF_omptp_region_clear(parallelID);
}

// ompt_event_MAY_ALWAYS
void CBTF_ompt_cb_task_begin (
  ompt_task_id_t parent_taskID,     /* parent task id */
  ompt_frame_t *parent_taskframe,   /* unused parent task frame data */
  ompt_task_id_t new_taskID,        /* new task id */
  void *task_function)              /* context of task - function pointer */
{
#ifndef NDEBUG
    if (cbtf_ompt_debug_blame) {
	fprintf(stderr,"[%d] CBTF_ompt_cb_task_begin: parent_taskID:%lu new_task_id:%lu task_function:%p\n",
	ompt_get_thread_id() ,parent_taskID, new_taskID, task_function);
    }
#endif
}

// ompt_event_MAY_ALWAYS
void CBTF_ompt_cb_task_end (
  ompt_task_id_t parent_taskID     /* parent task id */
  )
{
#ifndef NDEBUG
    if (cbtf_ompt_debug) {
	fprintf(stderr,"[%d] CBTF_ompt_cb_task_end: parent_taskID = %lu\n",ompt_get_thread_id() ,parent_taskID);
    }
#endif
}

// ompt_event_MAY_ALWAYS
// monitor notifies us of this...
// pass omp thread num to collector.
void CBTF_ompt_cb_thread_begin()
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
    if (tls == NULL) {
        tls = malloc(sizeof(TLS));
        Assert(tls != NULL);
        CBTF_SetTLS(TLSKey, tls);
        ompt_cb_init_tls_done = 1;
    }
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);

    tls->collector_etime = 0;
    tls->region_ttime = 0;
    tls->idle_ttime = 0;
    tls->barrier_ttime = 0;
    tls->wbarrier_ttime = 0;
    tls->itask_ttime = 0;
    tls->serial_ttime = 0;
    tls->serial_btime = CBTF_GetTime();
    tls->thread_btime = tls->serial_btime;
    tls->lock_btime = 0;
    tls->lock_time = 0;
#ifndef NDEBUG
    if (cbtf_ompt_debug) {
	fprintf(stderr, "[%d] CBTF_ompt_cb_thread_begin:\n",ompt_get_thread_id());
    }
#endif
    //cbtf_collector_set_openmp_threadid(ompt_get_thread_id());
}

// ompt_event_MAY_ALWAYS
// monitor notifies us of this as well...
void CBTF_ompt_cb_thread_end()
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);

    //tls->thread_ttime = CBTF_GetTime() - tls->thread_btime;
    tls->thread_ttime = tls->collector_etime - tls->thread_btime;

#ifndef NDEBUG
    if (cbtf_ompt_debug) {
	fprintf(stderr, "[%d] CBTF_ompt_cb_thread_end TIMES: thread:%f region:%f itask:%f serial:%f barrier:%f wait_barrier:%f idle:%f\n",
	    ompt_get_thread_id(),
	    (float)tls->thread_ttime/1000000000,
	    (float)tls->region_ttime/1000000000,
	    (float)tls->itask_ttime/1000000000,
	    (float)tls->serial_ttime/1000000000,
	    (float)tls->barrier_ttime/1000000000,
	    (float)tls->wbarrier_ttime/1000000000,
	    (float)tls->idle_ttime/1000000000
	);
    }
#endif
}

// ompt_event_MAY_ALWAYS
void CBTF_ompt_cb_control(uint64_t command, uint64_t modifier)
{
#ifndef NDEBUG
    if (cbtf_ompt_debug_details) {
	fprintf(stderr,"[%d] CBTF_ompt_cb_control: %llx, %llx\n",ompt_get_thread_id(), command, modifier);
    }
#endif
}

// ompt_event_MAY_ALWAYS
// shut down of ompt ...
void CBTF_ompt_cb_runtime_shutdown()
{
#ifndef NDEBUG
    if (cbtf_ompt_debug_details) {
	fprintf(stderr, "[%d] CBTF_ompt_cb_runtime_shutdown:\n",ompt_get_thread_id());
    }
#endif
}

// ompt_event_MAY_ALWAYS_TRACE
// set wait, start time wait on wait name
void CBTF_ompt_cb_wait_atomic (ompt_wait_id_t *waitID) {
#ifndef NDEBUG
    if (cbtf_ompt_debug_trace) {
	fprintf(stderr, "[%d] CBTF_ompt_cb_wait_atomic: waitID = %lu\n",ompt_get_thread_id(),waitID);
    }
#endif
}

// ompt_event_MAY_ALWAYS_TRACE
// set wait, start time wait on wait name
void CBTF_ompt_cb_wait_ordered (ompt_wait_id_t *waitID) {
#ifndef NDEBUG
    if (cbtf_ompt_debug_trace) {
        ompt_state_t task_state = ompt_get_state(waitID);
	fprintf(stderr, "[%d] CBTF_ompt_cb_wait_ordered: waitID:%x task_state:%x\n",
		ompt_get_thread_id(),waitID,task_state);
    }
#endif
}

// ompt_event_UNIMPLEMENTED
// set wait, start time wait on wait name
void CBTF_ompt_cb_wait_critical (ompt_wait_id_t *waitID) {
#ifndef NDEBUG
    if (cbtf_ompt_debug_wait) {
	fprintf(stderr, "[%d] CBTF_ompt_cb_wait_critical: waitID = %lu\n",ompt_get_thread_id(),waitID);
    }
#endif
}

// ompt_event_UNIMPLEMENTED
// set wait, start time wait on wait name
void CBTF_ompt_cb_wait_lock (ompt_wait_id_t *waitID) {
#ifndef NDEBUG
    if (cbtf_ompt_debug_wait) {
	fprintf(stderr, "[%d] CBTF_ompt_cb_wait_lock: waitID = %lu\n",ompt_get_thread_id(),waitID);
    }
#endif
}

// ompt_event_MAY_ALWAYS_TRACE
// if acquired, end time on wait name.
// clear wait, set acquired, start time on region name
void CBTF_ompt_cb_acquired_atomic (ompt_wait_id_t *waitID) {
#ifndef NDEBUG
    if (cbtf_ompt_debug_trace) {
	fprintf(stderr, "[%d] CBTF_ompt_cb_acquired_atomic: waitID = %lu\n",ompt_get_thread_id(),waitID);
    }
#endif
}

// ompt_event_MAY_ALWAYS_TRACE
// if acquired, end time on wait name.
// clear wait, set acquired, start time on region name
void CBTF_ompt_cb_acquired_ordered (ompt_wait_id_t *waitID) {
#ifndef NDEBUG
    if (cbtf_ompt_debug_trace) {
        ompt_state_t task_state = ompt_get_state(waitID);
	fprintf(stderr, "[%d] CBTF_ompt_cb_acquired_ordered: waitID:%x task_state:%x\n",
		ompt_get_thread_id(),waitID,task_state);
    }
#endif
}

// ompt_event_UNIMPLEMENTED
void CBTF_ompt_cb_acquired_critical (ompt_wait_id_t *waitID) {
#if 1
#ifndef NDEBUG
    if (cbtf_ompt_debug_wait) {
	fprintf(stderr, "[%d] CBTF_ompt_cb_acquired_critical: waitID = %lu\n",ompt_get_thread_id(),waitID);
    }
#endif
#endif
}

// ompt_event_UNIMPLEMENTED
// if acquired, end time on wait name.
// clear wait, set acquired, start time on region name
void CBTF_ompt_cb_acquired_lock (ompt_wait_id_t *waitID) {
#if 1
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);
    tls->lock_btime = CBTF_GetTime();
#ifndef NDEBUG
    if (cbtf_ompt_debug_trace) {
    }
#endif
	fprintf(stderr, "[%d] CBTF_ompt_cb_acquired_lock: waitID = %lu\n",ompt_get_thread_id(),waitID);
#endif
}

// ompt_event_MAY_ALWAYS_BLAME
void CBTF_ompt_cb_release_atomic (ompt_wait_id_t *waitID) {
#ifndef NDEBUG
    if (cbtf_ompt_debug_blame) {
	fprintf(stderr, "[%d] CBTF_ompt_cb_release_atomic: waitID = %lu\n",ompt_get_thread_id(),waitID);
    }
#endif
}

// ompt_event_MAY_ALWAYS_BLAME
void CBTF_ompt_cb_release_ordered (ompt_wait_id_t *waitID) {
#ifndef NDEBUG
    if (cbtf_ompt_debug_blame) {
        ompt_state_t task_state = ompt_get_state(waitID);
	fprintf(stderr, "[%d] CBTF_ompt_cb_release_ordered: waitID:%x task_state:%x\n",
		ompt_get_thread_id(),waitID,task_state);
    }
#endif
}

// ompt_event_MAY_ALWAYS_BLAME
void CBTF_ompt_cb_release_critical (ompt_wait_id_t *waitID) {
#ifndef NDEBUG
    if (cbtf_ompt_debug_blame) {
	fprintf(stderr, "[%d] CBTF_ompt_cb_release_critical: waitID = %lu\n",ompt_get_thread_id(),waitID);
    }
#endif
}

// ompt_event_MAY_ALWAYS_BLAME
// if acquired, end time region name, clear acquired
void CBTF_ompt_cb_release_lock (ompt_wait_id_t *waitID) {
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);

#ifndef NDEBUG
    if (cbtf_ompt_debug_blame) {
	fprintf(stderr, "[%d] CBTF_ompt_cb_release_lock: waitID = %lu\n",ompt_get_thread_id(),waitID);
    }
#endif

    tls->lock_time += CBTF_GetTime() - tls->lock_btime;
}

// TODO: LOCKS
// ompt_event_init_lock

void CBTF_ompt_cb_init_lock (ompt_wait_id_t *waitID) {
#ifndef NDEBUG
    if (cbtf_ompt_debug_lock) {
	fprintf(stderr, "[%d] CBTF_ompt_cb_init_lock: waitID = %lu\n",ompt_get_thread_id(),waitID);
    }
#endif
}

// ompt_event_init_destroy_lock
//
void CBTF_ompt_cb_destroy_lock (ompt_wait_id_t *waitID) {
#ifndef NDEBUG
    if (cbtf_ompt_debug_lock) {
	fprintf(stderr, "[%d] CBTF_ompt_cb_destroy_lock: waitID = %lu\n",ompt_get_thread_id(),waitID);
    }
#endif
}

void CBTF_ompt_cb_wait_nest_lock (ompt_wait_id_t *waitID) {
#ifndef NDEBUG
    if (cbtf_ompt_debug_lock) {
	fprintf(stderr, "[%d] CBTF_ompt_cb_wait_nest_lock: waitID = %lu\n",ompt_get_thread_id(),waitID);
    }
#endif
}

// ompt_event_release_nest_lock_last ompt_event_MAY_ALWAYS_BLAME
void CBTF_ompt_cb_release_nest_lock_last (ompt_wait_id_t *waitID) {
#ifndef NDEBUG
    if (cbtf_ompt_debug_lock) {
	fprintf(stderr, "[%d] CBTF_ompt_cb_release_nest_lock_last: waitID = %lu\n",ompt_get_thread_id(),waitID);
    }
#endif
}

// ompt_event_release_nest_lock_prev ompt_event_MAY_ALWAYS_TRACE
void CBTF_ompt_cb_release_nest_lock_prev (ompt_wait_id_t *waitID) {
#ifndef NDEBUG
    if (cbtf_ompt_debug_lock) {
	fprintf(stderr, "[%d] CBTF_ompt_cb_release_nest_lock_prev: waitID = %lu\n",ompt_get_thread_id(),waitID);
    }
#endif
}

//
void CBTF_ompt_cb_acquired_nest_lock_first (ompt_wait_id_t *waitID) {
#ifndef NDEBUG
    if (cbtf_ompt_debug_lock) {
	fprintf(stderr, "[%d] CBTF_ompt_cb_acquired_nest_lock_first: waitID = %lu\n",ompt_get_thread_id(),waitID);
    }
#endif
}

//
void CBTF_ompt_cb_acquired_nest_lock_next (ompt_wait_id_t *waitID) {
#ifndef NDEBUG
    if (cbtf_ompt_debug_lock) {
	fprintf(stderr, "[%d] CBTF_ompt_cb_acquired_nest_lock_next: waitID = %lu\n",ompt_get_thread_id(),waitID);
    }
#endif
}

// ompt_event_MAY_ALWAYS_TRACE but also seems used as BLAME??
// send notification to collector that a barrier has been entered.
// BLAME
/**
 * The OpenMP runtime system invokes this callback before an implicit task
 * begins execution of a barrier region. This callback executes in the context
 * of the implicit task that encountered the barrier construct.
 */
void CBTF_ompt_cb_barrier_begin (ompt_parallel_id_t parallelID,
		       ompt_task_id_t taskID)
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);

    // record barrier begin time.
    tls->barrier_btime = CBTF_GetTime();

#ifndef NDEBUG
    if (cbtf_ompt_debug_blame) {
        ompt_state_t task_state = ompt_get_state(NULL);
	fprintf(stderr,"[%d] CBTF_ompt_cb_barrier_begin parallelID:%lu taskID:%lu context:%p task_state:%x\n"
		,ompt_get_thread_id(), parallelID,taskID, current_region_context, task_state);
    }
#endif
}

// ompt_event_MAY_ALWAYS_TRACE but always used as BLAME??
// send notification to collector that a barrier has ended.
// BLAME
/**
 * The OpenMP runtime system invokes this callback after an implicit task
 * exits a barrier region. This callback executes in the context of the
 * implicit task that encountered the barrier construct.
 */
void CBTF_ompt_cb_barrier_end (ompt_parallel_id_t parallelID,
		     ompt_task_id_t taskID)
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);

    tls->barrier_ttime += CBTF_GetTime() - tls->barrier_btime;


    uint64_t t = CBTF_GetTime() - tls->barrier_btime;
    CBTF_omptp_event event;
    memset(&event, 0, sizeof(CBTF_omptp_event));
    event.time = t;
    tls->barrier_ttime += t;
    uint64_t stacktrace[MaxFramesPerStackTrace];
    int adjusted_size = tls->stacktrace_size;
    if (tls->stacktrace_size == MaxFramesPerStackTrace) {
	--adjusted_size;
    }
    if(adjusted_size > 0) {
	stacktrace[0] = (uint64_t)BARRIER;
	int i;
	for (i = 1; i < adjusted_size; ++i) {
	    stacktrace[i] = tls->stacktrace[i-1];
	}
    }
    omptp_record_event(&event, stacktrace, tls->stacktrace_size);

#ifndef NDEBUG
    if (cbtf_ompt_debug_blame) {
        ompt_state_t task_state = ompt_get_state(NULL);
	fprintf(stderr,"[%d] CBTF_ompt_cb_barrier_end: parallelID:%lu taskID:%lu context:%p barrier_time:%f task_state:%x\n"
	    ,ompt_get_thread_id(),parallelID,taskID, current_region_context, (float)t/1000000000, task_state);
    }
#endif
}

// ompt_event_MAY_ALWAYS_BLAME
// BLAME event
// send notification to collector that a wait_barrier has begun.
/**
 * The OpenMP runtime invokes this callback when an implicit task starts to
 * wait in a barrier region. One barrier region may generate multiple pairs
 * of barrier begin and end callbacks in a task,
 * e.g.
 * if waiting at the barrier occurs in multiple stages or
 * if another task is scheduled on this thread while it waits at the barrier.
 * The callback executes in the context of an implicit task waiting for a
 * barrier region to complete.
 */
void CBTF_ompt_cb_wait_barrier_begin (ompt_parallel_id_t parallelID,
		            ompt_task_id_t taskID)
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);
    tls->wbarrier_btime = CBTF_GetTime();
#ifndef NDEBUG
    if (cbtf_ompt_debug_blame) {
        ompt_state_t task_state = ompt_get_state(NULL);
	fprintf(stderr,"[%d] CBTF_ompt_cb_wait_barrier_begin parallelID:%lu taskID:%lu context:%p task_state:%x\n",
		ompt_get_thread_id(),parallelID,taskID,current_region_context,task_state);
    }
#endif
}

// ompt_event_MAY_ALWAYS_BLAME
// BLAME event
// send notification to collector that a wait_barrier has ended.
/**
 * The OpenMP runtime invokes this callback when an implicit task finishes
 * waiting in a barrier region. One barrier region may generate multiple pairs
 * of barrier begin and end callbacks in a task,
 * e.g.
 * if waiting at the barrier occurs in multiple stages or
 * if another task is scheduled on this thread while it waits at the barrier.
 * The callback executes in the context of an implicit task waiting for a
 * barrier region to complete.
 */
void CBTF_ompt_cb_wait_barrier_end (ompt_parallel_id_t parallelID,
			  ompt_task_id_t taskID)
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);

    tls->wbarrier_ttime += CBTF_GetTime() - tls->wbarrier_btime;

    uint64_t t = CBTF_GetTime() - tls->wbarrier_btime;
    CBTF_omptp_event event;
    memset(&event, 0, sizeof(CBTF_omptp_event));
    event.time = t;
    tls->wbarrier_ttime += t;
    uint64_t stacktrace[MaxFramesPerStackTrace];
    int adjusted_size = tls->stacktrace_size;
    if (tls->stacktrace_size == MaxFramesPerStackTrace) {
	--adjusted_size;
    }
    if(adjusted_size > 0) {
	stacktrace[0] = (uint64_t)WAIT_BARRIER;
	int i;
	for (i = 1; i < adjusted_size; ++i) {
	    stacktrace[i] = tls->stacktrace[i-1];
	}
    }
    omptp_record_event(&event, stacktrace, tls->stacktrace_size);
#ifndef NDEBUG
    if (cbtf_ompt_debug_blame) {
        ompt_state_t task_state = ompt_get_state(NULL);
	fprintf(stderr,"[%d] CBTF_ompt_cb_wait_barrier_end: parallelID:%lu taskID:%lu context:%p wbarrier_time:%f task_state:%x\n",
		ompt_get_thread_id(),parallelID,taskID,current_region_context,(float)t/1000000000, task_state);
    }
#endif
}

// ompt_event_MAY_ALWAYS_TRACE
/**
 * The OpenMP runtime system invokes this callback, after an implicit task
 * is fully initialized but before the task executes its work.
 * This callback executes in the context of the new implicit task.
 * We overide the callstack's first frame with the context provided
 * by the region this task is doing work in.
 * Without this context, call stack is at __kmp_invoke_task_func.
 */
void CBTF_ompt_cb_implicit_task_begin (ompt_parallel_id_t parallelID,
		             ompt_task_id_t taskID)
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);
    tls->itask_btime = CBTF_GetTime();
    tls->serial_ttime += tls->itask_btime - tls->serial_btime;
    tls->task_event.time = 0;
#ifndef NDEBUG
    if (cbtf_ompt_debug_trace) {
	fprintf(stderr,"[%d] CBTF_ompt_cb_implicit_task_begin parallelID:%lu taskID:%lu context:%p\n",
	    ompt_get_thread_id(),parallelID,taskID,current_region_context);
#if 0
	ompt_task_id_t tsk_id = ompt_get_task_id(0);
	ompt_frame_t *rt_frame = ompt_get_task_frame(0);
	fprintf(stderr, "[%d] tsk_id:%d CBTF_ompt_cb_implicit_task_begin: rt_frame->exit_runtime_frame=%p rt_frame->reenter_runtime_frame=%p\n",
	ompt_get_thread_id(), tsk_id, rt_frame->exit_runtime_frame,rt_frame->reenter_runtime_frame);
#endif
    }
#endif

    // Find the parallel region this task is running under.
    // Aquire it's parallel context and use it here.
    CBTF_omptp_region r = CBTF_omptp_region_with_id(parallelID);

    // copy region callstack to use as context for all individual
    // worker threads calling contexts.
    int i;
    tls->stacktrace_size = r.stacktrace_size;
    for (i = 0; i < r.stacktrace_size ; ++i) {
	tls->stacktrace[i] = r.stacktrace[i];
    }
}

// ompt_event_MAY_ALWAYS_TRACE
/**
 * The OpenMP runtime system invokes this callback after an implicit task
 * executes its closing synchronization barrier but before returning to idle
 * or the task is destroyed.
 * The callback executes in the context of the implicit task.
 */
void CBTF_ompt_cb_implicit_task_end (ompt_parallel_id_t parallelID,
			   ompt_task_id_t taskID)
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif

    Assert(tls != NULL);
    uint64_t etime = CBTF_GetTime(); - tls->itask_btime;
    uint64_t t = etime - tls->itask_btime;
    tls->serial_btime = etime;
    tls->task_event.time = t;
    tls->itask_ttime += t;

#ifndef NDEBUG
    if (cbtf_ompt_debug_trace) {
	fprintf(stderr,"[%d] CBTF_ompt_cb_implicit_task_end parallelID:%lu taskID:%lu context:%p time:%f\n",
		ompt_get_thread_id(),parallelID,taskID,current_region_context,(float)t/1000000000);
    }
#endif

    omptp_record_event(&tls->task_event, tls->stacktrace, tls->stacktrace_size);
}

// ompt_event_MAY_ALWAYS_TRACE
void CBTF_ompt_cb_master_begin (ompt_parallel_id_t parallelID,
		      ompt_task_id_t taskID)
{
#ifndef NDEBUG
    if (cbtf_ompt_debug_trace) {
	fprintf(stderr,"[%d] CBTF_ompt_cb_master_begin parallelID:%lu taskID:%lu\n",ompt_get_thread_id(),parallelID,taskID);
    }
#endif
}

// ompt_event_MAY_ALWAYS_TRACE
void CBTF_ompt_cb_master_end (ompt_parallel_id_t parallelID,
		    ompt_task_id_t taskID)
{
#ifndef NDEBUG
    if (cbtf_ompt_debug_trace) {
	fprintf(stderr,"[%d] CBTF_ompt_cb_master_end: parallelID:%lu taskID:%lu\n",ompt_get_thread_id(),parallelID,taskID);
    }
#endif
}

// ompt_event_MAY_ALWAYS_TRACE
void CBTF_ompt_cb_taskwait_begin (ompt_parallel_id_t parallelID,
		        ompt_task_id_t taskID)
{
#ifndef NDEBUG
    if (cbtf_ompt_debug_trace) {
	fprintf(stderr,"[%d] CBTF_ompt_cb_taskwait_begin parallelID:%lu taskID:%lu\n",ompt_get_thread_id(),parallelID,taskID);
    }
#endif
}

// ompt_event_MAY_ALWAYS_TRACE
void CBTF_ompt_cb_taskwait_end (ompt_parallel_id_t parallelID,
		      ompt_task_id_t taskID)
{
#ifndef NDEBUG
    if (cbtf_ompt_debug_trace) {
	fprintf(stderr,"[%d] CBTF_ompt_cb_taskwait_end: parallelID:%lu taskID:%lu\n",ompt_get_thread_id(),parallelID,taskID);
    }
#endif
}

// ompt_event_UNIMPLEMENTED
void CBTF_ompt_cb_wait_taskwait_begin (ompt_parallel_id_t parallelID,
			     ompt_task_id_t taskID)
{
#if 1
#ifndef NDEBUG
    if (cbtf_ompt_debug) {
	fprintf(stderr,"[%d] BLAME CBTF_ompt_cb_wait_taskwait_begin parallelID:%lu taskID:%lu\n",ompt_get_thread_id(),parallelID,taskID);
    }
#endif
#endif
}

// ompt_event_UNIMPLEMENTED
void CBTF_ompt_cb_wait_taskwait_end (ompt_parallel_id_t parallelID,
			   ompt_task_id_t taskID)
{
#if 1
#ifndef NDEBUG
    if (cbtf_ompt_debug) {
	fprintf(stderr,"[%d] BLAME CBTF_ompt_cb_wait_taskwait_end: parallelID:%lu taskID:%lu\n",ompt_get_thread_id(),parallelID,taskID);
    }
#endif
#endif
}

// ompt_event_UNIMPLEMENTED
void CBTF_ompt_cb_taskgroup_begin (ompt_parallel_id_t parallelID,
			 ompt_task_id_t taskID)
{
#if 1
#ifndef NDEBUG
    if (cbtf_ompt_debug) {
	fprintf(stderr,"[%d] CBTF_ompt_cb_taskgroup_begin parallelID:%lu taskID:%lu\n",ompt_get_thread_id(),parallelID,taskID);
    }
#endif
#endif
}

// ompt_event_UNIMPLEMENTED
void CBTF_ompt_cb_taskgroup_end (ompt_parallel_id_t parallelID,
		       ompt_task_id_t taskID)
{
#if 1
#ifndef NDEBUG
    if (cbtf_ompt_debug) {
	fprintf(stderr,"[%d] CBTF_ompt_cb_taskgroup_end: parallelID:%lu taskID:%lu\n",ompt_get_thread_id(),parallelID,taskID);
    }
#endif
#endif
}

// ompt_event_UNIMPLEMENTED
void CBTF_ompt_cb_wait_taskgroup_begin (ompt_parallel_id_t parallelID,
			      ompt_task_id_t taskID)
{
#if 1
#ifndef NDEBUG
    if (cbtf_ompt_debug) {
	fprintf(stderr,"[%d] CBTF_ompt_cb_wait_taskgroup_begin parallelID:%lu taskID:%lu\n",ompt_get_thread_id(),parallelID,taskID);
    }
#endif
#endif
}

// ompt_event_UNIMPLEMENTED
void CBTF_ompt_cb_wait_taskgroup_end (ompt_parallel_id_t parallelID,
			    ompt_task_id_t taskID)
{
#if 1
#ifndef NDEBUG
    if (cbtf_ompt_debug) {
	fprintf(stderr,"[%d] CBTF_ompt_cb_wait_taskgroup_end: parallelID:%lu taskID:%lu\n",ompt_get_thread_id(),parallelID,taskID);
    }
#endif
#endif
}

// ompt_event_MAY_ALWAYS_TRACE
void CBTF_ompt_cb_single_others_begin (ompt_parallel_id_t parallelID,
			     ompt_task_id_t taskID)
{
#ifndef NDEBUG
    if (cbtf_ompt_debug_trace) {
	fprintf(stderr,"[%d] CBTF_ompt_cb_single_others_begin parallelID:%lu taskID:%lu\n",ompt_get_thread_id(),parallelID,taskID);
    }
#endif
}

// ompt_event_MAY_ALWAYS_TRACE
void CBTF_ompt_cb_single_others_end (ompt_parallel_id_t parallelID,
			   ompt_task_id_t taskID)
{
#ifndef NDEBUG
    if (cbtf_ompt_debug_trace) {
	fprintf(stderr,"[%d] CBTF_ompt_cb_single_others_end: parallelID:%lu taskID:%lu\n",ompt_get_thread_id(),parallelID,taskID);
    }
#endif
}

// ompt_event_UNIMPLEMENTED
// notify taskID has begun
void CBTF_ompt_cb_initial_task_begin (ompt_task_id_t taskID)
{
#if 1
#ifndef NDEBUG
    if (cbtf_ompt_debug) {
	fprintf(stderr,"[%d] CBTF_ompt_cb_initial_task_begin taskID:%lu\n",ompt_get_thread_id(),taskID);
    }
#endif
#endif
}

// ompt_event_UNIMPLEMENTED
// notify taskID has ended
void CBTF_ompt_cb_initial_task_end (ompt_task_id_t taskID)
{
#if 1
#ifndef NDEBUG
    if (cbtf_ompt_debug) {
	fprintf(stderr,"[%d] CBTF_ompt_cb_initial_task_end: taskID:%lu\n",ompt_get_thread_id(),taskID);
    }
#endif
#endif
}

// ompt_event_MAY_ALWAYS_TRACE
void CBTF_ompt_cb_loop_begin (ompt_parallel_id_t parallelID, ompt_task_id_t taskID)
{
#ifndef NDEBUG
    if (cbtf_ompt_debug_trace) {
        ompt_state_t task_state = ompt_get_state(NULL);
	fprintf(stderr,"[%d] CBTF_ompt_cb_loop_begin parallelID:%lu taskID:%lu task_state:%x\n",
		ompt_get_thread_id(), parallelID, taskID, task_state);
    }
#endif
}

// ompt_event_MAY_ALWAYS_TRACE
void CBTF_ompt_cb_loop_end (ompt_parallel_id_t parallelID, ompt_task_id_t taskID)
{
#ifndef NDEBUG
    if (cbtf_ompt_debug_trace) {
        ompt_state_t task_state = ompt_get_state(NULL);
	fprintf(stderr,"[%d] CBTF_ompt_cb_loop_end: parallelID:%lu taskID:%lu task_state:%x\n",
		ompt_get_thread_id(), parallelID, taskID, task_state);
    }
#endif
}

// ompt_event_MAY_ALWAYS_TRACE
void CBTF_ompt_cb_single_in_block_begin (ompt_parallel_id_t parallelID, ompt_task_id_t taskID)
{
#ifndef NDEBUG
    if (cbtf_ompt_debug_trace) {
	fprintf(stderr,"[%d] CBTF_ompt_cb_single_in_block_begin parallelID:%lu taskID:%lu\n",ompt_get_thread_id(),parallelID,taskID);
    }
#endif
}

// ompt_event_MAY_ALWAYS_TRACE
void CBTF_ompt_cb_single_in_block_end (ompt_parallel_id_t parallelID, ompt_task_id_t taskID)
{
#ifndef NDEBUG
    if (cbtf_ompt_debug_trace) {
	fprintf(stderr,"[%d] CBTF_ompt_cb_single_in_block_end: parallelID:%lu taskID:%lu\n",ompt_get_thread_id(),parallelID,taskID);
    }
#endif
}

// ompt_event_UNIMPLEMENTED
void CBTF_ompt_cb_sections_begin (ompt_parallel_id_t parallelID, ompt_task_id_t taskID)
{
#if 1
#ifndef NDEBUG
    if (cbtf_ompt_debug) {
	fprintf(stderr,"[%d] CBTF_ompt_cb_sections_begin parallelID:%lu taskID:%lu\n",ompt_get_thread_id(),parallelID,taskID);
    }
#endif
#endif
}

// ompt_event_UNIMPLEMENTED
void CBTF_ompt_cb_sections_end (ompt_parallel_id_t parallelID, ompt_task_id_t taskID)
{
#if 1
#ifndef NDEBUG
    if (cbtf_ompt_debug) {
	fprintf(stderr,"[%d] CBTF_ompt_cb_sections_end: parallelID:%lu taskID:%lu\n",ompt_get_thread_id(),parallelID,taskID);
    }
#endif
#endif
}


// ompt_event_UNIMPLEMENTED
void CBTF_ompt_cb_workshare_begin (ompt_parallel_id_t parallelID, ompt_task_id_t taskID, void *workshare_function)

{
#if 1
#ifndef NDEBUG
    if (cbtf_ompt_debug) {
	fprintf(stderr,"[%d] CBTF_ompt_cb_workshare_begin parallelID:%lu taskID:%lu workshare_function:%p\n",ompt_get_thread_id(),parallelID,taskID,workshare_function);
    }
#endif
#endif
}

// ompt_event_UNIMPLEMENTED
void CBTF_ompt_cb_workshare_end (ompt_parallel_id_t parallelID, ompt_task_id_t taskID)
{
#if 1
#ifndef NDEBUG
    if (cbtf_ompt_debug) {
	fprintf(stderr,"[%d] CBTF_ompt_cb_workshare_end: parallelID:%lu taskID:%lu\n",ompt_get_thread_id(),parallelID,taskID);
    }
#endif
#endif
}

// ompt_event_MAY_ALWAYS_BLAME
// if parallel count is 0,
//    if idle and not busy, and return
//    if busy then stop time for tid, unset busy.
// set idle for tid and start time.
/**
 * The OpenMP runtime invokes this callback when a thread starts to idle
 * outside a parallel region. The callback executes in the environment
 * of the idling thread.
*/
void CBTF_ompt_cb_idle_begin(ompt_thread_id_t thread_id /* ID of thread*/)
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);
    ompt_task_id_t parent_tsk_id = ompt_get_task_id(1);
    ompt_task_id_t tsk_id = ompt_get_task_id(0);
    ompt_state_t task_state = ompt_get_state(NULL);

    tls->idle_btime = CBTF_GetTime();

#ifndef NDEBUG
    if (cbtf_ompt_debug_blame) {
	fprintf(stderr,"[%d] CBTF_ompt_cb_idle_begin context:%p parentTaskID:%d taskID:%d task_state:%x\n",
	    ompt_get_thread_id(), current_region_context, parent_tsk_id, tsk_id, task_state);
#if 0
	ompt_frame_t *idle_frame = ompt_get_idle_frame();
	fprintf(stderr, "[%d] tsk_id:%d CBTF_ompt_cb_idle_begin: idle_frame->exit_runtime_frame=%p idle_frame->reenter_runtime_frame=%p\n",
	ompt_get_thread_id(), tsk_id, idle_frame->exit_runtime_frame,idle_frame->reenter_runtime_frame);
#endif
    }
#endif
}

// ompt_event_MAY_ALWAYS_BLAME
// if parallel count is 0, start time for new parallel region and set
// busy this tid, else unset busy for this tid
/**
 * The OpenMP runtime invokes this callback when a thread finishes idling
 * outside a parallel region. The callback executes in the environment of
 * the thread that is about to resume useful work.
*/
void CBTF_ompt_cb_idle_end(ompt_thread_id_t thread_id /* ID of thread*/)
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);
    ompt_task_id_t parent_tsk_id = ompt_get_task_id(1);
    ompt_task_id_t tsk_id = ompt_get_task_id(0);
    ompt_state_t task_state = ompt_get_state(NULL);

    uint64_t t;
    if (tls->collector_active) {
	t = CBTF_GetTime() - tls->idle_btime;
    } else {
	t = tls->collector_etime - tls->idle_btime;
    }

    CBTF_omptp_event event;
    memset(&event, 0, sizeof(CBTF_omptp_event));
    event.time = t;
    tls->idle_ttime += t;

    // the tls stack trace is in the context of the master thread.
    // We add an additional frame below the context to attribute
    // the current idle to a specific region.
    uint64_t stacktrace[MaxFramesPerStackTrace];
    int adjusted_size = tls->stacktrace_size;
    if (tls->stacktrace_size == MaxFramesPerStackTrace) {
	--adjusted_size;
    }
    if(adjusted_size > 0) {
	stacktrace[0] = (uint64_t)IDLE;
	int i;
	for (i = 1; i < adjusted_size; ++i) {
	    stacktrace[i] = tls->stacktrace[i-1];
	}
    }

    // NOTE: FIXME: IDLE can occur outside of a implicit task.
    // Therefore the stacktrace here attributes the
    // idleness to activity in the task (region) context.
    omptp_record_event(&event, stacktrace, tls->stacktrace_size);

#ifndef NDEBUG
    if (cbtf_ompt_debug_blame) {
	fprintf(stderr,"[%d] CBTF_ompt_cb_idle_end context:%p parentTaskID:%d taskID:%d task_state:%x idle_time:%f\n",
		ompt_get_thread_id(), current_region_context, parent_tsk_id, tsk_id, task_state, (float)t/1000000000);
#if 0
	ompt_frame_t *idle_frame = ompt_get_idle_frame();
	fprintf(stderr, "[%d] tsk_id:%d CBTF_ompt_cb_idle_end: idle_frame->exit_runtime_frame=%p idle_frame->reenter_runtime_frame=%p\n",
	ompt_get_thread_id(), tsk_id, idle_frame->exit_runtime_frame,idle_frame->reenter_runtime_frame);
#endif
    }
#endif
}

// initialize our ompt services. Currently we initialize any callback available
// in the current libiomp ompt interface. Most callbacks are currently no-ops
// in the ompt service except for the blame events idle and wait_barrier.
// TODO. Allow collector runtimes to dictate which ompt callbacks to activate
// outside of the mandatory events.  We may not use all mandatory events depending
// on the collector.
//
// NOTE: Newer versions of libomp_oss have optver arg as int, latest has it unsigned int
// Older versions of ompt use ompt_initialize to initiallize the tool callbacks
// and newer versions use ompt_tool.
int ompt_initialize
    (ompt_function_lookup_t lookup_func,
                    const char *rtver, unsigned int omptver)
{
    /* Create and access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = malloc(sizeof(TLS));
    Assert(tls != NULL);
    CBTF_SetTLS(TLSKey, tls);
    ompt_cb_init_tls_done = 1;
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);

#ifndef NDEBUG
    if ( (getenv("CBTF_DEBUG_OMPT") != NULL)) {
	cbtf_ompt_debug = 1;
    }

    if ( (getenv("CBTF_DEBUG_OMPT_BLAME") != NULL)) {
	cbtf_ompt_debug_blame = 1;
    }

    if ( (getenv("CBTF_DEBUG_OMPT_TRACE") != NULL)) {
	cbtf_ompt_debug_trace = 1;
    }

    if ( (getenv("CBTF_DEBUG_OMPT_WAIT") != NULL)) {
	cbtf_ompt_debug_wait = 1;
    }

    if ( (getenv("CBTF_DEBUG_OMPT_LOCK") != NULL)) {
	cbtf_ompt_debug_lock = 1;
    }

    if ( (getenv("CBTF_DEBUG_OMPT_DETAILS") != NULL)) {
	cbtf_ompt_debug_details = 1;
    }

    if (cbtf_ompt_debug_details == 1) {
	fprintf(stderr, "CBTF OMPT Init: %s ver %i\n",rtver,omptver);
	fprintf(stderr, "CBTF OMPT Init: Registering callbacks\n");
    }
#endif

    ompt_enumerate_state = (ompt_enumerate_state_t) lookup_func("ompt_enumerate_state");
    ompt_set_callback = (ompt_set_callback_t) lookup_func("ompt_set_callback");
    ompt_get_callback = (ompt_get_callback_t) lookup_func("ompt_get_callback");
    ompt_get_idle_frame = (ompt_get_idle_frame_t) lookup_func("ompt_get_idle_frame");
    ompt_get_task_frame = (ompt_get_task_frame_t) lookup_func("ompt_get_task_frame");
    ompt_get_state = (ompt_get_state_t) lookup_func("ompt_get_state");
    ompt_get_parallel_id = (ompt_get_parallel_id_t) lookup_func("ompt_get_parallel_id");
    ompt_get_task_id = (ompt_get_task_id_t) lookup_func("ompt_get_task_id");
    ompt_get_thread_id = (ompt_get_thread_id_t) lookup_func("ompt_get_thread_id");


    CBTF_register_ompt_callback(ompt_event_parallel_begin,CBTF_ompt_cb_parallel_begin);
    CBTF_register_ompt_callback(ompt_event_parallel_end,CBTF_ompt_cb_parallel_end);
    CBTF_register_ompt_callback(ompt_event_task_begin,CBTF_ompt_cb_task_begin);
    CBTF_register_ompt_callback(ompt_event_task_end,CBTF_ompt_cb_task_end);
    CBTF_register_ompt_callback(ompt_event_thread_begin,CBTF_ompt_cb_thread_begin);
    CBTF_register_ompt_callback(ompt_event_thread_end,CBTF_ompt_cb_thread_end);
    CBTF_register_ompt_callback(ompt_event_control,CBTF_ompt_cb_control);
    CBTF_register_ompt_callback(ompt_event_runtime_shutdown,CBTF_ompt_cb_runtime_shutdown);
    CBTF_register_ompt_callback(ompt_event_wait_atomic,CBTF_ompt_cb_wait_atomic);
    CBTF_register_ompt_callback(ompt_event_wait_ordered,CBTF_ompt_cb_wait_ordered);
    CBTF_register_ompt_callback(ompt_event_wait_critical,CBTF_ompt_cb_wait_critical);
    CBTF_register_ompt_callback(ompt_event_wait_lock,CBTF_ompt_cb_wait_lock);
    CBTF_register_ompt_callback(ompt_event_acquired_atomic,CBTF_ompt_cb_acquired_atomic);
    CBTF_register_ompt_callback(ompt_event_acquired_ordered,CBTF_ompt_cb_acquired_ordered);
    CBTF_register_ompt_callback(ompt_event_acquired_critical,CBTF_ompt_cb_acquired_critical);
    CBTF_register_ompt_callback(ompt_event_acquired_lock,CBTF_ompt_cb_acquired_lock);
    CBTF_register_ompt_callback(ompt_event_release_atomic,CBTF_ompt_cb_release_atomic);
    CBTF_register_ompt_callback(ompt_event_release_ordered,CBTF_ompt_cb_release_ordered);
    CBTF_register_ompt_callback(ompt_event_release_critical,CBTF_ompt_cb_release_critical);
    CBTF_register_ompt_callback(ompt_event_release_lock,CBTF_ompt_cb_release_lock);

    CBTF_register_ompt_callback(ompt_event_init_lock,CBTF_ompt_cb_init_lock);
    CBTF_register_ompt_callback(ompt_event_destroy_lock,CBTF_ompt_cb_destroy_lock);
    CBTF_register_ompt_callback(ompt_event_wait_nest_lock,CBTF_ompt_cb_wait_nest_lock);
    CBTF_register_ompt_callback(ompt_event_acquired_nest_lock_first,CBTF_ompt_cb_acquired_nest_lock_first);
    CBTF_register_ompt_callback(ompt_event_acquired_nest_lock_next,CBTF_ompt_cb_acquired_nest_lock_next);
    CBTF_register_ompt_callback(ompt_event_release_nest_lock_prev,CBTF_ompt_cb_release_nest_lock_prev);
    CBTF_register_ompt_callback(ompt_event_release_nest_lock_last,CBTF_ompt_cb_release_nest_lock_last);

    CBTF_register_ompt_callback(ompt_event_barrier_begin,CBTF_ompt_cb_barrier_begin);
    CBTF_register_ompt_callback(ompt_event_barrier_end,CBTF_ompt_cb_barrier_end);
    CBTF_register_ompt_callback(ompt_event_wait_barrier_begin,CBTF_ompt_cb_wait_barrier_begin);
    CBTF_register_ompt_callback(ompt_event_wait_barrier_end,CBTF_ompt_cb_wait_barrier_end);
    CBTF_register_ompt_callback(ompt_event_implicit_task_begin,CBTF_ompt_cb_implicit_task_begin);
    CBTF_register_ompt_callback(ompt_event_implicit_task_end,CBTF_ompt_cb_implicit_task_end);
    CBTF_register_ompt_callback(ompt_event_master_begin,CBTF_ompt_cb_master_begin);
    CBTF_register_ompt_callback(ompt_event_master_end,CBTF_ompt_cb_master_end);
    CBTF_register_ompt_callback(ompt_event_taskwait_begin,CBTF_ompt_cb_taskwait_begin);
    CBTF_register_ompt_callback(ompt_event_taskwait_end,CBTF_ompt_cb_taskwait_end);
    CBTF_register_ompt_callback(ompt_event_wait_taskwait_begin,CBTF_ompt_cb_wait_taskwait_begin);
    CBTF_register_ompt_callback(ompt_event_wait_taskwait_end,CBTF_ompt_cb_wait_taskwait_end);
    CBTF_register_ompt_callback(ompt_event_taskgroup_begin,CBTF_ompt_cb_taskgroup_begin);
    CBTF_register_ompt_callback(ompt_event_taskgroup_end,CBTF_ompt_cb_taskgroup_end);
    CBTF_register_ompt_callback(ompt_event_wait_taskgroup_begin,CBTF_ompt_cb_wait_taskgroup_begin);
    CBTF_register_ompt_callback(ompt_event_wait_taskgroup_end,CBTF_ompt_cb_wait_taskgroup_end);
    CBTF_register_ompt_callback(ompt_event_single_others_begin,CBTF_ompt_cb_single_others_begin);
    CBTF_register_ompt_callback(ompt_event_single_others_end,CBTF_ompt_cb_single_others_end);
    CBTF_register_ompt_callback(ompt_event_initial_task_begin,CBTF_ompt_cb_initial_task_begin);
    CBTF_register_ompt_callback(ompt_event_initial_task_end,CBTF_ompt_cb_initial_task_end);
    CBTF_register_ompt_callback(ompt_event_loop_begin,CBTF_ompt_cb_loop_begin);
    CBTF_register_ompt_callback(ompt_event_loop_end,CBTF_ompt_cb_loop_end);
    CBTF_register_ompt_callback(ompt_event_single_in_block_begin,CBTF_ompt_cb_single_in_block_begin);
    CBTF_register_ompt_callback(ompt_event_single_in_block_end,CBTF_ompt_cb_single_in_block_end);
    CBTF_register_ompt_callback(ompt_event_sections_begin,CBTF_ompt_cb_sections_begin);
    CBTF_register_ompt_callback(ompt_event_sections_end,CBTF_ompt_cb_sections_end);
    CBTF_register_ompt_callback(ompt_event_workshare_begin,CBTF_ompt_cb_workshare_begin);
    CBTF_register_ompt_callback(ompt_event_workshare_end,CBTF_ompt_cb_workshare_end);
    CBTF_register_ompt_callback(ompt_event_idle_begin,CBTF_ompt_cb_idle_begin);
    CBTF_register_ompt_callback(ompt_event_idle_end,CBTF_ompt_cb_idle_end);
#ifndef NDEBUG
    if (cbtf_ompt_debug_details) {
	fprintf(stderr, "CBTF OMPT Init: FINISHED Registering callbacks\n");
    }
#endif
    return 1;
}

#if defined(INIT_AS_OMPT_TOOL)
ompt_initialize_t ompt_tool ()
{
    return (void*) ompt_initialize;
}
#endif
