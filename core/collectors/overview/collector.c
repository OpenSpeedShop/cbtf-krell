/*******************************************************************************
** Copyright (c) 2017-2018 Krell Institute.  All Rights Reserved.
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
 * Declaration and definition of the HWC sampling collector's runtime.
 *
 */

#define __USE_GNU /* required before including resource.h */
#ifndef RUSAGE_THREAD
#define RUSAGE_THREAD 1
#endif

#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <pthread.h>
#include <sys/resource.h>

#include "KrellInstitute/Messages/DataHeader.h"
#include "KrellInstitute/Messages/Overview.h"
#include "KrellInstitute/Messages/Overview_data.h"
#include "KrellInstitute/Messages/ToolMessageTags.h"
#include "KrellInstitute/Messages/Thread.h"
#include "KrellInstitute/Messages/ThreadEvents.h"
#include "KrellInstitute/Services/Assert.h"
#include "KrellInstitute/Services/Collector.h"
#include "KrellInstitute/Services/Common.h"
#include "KrellInstitute/Services/Context.h"
#include "KrellInstitute/Services/Data.h"
#include "KrellInstitute/Services/PapiAPI.h"
#include "KrellInstitute/Services/Time.h"
#include "KrellInstitute/Services/Timer.h"
#include "KrellInstitute/Services/TLS.h"
#include "monitor.h" /* monitor_get_thread_num and friends */
#include "Pthread_check.h"
#include "overviewTLS.h"
#include "collector.h"

/** String uniquely identifying this collector. */
const char* const cbtf_collector_unique_id = "overview";

// reference for hsw,snb.
const char* const mem_bandwidth_events = "MEM_LOAD_UOPS_RETIRED:L3_MISS,MEM_UOPS_RETIRED:ALL_STORES";
// master
const char* const master_papi_events = "PAPI_TOT_CYC,PAPI_TOT_INS,PAPI_LD_INS,PAPI_VEC_DP,PAPI_DP_OPS,PAPI_FDV_OPS,PAPI_FP_INS,PAPI_FP_OPS,PAPI_L3_TCM,PAPI_L2_TCM,PAPI_L1_TCM,PAPI_TLB_IM,PAPI_REF_CYC,PAPI_REF_NS,PAPI_FUL_CCY,PAPI_RES_STL";

/** Flag indicating if debugging is enabled. */
bool IsDebugEnabled = FALSE;

extern bool CBTF_in_mpi_startup;

// due to malloc issues in the services explicit TLS code,
// this critical flag should not use explicit tls.
static __thread int do_trace;

bool collector_do_trace()
{
    return do_trace;
}

/**
 * The number of threads for which are are collecting data (actively or not).
 * This value is atomically incremented in cbtf_collector_start(), decremented
 * in cbtf_collector_stop(), and is used by those functions to determine when
 * to perform process-wide initialization and finalization.
 */
static struct {
    int value;
    pthread_mutex_t mutex;
} ThreadCount = { 0, PTHREAD_MUTEX_INITIALIZER };

#if defined(CBTF_SERVICE_USE_FILEIO)
const char* const data_suffix = "cbtf-data";
#endif

// GOTCHA
//extern void init_mem_wrappers();

static int papi_init_done = 0;

/**
 * Checks that the given PAPI function call returns the value "PAPI_OK" or
 * "PAPI_VER_CURRENT". If the call was unsuccessful, the returned error is
 * reported on the standard error stream and the application is aborted.
 *
 * @param x    PAPI function call to be checked.
 */
