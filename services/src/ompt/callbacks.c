/*******************************************************************************
** Copyright (c) 2014  The Krell Institute. All Rights Reserved.
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

extern void cbtf_offline_service_stop_timer();
extern void cbtf_offline_service_start_timer();

#define DEBUGOMPT if(cbtf_ompt_debug)

void CBTF_register_ompt_callback(ompt_event_t event, void* callback)
{
    int rval = ompt_set_callback(event, (ompt_callback_t)callback);
    if (!rval) {
        fprintf(stderr,"register_ompt_callback: FAILURE registering ompt event %d\n",event);
    } else {
        fprintf(stderr,"register_ompt_callback: SUCCESS registering ompt event %d\n",event);
    }
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

void CBTF_ompt_cb_parallel_region_begin (
  ompt_task_id_t parent_taskID,     /* parent task id */
  ompt_frame_t *parent_taskframe,   /* unused parent task frame data */
  ompt_parallel_id_t parallelID,    /* parallel region ID */
  void *context)                    /* context of parallel - function pointer */
{
    // do not sample if inside our tool
    cbtf_offline_service_stop_timer();
// record parent_taskID, parallel regionID.
// record start time and get callstack of this context
// increment is parallel flag.
	fprintf(stderr,
	"[%d] CBTF_ompt_cb_parallel_region_begin: parent_taskID = %lu,  parallelID = %lu, context = %#lx\n",
	omp_get_thread_num(), parent_taskID, parallelID, context);
    // resume collection
    cbtf_offline_service_start_timer();
}

void CBTF_ompt_cb_parallel_region_end (
  ompt_task_id_t parent_taskID,     /* parent task id */
  ompt_frame_t *parent_taskframe,   /* unused parent task frame data */
  ompt_parallel_id_t parallelID,    /* parallel region ID */
  void *context)                    /* context of parallel - function pointer */
{
    // do not sample if inside our tool
    cbtf_offline_service_stop_timer();
// record regionID
// if parallel flag, record stop time, decrement parallel flag.
	fprintf(stderr,
	"[%d] CBTF_ompt_cb_parallel_region_end: parent_taskID = %lu,  parallelID = %lu, context = %p\n",
	omp_get_thread_num(), parent_taskID, parallelID, context);
    // resume collection
    cbtf_offline_service_start_timer();
}

void CBTF_ompt_cb_task_begin (
  ompt_task_id_t parent_taskID,     /* parent task id */
  ompt_frame_t *parent_taskframe,   /* unused parent task frame data */
  ompt_task_id_t new_taskID,        /* new task id */
  void *context)                    /* context of task - function pointer */
{
// return if inside our tool
// record new_taskID as taskid.
	fprintf(stderr,"[%d] CBTF_ompt_cb_task_begin: new task: parent_taskID = %lu new_task_id = %lu, task_function = %p\n",
	omp_get_thread_num() ,parent_taskID, new_taskID, context);
// record start time and get callstack of this context
// increment is parallel flag.
// reset inside our tool
}

#if 0
void CBTF_ompt_cb_task_begin (ompt_task_id_t taskID)
{
// return if inside our tool
// stop time for this task.
// reset inside our tool
}
#endif

void CBTF_ompt_cb_thread_begin()
{
// monitor notifies us of this...
	fprintf(stderr, "[%d] CBTF_ompt_cb_thread_begin:\n",omp_get_thread_num());
// do anything here? start collecting?
}

void CBTF_ompt_cb_thread_end()
{
// monitor notifies us of this...
	fprintf(stderr, "[%d] CBTF_ompt_cb_thread_end:\n",omp_get_thread_num());
// do anything here? stop collecting?
}

void CBTF_ompt_cb_control(uint64_t command, uint64_t modifier)
{
// do anything here?
// printf per thread below...
	fprintf(stderr,"[%d] CBTF_ompt_cb_control: %llx, %llx\n",omp_get_thread_num(), command, modifier);
}

void CBTF_ompt_cb_runtime_shutdown()
{
// shut down of omp..
	fprintf(stderr, "[%d] CBTF_ompt_cb_runtime_shutdown:\n",omp_get_thread_num());
}

void CBTF_ompt_cb_wait_atomic (ompt_wait_id_t waitID) {
// return if inside our tool
// set wait, start time wait on wait name
	fprintf(stderr, "[%d] CBTF_ompt_cb_wait_atomic: waitID = %lu\n",omp_get_thread_num(),waitID);
// reset inside our tool
}

