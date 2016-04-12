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
#include "KrellInstitute/Services/Assert.h"
#include <time.h>

#include "ompt.h"
#include "KrellInstitute/Services/Ompt.h"

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
extern void cbtf_thread_idle(bool);
extern void cbtf_thread_barrier(bool);
extern void cbtf_thread_wait_barrier(bool);

// local thread variables to record various details. May need a
// container for this later...
__thread uint64_t barrier_btime;
__thread uint64_t barrier_ttime;
__thread uint64_t lock_count;
__thread uint64_t lock_btime;
__thread uint64_t lock_time;

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
    // do not sample if inside our tool
    //cbtf_collector_pause();
 
#ifndef NDEBUG
    if (cbtf_ompt_debug) {
	fprintf(stderr,
	"[%d] CBTF_ompt_cb_parallel_region_begin: parent_taskID:%lu parallelID:%lu req_team_size:%u context:%p\n",
	omp_get_thread_num(), parent_taskID, parallelID, requested_team_size, parallel_function);
    }
#endif
    // This would record into the sample address buffer. We can not
    // use that buffer unless we can ensure we do not "count" this sample
    // in any way as a sample or event. We really just want the address so
    // we can later get a symbol. We may not need to pass this string as
    // an identifier. But we may wish to pass the parallel and task IDs.
    collector_record_addr("CBTF_OMP_PARALLEL_REGION_BEGIN",(uint64_t)parallel_function);
 
    // resume collection
    //cbtf_collector_resume();
}

// ompt_event_MAY_ALWAYS
// record regionID
// if parallel flag, record stop time, decrement parallel flag.
void CBTF_ompt_cb_parallel_region_end (
  ompt_parallel_id_t parallelID,    /* parallel region ID */
  ompt_task_id_t parent_taskID,     /* parent task id */
  ompt_invoker_t invoker)           /* pointer to outlined function */
{
    // do not sample if inside our tool
    //cbtf_collector_pause();
#ifndef NDEBUG
    if (cbtf_ompt_debug) {
	fprintf(stderr,
	"[%d] CBTF_ompt_cb_parallel_region_end: parent_taskID:%lu  parallelID:%lu\n",
	omp_get_thread_num(), parent_taskID, parallelID);
    }
#endif
    // resume collection
    //cbtf_collector_resume();
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
    //cbtf_collector_pause();
#ifndef NDEBUG
    if (cbtf_ompt_debug_blame) {
	fprintf(stderr,"[%d] CBTF_ompt_cb_task_begin: parent_taskID:%lu new_task_id:%lu task_function:%p\n",
	omp_get_thread_num() ,parent_taskID, new_taskID, task_function);
    }
#endif
    collector_record_addr("CBTF_OMP_TASK_BEGIN",(uint64_t)task_function);
    //cbtf_collector_resume();
}

// ompt_event_MAY_ALWAYS
//  new_taskID as taskid.
// decrement is parallel flag.
void CBTF_ompt_cb_task_end (
  ompt_task_id_t parent_taskID     /* parent task id */
  )
{
    //cbtf_collector_pause();
#ifndef NDEBUG
    if (cbtf_ompt_debug) {
	fprintf(stderr,"[%d] CBTF_ompt_cb_task_end: parent_taskID = %lu\n",omp_get_thread_num() ,parent_taskID);
    }
#endif
    //cbtf_collector_resume();
}

// ompt_event_MAY_ALWAYS
// monitor notifies us of this...
// pass omp thread num to collector.
void CBTF_ompt_cb_thread_begin()
{
    barrier_ttime = 0;
    lock_count = 0;
    lock_btime = 0;
    lock_time = 0;
#ifndef NDEBUG
    if (cbtf_ompt_debug) {
	fprintf(stderr, "[%d,%d] CBTF_ompt_cb_thread_begin:\n",omp_get_thread_num(),monitor_get_thread_num());
    }
#endif
    cbtf_collector_set_openmp_threadid(omp_get_thread_num());
}

// ompt_event_MAY_ALWAYS
// monitor notifies us of this as well...
void CBTF_ompt_cb_thread_end()
{
#ifndef NDEBUG
    if (cbtf_ompt_debug) {
	fprintf(stderr, "[%d] CBTF_ompt_cb_thread_end: barrier time:%f, lock count:%d lock_time:%f\n"
	,omp_get_thread_num(), (float)barrier_ttime/1000000000, lock_count, (float)lock_time/1000000000);
    }
#endif
}

