/*******************************************************************************
** Copyright (c) 2015-2016  The Krell Institute. All Rights Reserved.
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
#include <sys/types.h>
#include <unistd.h>  // getpid()

#include "collector.h"
#include "omp.h"     // omp_get_thread_num
#include "ompt.h"
#include "monitor.h" // monitor_get_thread_num
#include "KrellInstitute/Services/Assert.h"
#include "KrellInstitute/Services/Collector.h"
#include "KrellInstitute/Services/Common.h"
#include "KrellInstitute/Services/Ompt.h"
#include "KrellInstitute/Services/Time.h"
#include "KrellInstitute/Services/TLS.h"
#include "KrellInstitute/Messages/Ompt_data.h"

static ompt_set_callback_t ompt_set_callback;
static ompt_get_task_info_t ompt_get_task_info;
static ompt_get_thread_data_t ompt_get_thread_data;
static ompt_get_parallel_info_t ompt_get_parallel_info;
static ompt_get_unique_id_t ompt_get_unique_id;
static ompt_get_num_places_t ompt_get_num_places;
static ompt_get_place_proc_ids_t ompt_get_place_proc_ids;
static ompt_get_place_num_t ompt_get_place_num;
static ompt_get_partition_place_nums_t ompt_get_partition_place_nums;
static ompt_get_proc_id_t ompt_get_proc_id;
static ompt_enumerate_states_t ompt_enumerate_states;
static ompt_enumerate_mutex_impls_t ompt_enumerate_mutex_impls;

static ompt_get_thread_data_t ompt_get_thread_data;

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
  for (i = 0; i < size; i++) {
	fprintf (stderr,"%" PRIu64 ": [%u] %p:  %s\n", ompt_get_thread_data()->value, i, array[i], strings[i]);
  }

}

#define print_frame(level)\
do {\
  printf("%" PRIu64 ": __builtin_frame_address(%d)=%p\n", ompt_get_thread_data()->value, level, __builtin_frame_address(level));\
} while(0)

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

/* debug flags */
#ifndef NDEBUG
static bool cbtf_ompt_debug = false;
static bool cbtf_ompt_debug_details = false;
static bool cbtf_ompt_debug_blame = false;
static bool cbtf_ompt_debug_trace = false;
static bool cbtf_ompt_debug_wait = false;
static bool cbtf_ompt_debug_lock = false;
#endif

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
int ompt_callback_init_tls_done = 0;

#else

/** Thread-local storage. */
static __thread TLS the_tls;

#endif

void CBTF_ompt_set_collector_active(bool flag)
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
    if (tls == NULL) {
        tls = malloc(sizeof(TLS));
        Assert(tls != NULL);
        CBTF_SetTLS(TLSKey, tls);
        ompt_callback_init_tls_done = 1;
    }
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);
    tls->collector_active=flag;
    tls->collector_etime=CBTF_GetTime();
#ifndef NDEBUG
    //if ( (getenv("CBTF_DEBUG_COLLECTOR") != NULL)) {
    if (cbtf_ompt_debug) {
	fprintf(stderr, "[%d,%d] CBTF_ompt_set_collector_active flag:%d\n",getpid(),monitor_get_thread_num(),flag);
    }
#endif

}


// ompt_event_MAY_ALWAYS
// record start time and get address or callstack of this context
// This seems to only be active in the master thread.
// increment is parallel flag. This flag indicates that a
// parallel region is active to other callbacks that may be
// interested.
void CBTF_ompt_callback_parallel_begin(
  ompt_data_t *parent_task_data,
  const ompt_frame_t *parent_taskframe,
  ompt_data_t* parallel_data,
  uint32_t requested_team_size,
  ompt_invoker_t invoker,
  const void *parallel_function)
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
    parallel_data->value = ompt_get_unique_id();
    int nest_level = CBTF_omptp_region_add(parallel_data->value, stacktrace, stacktrace_size);