#define PAPI_CHECK(x)                                                \
    do {                                                             \
        int RETVAL = x;                                              \
        if ((RETVAL != PAPI_OK) && (RETVAL != PAPI_VER_CURRENT))     \
        {                                                            \
            const char* description = PAPI_strerror(RETVAL);         \
            if (description != NULL)                                 \
            {                                                        \
                fprintf(stderr, "[%d:%d] %s(): %s = %d (%s)\n", \
                        getpid(), monitor_get_thread_num(),          \
                        __func__, #x, RETVAL, description);          \
            }                                                        \
            else                                                     \
            {                                                        \
                fprintf(stderr, "[%d:%d] %s(): %s = %d\n",      \
                        getpid(), monitor_get_thread_num(),          \
                        __func__, #x, RETVAL);                       \
            }                                                        \
            fflush(stderr);                                          \
            abort();                                                 \
        }                                                            \
    } while (0)

/** 
 * RUSAGE.
 * ru_ixrss, ru_idss, ru_isrss do not appear to be filled in on this OS.
 *
 */
overview_rusage_t get_usage() {
    struct rusage r_usage;
    getrusage(RUSAGE_THREAD,&r_usage);
    overview_rusage_t ov_rusage;
    ov_rusage.ru_maxrss = r_usage.ru_maxrss;
    ov_rusage.ru_utime = r_usage.ru_utime;
    ov_rusage.ru_stime = r_usage.ru_stime;
    return ov_rusage;
}

/** 
 * PAPI_Dmem.
 */
overview_papi_dmem_t get_papi_dmem_info ()
{
    PAPI_dmem_info_t dmem;
    // FIXME: check return value...
    int  dmemval = PAPI_get_dmem_info(&dmem); /**< get dynamic memory usage information */
    PAPI_dmem_info_t *d = &dmem;

    overview_papi_dmem_t rval;
    rval.size = d->size;
    rval.resident = d->resident;
    rval.high_water_mark = d->high_water_mark;
    rval.shared = d->shared;
    rval.heap = d->heap;
    return rval;

#if 0
    fprintf( stderr, "[OVERVIEW %ld,%d] DMem Size:\t\t%lld\n",tls->data_header.pid,tls->data_header.omp_tid, d->size );
    fprintf( stderr, "[OVERVIEW %ld,%d] DMem Resident:\t\t%lld\n",tls->data_header.pid,tls->data_header.omp_tid, d->resident );
    fprintf( stderr, "[OVERVIEW %ld,%d] DMem High Water Mark:\t%lld\n",tls->data_header.pid,tls->data_header.omp_tid, d->high_water_mark );
    fprintf( stderr, "[OVERVIEW %ld,%d] DMem Shared:\t\t%lld\n",tls->data_header.pid,tls->data_header.omp_tid, d->shared );
    fprintf( stderr, "[OVERVIEW %ld,%d] DMem Heap:\t\t%lld\n",tls->data_header.pid,tls->data_header.omp_tid, d->heap );
    fprintf( stderr, "Mem Text:\t\t%lld\n", d->text );
    fprintf( stderr, "Mem Library:\t\t%lld\n", d->library );
    fprintf( stderr, "Mem Peak Size:\t\t%lld\n", d->peak ); 
    fprintf( stderr, "Mem Locked:\t\t%lld\n", d->locked );
    fprintf( stderr, "Mem Stack:\t\t%lld\n", d->stack );
    fprintf( stderr, "Mem Pagesize:\t\t%lld\n", d->pagesize );
    fprintf( stderr, "Mem Page Table Entries:\t\t%lld\n", d->pte );
#endif
}

#if defined (HAVE_OMPT)
/* these are ompt specific functions to shift sample to an
 * OMPT defined blame. These are only useful in a sampling
 * context such as pcsamp,hwcsamp,hwc,hwctime,usertime.
 */
void IDLE_SAMPLE(bool flag) {
    /* Access our thread-local storage */
    TLS* tls = TLS_get();
    tls->thread_idle=flag;
}

void BARRIER_SAMPLE(bool flag) {
    /* Access our thread-local storage */
    TLS* tls = TLS_get();
    tls->thread_barrier=flag;
}

void WAIT_BARRIER_SAMPLE(bool flag) {
    TLS* tls = TLS_get();
    tls->thread_wait_barrier=flag;
}

#endif // if defined HAVE_OMPT

/**
 * Update the performance data header contained within the given thread-local
 * storage with the specified time. Insures that the time interval defined by
 * time_begin and time_end contain the specified time.
 *
 * @param tls     Thread-local storage to be updated.
 * @param time    Time with which to update.
 */
inline void update_header_with_time(TLS* tls, uint64_t time)
{
    Assert(tls != NULL);

    if (time < tls->data_header.time_begin) {
        tls->data_header.time_begin = time;
    }
    if (time >= tls->data_header.time_end) {
        tls->data_header.time_end = time + 1;
    }
}



/**
 * Update the performance data header contained within the given thread-local
 * storage with the specified address. Insures that the address range defined
 * by addr_begin and addr_end contain the specified address.
 *
 * @param tls     Thread-local storage to be updated.
 * @param addr    Address with which to update.
 */
inline void update_header_with_address(TLS* tls, uint64_t addr)
{
    Assert(tls != NULL);

    if (addr < tls->data_header.addr_begin) {
        tls->data_header.addr_begin = addr;
    }
    if (addr >= tls->data_header.addr_end) {
        tls->data_header.addr_end = addr + 1;
    }
}

/*
 * Not used in default simple summary. Not enabled at this time.
 * This function can be called from within the sigprof handler and therefore
 * must be signal safe.  no strdup and friends.
 */
void send_samples (TLS* tls)
{
    Assert(tls != NULL);

    tls->data_header.time_end = CBTF_GetTime();
    tls->data_header.addr_begin = tls->buffer.addr_begin;
    tls->data_header.addr_end = tls->buffer.addr_end;

    /* rank is not filled until mpi_init finished. safe to set here*/
    tls->data_header.rank = monitor_mpi_comm_rank();

    tls->hwc_samp_data.pc.pc_len = tls->buffer.length;
    tls->hwc_samp_data.count.count_len = tls->buffer.length;
    tls->hwc_samp_data.hwc_events.hwc_events_len = tls->buffer.length;

#ifndef NDEBUG
    if (IsDebugEnabled) {
	    fprintf(stderr, "[%ld,%d] send_samples: time:%f time_range(%lu,%lu) addr range[%lx,%lx] pc_len(%d) count_len(%d)\n",
	 	tls->data_header.pid,tls->data_header.omp_tid,
		(float)(tls->data_header.time_end - tls->data_header.time_begin)/1000000000,
		tls->data_header.time_begin,tls->data_header.time_end,
		tls->data_header.addr_begin,tls->data_header.addr_end,
		tls->hwc_samp_data.pc.pc_len,
		tls->hwc_samp_data.count.count_len);
	}
#endif

    cbtf_collector_send(&(tls->data_header), (xdrproc_t)xdr_CBTF_overview_message, &(tls->data));

    /* Re-initialize the data blob's header */
    TLS_initialize_data(tls);
}


/**
 * Timer event handler.
 *
 * Called by the timer handler each time a sample is to be taken. Extracts the
 * program counter (PC) address from the signal context and places it into the
 * sample buffer. When the sample buffer is full, it is sent to the framework
 * for storage in the experiment's database.
 *
 * @note    
 * 
 * @param context    Thread context at timer interrupt.
 */
static void TimerHandler(const ucontext_t* context)
{
    /* Access our thread-local storage */
    TLS* tls = TLS_get();
    if(tls->defer_sampling == true) {
        return;
    }
 
    // TODO: If we allow a choice of flat or callstack profiles
    // we will need to either provide a callstack handler or
    // modify this handler to choose.
    /* Obtain the program counter (PC) address from the thread context */
    uint64_t pc = CBTF_GetPCFromContext(context);


#if defined (HAVE_OMPT)
    /* these are ompt specific.*/
    if (tls->thread_idle) {
	/* ompt. thread is in __kmp_wait_sleep from intel libomp runtime.
	 * sample count here is attributed as an idle.  Note that the sample
	 * PC address may be also be in any calls made by __kmp_wait_sleep
	 * while the ompt interface is in the idle state.
	 */
	pc = CBTF_GetAddressOfFunction(IDLE_SAMPLE);
    }

    else if (tls->thread_wait_barrier) {
	/* ompt. thread is in __kmp_wait_sleep from intel libomp runtime.
	 * sample count here is attributed as a wait_barrier.  Note that the sample
	 * PC address may be also be in any calls made by __kmp_wait_sleep
	 * while the ompt interface is in the wait_barrier state.
	 */
	pc = CBTF_GetAddressOfFunction(WAIT_BARRIER_SAMPLE);
    }

    else if (tls->thread_barrier) {
	/* ompt. thread is in __kmp_wait_sleep from intel libomp runtime.
	 * sample count here is attributed as an idle.  Note that the sample
	 * PC address may be also be in any calls made by __kmp_wait_sleep
	 * while the ompt interface is in the idle state.
	 */
	pc = CBTF_GetAddressOfFunction(BARRIER_SAMPLE);
    }
#endif // if defined (HAVE_OMPT)

    // TODO: if we allow sampling, we need to not allow multiplexing counters.
#if 0
    /* This is supposed to reset counters */
    CBTF_HWCAccum(tls->EventSet, tls->evalues);

    /* Update the sampling buffer and check if it has been filled */
    if(CBTF_UpdateHWCPCData(pc, &tls->buffer,tls->evalues)) {
	/* Send these samples */
	send_samples(tls);
    }

    /* reset our values */
    memset(tls->evalues,0,sizeof(tls->evalues));

#ifndef NDEBUG
    if (getenv("CBTF_DEBUG_COLLECTOR_DETAILS") != NULL) {
      int i;
      for (i = 0; i < 6; i++) {
        if (tls->buffer.hwccounts[tls->buffer.length-1][i] > 0) {
            fprintf(stderr,"%#lx TimerHandler %d count %d is %ld\n",pc,tls->buffer.length-1,i, tls->buffer.hwccounts[tls->buffer.length-1][i]);
        }
      }
    }
#endif
#endif
}

/* NOOP */
void collector_record_addr(char* name, uint64_t addr)
{
}


/**
 * Called by the CBTF collector service in order to start data collection.
 */
/**
 * Start sampling.
 *
 * Starts hardware counter (HWC) sampling for the thread executing this
 * function. Initializes the appropriate thread-local data structures and
 * then enables the sampling counter.
 *
 * @param arguments    Encoded function arguments.
 */
void cbtf_collector_start(const CBTF_DataHeader* header)
{
    /* Create and zero-initialize our thread-local storage */
    TLS_initialize();

    /* Access our thread-local storage */
    TLS* tls = TLS_get();
    tls->defer_sampling=true;

    if (CBTF_in_mpi_startup) {
	tls->started=false;
	return;
    }
    tls->started=true;

    /* Copy the header into our thread-local storage for future use */
    memcpy(&tls->data_header, header, sizeof(CBTF_DataHeader));

    /* Atomically increment the active thread count */
    PTHREAD_CHECK(pthread_mutex_lock(&ThreadCount.mutex));

    /* default sample rate if selected. */
    int samp_rate = 100;
    char* papi_event = "";

    if (ThreadCount.value == 0) {
        /* Should debugging be enabled? */
        IsDebugEnabled = (getenv("CBTF_DEBUG_COLLECTOR") != NULL);

#if 0
#if !defined(NDEBUG)
        if (IsDebugEnabled)
        {
            printf("[%d,%d] cbtf_collector_start(): "
                   "ThreadCount.value = %d --> %d\n",
                   getpid(), monitor_get_thread_num(),
                   ThreadCount.value, ThreadCount.value + 1);
        }
#endif
#endif
	/* Decode the passed function arguments */
	CBTF_overview_start_sampling_args args;
	memset(&args, 0, sizeof(args));
	args.sampling_rate = samp_rate;

#if defined (CBTF_SERVICE_USE_OFFLINE)
	char* event_param = getenv("OVERVIEW_HWC_EVENTS");
	if (event_param != NULL) {
            papi_event=event_param;
	}

	const char* sampling_rate = getenv("OVERVIEW_SAMP_RATE");
	if (sampling_rate != NULL) {
            samp_rate=atoi(sampling_rate);
	}
	args.collector = 1;
	args.experiment = 0;
#endif

    }

    tls->hwc_samp_data.interval = (uint64_t)(1000000000) / (uint64_t)(samp_rate);;
    ThreadCount.value++;

    PTHREAD_CHECK(pthread_mutex_unlock(&ThreadCount.mutex));

    /* Initialize our performance data header and blob */
    TLS_initialize_data(tls);

#if 0
#ifndef NDEBUG
    if (IsDebugEnabled) {
	fprintf(stderr,"[%ld,%d] cbtf_collector_start: initialize TLS data\n",
	tls->data_header.pid,tls->data_header.omp_tid);
    }
#endif
#endif


    /* We can not assign mpi rank in the header at this point as it may not
     * be set yet. assign an integer tid value.  omp_tid is used regardless of
     * whether the application is using openmp threads.
     * libmonitor uses the same numbering scheme as openmp.
     */
    tls->data_header.omp_tid = monitor_get_thread_num();
    tls->data_header.id = strdup(cbtf_collector_unique_id);

    if(papi_init_done == 0) {
#if 0
#ifndef NDEBUG
	if (IsDebugEnabled) {
	    fprintf(stderr,"[%ld,%d] cbtf_collector_start: initialize papi\n",
	    tls->data_header.pid,tls->data_header.omp_tid);
	}
#endif
#endif
	CBTF_init_papi();
	tls->hwc_samp_data.clock_mhz = (float) hw_info->mhz; // hw_info->mhz is deprecated.
#if 0
#ifndef NDEBUG
	if (IsDebugEnabled) {
           fprintf(stderr, "PAPI Version: %d.%d.%d.%d\n", PAPI_VERSION_MAJOR( PAPI_VERSION ),
                        PAPI_VERSION_MINOR( PAPI_VERSION ),
                        PAPI_VERSION_REVISION( PAPI_VERSION ),
                        PAPI_VERSION_INCREMENT( PAPI_VERSION ) );
	   fprintf(stderr,"CPU MODEL %s\n",hw_info->model_string);
           fprintf(stderr,"System has %d hardware counters.\n", PAPI_num_counters());
	}
#endif
#endif
	papi_init_done = 1;
    } else {
	tls->hwc_samp_data.clock_mhz = (float) hw_info->mhz;  // hw_info->mhz is deprecated.
    }


    /* PAPI SETUP */
    tls->EventSet = PAPI_NULL;
    PAPI_CHECK(PAPI_create_eventset(&tls->EventSet));


/* In Component PAPI, EventSets must be assigned a component index
 * before you can fiddle with their internals. 0 is always the cpu component */
#if (PAPI_VERSION_MAJOR(PAPI_VERSION)>=4)
    PAPI_CHECK(PAPI_assign_eventset_component(tls->EventSet,0));
#endif

    // TODO: Allow user so set multiplexing, choose events and provide
    // usable defaults for CPU types with reasonable events for whether
    // we are sampling (no multiplexing) or not.
    //
    /* NOTE: if multiplex is turned on, papi internaly uses a SIGPROF handler.
     * Since we are sampling potentially with SIGPROF or now SIGRTMIN and we
     * prefer to limit our events to 6, we do not need multiplexing.
     */
    if (getenv("OVERVIEW_HWC_MULTIPLEX") != NULL) {
    }

#if !defined(RUNTIME_PLATFORM_BGP) 
	PAPI_CHECK(PAPI_set_multiplex(tls->EventSet));
#endif

	/* Create a master prioritized list of desired PAPI events and
	 * cycle them one at a time and add them if they exist upto the MAX
	 * number of multiplexed counters we will allow
	 */ 

    // TODO: Move environment override here and handle.
	papi_event = master_papi_events;

    /* call PAPI directly rather than
     * call any service helper functions due to inconsitent
     * behaviour seen on various lab systems
     * NOTE: we now simply add events until we reach the max papi event
     * count or run out of counters in the papi_event list.
     */
    int eventcode = 0;
    if (papi_event != NULL) {
	char *tfptr, *saveptr, *tf_token;
	tfptr = strdup(papi_event);
	int i;
	for (i = 1;  ; i++, tfptr = NULL) {
	    tf_token = strtok_r(tfptr, ",", &saveptr);
	    if (tf_token == NULL) {
		break;
	    }

	    if (PAPI_event_name_to_code(tf_token,&eventcode) != PAPI_OK){
		continue;
	    }

	    if (PAPI_add_event(tls->EventSet,eventcode) != PAPI_OK) {
		continue;
	    } else {
	    }

	    if (tfptr) free(tfptr);
	}
	
    } else {
	/* safe defaults for all platforms */
	PAPI_event_name_to_code("PAPI_TOT_CYC",&eventcode);
	PAPI_CHECK(PAPI_add_event(tls->EventSet,eventcode));
	PAPI_event_name_to_code("PAPI_TOT_INS",&eventcode);
	PAPI_CHECK(PAPI_add_event(tls->EventSet,eventcode));
    }

#if defined (HAVE_OMPT)
    /* these are ompt specific.*/
    /* initialize the flags and counts for idle,wait_barrier.  */
    tls->thread_idle =  tls->thread_wait_barrier = tls->thread_barrier = false;
    TLS_ompt_set_collector_active(true);
#endif

    // TODO: eventually the user can choose to run a timer and
    // that affects the papi events multiplexing choice. So we
    // will need to add lojic to handle these choices.
    // By default we will use a simple summary report that does
    // not sample via a timer and use multiplexing to allow upto
    // 12 papi counters.
    //
    /* Begin sampling */
    PAPI_start(tls->EventSet);

    /*
     * TODO: add check to allow running the sample timer.
     * If we are multiplexing PAPI counters we cannot run our timer
     * due to papi internals for multiplexing.
     */
    //CBTF_Timer(tls->hwc_samp_data.interval, TimerHandler);

    tls->idle_ttime = 0;
    tls->defer_sampling=false;
    do_trace=true;
    tls->data_header.time_begin = CBTF_GetTime();
    tls->collector_start_time = tls->data_header.time_begin;

    // TODO: eventually we should support the GOTCHA api for
    // handling wrappers (mem,io,mpi,etc).
    // gotcha
    //init_mem_wrappers();
#ifndef NDEBUG
    if (IsDebugEnabled) {
	    fprintf(stderr,"[%ld,%d] cbtf_collector_start: STARTED\n",tls->data_header.pid,tls->data_header.omp_tid);
    }
#endif
}



/**
 * Called by the CBTF collector service in order to pause data collection.
 */
void cbtf_collector_pause()
{
    /* Access our thread-local storage */
    TLS* tls = TLS_get();

    if (!tls) {
	return;
    }

    TLS_ompt_set_collector_active(false);

    // TODO: eventually the user can choose to run a timer and
    // that affects the papi events multiplexing choice. So we
    // will need to add logic to handle these choices.
    // By default we will use a simple summary report that does
    // not sample via a timer and use multiplexing to allow upto
    // 12 papi counters.
    //
    // BLOCK profiling signals.
    // We need to do more than ignore samples (defer_sampling).
    // It is best to block the profiling signal. Currently that
    // is SIGPROF. When we add a posix based timer that handles
    // thread samples correctly we will be blocking one of the
    // real time signals (SIGRTMIN or SIGRTMIN+N) as well and
    // likely default to the posix based timer.
    // fixes issues seen with omnipath based mpi connects.
    CBTF_BlockTimerSignal();
    tls->defer_sampling=true;
    do_trace=false; // mem wrappers
    if (papi_init_done) {
	PAPI_stop(tls->EventSet, tls->evalues);
    }
}



/**
 * Called by the CBTF collector service in order to resume data collection.
 */
void cbtf_collector_resume()
{
    /* Access our thread-local storage */
    TLS* tls = TLS_get();
    if (!tls) {
        //fprintf(stderr,"[%d,%d] cbtf_collector_resume returns NO TLS\n",getpid(),monitor_get_thread_num());
	return;
    }

    // TODO: eventually the user can choose to run a timer and
    // that affects the papi events multiplexing choice. So we
    // will need to add logic to handle these choices.
    // By default we will use a simple summary report that does
    // not sample via a timer and use multiplexing to allow upto
    // 12 papi counters.
    //
    // UNBLOCK profiling signals.
    // We need to do more than ignore samples (defer_sampling).
    // It is best to block the profiling signal. Currently that
    // is SIGPROF. When we add a posix based timer that handles
    // thread samples correctly we will be blocking one of the
    // real time signals (SIGRTMIN or SIGRTMIN+N) as well and
    // likely default to the posix based timer.
    // fixes issues seen with omnipath based mpi connects.
    CBTF_UnBlockTimerSignal();
    tls->defer_sampling=false;
    if (papi_init_done) {
	PAPI_start(tls->EventSet);
    }
    tls->defer_sampling=false;
    do_trace=true; // mem wrappers
}

/**
 * Called by the CBTF collector service in order to stop data collection.
 */
void cbtf_collector_stop()
{
    /* Access our thread-local storage */
    TLS* tls = TLS_get();
    if (!tls) {
        fprintf(stderr,"[%d,%d] cbtf_collector_stop returns NO TLS\n",getpid(),monitor_get_thread_num());
	return;
    }

    if (!tls->started) {
	return;
    }

    tls->defer_sampling=true;
    do_trace=false;
    tls->data_header.time_end = CBTF_GetTime();
    tls->thread_ttime = tls->data_header.time_end - tls->data_header.time_begin;

#ifndef NDEBUG
    if (IsDebugEnabled) {
	fprintf(stderr,"[%ld,%d] cbtf_collector_stop: STOPPED\n",tls->data_header.pid,tls->data_header.omp_tid);
    }
#endif

    // TODO: eventually the user can choose to run a timer and
    // that affects the papi events multiplexing choice. So we
    // will need to add logic to handle these choices.
    // By default we will use a simple summary report that does
    // not sample via a timer and use multiplexing to allow upto
    // 12 papi counters.
    //
    if (tls->EventSet == PAPI_NULL) {
	/*fprintf(stderr,"collector_stop RETURNS - NO EVENTSET!\n");*/
	/* we are called before eny events are set in papi. just return */
        return;
    }

    /* Stop counters */
    PAPI_accum(tls->EventSet, tls->evalues);
    PAPI_stop(tls->EventSet, NULL);


    /* If not multiplexing counters and sampling is enabled - Stop sampling */
    // TODO: Need to check if multiplexing counters is enabled here and
    // if user wishes to sample.
    //CBTF_Timer(0, NULL);


    // the debug dump.
    TLS_print_data(tls);

    /* Are there any unsent samples? */
    if(tls->buffer.length > 0) {
	/* Send these samples */
	send_samples(tls);
    }

    /* Destroy our thread-local storage */
    TLS_destroy();
}
