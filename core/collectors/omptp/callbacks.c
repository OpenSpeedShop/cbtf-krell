/*******************************************************************************
** Copyright (c) 2014-2016  The Krell Institute. All Rights Reserved.
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
 * Definition of the ompt callbacks.
 *
 */

#include <stdbool.h> /* C99 std */
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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

static int cbtf_ompt_debug = 0;
static int cbtf_ompt_debug_details = 0;
static int cbtf_ompt_debug_blame = 0;
static int cbtf_ompt_debug_trace = 0;
static int cbtf_ompt_debug_wait = 0;

// these external calls are expected in the cbtf collectors either
// as implementations or as empty functions.
//
// pause and resume collection.
extern void cbtf_collector_pause();
extern void cbtf_collector_resume();

// set an openmp thread num.  Always a match to monitor threadnum
// so this is likely not going to be used.
extern void cbtf_collector_set_omp_threadnum(int32_t);

// pass a name and a context to collector.  the context
// is only provided to find a symbol later and collectors should
// not attempt to add any metric value to this context. 
extern void collector_record_addr(char*,uint64_t);

// notification to collector of begin and end of these BLAME events.
// These callbacks are in the sampling and hwc collectors and
// essentially record these ompt states as the sample point rather
// than the openmp library address where sample was taken.
extern void OMPT_THREAD_IDLE(bool);
extern void OMPT_THREAD_BARRIER(bool);
extern void OMPT_THREAD_WAIT_BARRIER(bool);


#define MaxFramesPerStackTrace 32

/** Type defining the items stored in thread-local storage. */
typedef struct {
    uint64_t region_count;
    uint64_t region_btime;
    uint64_t region_ttime;
    uint64_t idle_count;
    uint64_t idle_btime;
    uint64_t idle_ttime;
    uint64_t barrier_count;
    uint64_t barrier_btime;
    uint64_t barrier_ttime;
    uint64_t wbarrier_count;
    uint64_t wbarrier_btime;
    uint64_t wbarrier_ttime;
    uint64_t lock_count;
    uint64_t lock_btime;
    uint64_t lock_time;
    uint64_t itask_count;
    uint64_t itask_btime;
    uint64_t itask_ttime;
    uint64_t stacktrace[MaxFramesPerStackTrace];
    unsigned stacktrace_size;
    CBTF_omptp_event region_event;
} TLS;

static uint64_t current_region_context = NULL;
static uint64_t current_region_stacktrace[MaxFramesPerStackTrace];
static unsigned current_region_stacktrace_size;

extern void omptp_record_event(const CBTF_omptp_event*, uint64_t*, unsigned);
extern void omptp_start_event(const CBTF_omptp_event*, uint64_t, uint64_t*, unsigned*);
extern void omptp_set_in_parallel_region(bool flag);

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
void CBTF_ompt_cb_parallel_region_begin (
  ompt_task_id_t parent_taskID,     /* parent task id */
  ompt_frame_t *parent_taskframe,   /* unused parent task frame data */
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
    cbtf_collector_pause();
 
    ++tls->region_count;
    tls->region_btime = CBTF_GetTime();

 
    /* may need a stack to push nested regions... */
    /* Regions are only tracked on the master thread.  We will create a
     * callstack context here that is used by all the worker threads.
     * The implicit_task callbasks will record the original context and
     * for now we will not record the orignal context and region time
     * when we get to the region_end callback.
     *
     * TODO: Handle nested regions...
     */

    current_region_context = (uint64_t)parallel_function;
    omptp_start_event(&tls->region_event,(uint64_t)parallel_function, current_region_stacktrace, &current_region_stacktrace_size);

    tls->region_event.time = 0;

#if 0
    int i;
    for (i=0; i < current_region_stacktrace_size; ++i) {
	fprintf(stderr, "[%d] CBTF_ompt_cb_parallel_region_begin: current_region_stacktrace[%d]=%p\n",
	    omp_get_thread_num(), i, current_region_stacktrace[i]);
    }
#endif

    ompt_task_id_t tsk_id = ompt_get_task_id(0);
    ompt_frame_t *rt_frame = ompt_get_task_frame(0);

#ifndef NDEBUG
    if (cbtf_ompt_debug) {
	fprintf(stderr,
	"[%d] CBTF_ompt_cb_parallel_region_begin: parent_taskID:%lu parallelID:%lu req_team_size:%u context:%p\n",
	omp_get_thread_num(), parent_taskID, parallelID, requested_team_size, parallel_function);
	fprintf(stderr, "[%d] tsk_id:%d CBTF_ompt_cb_parallel_region_begin: rt_frame->exit_runtime_frame=%p rt_frame->reenter_runtime_frame=%p\n",
	omp_get_thread_num(), tsk_id, rt_frame->exit_runtime_frame,rt_frame->reenter_runtime_frame);
    }
#endif

    // resume collection
    cbtf_collector_resume();
}