#ifndef NDEBUG
    if (cbtf_ompt_debug) {
	fprintf(stderr,
	"[%d,%d] CBTF_ompt_callback_parallel_begin parallelID:%lu parent_taskID:%lu req_team_size:%u invoker:%d context:%p nest_level=%d\n",
	getpid(),monitor_get_thread_num(), parallel_data->value, parent_task_data->value, requested_team_size, invoker, parallel_function, nest_level);
    }
#endif
}


// ompt_event_MAY_ALWAYS
void CBTF_ompt_callback_parallel_end(
  ompt_data_t *parallel_data,
  ompt_data_t *task_data,
  ompt_invoker_t invoker,
  const void *codeptr_ra)
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
	"[%d,%d] CBTF_ompt_callback_parallel_end parallelID:%lu parent_taskID:%lu invoker:%d context:%lx total time:%f\n",
	getpid(),monitor_get_thread_num(), parallel_data->value, task_data->value, invoker, current_region_context, (float)tls->region_ttime/1000000000);
    }
#endif

    current_region_context = (uint64_t)NULL;

    CBTF_omptp_region_clear(parallel_data->value);
}

// ompt_event_MAY_ALWAYS
// monitor notifies us of this...
// pass omp thread num to collector.
void CBTF_ompt_callback_thread_begin()
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
    if (tls == NULL) {
        tls = malloc(sizeof(TLS));
        Assert(tls != NULL);
        CBTF_SetTLS(TLSKey, tls);
        ompt_callback_init_tls_done = 1;
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
	fprintf(stderr, "[%d,%d] CBTF_ompt_callback_thread_begin:%ld\n",getpid(),monitor_get_thread_num(),ompt_get_thread_data()->value);
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
	fprintf(stderr, "[%d,%d] CBTF_ompt_callback_thread_end TIMES: thread:%f region:%f itask:%f serial:%f barrier:%f wait_barrier:%f idle:%f\n",
	    getpid(),monitor_get_thread_num(),
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

    set_ompt_thread_finished(true);
    cbtf_timer_service_stop_sampling(NULL);
    set_ompt_flag(false);
}

// ompt_event_MAY_ALWAYS
void CBTF_ompt_callback_control(uint64_t command, uint64_t modifier)
{
#ifndef NDEBUG
    if (cbtf_ompt_debug_details) {
	fprintf(stderr,"[%d,%d] CBTF_ompt_callback_control: %lu, %lu\n",getpid(),monitor_get_thread_num(), command, modifier);
    }
#endif
}

// ompt_event_MAY_ALWAYS
// shut down of ompt ...
void CBTF_ompt_callback_runtime_shutdown()
{
#ifndef NDEBUG
    if (cbtf_ompt_debug_details) {
	fprintf(stderr, "[%d,%d] CBTF_ompt_callback_runtime_shutdown:\n",getpid(),monitor_get_thread_num());
    }
#endif
}

// ompt_event_UNIMPLEMENTED
// if acquired, end time on wait name.
// clear wait, set acquired, start time on region name
void CBTF_ompt_callback_acquired_lock (ompt_wait_id_t *waitID) {
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
	fprintf(stderr, "[%ld] CBTF_ompt_callback_acquired_lock: waitID:%p\n",ompt_get_thread_data()->value,waitID);
#endif
}

// ompt_event_MAY_ALWAYS_BLAME
// if acquired, end time region name, clear acquired
void CBTF_ompt_callback_release_lock (ompt_wait_id_t *waitID) {
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);

#ifndef NDEBUG
    if (cbtf_ompt_debug_trace) {
	fprintf(stderr, "[%ld] CBTF_ompt_callback_release_lock: waitID:%p\n",ompt_get_thread_data()->value,waitID);
    }
#endif

    tls->lock_time += CBTF_GetTime() - tls->lock_btime;
}

