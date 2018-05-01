/*******************************************************************************
** Copyright (c) 2017 The Krell Institute. All Rights Reserved.
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
#include <unistd.h>

#include "omp.h"     // omp_get_thread_num
#include "ompt.h"
#include "KrellInstitute/Services/Assert.h"
#include "KrellInstitute/Services/Collector.h"
#include "KrellInstitute/Services/Ompt.h"
#include "monitor.h" // monitor_get_thread_num

/* FIXME: This exists in services/collector/collector.c.  need include. */
//extern void cbtf_collector_set_openmp_threadid(int32_t omptid);

static ompt_set_callback_t ompt_set_callback;
static ompt_get_task_info_t ompt_get_task_info;
static ompt_get_thread_data_t ompt_get_thread_data;
static ompt_get_parallel_info_t ompt_get_parallel_info;
static ompt_get_unique_id_t ompt_get_unique_id;
static ompt_enumerate_states_t ompt_enumerate_states;

static int cbtf_ompt_debug = 0;
static int cbtf_ompt_debug_details = 0;
static int cbtf_ompt_debug_blame = 0;
static int cbtf_ompt_debug_trace = 0;
static int cbtf_ompt_debug_wait = 0;

static ompt_get_thread_data_t ompt_get_thread_data;

static void print_ids(int level)
{
  ompt_frame_t* frame ;
  ompt_data_t* parallel_data;
  ompt_data_t* task_data;
  int exists_task = ompt_get_task_info(level, NULL, &task_data, &frame, &parallel_data, NULL);
  if (frame)
  {
    printf("%" PRIu64 ": task level %d: parallel_id=%" PRIu64 ", task_id=%" PRIu64 ", exit_frame=%p, reenter_frame=%p\n", ompt_get_thread_data()->value, level, exists_task ? parallel_data->value : 0, exists_task ? task_data->value : 0, frame->exit_frame, frame->enter_frame);
  }
  else
    printf("%" PRIu64 ": task level %d: parallel_id=%" PRIu64 ", task_id=%" PRIu64 ", frame=%p\n", ompt_get_thread_data()->value, level, exists_task ? parallel_data->value : 0, exists_task ? task_data->value : 0, frame);
}

// these external calls are expected in the cbtf collectors either
// as implementations or as empty functions.
//
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
extern void OMPT_THREAD_BARRIER(bool);
extern void OMPT_THREAD_WAIT_BARRIER(bool);
extern void OMPT_THREAD_IDLE(bool);

// local thread variables to record various details. May need a
// container for this later...
#ifndef NDEBUG
__thread uint64_t barrier_btime;
__thread uint64_t barrier_ttime;
__thread uint64_t lock_count;
__thread uint64_t lock_btime;
__thread uint64_t lock_time;
#endif

// OMPT 50

#define print_frame(level)\
do {\
  printf("%" PRIu64 ": __builtin_frame_address(%d)=%p\n", ompt_get_thread_data()->value, level, __builtin_frame_address(level));\
} while(0)