// ompt_event_MAY_ALWAYS
// record regionID
// if parallel flag, record stop time, decrement parallel flag.
void CBTF_ompt_cb_parallel_region_end (
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
    cbtf_collector_pause();

    tls->region_ttime += CBTF_GetTime() - tls->region_btime;

#ifndef NDEBUG
    if (cbtf_ompt_debug) {
	fprintf(stderr,
	"[%d] CBTF_ompt_cb_parallel_region_end: total time:%f  context:%p  parent_taskID:%lu  parallelID:%lu\n",
	omp_get_thread_num(), (float)tls->region_ttime/1000000000, current_region_context, parent_taskID, parallelID);
    }
#endif

#if 0
    int i;
    for (i=0; i < current_region_stacktrace_size; ++i) {
	fprintf(stderr, "[%d] CBTF_ompt_cb_parallel_region_end: current_region_stacktrace[%d]=%p\n",
	    omp_get_thread_num(), i, current_region_stacktrace[i]);
    }

    ompt_task_id_t tsk_id = ompt_get_task_id(0);
    ompt_frame_t *rt_frame = ompt_get_task_frame(0);

    fprintf(stderr, "[%d] tsk_id:%d CBTF_ompt_cb_parallel_region_end: rt_frame->exit_runtime_frame=%p rt_frame->reenter_runtime_frame=%p\n",
	omp_get_thread_num(), tsk_id, rt_frame->exit_runtime_frame,rt_frame->reenter_runtime_frame);
#endif

    tls->region_event.time = CBTF_GetTime() - tls->region_btime;
    //omptp_record_event(&tls->region_event, current_region_stacktrace, current_region_stacktrace_size);
    // resume collection
    cbtf_collector_resume();
}

// ompt_event_MAY_ALWAYS
// record new_taskID as taskid.
// record start time and get callstack of this context
// increment is parallel flag.
void CBTF_ompt_cb_task_begin (
  ompt_task_id_t parent_taskID,     /* parent task id */
  ompt_frame_t *parent_taskframe,   /* unused parent task frame data */
  ompt_task_id_t new_taskID,        /* new task id */
  void *task_function)                    /* context of task - function pointer */
{
    cbtf_collector_pause();
#ifndef NDEBUG
    if (cbtf_ompt_debug_blame) {
	fprintf(stderr,"[%d] CBTF_ompt_cb_task_begin: parent_taskID:%lu new_task_id:%lu task_function:%p\n",
	omp_get_thread_num() ,parent_taskID, new_taskID, task_function);
    }
#endif
    //collector_record_addr("CBTF_OMP_TASK_BEGIN",(uint64_t)task_function);
    cbtf_collector_resume();
}

// ompt_event_MAY_ALWAYS
//  new_taskID as taskid.
// decrement is parallel flag.
void CBTF_ompt_cb_task_end (
  ompt_task_id_t parent_taskID     /* parent task id */
  )
{
    cbtf_collector_pause();
#ifndef NDEBUG
    if (cbtf_ompt_debug) {
	fprintf(stderr,"[%d] CBTF_ompt_cb_task_end: parent_taskID = %lu\n",omp_get_thread_num() ,parent_taskID);
    }
#endif
    cbtf_collector_resume();
}

// ompt_event_MAY_ALWAYS
// monitor notifies us of this...
// pass omp thread num to collector.
void CBTF_ompt_cb_thread_begin()
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);

    tls->region_count = 0;
    tls->region_ttime = 0;
    tls->idle_count = 0;
    tls->idle_ttime = 0;
    tls->barrier_count = 0;
    tls->barrier_ttime = 0;
    tls->wbarrier_count = 0;
    tls->wbarrier_ttime = 0;
    tls->itask_count = 0;
    tls->itask_ttime = 0;
    tls->lock_count = 0;
    tls->lock_btime = 0;
    tls->lock_time = 0;
#ifndef NDEBUG
    if (cbtf_ompt_debug) {
	fprintf(stderr, "[%d,%d] CBTF_ompt_cb_thread_begin:\n",omp_get_thread_num(),monitor_get_thread_num());
    }
#endif
    //cbtf_collector_set_openmp_threadid(omp_get_thread_num());
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

#ifndef NDEBUG
    if (cbtf_ompt_debug) {
	fprintf(stderr, "[%d] CBTF_ompt_cb_thread_end: region_time:%f, barrier time:%f, wait_barrier_time:%f idle_time:%f region_count:%d, idle_count:%d, barrier_count:%d, wait_barrier_count:%d\n" ,omp_get_thread_num(), (float)tls->region_ttime/1000000000, (float)tls->barrier_ttime/1000000000, (float)tls->wbarrier_ttime/1000000000, (float)tls->idle_ttime/1000000000, tls->region_count,tls->idle_count,tls->barrier_count,tls->wbarrier_count);
	fprintf(stderr, "[%d] CBTF_ompt_cb_thread_end: itask_time:%f itask_count:%d\n" ,omp_get_thread_num(), (float)tls->itask_ttime/1000000000, tls->itask_count);
    }