void CBTF_ompt_cb_wait_ordered (ompt_wait_id_t waitID) {
// return if inside our tool
// set wait, start time wait on wait name
	fprintf(stderr, "[%d] CBTF_ompt_cb_wait_ordered: waitID = %lu\n",omp_get_thread_num(),waitID);
// reset inside our tool
}

void CBTF_ompt_cb_wait_critical (ompt_wait_id_t waitID) {
// return if inside our tool
// set wait, start time wait on wait name
	fprintf(stderr, "[%d] CBTF_ompt_cb_wait_critical: waitID = %lu\n",omp_get_thread_num(),waitID);
// reset inside our tool
}

void CBTF_ompt_cb_wait_lock (ompt_wait_id_t waitID) {
// return if inside our tool
// set wait, start time wait on wait name
	fprintf(stderr, "[%d] CBTF_ompt_cb_wait_lock: waitID = %lu\n",omp_get_thread_num(),waitID);
// reset inside our tool
}

void CBTF_ompt_cb_aquired_atomic (ompt_wait_id_t waitID) {
// return if inside our tool
// if aquired, end time on wait name.
// clear wait, set aquired, start time on region name
	fprintf(stderr, "[%d] CBTF_ompt_cb_aquired_atomic: waitID = %lu\n",omp_get_thread_num(),waitID);
// reset inside our tool
}

void CBTF_ompt_cb_aquired_ordered (ompt_wait_id_t waitID) {
// return if inside our tool
// if aquired, end time on wait name.
// clear wait, set aquired, start time on region name
	fprintf(stderr, "[%d] CBTF_ompt_cb_aquired_ordered: waitID = %lu\n",omp_get_thread_num(),waitID);
// reset inside our tool
}

void CBTF_ompt_cb_aquired_critical (ompt_wait_id_t waitID) {
// return if inside our tool
// if aquired, end time on wait name.
// clear wait, set aquired, start time on region name
	fprintf(stderr, "[%d] CBTF_ompt_cb_aquired_critical: waitID = %lu\n",omp_get_thread_num(),waitID);
// reset inside our tool
}

void CBTF_ompt_cb_aquired_lock (ompt_wait_id_t waitID) {
// return if inside our tool
// if aquired, end time on wait name.
// clear wait, set aquired, start time on region name
	fprintf(stderr, "[%d] CBTF_ompt_cb_aquired_lock: waitID = %lu\n",omp_get_thread_num(),waitID);
// reset inside our tool
}

void CBTF_ompt_cb_release_atomic (ompt_wait_id_t waitID) {
// return if inside our tool
// if aquired, end time region name, clear aquired
	fprintf(stderr, "[%d] CBTF_ompt_cb_release_atomic: waitID = %lu\n",omp_get_thread_num(),waitID);
// reset inside our tool
}

void CBTF_ompt_cb_release_ordered (ompt_wait_id_t waitID) {
// return if inside our tool
// if aquired, end time region name, clear aquired
	fprintf(stderr, "[%d] CBTF_ompt_cb_release_ordered: waitID = %lu\n",omp_get_thread_num(),waitID);
// reset inside our tool
}

void CBTF_ompt_cb_release_critical (ompt_wait_id_t waitID) {
// return if inside our tool
// if aquired, end time region name, clear aquired
	fprintf(stderr, "[%d] CBTF_ompt_cb_release_critical: waitID = %lu\n",omp_get_thread_num(),waitID);
// reset inside our tool
}

void CBTF_ompt_cb_release_lock (ompt_wait_id_t waitID) {
// return if inside our tool
// if aquired, end time region name, clear aquired
	fprintf(stderr, "[%d] CBTF_ompt_cb_release_lock: waitID = %lu\n",omp_get_thread_num(),waitID);
// reset inside our tool
}


void CBTF_ompt_cb_barrier_begin (ompt_parallel_id_t parallelID,
		       ompt_task_id_t taskID)
{
// return if inside our tool
// set regionId to parallelID, set taskID
// record start time. 
	fprintf(stderr,"[%d] CBTF_ompt_cb_barrier_begin new: parallelID = %lu, taskID = %lu\n",omp_get_thread_num(),parallelID,taskID);
// reset inside our tool
}