// ompt_event_MAY_ALWAYS
// do anything here?
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
    lock_count++;
    struct timespec now;
    Assert(clock_gettime(CLOCK_REALTIME, &now) == 0);
    lock_btime = ((uint64_t)(now.tv_sec) * (uint64_t)(1000000000)) +
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
// cbtf_collector_pause();

#ifndef NDEBUG
    if (cbtf_ompt_debug_blame) {
	fprintf(stderr, "[%d] CBTF_ompt_cb_release_lock: waitID = %lu\n",omp_get_thread_num(),waitID);
    }
#endif

   lock_count++;

#if 0
    struct timespec now;
    Assert(clock_gettime(CLOCK_REALTIME, &now) == 0);
    uint64_t lock_etime = ((uint64_t)(now.tv_sec) * (uint64_t)(1000000000)) +
        (uint64_t)(now.tv_nsec);
    lock_time += (lock_etime - lock_btime);
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
    cbtf_collector_pause();
    cbtf_thread_barrier(true);

    // record barrier begin time.
    struct timespec now;
    Assert(clock_gettime(CLOCK_REALTIME, &now) == 0);
    barrier_btime = ((uint64_t)(now.tv_sec) * (uint64_t)(1000000000)) +
        (uint64_t)(now.tv_nsec);

#ifndef NDEBUG
    if (cbtf_ompt_debug_blame) {
	fprintf(stderr,"[%d] CBTF_ompt_cb_barrier_begin parallelID:%lu taskID:%lu begin time:%lu\n"
		,omp_get_thread_num(), parallelID,taskID, barrier_btime);
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
    cbtf_collector_pause();
    cbtf_thread_barrier(false);

    struct timespec now;
    Assert(clock_gettime(CLOCK_REALTIME, &now) == 0);
    uint64_t etime = ((uint64_t)(now.tv_sec) * (uint64_t)(1000000000)) +
        (uint64_t)(now.tv_nsec);
    barrier_ttime += (etime - barrier_btime);

#ifndef NDEBUG
    if (cbtf_ompt_debug_blame) {
	fprintf(stderr,"[%d] CBTF_ompt_cb_barrier_end: parallelID:%lu taskID:%lu total barrier time:%lu\n"
	    ,omp_get_thread_num(),parallelID,taskID,barrier_ttime);
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
    cbtf_collector_pause();
    cbtf_thread_wait_barrier(true);
#ifndef NDEBUG
    if (cbtf_ompt_debug_blame) {
	fprintf(stderr,"[%d] CBTF_ompt_cb_wait_barrier_begin parallelID:%lu taskID:%lu\n",omp_get_thread_num(),parallelID,taskID);
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
    cbtf_collector_pause();
    cbtf_thread_wait_barrier(false);
#ifndef NDEBUG
    if (cbtf_ompt_debug_blame) {
	fprintf(stderr,"[%d] CBTF_ompt_cb_wait_barrier_end: parallelID:%lu taskID:%lu\n",omp_get_thread_num(),parallelID,taskID);
    }
#endif
    cbtf_collector_resume();
}

// ompt_event_MAY_ALWAYS_TRACE
// set regionId to parallelID, set taskID
void CBTF_ompt_cb_implicit_task_begin (ompt_parallel_id_t parallelID,
		             ompt_task_id_t taskID)
{
    //cbtf_collector_pause();
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
    //cbtf_collector_pause();
#ifndef NDEBUG
    if (cbtf_ompt_debug_trace) {
	fprintf(stderr,"[%d] CBTF_ompt_cb_implicit_task_end: parallelID:%lu taskID:%lu\n",omp_get_thread_num(),parallelID,taskID);
    }
#endif
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
    cbtf_collector_pause();
    cbtf_thread_idle(true);
#ifndef NDEBUG
    if (cbtf_ompt_debug_blame) {
	fprintf(stderr,"[%d] CBTF_ompt_cb_idle_begin %u\n" ,omp_get_thread_num(), thread_id);
    }
#endif
    cbtf_collector_resume();
}

// ompt_event_MAY_ALWAYS_BLAME
// if parallel count is 0, start time for new parallel region and set busy this tid.
// else unset busy for this tid
void CBTF_ompt_cb_idle_end(ompt_thread_id_t thread_id        /* ID of thread*/)
{
    cbtf_collector_pause();
    cbtf_thread_idle(false);
#ifndef NDEBUG
    if (cbtf_ompt_debug_blame) {
	fprintf(stderr,"[%d] CBTF_ompt_cb_idle_end %u\n" ,omp_get_thread_num(), thread_id);
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