#endif
}

// ompt_event_MAY_ALWAYS
void CBTF_ompt_cb_control(uint64_t command, uint64_t modifier)
{
#ifndef NDEBUG
    if (cbtf_ompt_debug_details) {
	fprintf(stderr,"[%d] CBTF_ompt_cb_control: %llx, %llx\n",omp_get_thread_num(), command, modifier);
    }
#endif
}

// ompt_event_MAY_ALWAYS
// shut down of ompt ...
void CBTF_ompt_cb_runtime_shutdown()
{
#ifndef NDEBUG
    if (cbtf_ompt_debug_details) {
	fprintf(stderr, "[%d] CBTF_ompt_cb_runtime_shutdown:\n",omp_get_thread_num());
    }
#endif
}

// ompt_event_MAY_ALWAYS_TRACE
// set wait, start time wait on wait name
void CBTF_ompt_cb_wait_atomic (ompt_wait_id_t waitID) {
// cbtf_collector_pause();
#ifndef NDEBUG
    if (cbtf_ompt_debug_trace) {
	fprintf(stderr, "[%d] CBTF_ompt_cb_wait_atomic: waitID = %lu\n",omp_get_thread_num(),waitID);
    }
#endif
// cbtf_collector_resume();
}

// ompt_event_MAY_ALWAYS_TRACE
// set wait, start time wait on wait name
void CBTF_ompt_cb_wait_ordered (ompt_wait_id_t waitID) {
// cbtf_collector_pause();
#ifndef NDEBUG
    if (cbtf_ompt_debug_trace) {
	fprintf(stderr, "[%d] CBTF_ompt_cb_wait_ordered: waitID = %lu\n",omp_get_thread_num(),waitID);
    }
#endif
// cbtf_collector_resume();
}

// ompt_event_UNIMPLEMENTED
// set wait, start time wait on wait name
void CBTF_ompt_cb_wait_critical (ompt_wait_id_t waitID) {
// cbtf_collector_pause();
#ifndef NDEBUG
    if (cbtf_ompt_debug_wait) {
	fprintf(stderr, "[%d] CBTF_ompt_cb_wait_critical: waitID = %lu\n",omp_get_thread_num(),waitID);
    }
#endif
// cbtf_collector_resume();
}

// ompt_event_UNIMPLEMENTED
// set wait, start time wait on wait name
void CBTF_ompt_cb_wait_lock (ompt_wait_id_t waitID) {
// cbtf_collector_pause();
#ifndef NDEBUG
    if (cbtf_ompt_debug_wait) {
	fprintf(stderr, "[%d] CBTF_ompt_cb_wait_lock: waitID = %lu\n",omp_get_thread_num(),waitID);
    }
#endif
// cbtf_collector_resume();
}

// ompt_event_MAY_ALWAYS_TRACE
// if acquired, end time on wait name.
// clear wait, set acquired, start time on region name
void CBTF_ompt_cb_acquired_atomic (ompt_wait_id_t waitID) {
// cbtf_collector_pause();
#ifndef NDEBUG
    if (cbtf_ompt_debug_trace) {
	fprintf(stderr, "[%d] CBTF_ompt_cb_acquired_atomic: waitID = %lu\n",omp_get_thread_num(),waitID);
    }
#endif
// cbtf_collector_resume();
}

// ompt_event_MAY_ALWAYS_TRACE
// if acquired, end time on wait name.
// clear wait, set acquired, start time on region name
void CBTF_ompt_cb_acquired_ordered (ompt_wait_id_t waitID) {
// cbtf_collector_pause();
#ifndef NDEBUG
    if (cbtf_ompt_debug_trace) {
	fprintf(stderr, "[%d] CBTF_ompt_cb_acquired_ordered: waitID = %lu\n",omp_get_thread_num(),waitID);
    }
#endif
// cbtf_collector_resume();
}

// ompt_event_UNIMPLEMENTED
void CBTF_ompt_cb_acquired_critical (ompt_wait_id_t waitID) {
#if 0
// cbtf_collector_pause();
#ifndef NDEBUG
    if (cbtf_ompt_debug_wait) {
	fprintf(stderr, "[%d] CBTF_ompt_cb_acquired_critical: waitID = %lu\n",omp_get_thread_num(),waitID);
    }
#endif
// cbtf_collector_resume();
#endif
}

// ompt_event_UNIMPLEMENTED
// if acquired, end time on wait name.
// clear wait, set acquired, start time on region name
void CBTF_ompt_cb_acquired_lock (ompt_wait_id_t waitID) {
// cbtf_collector_pause();

#if 0
    tls->lock_count++;
    struct timespec now;
    Assert(clock_gettime(CLOCK_REALTIME, &now) == 0);
    tls->lock_btime = ((uint64_t)(now.tv_sec) * (uint64_t)(1000000000)) +
        (uint64_t)(now.tv_nsec);

#ifndef NDEBUG
    if (cbtf_ompt_debug_trace) {
	fprintf(stderr, "[%d] CBTF_ompt_cb_acquired_lock: waitID = %lu\n",omp_get_thread_num(),waitID);
    }
#endif

// cbtf_collector_resume();
#endif
}