// ompt_event_MAY_ALWAYS_TRACE but also seems used as BLAME??
// send notification to collector that a barrier has been entered.
// BLAME
/**
 * The OpenMP runtime system invokes this callback before an implicit task
 * begins execution of a barrier region. This callback executes in the context
 * of the implicit task that encountered the barrier construct.
 */
void
CBTF_ompt_callback_sync_region(
  ompt_sync_region_kind_t kind,
  ompt_scope_endpoint_t endpoint,
  ompt_data_t *parallel_data,
  ompt_data_t *task_data,
  const void *codeptr_ra)
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);


  switch(endpoint)
  {
      case ompt_scope_begin:
      switch(kind)
      {
        case ompt_sync_region_barrier:
	    // record barrier begin time.
	    tls->barrier_btime = CBTF_GetTime();
#ifndef NDEBUG
	    if (cbtf_ompt_debug_blame) {
		fprintf(stderr,"[%d,%d] CBTF_ompt_callback_barrier_begin parallelID:%lu taskID:%lu context:%lx\n"
		,getpid(),monitor_get_thread_num(), (parallel_data)?parallel_data->value:0, task_data->value, current_region_context);
	    }
	    if (cbtf_ompt_debug_details) {
		fprintf(stderr,"%" PRIu64 ": ompt_event_barrier_begin: parallel_id=%" PRIu64 ", task_id=%" PRIu64 ", codeptr_ra=%p\n",
		ompt_get_thread_data()->value, (parallel_data)?parallel_data->value:0, task_data->value, codeptr_ra);
		print_ids(0);
	    }
#endif
          break;
        case ompt_sync_region_taskwait:
#ifndef NDEBUG
	    if (cbtf_ompt_debug_details) {
		fprintf(stderr,"%" PRIu64 ": ompt_event_taskwait_begin: parallel_id=%" PRIu64 ", task_id=%" PRIu64 ", codeptr_ra=%p\n",
		ompt_get_thread_data()->value, (parallel_data)?parallel_data->value:0, task_data->value, codeptr_ra);
	    }
#endif
          break;
        case ompt_sync_region_taskgroup:
#ifndef NDEBUG
	    if (cbtf_ompt_debug_details) {
		fprintf(stderr,"%" PRIu64 ": ompt_event_taskgroup_begin: parallel_id=%" PRIu64 ", task_id=%" PRIu64 ", codeptr_ra=%p\n",
		ompt_get_thread_data()->value, (parallel_data)?parallel_data->value:0, task_data->value, codeptr_ra);
	    }
#endif
          break;
      }
      break;

      case ompt_scope_end:

      switch(kind)
      {
        case ompt_sync_region_barrier:
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
		fprintf(stderr,"[%d,%d] CBTF_ompt_callback_barrier_end: parallelID:%lu taskID:%lu context:%lx barrier_time:%f\n"
		,getpid(),monitor_get_thread_num(),(parallel_data)?parallel_data->value:0, task_data->value, current_region_context, (float)t/1000000000);
	    }
	    if (cbtf_ompt_debug_details) {
		fprintf(stderr,"%" PRIu64 ": ompt_event_barrier_end: parallel_id=%" PRIu64 ", task_id=%" PRIu64 ", codeptr_ra=%p\n",
		ompt_get_thread_data()->value, (parallel_data)?parallel_data->value:0, task_data->value, codeptr_ra);
	    }
#endif
          break;
        case ompt_sync_region_taskwait:
#ifndef NDEBUG
	    if (cbtf_ompt_debug_details) {
		fprintf(stderr,"%" PRIu64 ": ompt_event_taskwait_end: parallel_id=%" PRIu64 ", task_id=%" PRIu64 ", codeptr_ra=%p\n",
		ompt_get_thread_data()->value, (parallel_data)?parallel_data->value:0, task_data->value, codeptr_ra);
	    }
#endif
          break;
        case ompt_sync_region_taskgroup:
#ifndef NDEBUG
	    if (cbtf_ompt_debug_details) {
		fprintf(stderr,"%" PRIu64 ": ompt_event_taskgroup_end: parallel_id=%" PRIu64 ", task_id=%" PRIu64 ", codeptr_ra=%p\n",
		ompt_get_thread_data()->value, (parallel_data)?parallel_data->value:0, task_data->value, codeptr_ra);
	    }
#endif
          break;
      }
      break;
  }
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
void CBTF_ompt_callback_sync_region_wait(
  ompt_sync_region_kind_t kind,
  ompt_scope_endpoint_t endpoint,
  ompt_data_t *parallel_data,
  ompt_data_t *task_data,
  const void *codeptr_ra)

