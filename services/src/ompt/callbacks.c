/*******************************************************************************
** Copyright (c) 2014-2015  The Krell Institute. All Rights Reserved.
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

// notification to collector of begin and end of these events.
extern void cbtf_thread_idle(bool);
extern void cbtf_thread_barrier(bool);
extern void cbtf_thread_wait_barrier(bool);


// Attempt to register a callback with the ompt api.  Not all callbacks
// are currently implemented in all ompt implementation begining with
// intel 12.  No intel release currently has  ompt and the use must
// supply the libiomp built from intel source.
void CBTF_register_ompt_callback(ompt_event_t event, void* callback)
{
    int rval = ompt_set_callback(event, (ompt_callback_t)callback);
#ifndef NDEBUG
    if (cbtf_ompt_debug) {
	if (!rval) {
	    fprintf(stderr,"register_ompt_callback: FAILURE registering ompt event %d\n",event);
	} else {
	    fprintf(stderr,"register_ompt_callback: SUCCESS registering ompt event %d\n",event);
	}
    }
#endif
}


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

// record parent_taskID, parallel regionID.
// record start time and get address or callstack of this context
// This seems to only be active in the master thread.
// increment is parallel flag. This flag indicates that a
// parallel region is active to other callbacks that may be
// interested.
void CBTF_ompt_cb_parallel_region_begin (
  ompt_task_id_t parent_taskID,     /* parent task id */
  ompt_frame_t *parent_taskframe,   /* unused parent task frame data */
  ompt_parallel_id_t parallelID,    /* parallel region ID */
  void *context)                    /* context of parallel - function pointer */
{
    // do not sample if inside our tool
    //cbtf_collector_pause();
 
#ifndef NDEBUG
    if (cbtf_ompt_debug) {
	//fprintf(stderr,
	//"[%d] CBTF_ompt_cb_parallel_region_begin: parent_taskID = %lu,  parallelID = %lu, context = %#lx\n",
	//omp_get_thread_num(), parent_taskID, parallelID, context);
    }
#endif
    // This would record into the sample address buffer. We can not
    // use that buffer unless we can ensure we do not "count" this sample
    // in any way as a sample or event. We really just want the address so
    // we can later get a symbol. We may not need to pass this string as
    // an identifier. But we may wish to pass the parallel and task IDs.
    //collector_record_addr("CBTF_OMP_PARALLEL_REGION_BEGIN",(uint64_t)context);
 
    // resume collection
    //cbtf_collector_resume();
}

// record regionID
// if parallel flag, record stop time, decrement parallel flag.
void CBTF_ompt_cb_parallel_region_end (
  ompt_task_id_t parent_taskID,     /* parent task id */
  ompt_frame_t *parent_taskframe,   /* unused parent task frame data */
  ompt_parallel_id_t parallelID,    /* parallel region ID */
  void *context)                    /* context of parallel - function pointer */
{
    // do not sample if inside our tool
    //cbtf_collector_pause();
//	fprintf(stderr,
//	"[%d] CBTF_ompt_cb_parallel_region_end: parent_taskID = %lu,  parallelID = %lu, context = %p\n",
//	omp_get_thread_num(), parent_taskID, parallelID, context);
    // resume collection
 //   cbtf_collector_resume();
}

// record new_taskID as taskid.
// record start time and get callstack of this context
// increment is parallel flag.
void CBTF_ompt_cb_task_begin (
  ompt_task_id_t parent_taskID,     /* parent task id */
  ompt_frame_t *parent_taskframe,   /* unused parent task frame data */
  ompt_task_id_t new_taskID,        /* new task id */
  void *context)                    /* context of task - function pointer */
{
    //cbtf_collector_pause();
//	fprintf(stderr,"[%d] CBTF_ompt_cb_task_begin: new task: parent_taskID = %lu new_task_id = %lu, task_function = %p\n",
//	omp_get_thread_num() ,parent_taskID, new_taskID, context);
 //   cbtf_collector_resume();
}

//  new_taskID as taskid.
// decrement is parallel flag.
void CBTF_ompt_cb_task_end (
  ompt_task_id_t parent_taskID,     /* parent task id */
  ompt_frame_t *parent_taskframe,   /* unused parent task frame data */
  ompt_task_id_t new_taskID,        /* new task id */
  void *context)                    /* context of task - function pointer */
{
    //cbtf_collector_pause();
//	fprintf(stderr,"[%d] CBTF_ompt_cb_task_end: new task: parent_taskID = %lu new_task_id = %lu, task_function = %p\n",
//	omp_get_thread_num() ,parent_taskID, new_taskID, context);
 //   cbtf_collector_resume();
}