void CBTF_ompt_cb_barrier_end (ompt_parallel_id_t parallelID,
		     ompt_task_id_t taskID)
{
// return if inside our tool
// set regionId to parallelID, set taskID
// record stop time.
	fprintf(stderr,"[%d] CBTF_ompt_cb_barrier_end: parallelID = %lu, taskID = %lu\n",omp_get_thread_num(),parallelID,taskID);
// reset inside our tool
}

void CBTF_ompt_cb_wait_barrier_begin (ompt_parallel_id_t parallelID,
		            ompt_task_id_t taskID)
{
// return if inside our tool
// set regionId to parallelID, set taskID
// record start time. 
	fprintf(stderr,"[%d] CBTF_ompt_cb_wait_barrier_begin new: parallelID = %lu, taskID = %lu\n",omp_get_thread_num(),parallelID,taskID);
// reset inside our tool
}

void CBTF_ompt_cb_wait_barrier_end (ompt_parallel_id_t parallelID,
			  ompt_task_id_t taskID)
{
// return if inside our tool
// set regionId to parallelID, set taskID
// record stop time.
	fprintf(stderr,"[%d] CBTF_ompt_cb_wait_barrier_end: parallelID = %lu, taskID = %lu\n",omp_get_thread_num(),parallelID,taskID);
// reset inside our tool
}

void CBTF_ompt_cb_implicit_task_begin (ompt_parallel_id_t parallelID,
		             ompt_task_id_t taskID)
{
// return if inside our tool
// set regionId to parallelID, set taskID
// record start time. 
	fprintf(stderr,"[%d] CBTF_ompt_cb_implicit_task_begin new: parallelID = %lu, taskID = %lu\n",omp_get_thread_num(),parallelID,taskID);
// reset inside our tool
}

void CBTF_ompt_cb_implicit_task_end (ompt_parallel_id_t parallelID,
			   ompt_task_id_t taskID)
{
// return if inside our tool
// set regionId to parallelID, set taskID
// record stop time.
	fprintf(stderr,"[%d] CBTF_ompt_cb_implicit_task_end: parallelID = %lu, taskID = %lu\n",omp_get_thread_num(),parallelID,taskID);
// reset inside our tool
}

void CBTF_ompt_cb_master_begin (ompt_parallel_id_t parallelID,
		      ompt_task_id_t taskID)
{
// return if inside our tool
// set regionId to parallelID, set taskID
// record start time. 
	fprintf(stderr,"[%d] CBTF_ompt_cb_master_begin new: parallelID = %lu, taskID = %lu\n",omp_get_thread_num(),parallelID,taskID);
// reset inside our tool
}

void CBTF_ompt_cb_master_end (ompt_parallel_id_t parallelID,
		    ompt_task_id_t taskID)
{
// return if inside our tool
// set regionId to parallelID, set taskID
// record stop time.
	fprintf(stderr,"[%d] CBTF_ompt_cb_master_end: parallelID = %lu, taskID = %lu\n",omp_get_thread_num(),parallelID,taskID);
// reset inside our tool
}

void CBTF_ompt_cb_taskwait_begin (ompt_parallel_id_t parallelID,
		        ompt_task_id_t taskID)
{
// return if inside our tool
// set regionId to parallelID, set taskID
// record start time. 
	fprintf(stderr,"[%d] CBTF_ompt_cb_taskwait_begin new: parallelID = %lu, taskID = %lu\n",omp_get_thread_num(),parallelID,taskID);
// reset inside our tool
}

void CBTF_ompt_cb_taskwait_end (ompt_parallel_id_t parallelID,
		      ompt_task_id_t taskID)
{
// return if inside our tool
// set regionId to parallelID, set taskID
// record stop time.
	fprintf(stderr,"[%d] CBTF_ompt_cb_taskwait_end: parallelID = %lu, taskID = %lu\n",omp_get_thread_num(),parallelID,taskID);
// reset inside our tool
}

void CBTF_ompt_cb_wait_taskwait_begin (ompt_parallel_id_t parallelID,
			     ompt_task_id_t taskID)
{
// return if inside our tool
// set regionId to parallelID, set taskID
// record start time. 
	fprintf(stderr,"[%d] CBTF_ompt_cb_wait_taskwait_begin new: parallelID = %lu, taskID = %lu\n",omp_get_thread_num(),parallelID,taskID);
// reset inside our tool
}