#define print_current_address(id)\
asm ("nop"); /* provide an instruction as jump target (compiler would insert an instruction if label is target of a jmp ) */ \
ompt_label_##id:\
    printf("%" PRIu64 ": current_address=%p or %p\n", ompt_get_thread_data()->value, (char*)(&& ompt_label_##id)-1, (char*)(&& ompt_label_##id)-4) 
    /* "&& label" returns the address of the label (GNU extension); works with gcc, clang, icc */
    /* for void-type runtime function, the label is after the nop (-1), for functions with return value, there is a mov instruction before the label (-4) */

static void
CBTF_ompt_callback_parallel_begin(
  ompt_data_t *parent_task_data,
  const ompt_frame_t *parent_task_frame,
  ompt_data_t* parallel_data,
  uint32_t requested_team_size,
  ompt_invoker_t invoker,
  const void *codeptr_ra)
{
  if(parallel_data && parallel_data->ptr) {
#ifndef NDEBUG
    if (cbtf_ompt_debug) {
	fprintf(stderr,"%s\n", "0: parallel_data initially not null");
    }
#endif
  }
  parallel_data->value = ompt_get_unique_id();
#ifndef NDEBUG
  if (cbtf_ompt_debug) {
	fprintf(stderr,"[%d,%d] CBTF_ompt_callback_parallel_begin parallel_id:%lu requested_team_size:%u invoker:%u codeptr_ra:%p\n"
	,getpid(),monitor_get_thread_num(),(parallel_data)?parallel_data->value:0, requested_team_size, invoker, codeptr_ra);
#endif
  }
}

static void
CBTF_ompt_callback_parallel_end(
  ompt_data_t *parallel_data,
  ompt_data_t *task_data,
  ompt_invoker_t invoker,
  const void *codeptr_ra)
{
#ifndef NDEBUG
    if (cbtf_ompt_debug) {
	fprintf(stderr,"[%d,%d] CBTF_ompt_callback_parallel_end parallel_id:%lu task_id:%lu invoker:%u codeptr_ra:%p\n"
	,getpid(),monitor_get_thread_num(),(parallel_data)?parallel_data->value:0, task_data->value, invoker, codeptr_ra);
    }
#endif
}

// ompt_event_MAY_ALWAYS
// monitor notifies us of this...
// pass omp thread num to collector.
void CBTF_ompt_callback_thread_begin()
{
#ifndef NDEBUG
    lock_count = 0;
    lock_btime = 0;
    lock_time = 0;
    barrier_ttime = 0;
    if (cbtf_ompt_debug) {
	fprintf(stderr, "[%d,%d] CBTF_ompt_callback_thread_begin: omp tid:%d\n",getpid(),monitor_get_thread_num(),omp_get_thread_num());
    }
#endif
    cbtf_collector_set_openmp_threadid(omp_get_thread_num());
    set_ompt_thread_finished(false);
    set_ompt_flag(true);
}

// ompt_event_MAY_ALWAYS
// monitor notifies us of this as well...
void CBTF_ompt_callback_thread_end()
{
#ifndef NDEBUG
    if (cbtf_ompt_debug) {
	fprintf(stderr, "[%d,%d] CBTF_ompt_callback_thread_end: barrier time:%f, lock count:%lu lock_time:%f\n"
	,getpid(),monitor_get_thread_num(), (float)barrier_ttime/1000000000, lock_count, (float)lock_time/1000000000);
    }
#endif
    set_ompt_thread_finished(true);
    cbtf_timer_service_stop_sampling(NULL);
    set_ompt_flag(false);
}

void CBTF_ompt_callback_sync_region(
  ompt_sync_region_kind_t kind,
  ompt_scope_endpoint_t endpoint,
  ompt_data_t *parallel_data,
  ompt_data_t *task_data,
  const void *codeptr_ra)
{
  struct timespec now;
  switch(endpoint) {
    case ompt_scope_begin:

	switch(kind) {
	  case ompt_sync_region_barrier:
		OMPT_THREAD_BARRIER(true);
		// record barrier begin time.
		Assert(clock_gettime(CLOCK_REALTIME, &now) == 0);
		barrier_btime = ((uint64_t)(now.tv_sec) * (uint64_t)(1000000000)) +
        			(uint64_t)(now.tv_nsec);
#ifndef NDEBUG
	    if (cbtf_ompt_debug_blame) {
		//print_ids(0);
		fprintf(stderr,"[%d,%d] CBTF_ompt_callback_sync_region barrier_begin parallel_id:%lu task_id:%lu codeptr_ra:%p\n"
		,getpid(),monitor_get_thread_num(),(parallel_data)?parallel_data->value:0, task_data->value, codeptr_ra);
	    }
#endif
	  break;
	  case ompt_sync_region_taskwait:
#ifndef NDEBUG
	    if (cbtf_ompt_debug_blame) {
		fprintf(stderr,"%" PRIu64 ": ompt_event_taskwait_begin: parallel_id=%" PRIu64 ", task_id=%" PRIu64 ", codeptr_ra=%p\n",
		ompt_get_thread_data()->value, parallel_data->value, task_data->value, codeptr_ra);
	    }
#endif
	  break;
	  case ompt_sync_region_taskgroup:
#ifndef NDEBUG
	    if (cbtf_ompt_debug_blame) {
		fprintf(stderr,"%" PRIu64 ": ompt_event_taskgroup_begin: parallel_id=%" PRIu64 ", task_id=%" PRIu64 ", codeptr_ra=%p\n",
		ompt_get_thread_data()->value, parallel_data->value, task_data->value, codeptr_ra);
	    }
#endif
	  break;
	}
	break;

    case ompt_scope_end:
	switch(kind) {
	  case ompt_sync_region_barrier:
		OMPT_THREAD_BARRIER(false);
#ifndef NDEBUG
	    if (cbtf_ompt_debug_blame) {
		Assert(clock_gettime(CLOCK_REALTIME, &now) == 0);
		uint64_t etime = ((uint64_t)(now.tv_sec) * (uint64_t)(1000000000)) +
			(uint64_t)(now.tv_nsec);
		barrier_ttime += (etime - barrier_btime);
		fprintf(stderr,"[%d,%d] CBTF_ompt_callback_sync_region barrier_end parallel_id:%lu task_id:%lu codeptr_ra:%p\n"
		,getpid(),monitor_get_thread_num(),(parallel_data)?parallel_data->value:0, task_data->value, codeptr_ra);
	    }
#endif
	  break;
	  case ompt_sync_region_taskwait:
#ifndef NDEBUG
	    if (cbtf_ompt_debug_blame) {
		fprintf(stderr,"%" PRIu64 ": ompt_event_taskwait_end: parallel_id=%" PRIu64 ", task_id=%" PRIu64 ", codeptr_ra=%p\n",
		ompt_get_thread_data()->value, (parallel_data)?parallel_data->value:0, task_data->value, codeptr_ra);
	    }
#endif
	  break;
	  case ompt_sync_region_taskgroup:
#ifndef NDEBUG
	    if (cbtf_ompt_debug_blame) {
		fprintf(stderr,"%" PRIu64 ": ompt_event_taskgroup_end: parallel_id=%" PRIu64 ", task_id=%" PRIu64 ", codeptr_ra=%p\n",
		ompt_get_thread_data()->value, (parallel_data)?parallel_data->value:0, task_data->value, codeptr_ra);
	    }
#endif
          break;
	}
	break;
  }
}


// WAIT
void CBTF_ompt_callback_sync_region_wait(
  ompt_sync_region_kind_t kind,
  ompt_scope_endpoint_t endpoint,
  ompt_data_t *parallel_data,
  ompt_data_t *task_data,
  const void *codeptr_ra)
{
    switch(endpoint) {
    case ompt_scope_begin:
	switch(kind) { 
	case ompt_sync_region_barrier:
            OMPT_THREAD_WAIT_BARRIER(true);
#ifndef NDEBUG
	    if (cbtf_ompt_debug_blame) {
		fprintf(stderr,"[%d,%d] CBTF_ompt_callback_sync_region_wait wait_barrier_begin parallel_id:%lu task_id:%lu codeptr_ra:%p\n"
		,getpid(),monitor_get_thread_num(),(parallel_data)?parallel_data->value:0, task_data->value, codeptr_ra);
	    }
#endif
	    break;
        case ompt_sync_region_taskwait:
#ifndef NDEBUG
	    if (cbtf_ompt_debug_blame) {
		fprintf(stderr,"%" PRIu64 ": ompt_event_wait_taskwait_begin: parallel_id=%" PRIu64 ", task_id=%" PRIu64 ", codeptr_ra=%p\n",
		ompt_get_thread_data()->value, parallel_data->value, task_data->value, codeptr_ra);
	    }
#endif
	    break;
        case ompt_sync_region_taskgroup:
#ifndef NDEBUG
	    if (cbtf_ompt_debug_blame) {
		fprintf(stderr,"%" PRIu64 ": ompt_event_wait_taskgroup_begin: parallel_id=%" PRIu64 ", task_id=%" PRIu64 ", codeptr_ra=%p\n",
		ompt_get_thread_data()->value, parallel_data->value, task_data->value, codeptr_ra);
	    }
#endif
	    break;
	}
	break;
    case ompt_scope_end:
	switch(kind) { 
        case ompt_sync_region_barrier:
            OMPT_THREAD_WAIT_BARRIER(false);
#ifndef NDEBUG
	    if (cbtf_ompt_debug_blame) {
		fprintf(stderr,"[%d,%d] CBTF_ompt_callback_sync_region_wait wait_barrier_end parallel_id:%ld task_id:%ld codeptr_ra:%p\n"
		,getpid(),monitor_get_thread_num(),(parallel_data)?parallel_data->value:0, task_data->value, codeptr_ra);
	    }
#endif
	    break;
        case ompt_sync_region_taskwait:
#ifndef NDEBUG
	    if (cbtf_ompt_debug_blame) {
		fprintf(stderr,"%" PRIu64 ": ompt_event_wait_taskwait_end: parallel_id=%" PRIu64 ", task_id=%" PRIu64 ", codeptr_ra=%p\n",
		ompt_get_thread_data()->value, (parallel_data)?parallel_data->value:0, task_data->value, codeptr_ra);
	    }
#endif
	    break;
        case ompt_sync_region_taskgroup:
#ifndef NDEBUG
	    if (cbtf_ompt_debug_blame) {
		fprintf(stderr,"%" PRIu64 ": ompt_event_wait_taskgroup_end: parallel_id=%" PRIu64 ", task_id=%" PRIu64 ", codeptr_ra=%p\n",
		ompt_get_thread_data()->value, (parallel_data)?parallel_data->value:0, task_data->value, codeptr_ra);
	    }
#endif
	    break;
	}
	break;
    }
}


// ompt_event_MAY_ALWAYS_BLAME
void CBTF_ompt_callback_idle(ompt_scope_endpoint_t endpoint)
{
  switch(endpoint)
  { 
    case ompt_scope_begin:
	OMPT_THREAD_IDLE(true);
#ifndef NDEBUG
	if (cbtf_ompt_debug_blame) {
	    fprintf(stderr,"[%d,%d] CBTF_ompt_callback_idle begin\n" ,getpid(),monitor_get_thread_num());
	}
#endif
	break;
    case ompt_scope_end:
	OMPT_THREAD_IDLE(false);
#ifndef NDEBUG
	if (cbtf_ompt_debug_blame) {
	    fprintf(stderr,"[%d,%d] CBTF_ompt_callback_idle end\n" ,getpid(),monitor_get_thread_num());
	}
#endif
	break;
  }
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
void
CBTF_ompt_callback_implicit_task(
    ompt_scope_endpoint_t endpoint,
    ompt_data_t *parallel_data,
    ompt_data_t *task_data,
    unsigned int team_size,
    unsigned int thread_num)
{
    switch(endpoint) {
    case ompt_scope_begin:
	if(task_data->ptr) {
	    fprintf(stderr, "%s\n", "0: task_data initially not null");
	}
	task_data->value = ompt_get_unique_id();
#ifndef NDEBUG
	if (cbtf_ompt_debug_blame) {
	    fprintf(stderr,"[%d,%d] ompt_event_implicit_task_begin: parallel_id:%lu task_id:%lu team_size:%u thread_num:%u\n"
	    ,getpid(),monitor_get_thread_num(), (parallel_data)?parallel_data->value:0, task_data->value, team_size, thread_num);
	}
#endif
	break;

    case ompt_scope_end:
#ifndef NDEBUG
	if (cbtf_ompt_debug_trace) {
	    fprintf(stderr,"[%d,%d] ompt_event_implicit_task_end: parallel_id:%lu task_id:%lu team_size:%u thread_num:%u\n"
	    ,getpid(),monitor_get_thread_num(), (parallel_data)?parallel_data->value:0, task_data->value, team_size, thread_num);
	}
#endif
	break;
    }
}

static int
CBTF_ompt_callback_control_tool(
  uint64_t command,
  uint64_t modifier,
  void *arg,
  const void *codeptr_ra)
{
  ompt_frame_t* omptTaskFrame;
  ompt_get_task_info(0, NULL, (ompt_data_t**) NULL, &omptTaskFrame, NULL, NULL);
#ifndef NDEBUG
  if (cbtf_ompt_debug_blame) {
    fprintf(stderr,"%" PRIu64 ": ompt_event_control_tool: command=%" PRIu64 ", modifier=%" PRIu64 ", arg=%p, codeptr_ra=%p\n", ompt_get_thread_data()->value, command, modifier, arg, codeptr_ra);
  }
#endif
  return 0; //success
}

#define register_callback_t(name, type)                       \
do{                                                           \
  type f_##name = &CBTF_##name;                                 \
  if (ompt_set_callback(name, (ompt_callback_t)f_##name) ==   \
      ompt_set_never)                                         \
    printf("0: Could not register callback '" #name "'\n");   \
}while(0)

#define register_callback(name) register_callback_t(name, name##_t)

// initialize our ompt services.
// For the ompt services we wish to annotate sampling collectors
// with idle,wait,barrier samples.  ie we flag as sample if it
// is taking while in one of these states.
//
// NOTE:  as of openMP 50, the tool is started via ompt_start_tool.
int ompt_initialize(
  ompt_function_lookup_t lookup,
  ompt_data_t *tool_data)
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
	fprintf(stderr, "CBTF OMPT Init: Registering callbacks\n");
    }
#endif

  ompt_set_callback = (ompt_set_callback_t) lookup("ompt_set_callback");
  ompt_get_task_info = (ompt_get_task_info_t) lookup("ompt_get_task_info");
  ompt_get_thread_data = (ompt_get_thread_data_t) lookup("ompt_get_thread_data");
  ompt_get_parallel_info = (ompt_get_parallel_info_t) lookup("ompt_get_parallel_info");
  ompt_get_unique_id = (ompt_get_unique_id_t) lookup("ompt_get_unique_id");
  
  ompt_enumerate_states = (ompt_enumerate_states_t) lookup("ompt_enumerate_states");

  register_callback(ompt_callback_idle);
  register_callback(ompt_callback_sync_region);
  register_callback_t(ompt_callback_sync_region_wait, ompt_callback_sync_region_t);
  register_callback(ompt_callback_control_tool);
  register_callback(ompt_callback_implicit_task);
  register_callback(ompt_callback_parallel_begin);
  register_callback(ompt_callback_parallel_end);
  register_callback(ompt_callback_thread_begin);
  register_callback(ompt_callback_thread_end);

#ifndef NDEBUG
  if (cbtf_ompt_debug_details) {
    fprintf(stderr, "CBTF OMPT SERVICE Init: FINISHED Registering callbacks\n");
  }
#endif
  return 1; //success
}

void ompt_finalize(ompt_data_t *tool_data)
{
#ifndef NDEBUG
  if (cbtf_ompt_debug_details) {
    fprintf(stderr,"%d,%d: ompt_event_runtime_shutdown\n",getpid(),monitor_get_thread_num());
  }
#endif
}

#if defined(INIT_AS_OMPT50_TOOL)
ompt_start_tool_result_t* ompt_start_tool(
  unsigned int omp_version,
  const char *runtime_version)
{
  static ompt_start_tool_result_t ompt_start_tool_result = {&ompt_initialize,&ompt_finalize, 0};
  return &ompt_start_tool_result;
}
#endif