// monitor notifies us of this...
// pass omp thread num to collector.
void CBTF_ompt_cb_thread_begin()
{
    //fprintf(stderr, "[%d] CBTF_ompt_cb_thread_begin:\n",omp_get_thread_num());
    //cbtf_collector_set_omp_threadnum(omp_get_thread_num());
}

// monitor notifies us of this...
void CBTF_ompt_cb_thread_end()
{
//	fprintf(stderr, "[%d] CBTF_ompt_cb_thread_end:\n",omp_get_thread_num());
}

// do anything here?
void CBTF_ompt_cb_control(uint64_t command, uint64_t modifier)
{
	//fprintf(stderr,"[%d] CBTF_ompt_cb_control: %llx, %llx\n",omp_get_thread_num(), command, modifier);
}

// shut down of ompt ...
void CBTF_ompt_cb_runtime_shutdown()
{
	//fprintf(stderr, "[%d] CBTF_ompt_cb_runtime_shutdown:\n",omp_get_thread_num());
}

// set wait, start time wait on wait name
void CBTF_ompt_cb_wait_atomic (ompt_wait_id_t waitID) {
// cbtf_collector_pause();
	//fprintf(stderr, "[%d] CBTF_ompt_cb_wait_atomic: waitID = %lu\n",omp_get_thread_num(),waitID);
// cbtf_collector_resume();
}

// set wait, start time wait on wait name
void CBTF_ompt_cb_wait_ordered (ompt_wait_id_t waitID) {
// cbtf_collector_pause();
	//fprintf(stderr, "[%d] CBTF_ompt_cb_wait_ordered: waitID = %lu\n",omp_get_thread_num(),waitID);
// cbtf_collector_resume();
}

// set wait, start time wait on wait name
void CBTF_ompt_cb_wait_critical (ompt_wait_id_t waitID) {
// cbtf_collector_pause();
	//fprintf(stderr, "[%d] CBTF_ompt_cb_wait_critical: waitID = %lu\n",omp_get_thread_num(),waitID);
// cbtf_collector_resume();
}

// set wait, start time wait on wait name
void CBTF_ompt_cb_wait_lock (ompt_wait_id_t waitID) {
// cbtf_collector_pause();
	//fprintf(stderr, "[%d] CBTF_ompt_cb_wait_lock: waitID = %lu\n",omp_get_thread_num(),waitID);
// cbtf_collector_resume();
}

// if aquired, end time on wait name.
// clear wait, set aquired, start time on region name
void CBTF_ompt_cb_aquired_atomic (ompt_wait_id_t waitID) {
// cbtf_collector_pause();
	//fprintf(stderr, "[%d] CBTF_ompt_cb_aquired_atomic: waitID = %lu\n",omp_get_thread_num(),waitID);
// cbtf_collector_resume();
}

// if aquired, end time on wait name.
// clear wait, set aquired, start time on region name
void CBTF_ompt_cb_aquired_ordered (ompt_wait_id_t waitID) {
// cbtf_collector_pause();
	//fprintf(stderr, "[%d] CBTF_ompt_cb_aquired_ordered: waitID = %lu\n",omp_get_thread_num(),waitID);
// cbtf_collector_resume();
}

// if aquired, end time on wait name.
// clear wait, set aquired, start time on region name
void CBTF_ompt_cb_aquired_critical (ompt_wait_id_t waitID) {
// cbtf_collector_pause();
	//fprintf(stderr, "[%d] CBTF_ompt_cb_aquired_critical: waitID = %lu\n",omp_get_thread_num(),waitID);
// cbtf_collector_resume();
}

// if aquired, end time on wait name.
// clear wait, set aquired, start time on region name
void CBTF_ompt_cb_aquired_lock (ompt_wait_id_t waitID) {
// cbtf_collector_pause();
	//fprintf(stderr, "[%d] CBTF_ompt_cb_aquired_lock: waitID = %lu\n",omp_get_thread_num(),waitID);
// cbtf_collector_resume();
}

// if aquired, end time region name, clear aquired
void CBTF_ompt_cb_release_atomic (ompt_wait_id_t waitID) {
// cbtf_collector_pause();
	//fprintf(stderr, "[%d] CBTF_ompt_cb_release_atomic: waitID = %lu\n",omp_get_thread_num(),waitID);
// cbtf_collector_resume();
}