void CBTF_ompt_cb_wait_taskwait_end (ompt_parallel_id_t parallelID,
			   ompt_task_id_t taskID)
{
// return if inside our tool
// set regionId to parallelID, set taskID
// record stop time.
	fprintf(stderr,"[%d] CBTF_ompt_cb_wait_taskwait_end: parallelID = %lu, taskID = %lu\n",omp_get_thread_num(),parallelID,taskID);
// reset inside our tool
}

void CBTF_ompt_cb_taskgroup_begin (ompt_parallel_id_t parallelID,
			 ompt_task_id_t taskID)
{
// return if inside our tool
// set regionId to parallelID, set taskID
// record start time. 
	fprintf(stderr,"[%d] CBTF_ompt_cb_taskgroup_begin new: parallelID = %lu, taskID = %lu\n",omp_get_thread_num(),parallelID,taskID);
// reset inside our tool
}

void CBTF_ompt_cb_taskgroup_end (ompt_parallel_id_t parallelID,
		       ompt_task_id_t taskID)
{
// return if inside our tool
// set regionId to parallelID, set taskID
// record stop time.
	fprintf(stderr,"[%d] CBTF_ompt_cb_taskgroup_end: parallelID = %lu, taskID = %lu\n",omp_get_thread_num(),parallelID,taskID);
// reset inside our tool
}

void CBTF_ompt_cb_wait_taskgroup_begin (ompt_parallel_id_t parallelID,
			      ompt_task_id_t taskID)
{
// return if inside our tool
// set regionId to parallelID, set taskID
// record start time. 
	fprintf(stderr,"[%d] CBTF_ompt_cb_wait_taskgroup_begin new: parallelID = %lu, taskID = %lu\n",omp_get_thread_num(),parallelID,taskID);
// reset inside our tool
}

void CBTF_ompt_cb_wait_taskgroup_end (ompt_parallel_id_t parallelID,
			    ompt_task_id_t taskID)
{
// return if inside our tool
// set regionId to parallelID, set taskID
// record stop time.
	fprintf(stderr,"[%d] CBTF_ompt_cb_wait_taskgroup_end: parallelID = %lu, taskID = %lu\n",omp_get_thread_num(),parallelID,taskID);
// reset inside our tool
}

void CBTF_ompt_cb_single_others_begin (ompt_parallel_id_t parallelID,
			     ompt_task_id_t taskID)
{
// return if inside our tool
// set regionId to parallelID, set taskID
// record start time. 
	fprintf(stderr,"[%d] CBTF_ompt_cb_single_others_begin new: parallelID = %lu, taskID = %lu\n",omp_get_thread_num(),parallelID,taskID);
// reset inside our tool
}

void CBTF_ompt_cb_single_others_end (ompt_parallel_id_t parallelID,
			   ompt_task_id_t taskID)
{
// return if inside our tool
// set regionId to parallelID, set taskID
// record stop time.
	fprintf(stderr,"[%d] CBTF_ompt_cb_single_others_end: parallelID = %lu, taskID = %lu\n",omp_get_thread_num(),parallelID,taskID);
// reset inside our tool
}

void CBTF_ompt_cb_initial_task_begin (ompt_task_id_t taskID)
{
// return if inside our tool
// set taskID
// record start time. 
	fprintf(stderr,"[%d] CBTF_ompt_cb_single_others_begin new: taskID = %lu\n",omp_get_thread_num(),taskID);
// reset inside our tool
}

void CBTF_ompt_cb_initial_task_end (ompt_task_id_t taskID)
{
// return if inside our tool
// set taskID
// record stop time.
	fprintf(stderr,"[%d] CBTF_ompt_cb_single_others_end: taskID = %lu\n",omp_get_thread_num(),taskID);
// reset inside our tool
}

void CBTF_ompt_cb_loop_begin (ompt_parallel_id_t parallelID, ompt_task_id_t taskID)
{
// return if inside our tool
// set regionId to parallelID, set taskID
// record start time. 
	fprintf(stderr,"[%d] CBTF_ompt_cb_loop_begin new: parallelID = %lu, taskID = %lu\n",omp_get_thread_num(), parallelID, taskID);
// reset inside our tool
}

void CBTF_ompt_cb_loop_end (ompt_parallel_id_t parallelID, ompt_task_id_t taskID)
{
// return if inside our tool
// set regionId to parallelID, set taskID
// record stop time.
	fprintf(stderr,"[%d] CBTF_ompt_cb_loop_end: parallelID = %lu, taskID = %lu\n",omp_get_thread_num(),parallelID,taskID);
// reset inside our tool
}