{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);
    switch(endpoint) {
      case ompt_scope_begin:
      switch(kind) {
        case ompt_sync_region_barrier:
	    tls->wbarrier_btime = CBTF_GetTime();
#ifndef NDEBUG
	    if (cbtf_ompt_debug_blame) {
		fprintf(stderr,"[%d,%d] CBTF_ompt_callback_wait_barrier_begin parallelID:%lu taskID:%lu context:%lx\n",
		getpid(),monitor_get_thread_num(),(parallel_data)?parallel_data->value:0, task_data->value,current_region_context);
	    }
	    if (cbtf_ompt_debug_details) {
		fprintf(stderr,"%" PRIu64 ": ompt_event_wait_barrier_begin: parallel_id=%" PRIu64 ", task_id=%" PRIu64 ", codeptr_ra=%p\n",
		ompt_get_thread_data()->value, (parallel_data)?parallel_data->value:0, task_data->value, codeptr_ra);
	    }
#endif
          break;
        case ompt_sync_region_taskwait:
#ifndef NDEBUG
	    if (cbtf_ompt_debug_details) {
		fprintf(stderr,"%" PRIu64 ": ompt_event_wait_taskwait_begin: parallel_id=%" PRIu64 ", task_id=%" PRIu64 ", codeptr_ra=%p\n",
		ompt_get_thread_data()->value, (parallel_data)?parallel_data->value:0, task_data->value, codeptr_ra);
	    }
#endif
          break;
        case ompt_sync_region_taskgroup:
#ifndef NDEBUG
	    if (cbtf_ompt_debug_details) {
		fprintf(stderr,"%" PRIu64 ": ompt_event_wait_taskgroup_begin: parallel_id=%" PRIu64 ", task_id=%" PRIu64 ", codeptr_ra=%p\n",
		ompt_get_thread_data()->value, (parallel_data)?parallel_data->value:0, task_data->value, codeptr_ra);
	    }
#endif
          break;
      }
      break;
    case ompt_scope_end:
      switch(kind)
      {
        case ompt_sync_region_barrier:
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
		fprintf(stderr,"[%d,%d] CBTF_ompt_callback_wait_barrier_end: parallelID:%lu taskID:%lu context:%lx wbarrier_time:%f\n",
		getpid(),monitor_get_thread_num(),(parallel_data)?parallel_data->value:0, task_data->value,current_region_context,(float)t/1000000000);
	    }
	    if (cbtf_ompt_debug_details) {
		fprintf(stderr,"%" PRIu64 ": ompt_event_wait_barrier_end: parallel_id=%" PRIu64 ", task_id=%" PRIu64 ", codeptr_ra=%p\n",
		ompt_get_thread_data()->value, (parallel_data)?parallel_data->value:0, task_data->value, codeptr_ra);
	    }
#endif
          break;
        case ompt_sync_region_taskwait:
#ifndef NDEBUG
	    if (cbtf_ompt_debug_details) {
		fprintf(stderr,"%" PRIu64 ": ompt_event_wait_taskwait_end: parallel_id=%" PRIu64 ", task_id=%" PRIu64 ", codeptr_ra=%p\n",
		ompt_get_thread_data()->value, (parallel_data)?parallel_data->value:0, task_data->value, codeptr_ra);
	    }
#endif
          break;
        case ompt_sync_region_taskgroup:
#ifndef NDEBUG
	    if (cbtf_ompt_debug_details) {
		fprintf(stderr,"%" PRIu64 ": ompt_event_wait_taskgroup_end: parallel_id=%" PRIu64 ", task_id=%" PRIu64 ", codeptr_ra=%p\n",
		ompt_get_thread_data()->value, (parallel_data)?parallel_data->value:0, task_data->value, codeptr_ra);
	    }
#endif
          break;
      }
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
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);
    switch(endpoint) {
    case ompt_scope_begin:
	if(task_data->ptr) {
	    fprintf(stderr, "%s\n", "0: task_data initially not null");
	}
	task_data->value = ompt_get_unique_id();
#ifndef NDEBUG
	if (cbtf_ompt_debug_trace) {
	    fprintf(stderr,"%" PRIu64 ": ompt_event_implicit_task_begin: parallel_id=%" PRIu64 ", task_id=%" PRIu64 ", team_size=%" PRIu32 ", thread_num=%" PRIu32 "\n", ompt_get_thread_data()->value, (parallel_data)?parallel_data->value:0, task_data->value, team_size, thread_num);
	}
#endif

	tls->itask_btime = CBTF_GetTime();
	tls->serial_ttime += tls->itask_btime - tls->serial_btime;
	tls->task_event.time = 0;
#ifndef NDEBUG
	if (cbtf_ompt_debug_trace) {
	    fprintf(stderr,"[%ld] CBTF_ompt_callback_implicit_task_begin parallelID:%lu taskID:%lu context:%lx\n",
	    ompt_get_thread_data()->value,(parallel_data)?parallel_data->value:0, task_data->value,current_region_context);
	}
#endif

	// Find the parallel region this task is running under.
	// Aquire it's parallel context and use it here.
	CBTF_omptp_region r = CBTF_omptp_region_with_id(parallel_data->value);

	// copy region callstack to use as context for all individual
	// worker threads calling contexts.
	int i;
	tls->stacktrace_size = r.stacktrace_size;
	for (i = 0; i < r.stacktrace_size ; ++i) {
	    tls->stacktrace[i] = r.stacktrace[i];
	}
	break;

    case ompt_scope_end:
#ifndef NDEBUG
	if (cbtf_ompt_debug_trace) {
	    fprintf(stderr,"%" PRIu64 ": ompt_event_implicit_task_end: parallel_id=%" PRIu64 ", task_id=%" PRIu64 ", team_size=%" PRIu32 ", thread_num=%" PRIu32 "\n",
	    ompt_get_thread_data()->value, (parallel_data)?parallel_data->value:0, task_data->value, team_size, thread_num);
	}
#endif
	uint64_t etime = CBTF_GetTime();
	uint64_t t = etime - tls->itask_btime;
	tls->serial_btime = etime;
	tls->task_event.time = t;
	tls->itask_ttime += t;

#ifndef NDEBUG
	if (cbtf_ompt_debug_trace) {
	    fprintf(stderr,"[%ld] CBTF_ompt_callback_implicit_task_end parallelID:%lu taskID:%lu context:%lx time:%f\n",
		ompt_get_thread_data()->value,(parallel_data)?parallel_data->value:0, task_data->value,current_region_context,(float)t/1000000000);
	}
#endif
	omptp_record_event(&tls->task_event, tls->stacktrace, tls->stacktrace_size);
	break;
    }
}