// if aquired, end time region name, clear aquired
void CBTF_ompt_cb_release_ordered (ompt_wait_id_t waitID) {
// cbtf_collector_pause();
	//fprintf(stderr, "[%d] CBTF_ompt_cb_release_ordered: waitID = %lu\n",omp_get_thread_num(),waitID);
// cbtf_collector_resume();
}

// if aquired, end time region name, clear aquired
void CBTF_ompt_cb_release_critical (ompt_wait_id_t waitID) {
// cbtf_collector_pause();
	//fprintf(stderr, "[%d] CBTF_ompt_cb_release_critical: waitID = %lu\n",omp_get_thread_num(),waitID);
// cbtf_collector_resume();
}

// if aquired, end time region name, clear aquired
void CBTF_ompt_cb_release_lock (ompt_wait_id_t waitID) {
// cbtf_collector_pause();
	//fprintf(stderr, "[%d] CBTF_ompt_cb_release_lock: waitID = %lu\n",omp_get_thread_num(),waitID);
// cbtf_collector_resume();
}


// sync event. disabled for now.
// set regionId to parallelID, set taskID
// send notification to collector that a barrier has been entered.
// TODO: pass the task and parallel IDs if needed.
void CBTF_ompt_cb_barrier_begin (ompt_parallel_id_t parallelID,
		       ompt_task_id_t taskID)
{
    //cbtf_collector_pause()();
    //cbtf_thread_barrier(true);
	//fprintf(stderr,"[%d] CBTF_ompt_cb_barrier_begin new: parallelID = %lu, taskID = %lu\n",omp_get_thread_num(),parallelID,taskID);
    //cbtf_collector_resume();
}

// sync event. disabled for now.
// set regionId to parallelID, set taskID
// send notification to collector that a barrier has ended.
// TODO: pass the task and parallel IDs if needed.
void CBTF_ompt_cb_barrier_end (ompt_parallel_id_t parallelID,
		     ompt_task_id_t taskID)
{
    //cbtf_collector_pause()();
    //cbtf_thread_barrier(false);
	//fprintf(stderr,"[%d] CBTF_ompt_cb_barrier_end: parallelID = %lu, taskID = %lu\n",omp_get_thread_num(),parallelID,taskID);
    //cbtf_collector_resume();
}

// blame event
// set regionId to parallelID, set taskID
// send notification to collector that a wait_barrier has begun.
// TODO: pass the task and parallel IDs if needed.
void CBTF_ompt_cb_wait_barrier_begin (ompt_parallel_id_t parallelID,
		            ompt_task_id_t taskID)
{
    cbtf_collector_pause();
    cbtf_thread_wait_barrier(true);
	//fprintf(stderr,"[%d] CBTF_ompt_cb_wait_barrier_begin new: parallelID = %lu, taskID = %lu\n",omp_get_thread_num(),parallelID,taskID);
    cbtf_collector_resume();
}

// blame event
// set regionId to parallelID, set taskID
// send notification to collector that a wait_barrier has ended.
// TODO: pass the task and parallel IDs if needed.
void CBTF_ompt_cb_wait_barrier_end (ompt_parallel_id_t parallelID,
			  ompt_task_id_t taskID)
{
    cbtf_collector_pause();
    cbtf_thread_wait_barrier(false);
	//fprintf(stderr,"[%d] CBTF_ompt_cb_wait_barrier_end: parallelID = %lu, taskID = %lu\n",omp_get_thread_num(),parallelID,taskID);
    cbtf_collector_resume();
}

// set regionId to parallelID, set taskID
void CBTF_ompt_cb_implicit_task_begin (ompt_parallel_id_t parallelID,
		             ompt_task_id_t taskID)
{
    //cbtf_collector_pause();
	//fprintf(stderr,"[%d] CBTF_ompt_cb_implicit_task_begin new: parallelID = %lu, taskID = %lu\n",omp_get_thread_num(),parallelID,taskID);
    //cbtf_collector_resume();
}

// set regionId to parallelID, set taskID
void CBTF_ompt_cb_implicit_task_end (ompt_parallel_id_t parallelID,
			   ompt_task_id_t taskID)
{
    //cbtf_collector_pause();
	//fprintf(stderr,"[%d] CBTF_ompt_cb_implicit_task_end: parallelID = %lu, taskID = %lu\n",omp_get_thread_num(),parallelID,taskID);
    //cbtf_collector_resume();
}