void CBTF_ompt_cb_single_in_block_begin (ompt_parallel_id_t parallelID, ompt_task_id_t taskID)
{
// return if inside our tool
// set regionId to parallelID, set taskID
// record start time. 
	fprintf(stderr,"[%d] CBTF_ompt_cb_single_in_block_begin new: parallelID = %lu, taskID = %lu\n",omp_get_thread_num(),parallelID,taskID);
// reset inside our tool
}

void CBTF_ompt_cb_single_in_block_end (ompt_parallel_id_t parallelID, ompt_task_id_t taskID)
{
// return if inside our tool
// set regionId to parallelID, set taskID
// record stop time.
	fprintf(stderr,"[%d] CBTF_ompt_cb_single_in_block_end: parallelID = %lu, taskID = %lu\n",omp_get_thread_num(),parallelID,taskID);
// reset inside our tool
}

void CBTF_ompt_cb_sections_begin (ompt_parallel_id_t parallelID, ompt_task_id_t taskID)
{
// return if inside our tool
// set regionId to parallelID, set taskID
// record start time. 
	fprintf(stderr,"[%d] CBTF_ompt_cb_sections_begin new: parallelID = %lu, taskID = %lu\n",omp_get_thread_num(),parallelID,taskID);
// reset inside our tool
}

void CBTF_ompt_cb_sections_end (ompt_parallel_id_t parallelID, ompt_task_id_t taskID)
{
// return if inside our tool
// set regionId to parallelID, set taskID
// record stop time.
	fprintf(stderr,"[%d] CBTF_ompt_cb_sections_end: parallelID = %lu, taskID = %lu\n",omp_get_thread_num(),parallelID,taskID);
// reset inside our tool
}


void CBTF_ompt_cb_workshare_begin (ompt_parallel_id_t parallelID, ompt_task_id_t taskID, void *context)

{
// return if inside our tool
// set regionId to parallelID, set taskID?
// get callstack for context.
// record start time. 
	fprintf(stderr,"[%d] CBTF_ompt_cb_workshare_begin new: parallelID = %lu, taskID = %lu\n",omp_get_thread_num(),parallelID,taskID);
// reset inside our tool
}

void CBTF_ompt_cb_workshare_end (ompt_parallel_id_t parallelID, ompt_task_id_t taskID)
{
// return if inside our tool
// set regionId to parallelID, set taskID
// record stop time.
	fprintf(stderr,"[%d] CBTF_ompt_cb_workshare_end: parallelID = %lu, taskID = %lu\n",omp_get_thread_num(),parallelID,taskID);
// reset inside our tool
}

void CBTF_ompt_cb_idle_begin()
{
// return if inside our tool
	fprintf(stderr,"[%d] CBTF_ompt_cb_idle_begin:\n",omp_get_thread_num());
// if parallel count is 0,
//    if idle and not busy, reset inside our tool and return
//    if busy then stop time for tid, unset busy.
// set idle for tid and start time.
// reset inside our tool
}

void CBTF_ompt_cb_idle_end()
{
// return if inside our tool
	fprintf(stderr,"[%d] CBTF_ompt_cb_idle_end:\n",omp_get_thread_num());
// stop any collecting for tid.
// if parallel count is 0, start time for new parallel region and set busy this tid.
// else unset busy for this tid
// reset inside our tool
}

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
    CBTF_register_ompt_callback(ompt_event_task_begin,CBTF_ompt_cb_task_begin);
    CBTF_register_ompt_callback(ompt_event_thread_begin,CBTF_ompt_cb_thread_begin);
    CBTF_register_ompt_callback(ompt_event_thread_end,CBTF_ompt_cb_thread_end);
    CBTF_register_ompt_callback(ompt_event_control,CBTF_ompt_cb_control);
    CBTF_register_ompt_callback(ompt_event_runtime_shutdown,CBTF_ompt_cb_runtime_shutdown);
    CBTF_register_ompt_callback(ompt_event_wait_atomic,CBTF_ompt_cb_wait_atomic);
    CBTF_register_ompt_callback(ompt_event_wait_ordered,CBTF_ompt_cb_wait_ordered);
    CBTF_register_ompt_callback(ompt_event_wait_critical,CBTF_ompt_cb_wait_critical);
    CBTF_register_ompt_callback(ompt_event_wait_lock,CBTF_ompt_cb_wait_lock);
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
    fprintf(stderr, "CBTF OMPT Init: Registering callbacks\n");
    return 1;
}