// ompt_event_MAY_ALWAYS_BLAME
void CBTF_ompt_cb_release_atomic (ompt_wait_id_t waitID) {
// cbtf_collector_pause();
#ifndef NDEBUG
    if (cbtf_ompt_debug_blame) {
	fprintf(stderr, "[%d] CBTF_ompt_cb_release_atomic: waitID = %lu\n",omp_get_thread_num(),waitID);
    }
#endif
// cbtf_collector_resume();
}

// ompt_event_MAY_ALWAYS_BLAME
void CBTF_ompt_cb_release_ordered (ompt_wait_id_t waitID) {
// cbtf_collector_pause();
#ifndef NDEBUG
    if (cbtf_ompt_debug_blame) {
	fprintf(stderr, "[%d] CBTF_ompt_cb_release_ordered: waitID:%lu\n",omp_get_thread_num(),waitID);
    }
#endif
// cbtf_collector_resume();
}

// ompt_event_MAY_ALWAYS_BLAME
void CBTF_ompt_cb_release_critical (ompt_wait_id_t waitID) {
// cbtf_collector_pause();
#ifndef NDEBUG
    if (cbtf_ompt_debug_blame) {
	fprintf(stderr, "[%d] CBTF_ompt_cb_release_critical: waitID = %lu\n",omp_get_thread_num(),waitID);
    }
#endif
// cbtf_collector_resume();
}

// ompt_event_MAY_ALWAYS_BLAME
// if acquired, end time region name, clear acquired
void CBTF_ompt_cb_release_lock (ompt_wait_id_t waitID) {
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);

// cbtf_collector_pause();

#ifndef NDEBUG
    if (cbtf_ompt_debug_blame) {
	fprintf(stderr, "[%d] CBTF_ompt_cb_release_lock: waitID = %lu\n",omp_get_thread_num(),waitID);
    }
#endif

   tls->lock_count++;

#if 0
    struct timespec now;
    Assert(clock_gettime(CLOCK_REALTIME, &now) == 0);
    uint64_t tls->lock_etime = ((uint64_t)(now.tv_sec) * (uint64_t)(1000000000)) +
        (uint64_t)(now.tv_nsec);
    tls->lock_time += (tls->lock_etime - tls->lock_btime);
#endif

// cbtf_collector_resume();
}

// TODO:
// ompt_event_release_nest_lock_last ompt_event_MAY_ALWAYS_BLAME
// ompt_event_release_nest_lock_prev ompt_event_MAY_ALWAYS_TRACE
//

// ompt_event_MAY_ALWAYS_TRACE but also seems used as BLAME??
// send notification to collector that a barrier has been entered.
// BLAME
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

    cbtf_collector_pause();
    OMPT_THREAD_BARRIER(true);
    ++tls->barrier_count;

    // record barrier begin time.
    tls->barrier_btime = CBTF_GetTime();

#ifndef NDEBUG
    if (cbtf_ompt_debug_blame) {
	fprintf(stderr,"[%d] CBTF_ompt_cb_barrier_begin parallelID:%lu taskID:%lu context:%p\n"
		,omp_get_thread_num(), parallelID,taskID, current_region_context);
    }
#endif

    cbtf_collector_resume();
}

// ompt_event_MAY_ALWAYS_TRACE but always used as BLAME??
// send notification to collector that a barrier has ended.
// BLAME
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

    cbtf_collector_pause();
    OMPT_THREAD_BARRIER(false);
    tls->barrier_ttime += CBTF_GetTime() - tls->barrier_btime;


    uint64_t t = CBTF_GetTime() - tls->barrier_btime;
    CBTF_omptp_event event;
    memset(&event, 0, sizeof(CBTF_omptp_event));
    event.time = t;
    tls->barrier_ttime += t;
    if(current_region_stacktrace_size > 0) {
	memcpy(tls->stacktrace,current_region_stacktrace,sizeof(current_region_stacktrace));
	tls->stacktrace[0] = (uint64_t)OMPT_THREAD_BARRIER;
    }
    omptp_record_event(&event, tls->stacktrace, current_region_stacktrace_size);

#ifndef NDEBUG
    if (cbtf_ompt_debug_blame) {
	fprintf(stderr,"[%d] CBTF_ompt_cb_barrier_end: parallelID:%lu taskID:%lu context:%p\n"
	    ,omp_get_thread_num(),parallelID,taskID,tls->barrier_ttime, current_region_context);
	fprintf(stderr, "[%d] CBTF_ompt_cb_barrier_end: barrier_time:%f\n" ,omp_get_thread_num(), (float)t/1000000000);
    }
#endif

    cbtf_collector_resume();
}