// set regionId to parallelID, set taskID
void CBTF_ompt_cb_master_begin (ompt_parallel_id_t parallelID,
		      ompt_task_id_t taskID)
{
    //cbtf_collector_pause();
	//fprintf(stderr,"[%d] CBTF_ompt_cb_master_begin new: parallelID = %lu, taskID = %lu\n",omp_get_thread_num(),parallelID,taskID);
    //cbtf_collector_resume();
}

// set regionId to parallelID, set taskID
void CBTF_ompt_cb_master_end (ompt_parallel_id_t parallelID,
		    ompt_task_id_t taskID)
{
    //cbtf_collector_pause();
	//fprintf(stderr,"[%d] CBTF_ompt_cb_master_end: parallelID = %lu, taskID = %lu\n",omp_get_thread_num(),parallelID,taskID);
    //cbtf_collector_resume();
}

// set regionId to parallelID, set taskID
void CBTF_ompt_cb_taskwait_begin (ompt_parallel_id_t parallelID,
		        ompt_task_id_t taskID)
{
    //cbtf_collector_pause();
	//fprintf(stderr,"[%d] CBTF_ompt_cb_taskwait_begin new: parallelID = %lu, taskID = %lu\n",omp_get_thread_num(),parallelID,taskID);
    //cbtf_collector_resume();
}

// set regionId to parallelID, set taskID
void CBTF_ompt_cb_taskwait_end (ompt_parallel_id_t parallelID,
		      ompt_task_id_t taskID)
{
    //cbtf_collector_pause();
	//fprintf(stderr,"[%d] CBTF_ompt_cb_taskwait_end: parallelID = %lu, taskID = %lu\n",omp_get_thread_num(),parallelID,taskID);
    //cbtf_collector_resume();
}

// set regionId to parallelID, set taskID
void CBTF_ompt_cb_wait_taskwait_begin (ompt_parallel_id_t parallelID,
			     ompt_task_id_t taskID)
{
    //cbtf_collector_pause();
	//fprintf(stderr,"[%d] CBTF_ompt_cb_wait_taskwait_begin new: parallelID = %lu, taskID = %lu\n",omp_get_thread_num(),parallelID,taskID);
    //cbtf_collector_resume();
}

// set regionId to parallelID, set taskID
void CBTF_ompt_cb_wait_taskwait_end (ompt_parallel_id_t parallelID,
			   ompt_task_id_t taskID)
{
    //cbtf_collector_pause();
	//fprintf(stderr,"[%d] CBTF_ompt_cb_wait_taskwait_end: parallelID = %lu, taskID = %lu\n",omp_get_thread_num(),parallelID,taskID);
    //cbtf_collector_resume();
}

// set regionId to parallelID, set taskID
void CBTF_ompt_cb_taskgroup_begin (ompt_parallel_id_t parallelID,
			 ompt_task_id_t taskID)
{
    //cbtf_collector_pause();
	//fprintf(stderr,"[%d] CBTF_ompt_cb_taskgroup_begin new: parallelID = %lu, taskID = %lu\n",omp_get_thread_num(),parallelID,taskID);
    //cbtf_collector_resume();
}

// set regionId to parallelID, set taskID
void CBTF_ompt_cb_taskgroup_end (ompt_parallel_id_t parallelID,
		       ompt_task_id_t taskID)
{
    //cbtf_collector_pause();
	//fprintf(stderr,"[%d] CBTF_ompt_cb_taskgroup_end: parallelID = %lu, taskID = %lu\n",omp_get_thread_num(),parallelID,taskID);
    //cbtf_collector_resume();
}

// set regionId to parallelID, set taskID
void CBTF_ompt_cb_wait_taskgroup_begin (ompt_parallel_id_t parallelID,
			      ompt_task_id_t taskID)
{
    //cbtf_collector_pause();
	//fprintf(stderr,"[%d] CBTF_ompt_cb_wait_taskgroup_begin new: parallelID = %lu, taskID = %lu\n",omp_get_thread_num(),parallelID,taskID);
    //cbtf_collector_resume();
}