// ompt_event_MAY_ALWAYS_BLAME
/**
 * The OpenMP runtime invokes this callback when a thread starts to idle
 * outside a parallel region. The callback executes in the environment
 * of the idling thread. ompt_scope_begin.
 * The OpenMP runtime invokes this callback when a thread finishes idling
 * outside a parallel region. The callback executes in the environment of
 * the thread that is about to resume useful work. ompt_scope_end.
*/
void CBTF_ompt_callback_idle(ompt_scope_endpoint_t endpoint)
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);
  switch(endpoint)
  {
    case ompt_scope_begin:
    tls->idle_btime = CBTF_GetTime();
#ifndef NDEBUG
    if (cbtf_ompt_debug_blame) {
	fprintf(stderr,"[%d,%d] CBTF_ompt_callback_idle ompt_scope_begin context:%lx \n",
	    getpid(),monitor_get_thread_num(), current_region_context);
    }
#endif
     break;
    case ompt_scope_end:
#ifndef NDEBUG
    if (cbtf_ompt_debug_blame) {
	fprintf(stderr,"[%d,%d] CBTF_ompt_callback_idle ompt_scope_end context:%lx \n",
	    getpid(),monitor_get_thread_num(), current_region_context);
    }
#endif
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
	fprintf(stderr,"[%d,%d] CBTF_ompt_callback_idle ompt_scope_begin context:%lx idle_time:%f\n",
		getpid(),monitor_get_thread_num(), current_region_context,(float)t/1000000000);
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
int ompt_initialize(
  ompt_function_lookup_t lookup,
  ompt_data_t *tool_data)
{
    /* Create and access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = malloc(sizeof(TLS));
    Assert(tls != NULL);
    CBTF_SetTLS(TLSKey, tls);
    ompt_callback_init_tls_done = 1;
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);

#ifndef NDEBUG
    cbtf_ompt_debug = (getenv("CBTF_DEBUG_OMPT") != NULL);
    cbtf_ompt_debug_blame = (getenv("CBTF_DEBUG_OMPT_BLAME") != NULL);
    cbtf_ompt_debug_trace = (getenv("CBTF_DEBUG_OMPT_TRACE") != NULL);
    cbtf_ompt_debug_wait = (getenv("CBTF_DEBUG_OMPT_WAIT") != NULL);
    cbtf_ompt_debug_lock = (getenv("CBTF_DEBUG_OMPT_LOCK") != NULL);
    cbtf_ompt_debug_details = (getenv("CBTF_DEBUG_OMPT_DETAILS") != NULL);
    if (cbtf_ompt_debug_details) {
	fprintf(stderr, "CBTF OMPT Init: Registering callbacks\n");
    }
#endif

  ompt_set_callback = (ompt_set_callback_t) lookup("ompt_set_callback");
  ompt_get_task_info = (ompt_get_task_info_t) lookup("ompt_get_task_info");
  ompt_get_thread_data = (ompt_get_thread_data_t) lookup("ompt_get_thread_data");
  ompt_get_parallel_info = (ompt_get_parallel_info_t) lookup("ompt_get_parallel_info");
  ompt_get_unique_id = (ompt_get_unique_id_t) lookup("ompt_get_unique_id");

  ompt_get_num_places = (ompt_get_num_places_t) lookup("ompt_get_num_places");
  ompt_get_place_proc_ids = (ompt_get_place_proc_ids_t) lookup("ompt_get_place_proc_ids");
  ompt_get_place_num = (ompt_get_place_num_t) lookup("ompt_get_place_num");
  ompt_get_partition_place_nums = (ompt_get_partition_place_nums_t) lookup("ompt_get_partition_place_nums");
  ompt_get_proc_id = (ompt_get_proc_id_t) lookup("ompt_get_proc_id");
  ompt_enumerate_states = (ompt_enumerate_states_t) lookup("ompt_enumerate_states");
  ompt_enumerate_mutex_impls = (ompt_enumerate_mutex_impls_t) lookup("ompt_enumerate_mutex_impls");

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
  if (cbtf_ompt_debug) {
    fprintf(stderr, "[%d,%d] CBTF OMPT Init: FINISHED Registering callbacks\n",getpid(),monitor_get_thread_num());
  }
#endif
    return 1;
}

void ompt_finalize(ompt_data_t *tool_data)
{
#ifndef NDEBUG
  if (cbtf_ompt_debug) {
    fprintf(stderr,"[%d,%d] ompt_event_runtime_shutdown\n",getpid(),monitor_get_thread_num());
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