// ompt_event_MAY_ALWAYS_BLAME
// BLAME event
// send notification to collector that a wait_barrier has begun.
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
    cbtf_collector_pause();
    OMPT_THREAD_WAIT_BARRIER(true);
    ++tls->wbarrier_count;
    tls->wbarrier_btime = CBTF_GetTime();
#ifndef NDEBUG
    if (cbtf_ompt_debug_blame) {
	fprintf(stderr,"[%d] CBTF_ompt_cb_wait_barrier_begin parallelID:%lu taskID:%lu context:%p\n",omp_get_thread_num(),parallelID,taskID,current_region_context);
    }
#endif
    cbtf_collector_resume();
}

// ompt_event_MAY_ALWAYS_BLAME
// BLAME event
// send notification to collector that a wait_barrier has ended.
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
    cbtf_collector_pause();
    OMPT_THREAD_WAIT_BARRIER(false);

    tls->wbarrier_ttime += CBTF_GetTime() - tls->wbarrier_btime;

    uint64_t t = CBTF_GetTime() - tls->wbarrier_btime;
    CBTF_omptp_event event;
    memset(&event, 0, sizeof(CBTF_omptp_event));
    event.time = t;
    tls->wbarrier_ttime += t;
    if(current_region_stacktrace_size > 0) {
	memcpy(tls->stacktrace,current_region_stacktrace,sizeof(current_region_stacktrace));
	tls->stacktrace[0] = (uint64_t)OMPT_THREAD_WAIT_BARRIER;
    }
    omptp_record_event(&event, tls->stacktrace, current_region_stacktrace_size);
#ifndef NDEBUG
    if (cbtf_ompt_debug_blame) {
	fprintf(stderr,"[%d] CBTF_ompt_cb_wait_barrier_end: parallelID:%lu taskID:%lu context:%p\n",omp_get_thread_num(),parallelID,taskID,current_region_context);
	fprintf(stderr,"[%d] CBTF_ompt_cb_wait_barrier_end: wbarrier_time:%f\n" ,omp_get_thread_num(), (float)t/1000000000);
    }
#endif


    cbtf_collector_resume();
}

// ompt_event_MAY_ALWAYS_TRACE
// set regionId to parallelID, set taskID
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
    //cbtf_collector_pause();
    ++tls->itask_count;
    tls->itask_btime = CBTF_GetTime();
#ifndef NDEBUG
    if (cbtf_ompt_debug_trace) {
	fprintf(stderr,"[%d] CBTF_ompt_cb_implicit_task_begin parallelID:%lu taskID:%lu\n",omp_get_thread_num(),parallelID,taskID);
    }
#endif
    //cbtf_collector_resume();
}

// ompt_event_MAY_ALWAYS_TRACE
// set regionId to parallelID, set taskID
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
    //cbtf_collector_pause();
    uint64_t t = CBTF_GetTime() - tls->itask_btime;
    CBTF_omptp_event event;
    memset(&event, 0, sizeof(CBTF_omptp_event));
    event.time = t;

    tls->itask_ttime += t;
#ifndef NDEBUG
    if (cbtf_ompt_debug_trace) {
	fprintf(stderr,"[%d] CBTF_ompt_cb_implicit_task_end: parallelID:%lu taskID:%lu\n",omp_get_thread_num(),parallelID,taskID);
	fprintf(stderr, "[%d] CBTF_ompt_cb_implicit_task_end: itask_time:%f\n" ,omp_get_thread_num(), (float)t/1000000000);
    }
#endif
    omptp_record_event(&event, current_region_stacktrace, current_region_stacktrace_size);
    //cbtf_collector_resume();
}

// ompt_event_MAY_ALWAYS_TRACE
// set regionId to parallelID, set taskID
void CBTF_ompt_cb_master_begin (ompt_parallel_id_t parallelID,
		      ompt_task_id_t taskID)
{
    //cbtf_collector_pause();
#ifndef NDEBUG
    if (cbtf_ompt_debug_trace) {
	fprintf(stderr,"[%d] CBTF_ompt_cb_master_begin parallelID:%lu taskID:%lu\n",omp_get_thread_num(),parallelID,taskID);
    }
#endif
    //cbtf_collector_resume();
}

// ompt_event_MAY_ALWAYS_TRACE
// set regionId to parallelID, set taskID
void CBTF_ompt_cb_master_end (ompt_parallel_id_t parallelID,
		    ompt_task_id_t taskID)
{
    //cbtf_collector_pause();
#ifndef NDEBUG
    if (cbtf_ompt_debug_trace) {
	fprintf(stderr,"[%d] CBTF_ompt_cb_master_end: parallelID:%lu taskID:%lu\n",omp_get_thread_num(),parallelID,taskID);
    }
#endif
    //cbtf_collector_resume();
}