// set regionId to parallelID, set taskID
void CBTF_ompt_cb_wait_taskgroup_end (ompt_parallel_id_t parallelID,
			    ompt_task_id_t taskID)
{
    //cbtf_collector_pause();
	//fprintf(stderr,"[%d] CBTF_ompt_cb_wait_taskgroup_end: parallelID = %lu, taskID = %lu\n",omp_get_thread_num(),parallelID,taskID);
    //cbtf_collector_resume();
}

// set regionId to parallelID, set taskID
void CBTF_ompt_cb_single_others_begin (ompt_parallel_id_t parallelID,
			     ompt_task_id_t taskID)
{
    //cbtf_collector_pause();
	//fprintf(stderr,"[%d] CBTF_ompt_cb_single_others_begin new: parallelID = %lu, taskID = %lu\n",omp_get_thread_num(),parallelID,taskID);
    //cbtf_collector_resume();
}

// set regionId to parallelID, set taskID
void CBTF_ompt_cb_single_others_end (ompt_parallel_id_t parallelID,
			   ompt_task_id_t taskID)
{
    //cbtf_collector_pause();
	//fprintf(stderr,"[%d] CBTF_ompt_cb_single_others_end: parallelID = %lu, taskID = %lu\n",omp_get_thread_num(),parallelID,taskID);
    //cbtf_collector_resume();
}

// notify taskID has begun
void CBTF_ompt_cb_initial_task_begin (ompt_task_id_t taskID)
{
    //cbtf_collector_pause();
	//fprintf(stderr,"[%d] CBTF_ompt_cb_single_others_begin new: taskID = %lu\n",omp_get_thread_num(),taskID);
    //cbtf_collector_resume();
}

// notify taskID has ended
void CBTF_ompt_cb_initial_task_end (ompt_task_id_t taskID)
{
    //cbtf_collector_pause();
	//fprintf(stderr,"[%d] CBTF_ompt_cb_single_others_end: taskID = %lu\n",omp_get_thread_num(),taskID);
    //cbtf_collector_resume();
}

// set regionId to parallelID, set taskID
void CBTF_ompt_cb_loop_begin (ompt_parallel_id_t parallelID, ompt_task_id_t taskID)
{
    //cbtf_collector_pause();
	//fprintf(stderr,"[%d] CBTF_ompt_cb_loop_begin new: parallelID = %lu, taskID = %lu\n",omp_get_thread_num(), parallelID, taskID);
    //cbtf_collector_resume();
}

// set regionId to parallelID, set taskID
void CBTF_ompt_cb_loop_end (ompt_parallel_id_t parallelID, ompt_task_id_t taskID)
{
    //cbtf_collector_pause();
	//fprintf(stderr,"[%d] CBTF_ompt_cb_loop_end: parallelID = %lu, taskID = %lu\n",omp_get_thread_num(),parallelID,taskID);
    //cbtf_collector_resume();
}

// set regionId to parallelID, set taskID
void CBTF_ompt_cb_single_in_block_begin (ompt_parallel_id_t parallelID, ompt_task_id_t taskID)
{
    //cbtf_collector_pause();
	//fprintf(stderr,"[%d] CBTF_ompt_cb_single_in_block_begin new: parallelID = %lu, taskID = %lu\n",omp_get_thread_num(),parallelID,taskID);
    //cbtf_collector_resume();
}

// set regionId to parallelID, set taskID
void CBTF_ompt_cb_single_in_block_end (ompt_parallel_id_t parallelID, ompt_task_id_t taskID)
{
    //cbtf_collector_pause();
	//fprintf(stderr,"[%d] CBTF_ompt_cb_single_in_block_end: parallelID = %lu, taskID = %lu\n",omp_get_thread_num(),parallelID,taskID);
    //cbtf_collector_resume();
}

// set regionId to parallelID, set taskID
void CBTF_ompt_cb_sections_begin (ompt_parallel_id_t parallelID, ompt_task_id_t taskID)
{
    //cbtf_collector_pause();
	//fprintf(stderr,"[%d] CBTF_ompt_cb_sections_begin new: parallelID = %lu, taskID = %lu\n",omp_get_thread_num(),parallelID,taskID);
    //cbtf_collector_resume();
}

// set regionId to parallelID, set taskID
void CBTF_ompt_cb_sections_end (ompt_parallel_id_t parallelID, ompt_task_id_t taskID)
{
    //cbtf_collector_pause();
	//fprintf(stderr,"[%d] CBTF_ompt_cb_sections_end: parallelID = %lu, taskID = %lu\n",omp_get_thread_num(),parallelID,taskID);
    //cbtf_collector_resume();
}