// ompt_event_MAY_ALWAYS_TRACE
// set regionId to parallelID, set taskID
void CBTF_ompt_cb_taskwait_begin (ompt_parallel_id_t parallelID,
		        ompt_task_id_t taskID)
{
    //cbtf_collector_pause();
#ifndef NDEBUG
    if (cbtf_ompt_debug_trace) {
	fprintf(stderr,"[%d] CBTF_ompt_cb_taskwait_begin parallelID:%lu taskID:%lu\n",omp_get_thread_num(),parallelID,taskID);
    }
#endif
    //cbtf_collector_resume();
}

// ompt_event_MAY_ALWAYS_TRACE
// set regionId to parallelID, set taskID
void CBTF_ompt_cb_taskwait_end (ompt_parallel_id_t parallelID,
		      ompt_task_id_t taskID)
{
    //cbtf_collector_pause();
#ifndef NDEBUG
    if (cbtf_ompt_debug_trace) {
	fprintf(stderr,"[%d] CBTF_ompt_cb_taskwait_end: parallelID:%lu taskID:%lu\n",omp_get_thread_num(),parallelID,taskID);
    }
#endif
    //cbtf_collector_resume();
}

// ompt_event_UNIMPLEMENTED
// set regionId to parallelID, set taskID
void CBTF_ompt_cb_wait_taskwait_begin (ompt_parallel_id_t parallelID,
			     ompt_task_id_t taskID)
{
#if 0
    //cbtf_collector_pause();
#ifndef NDEBUG
    if (cbtf_ompt_debug) {
	fprintf(stderr,"[%d] BLAME CBTF_ompt_cb_wait_taskwait_begin parallelID:%lu taskID:%lu\n",omp_get_thread_num(),parallelID,taskID);
    }
#endif
    //cbtf_collector_resume();
#endif
}

// ompt_event_UNIMPLEMENTED
// set regionId to parallelID, set taskID
void CBTF_ompt_cb_wait_taskwait_end (ompt_parallel_id_t parallelID,
			   ompt_task_id_t taskID)
{
#if 0
    //cbtf_collector_pause();
#ifndef NDEBUG
    if (cbtf_ompt_debug) {
	fprintf(stderr,"[%d] BLAME CBTF_ompt_cb_wait_taskwait_end: parallelID:%lu taskID:%lu\n",omp_get_thread_num(),parallelID,taskID);
    }
#endif
    //cbtf_collector_resume();
#endif
}

// ompt_event_UNIMPLEMENTED
// set regionId to parallelID, set taskID
void CBTF_ompt_cb_taskgroup_begin (ompt_parallel_id_t parallelID,
			 ompt_task_id_t taskID)
{
#if 0
    //cbtf_collector_pause();
#ifndef NDEBUG
    if (cbtf_ompt_debug) {
	fprintf(stderr,"[%d] CBTF_ompt_cb_taskgroup_begin parallelID:%lu taskID:%lu\n",omp_get_thread_num(),parallelID,taskID);
    }
#endif
    //cbtf_collector_resume();
#endif
}

// ompt_event_UNIMPLEMENTED
// set regionId to parallelID, set taskID
void CBTF_ompt_cb_taskgroup_end (ompt_parallel_id_t parallelID,
		       ompt_task_id_t taskID)
{
#if 0
    //cbtf_collector_pause();
#ifndef NDEBUG
    if (cbtf_ompt_debug) {
	fprintf(stderr,"[%d] CBTF_ompt_cb_taskgroup_end: parallelID:%lu taskID:%lu\n",omp_get_thread_num(),parallelID,taskID);
    }
#endif
    //cbtf_collector_resume();
#endif
}

// ompt_event_UNIMPLEMENTED
// set regionId to parallelID, set taskID
void CBTF_ompt_cb_wait_taskgroup_begin (ompt_parallel_id_t parallelID,
			      ompt_task_id_t taskID)
{
#if 0
    //cbtf_collector_pause();
#ifndef NDEBUG
    if (cbtf_ompt_debug) {
	fprintf(stderr,"[%d] CBTF_ompt_cb_wait_taskgroup_begin parallelID:%lu taskID:%lu\n",omp_get_thread_num(),parallelID,taskID);
    }
#endif
    //cbtf_collector_resume();
#endif
}

// ompt_event_UNIMPLEMENTED
// set regionId to parallelID, set taskID
void CBTF_ompt_cb_wait_taskgroup_end (ompt_parallel_id_t parallelID,
			    ompt_task_id_t taskID)
{
#if 0
    //cbtf_collector_pause();
#ifndef NDEBUG
    if (cbtf_ompt_debug) {
	fprintf(stderr,"[%d] CBTF_ompt_cb_wait_taskgroup_end: parallelID:%lu taskID:%lu\n",omp_get_thread_num(),parallelID,taskID);
    }
#endif
    //cbtf_collector_resume();
#endif
}

// ompt_event_MAY_ALWAYS_TRACE
// set regionId to parallelID, set taskID
void CBTF_ompt_cb_single_others_begin (ompt_parallel_id_t parallelID,
			     ompt_task_id_t taskID)
{
    //cbtf_collector_pause();
#ifndef NDEBUG
    if (cbtf_ompt_debug_trace) {
	fprintf(stderr,"[%d] CBTF_ompt_cb_single_others_begin parallelID:%lu taskID:%lu\n",omp_get_thread_num(),parallelID,taskID);
    }
#endif
    //cbtf_collector_resume();
}

// ompt_event_MAY_ALWAYS_TRACE
// set regionId to parallelID, set taskID
void CBTF_ompt_cb_single_others_end (ompt_parallel_id_t parallelID,
			   ompt_task_id_t taskID)
{
    //cbtf_collector_pause();
#ifndef NDEBUG
    if (cbtf_ompt_debug_trace) {
	fprintf(stderr,"[%d] CBTF_ompt_cb_single_others_end: parallelID:%lu taskID:%lu\n",omp_get_thread_num(),parallelID,taskID);
    }
#endif
    //cbtf_collector_resume();
}

// ompt_event_UNIMPLEMENTED
// notify taskID has begun
void CBTF_ompt_cb_initial_task_begin (ompt_task_id_t taskID)
{
#if 0
    //cbtf_collector_pause();
#ifndef NDEBUG
    if (cbtf_ompt_debug) {
	fprintf(stderr,"[%d] CBTF_ompt_cb_single_others_begin taskID:%lu\n",omp_get_thread_num(),taskID);
    }
#endif
    //cbtf_collector_resume();
#endif
}

// ompt_event_UNIMPLEMENTED
// notify taskID has ended
void CBTF_ompt_cb_initial_task_end (ompt_task_id_t taskID)
{
#if 0
    //cbtf_collector_pause();
#ifndef NDEBUG
    if (cbtf_ompt_debug) {
	fprintf(stderr,"[%d] CBTF_ompt_cb_single_others_end: taskID:%lu\n",omp_get_thread_num(),taskID);
    }
#endif
    //cbtf_collector_resume();
#endif
}

// ompt_event_MAY_ALWAYS_TRACE
// set regionId to parallelID, set taskID
void CBTF_ompt_cb_loop_begin (ompt_parallel_id_t parallelID, ompt_task_id_t taskID)
{
    //cbtf_collector_pause();
#ifndef NDEBUG
    if (cbtf_ompt_debug_trace) {
	fprintf(stderr,"[%d] CBTF_ompt_cb_loop_begin parallelID:%lu taskID:%lu\n",omp_get_thread_num(), parallelID, taskID);
    }
#endif
    //cbtf_collector_resume();
}

// ompt_event_MAY_ALWAYS_TRACE
// set regionId to parallelID, set taskID
void CBTF_ompt_cb_loop_end (ompt_parallel_id_t parallelID, ompt_task_id_t taskID)
{
    //cbtf_collector_pause();
#ifndef NDEBUG
    if (cbtf_ompt_debug_trace) {
	fprintf(stderr,"[%d] CBTF_ompt_cb_loop_end: parallelID:%lu taskID:%lu\n",omp_get_thread_num(),parallelID,taskID);
    }
#endif
    //cbtf_collector_resume();
}

// ompt_event_MAY_ALWAYS_TRACE
// set regionId to parallelID, set taskID
void CBTF_ompt_cb_single_in_block_begin (ompt_parallel_id_t parallelID, ompt_task_id_t taskID)
{
    //cbtf_collector_pause();
#ifndef NDEBUG
    if (cbtf_ompt_debug_trace) {
	fprintf(stderr,"[%d] CBTF_ompt_cb_single_in_block_begin parallelID:%lu taskID:%lu\n",omp_get_thread_num(),parallelID,taskID);
    }
#endif
    //cbtf_collector_resume();
}

// ompt_event_MAY_ALWAYS_TRACE
// set regionId to parallelID, set taskID
void CBTF_ompt_cb_single_in_block_end (ompt_parallel_id_t parallelID, ompt_task_id_t taskID)
{
    //cbtf_collector_pause();
#ifndef NDEBUG
    if (cbtf_ompt_debug_trace) {
	fprintf(stderr,"[%d] CBTF_ompt_cb_single_in_block_end: parallelID:%lu taskID:%lu\n",omp_get_thread_num(),parallelID,taskID);
    }
#endif
    //cbtf_collector_resume();
}

// ompt_event_UNIMPLEMENTED
void CBTF_ompt_cb_sections_begin (ompt_parallel_id_t parallelID, ompt_task_id_t taskID)
{
#if 0
    //cbtf_collector_pause();
#ifndef NDEBUG
    if (cbtf_ompt_debug) {
	fprintf(stderr,"[%d] CBTF_ompt_cb_sections_begin parallelID:%lu taskID:%lu\n",omp_get_thread_num(),parallelID,taskID);
    }
#endif
    //cbtf_collector_resume();
#endif
}