// set regionId to parallelID, set taskID?
// get callstack for context.
void CBTF_ompt_cb_workshare_begin (ompt_parallel_id_t parallelID, ompt_task_id_t taskID, void *context)

{
    //cbtf_collector_pause();
	//fprintf(stderr,"[%d] CBTF_ompt_cb_workshare_begin new: parallelID = %lu, taskID = %lu\n",omp_get_thread_num(),parallelID,taskID);
    //cbtf_collector_resume();
}

// set regionId to parallelID, set taskID
void CBTF_ompt_cb_workshare_end (ompt_parallel_id_t parallelID, ompt_task_id_t taskID)
{
    //cbtf_collector_pause();
	//fprintf(stderr,"[%d] CBTF_ompt_cb_workshare_end: parallelID = %lu, taskID = %lu\n",omp_get_thread_num(),parallelID,taskID);
    //cbtf_collector_resume();
}

// blame event. TODO.
// if parallel count is 0,
//    if idle and not busy, cbtf_collector_resume(); and return
//    if busy then stop time for tid, unset busy.
// set idle for tid and start time.
void CBTF_ompt_cb_idle_begin()
{
    cbtf_collector_pause();
    cbtf_thread_idle(true);
	//fprintf(stderr,"[%d] CBTF_ompt_cb_idle_begin:\n",omp_get_thread_num());
    cbtf_collector_resume();
}

// blame event.  TODO.
// if parallel count is 0, start time for new parallel region and set busy this tid.
// else unset busy for this tid
void CBTF_ompt_cb_idle_end()
{
    cbtf_collector_pause();
    cbtf_thread_idle(false);
	//fprintf(stderr,"[%d] CBTF_ompt_cb_idle_end:\n",omp_get_thread_num());
    cbtf_collector_resume();
}

// initialize our ompt services. Currently we initialize any callback available
// in the current libiomp ompt interface. Most callbacks are currently no-ops
// in the ompt service except for the blame events idle and wait_barrier.
// TODO. Allow collector runtimes to dictate which ompt callbacks to activate
// outside of the mandatory events.  We may not use all mandatory events depending
// on the collector.
int ompt_initialize(ompt_function_lookup_t lookup_func,
                    const char *rtver, int omptver)
{
#ifndef NDEBUG
    if ( (getenv("CBTF_DEBUG_OMPT") != NULL)) {
	cbtf_ompt_debug = 1;
    }
#endif

    fprintf(stderr, "CBTF OMPT Init: %s ver %i\n",rtver,omptver);

    ompt_enumerate_state = (ompt_enumerate_state_t) lookup_func("ompt_enumerate_state");
    ompt_set_callback = (ompt_set_callback_t) lookup_func("ompt_set_callback");
    ompt_get_callback = (ompt_get_callback_t) lookup_func("ompt_get_callback");
    ompt_get_idle_frame = (ompt_get_idle_frame_t) lookup_func("ompt_get_idle_frame");
    ompt_get_task_frame = (ompt_get_task_frame_t) lookup_func("ompt_get_task_frame");
    ompt_get_state = (ompt_get_state_t) lookup_func("ompt_get_state");
    ompt_get_parallel_id = (ompt_get_parallel_id_t) lookup_func("ompt_get_parallel_id");
    ompt_get_task_id = (ompt_get_task_id_t) lookup_func("ompt_get_task_id");
    ompt_get_thread_id = (ompt_get_thread_id_t) lookup_func("ompt_get_thread_id");


    fprintf(stderr, "CBTF OMPT Init: Registering callbacks\n");
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
// Not all versions of libiomp have all events implemented.
// ie. ompt changes between intel 12,13,14 compiler releases.
// currently, the ompt interface is not available in a released intel compiler.
#if 0
    CBTF_register_ompt_callback(ompt_event_aquired_atomic,CBTF_ompt_cb_aquired_atomic);
    CBTF_register_ompt_callback(ompt_event_aquired_ordered,CBTF_ompt_cb_aquired_ordered);
    CBTF_register_ompt_callback(ompt_event_aquired_critical,CBTF_ompt_cb_aquired_critical);
    CBTF_register_ompt_callback(ompt_event_aquired_lock,CBTF_ompt_cb_aquired_lock);
#endif
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
    //fprintf(stderr, "CBTF OMPT Init: FINISHED Registering callbacks\n");
    return 1;
}