// ompt_event_UNIMPLEMENTED
void CBTF_ompt_cb_sections_end (ompt_parallel_id_t parallelID, ompt_task_id_t taskID)
{
#if 0
    //cbtf_collector_pause();
#ifndef NDEBUG
    if (cbtf_ompt_debug) {
	fprintf(stderr,"[%d] CBTF_ompt_cb_sections_end: parallelID:%lu taskID:%lu\n",omp_get_thread_num(),parallelID,taskID);
    }
#endif
    //cbtf_collector_resume();
#endif
}


// ompt_event_UNIMPLEMENTED
void CBTF_ompt_cb_workshare_begin (ompt_parallel_id_t parallelID, ompt_task_id_t taskID, void *workshare_function)

{
#if 0
    //cbtf_collector_pause();
#ifndef NDEBUG
    if (cbtf_ompt_debug) {
	fprintf(stderr,"[%d] CBTF_ompt_cb_workshare_begin parallelID:%lu taskID:%lu workshare_function:%p\n",omp_get_thread_num(),parallelID,taskID,workshare_function);
    }
#endif
    //cbtf_collector_resume();
#endif
}

// ompt_event_UNIMPLEMENTED
void CBTF_ompt_cb_workshare_end (ompt_parallel_id_t parallelID, ompt_task_id_t taskID)
{
#if 0
    //cbtf_collector_pause();
#ifndef NDEBUG
    if (cbtf_ompt_debug) {
	fprintf(stderr,"[%d] CBTF_ompt_cb_workshare_end: parallelID:%lu taskID:%lu\n",omp_get_thread_num(),parallelID,taskID);
    }
#endif
    //cbtf_collector_resume();
#endif
}

// ompt_event_MAY_ALWAYS_BLAME
// if parallel count is 0,
//    if idle and not busy, cbtf_collector_resume(); and return
//    if busy then stop time for tid, unset busy.
// set idle for tid and start time.
void CBTF_ompt_cb_idle_begin(ompt_thread_id_t thread_id /* ID of thread*/)
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);
    cbtf_collector_pause();
    OMPT_THREAD_IDLE(true);
    ++tls->idle_count;
    tls->idle_btime = CBTF_GetTime();
    ompt_task_id_t tsk_id = ompt_get_task_id(0);
    ompt_frame_t *idle_frame = ompt_get_idle_frame();

#ifndef NDEBUG
    if (cbtf_ompt_debug_blame) {
	fprintf(stderr,"[%d] CBTF_ompt_cb_idle_begin context:%p\n" ,omp_get_thread_num(), current_region_context);
	fprintf(stderr, "[%d] tsk_id:%d CBTF_ompt_cb_idle_begin: idle_frame->exit_runtime_frame=%p idle_frame->reenter_runtime_frame=%p\n",
	omp_get_thread_num(), tsk_id, idle_frame->exit_runtime_frame,idle_frame->reenter_runtime_frame);
    }
#endif
    cbtf_collector_resume();
}

// ompt_event_MAY_ALWAYS_BLAME
// if parallel count is 0, start time for new parallel region and set busy this tid.
// else unset busy for this tid
void CBTF_ompt_cb_idle_end(ompt_thread_id_t thread_id        /* ID of thread*/)
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);
    cbtf_collector_pause();
    OMPT_THREAD_IDLE(false);

    uint64_t t = CBTF_GetTime() - tls->idle_btime;
    CBTF_omptp_event event;
    memset(&event, 0, sizeof(CBTF_omptp_event));
    event.time = t;
    tls->idle_ttime += t;
    if(current_region_stacktrace_size > 0) {
	memcpy(tls->stacktrace,current_region_stacktrace,sizeof(current_region_stacktrace));
	tls->stacktrace[0] = (uint64_t)OMPT_THREAD_IDLE;
    }
    omptp_record_event(&event, tls->stacktrace, current_region_stacktrace_size);

    ompt_task_id_t tsk_id = ompt_get_task_id(0);
    ompt_frame_t *idle_frame = ompt_get_idle_frame();

#ifndef NDEBUG
    if (cbtf_ompt_debug_blame) {
	fprintf(stderr,"[%d] CBTF_ompt_cb_idle_end context:%p\n" ,omp_get_thread_num(), current_region_context);
	fprintf(stderr, "[%d] tsk_id:%d CBTF_ompt_cb_idle_end: idle_frame->exit_runtime_frame=%p idle_frame->reenter_runtime_frame=%p\n",
	omp_get_thread_num(), tsk_id, idle_frame->exit_runtime_frame,idle_frame->reenter_runtime_frame);
	fprintf(stderr, "[%d] CBTF_ompt_cb_idle_end: idle_time:%f\n" ,omp_get_thread_num(), (float)t/1000000000);
    }
#endif

    cbtf_collector_resume();
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


    CBTF_register_ompt_callback(ompt_event_parallel_begin,CBTF_ompt_cb_parallel_region_begin);
    CBTF_register_ompt_callback(ompt_event_parallel_end,CBTF_ompt_cb_parallel_region_end);
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
