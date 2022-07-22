/*******************************************************************************
** Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
** Copyright (c) 2007,2008 William Hachfeld. All Rights Reserved.
** Copyright (c) 2007-2016 Krell Institute.  All Rights Reserved.
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
 * Declaration and definition of the UserTime collector's runtime.
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "KrellInstitute/Messages/DataHeader.h"
#include "KrellInstitute/Messages/Usertime.h"
#include "KrellInstitute/Messages/Usertime_data.h"
#include "KrellInstitute/Messages/ToolMessageTags.h"
#include "KrellInstitute/Messages/Thread.h"
#include "KrellInstitute/Messages/ThreadEvents.h"
#include "KrellInstitute/Services/Assert.h"
#include "KrellInstitute/Services/Collector.h"
#include "KrellInstitute/Services/Common.h"
#include "KrellInstitute/Services/Context.h"
#include "KrellInstitute/Services/Data.h"
#include "KrellInstitute/Services/Time.h"
#include "KrellInstitute/Services/Timer.h"
#include "KrellInstitute/Services/Unwind.h"
#include "KrellInstitute/Services/TLS.h"
#include <libunwind.h>
#include "monitor.h"

#if UNW_TARGET_X86 || UNW_TARGET_X86_64
# define STACK_SIZE     (128*1024)      /* On x86, SIGSTKSZ is too small */
#else
# define STACK_SIZE     SIGSTKSZ
#endif

/*
 * NOTE: For some reason GCC doesn't like it when the following two macros are
 *       replaced with constant unsigned integers. It complains about the arrays
 *       in the tls structure being "variable-size type declared outside of any
 *       function" even though the size IS constant... Maybe this can be fixed?
 */

/** Number of entries in the sample buffer. */
#define CBTF_USERTIME_BUFFERSIZE  1024

/** Man number of frames for callstack collection */
#define CBTF_USERTIME_MAXFRAMES 100

/** String uniquely identifying this collector. */
const char* const cbtf_collector_unique_id = "usertime";
#if defined(CBTF_SERVICE_USE_FILEIO)
const char* const data_suffix = "cbtf-data";
#endif

#define CBTF_HANDLE_UNWIND_SEGV 1
#if defined(CBTF_HANDLE_UNWIND_SEGV)
#include <setjmp.h>
#endif

/** Type defining the items stored in thread-local storage. */
typedef struct {

    CBTF_DataHeader header;  /**< Header for following data blob. */
    CBTF_usertime_data data;        /**< Actual data blob. */

    /** Sample buffer. */
    /**< buffer.stacktraces: Stack trace (PC) addresses. */
    /**< buffer.count: count value greater than 0 is top */
    /**< of stack. A count of 255 indicates */
    /**< another instance of this stack may */
    /**< exist in buffer stacktraces. */
    CBTF_StackTraceData buffer;

#if defined (HAVE_OMPT)
    /* these are ompt specific. */
    bool thread_idle, thread_wait_barrier, thread_barrier;
#endif

    /* debug flags */
    bool defer_sampling;

#if defined(CBTF_HANDLE_UNWIND_SEGV)
    sigjmp_buf unwind_jmp;
    bool is_unwinding;
    int unwind_segvcount;
    int sample_count;
#endif

} TLS;

/* debug flags */
#ifndef NDEBUG
static bool IsCollectorDebugEnabled = false;
#if defined (HAVE_OMPT)
static bool IsOMPTDebugEnabled = false;
#endif
#endif

#if defined(USE_EXPLICIT_TLS)

/**
 * Key used to look up our thread-local storage. This key <em>must</em> be
 * unique from any other key used by any of the CBTF services.
 */
static const uint32_t TLSKey = 0x00001EF4;

#else

/** Thread-local storage. */
static __thread TLS the_tls;

#endif

#if defined (HAVE_OMPT)
/* these are ompt specific functions to shift sample to an
 * OMPT defined blame.  These are only useful in a sampling
 * context such as pcsamp,hwcsamp,hwc,hwctime,usertime.
 */
void OMPT_THREAD_IDLE(bool flag) {
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    if (tls == NULL)
	return;
    tls->thread_idle=flag;
}

void OMPT_THREAD_BARRIER(bool flag) {
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    if (tls == NULL)
	return;
    tls->thread_barrier=flag;
}

void OMPT_THREAD_WAIT_BARRIER(bool flag) {
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    if (tls == NULL)
	return;
    tls->thread_wait_barrier=flag;
}
#endif // if defined HAVE_OMPT


#if defined(CBTF_HANDLE_UNWIND_SEGV)
int CBTF_Usertime_SEGVhandler(int sig, siginfo_t *siginfo, void *context)
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    if (tls == NULL) {
	return 1;
    }

    if (tls != NULL && tls->is_unwinding) {
	/* keep a count of unwinds that did not complete due to sigsegv or sigbus. */
        tls->unwind_segvcount++;
        //fprintf(stderr,"usertime unwinder SIGSEGV count:%d\n",tls->unwind_segvcount);
        siglongjmp(tls->unwind_jmp,9);
	return 0;
    }
    return 1;
}

// Implement a sigsegv,sigbus handler specific to usertime.
// We see both sigsegv or sigbus when unwinding fails do to
// any number of reasons (libunwind).  So install a handler for both.
// libmonitor provides an easy to use sigaction override so use it.:)
int CBTF_Usertime_SetSEGVhandler(void)
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    if (tls == NULL)
	return -1;

    /* inititalization of unwind flag and count of unwinds that did not
     * complete due to sigsegv or sigbus.
    */
    tls->is_unwinding = false;
    tls->unwind_segvcount = 0;

    int rval = monitor_sigaction(SIGSEGV, &CBTF_Usertime_SEGVhandler, 0, NULL);
    if (rval != 0) {
        fprintf(stderr,"usertime unwinder SIGSEGV handler failed to install\n");
    }
    rval = monitor_sigaction(SIGBUS, &CBTF_Usertime_SEGVhandler, 0, NULL);
    if (rval != 0) {
        fprintf(stderr,"usertime unwinder SIGBUS handler failed to install\n");
    }
    return rval;
}
#endif

/**
 * Initialize the performance data header and blob contained within the given
 * thread-local storage. This function <em>must</em> be called before any of
 * the collection routines attempts to add a message.
 *
 * @param tls    Thread-local storage to be initialized.
 */
static void initialize_data(TLS* tls)
{
    Assert(tls != NULL);

    tls->header.time_begin = CBTF_GetTime();
    tls->header.time_end = CBTF_GetTime();
    tls->header.addr_begin = ~0;
    tls->header.addr_end = 0;
    
    /* Re-initialize the actual data blob */
    tls->data.stacktraces.stacktraces_val = tls->buffer.stacktraces;
    tls->data.stacktraces.stacktraces_len = 0;
    tls->data.count.count_val = tls->buffer.count;
    tls->data.count.count_len = 0;

    /* Re-initialize the sampling buffer */
    memset(tls->buffer.stacktraces, 0, sizeof(tls->buffer.stacktraces));
    memset(tls->buffer.count, 0, sizeof(tls->buffer.count));
}


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

    if (time < tls->header.time_begin)
    {
        tls->header.time_begin = time;
    }
    if (time >= tls->header.time_end)
    {
        tls->header.time_end = time + 1;
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

    if (addr < tls->header.addr_begin)
    {
        tls->header.addr_begin = addr;
    }
    if (addr >= tls->header.addr_end)
    {
        tls->header.addr_end = addr + 1;
    }
}


/**
 * Send samples.
 *
 * This function can be called from within the sigprof handler and therefore
 * must be signal safe.  no strdup and friends.
 */
static void send_samples(TLS *tls)
{
    Assert(tls != NULL);

    /* the mpi rank is not available until applications has called mpi_init.*/
    /* safe to call here. */
#if defined (CBTF_SERVICE_USE_OFFLINE)
    tls->header.rank = monitor_mpi_comm_rank();
#endif

#ifndef NDEBUG
	if (IsCollectorDebugEnabled) {
	    fprintf(stderr, "[%ld:%d] usertime send_samples:\n",tls->header.pid, tls->header.omp_tid);
	    fprintf(stderr, "[%ld:%d] time_range(%lu,%lu) addr range[%lx, %lx] stacktraces_len(%d) count_len(%d)\n",
		tls->header.pid, tls->header.omp_tid,
		tls->header.time_begin,tls->header.time_end,
		tls->header.addr_begin,tls->header.addr_end,
		tls->data.stacktraces.stacktraces_len,
		tls->data.count.count_len);
	}
#endif

    cbtf_collector_send(&(tls->header), (xdrproc_t)xdr_CBTF_usertime_data, &(tls->data));

    /* Re-initialize the data blob's header */
    initialize_data(tls);
}


/**
 * Timer event handler.
 *
 * Called by the timer handler each time a sample is to be taken. 
 * Extract the PC address for each frame in the current stack trace and store
 * them into the sample buffer. For each address that represents the
 * top of a unique stack update it's count in the count buffer.
 * If a stack count reaches 255 in the count buffer, start a new stack
 * entry in the sample buffer.
 * When the sample buffer is full, it is sent to the framework
 * for storage in the experiment's database.
 *
 * @note    
 * 
 * @param context    Thread context at timer interrupt.
 */
static void serviceTimerHandler(const ucontext_t* context)
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);

    if(tls->defer_sampling == true) {
        return;
    }
 
    unsigned int framecount = 0;
    int stackindex = 0;
    uint64_t framebuf[CBTF_USERTIME_MAXFRAMES];

    memset(framebuf,0, sizeof(framebuf));

#if defined(CBTF_HANDLE_UNWIND_SEGV)
    /* counter for total samples seen. use for getting ratio of failed
     * unwind samples due to segv. */
    tls->sample_count++;
    /* flag handling of unwind for segv handler. */
    /* This enables our segv handling while unwinding this sample (libunwind). */
    /* If libunwind encounters a sigsegv or sigbus, do not die. */
    tls->is_unwinding = true;

    int ourlongjmp = sigsetjmp(tls->unwind_jmp, 1);
    if (ourlongjmp == 0 ) {

	/* simple test to verify segv handler works as expected. */
	//if (tls->sample_count == 10) {
	//   raise(SIGSEGV);
	//}
#endif

	/* get stack address for current context and store them into framebuf. */
#if defined(__linux) && defined(__x86_64)

#if defined(USE_FASTTRACE)
	/* The x86_64 unwinder uses fast cache based storage of addresses. */
	CBTF_GetStackTrace(TRUE, 0,
                        CBTF_USERTIME_MAXFRAMES /* maxframes*/, &framecount, framebuf) ;
#else
	CBTF_GetStackTraceFromContext (context, TRUE, 0,
                        CBTF_USERTIME_MAXFRAMES /* maxframes*/, &framecount, framebuf) ;
#endif

#else
	CBTF_GetStackTraceFromContext (context, TRUE, 0,
                        CBTF_USERTIME_MAXFRAMES /* maxframes*/, &framecount, framebuf) ;
#endif

#if defined(CBTF_HANDLE_UNWIND_SEGV)
    } else {
	/* Anything to do here if setsigjmp failes?. */
    }

    /* This restores normal segv handling outside of the unwinder (libunwind). */
    tls->is_unwinding = false;
#endif


#if defined (HAVE_OMPT)
    /* these are ompt specific.*/
    if (tls->thread_idle) {
	/* ompt. thread is in __kmp_wait_sleep from intel libomp runtime.
	 * sample count here is attributed as an idle.  Note that the sample
	 * PC address may be also be in any calls made by __kmp_wait_sleep
	 * while the ompt interface is in the idle state.
	 */
	framebuf[0] = CBTF_GetAddressOfFunction(OMPT_THREAD_IDLE);
    }

    else if (tls->thread_wait_barrier) {
	/* ompt. thread is in __kmp_wait_sleep from intel libomp runtime.
	 * sample count here is attributed as a wait_barrier.  Note that the sample
	 * PC address may be also be in any calls made by __kmp_wait_sleep
	 * while the ompt interface is in the wait_barrier state.
	 */
	framebuf[0] = CBTF_GetAddressOfFunction(OMPT_THREAD_WAIT_BARRIER);
    }

    else if (tls->thread_barrier) {
	/* ompt. thread is in __kmp_wait_sleep from intel libomp runtime.
	 * sample count here is attributed as a barrier.  Note that the sample
	 * PC address may be also be in any calls made by __kmp_wait_sleep
	 * while the ompt interface is in the wait_barrier state.
	 */
	framebuf[0] = CBTF_GetAddressOfFunction(OMPT_THREAD_BARRIER);
    }
#endif // if defined (HAVE_OMPT)

    bool_t stack_already_exists = FALSE;

    unsigned int i, j;
    /* search individual stacks via count/indexing array */
    for (i = 0; i < tls->data.count.count_len ; i++ )
    {
	/* a count > 0 indexes the top of stack in the data buffer. */
	/* a count == 255 indicates this stack is at the count limit. */

	if (tls->buffer.count[i] == 0) {
	    continue;
	}
	if (tls->buffer.count[i] == 255) {
	    continue;
	}

	/* see if the stack addresses match */
	for (j = 0; j < framecount ; j++ )
	{
	    if ( tls->buffer.stacktraces[i+j] != framebuf[j] ) {
		   break;
	    }
	}

	if ( j == framecount) {
	    stack_already_exists = TRUE;
	    stackindex = i;
	}
    }

    /* if the stack already exisits in the buffer, update its count
     * and return. If the stack is already at the count limit.
    */
    if (stack_already_exists && tls->buffer.count[stackindex] < 255 ) {
	/* update count for this stack */
	tls->buffer.count[stackindex] = tls->buffer.count[stackindex] + 1;
	return;
    }

    /* sample buffer has no room for these stack frames.*/
    int buflen = tls->data.stacktraces.stacktraces_len + framecount;
    if ( buflen > CBTF_USERTIME_BUFFERSIZE) {
	/* send the current sample buffer. (will init a new buffer) */
        tls->header.time_end = CBTF_GetTime();
	send_samples(tls);
    }

    /* add frames to sample buffer, compute addresss range */
    for (i = 0; i < framecount ; i++)
    {
	/* always add address to buffer bt */
	tls->buffer.stacktraces[tls->data.stacktraces.stacktraces_len] = framebuf[i];

	/* top of stack indicated by a positive count. */
	/* all other elements are 0 */
	if (i > 0 ) {
	    tls->buffer.count[tls->data.count.count_len] = 0;
	} else {
	    tls->buffer.count[tls->data.count.count_len] = 1;
	}

	if (framebuf[i] < tls->header.addr_begin ) {
	    tls->header.addr_begin = framebuf[i];
	}
	if (framebuf[i] > tls->header.addr_end ) {
	    tls->header.addr_end = framebuf[i];
	}
	tls->data.stacktraces.stacktraces_len++;
	tls->data.count.count_len++;
    }
}

void collector_record_addr(char* name, uint64_t addr)
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);
}


/**
 * Called by the CBTF collector service in order to start data collection.
 */
void cbtf_collector_start(const CBTF_DataHeader* header)
{
/**
 * Start sampling.
 *
 * Starts user time sampling for the thread executing this function.
 * Initializes the appropriate thread-local data structures and then enables the
 * sampling timer.
 *
 * @param arguments    Encoded function arguments.
 */
    /* Create and access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = malloc(sizeof(TLS));
    Assert(tls != NULL);
    CBTF_SetTLS(TLSKey, tls);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);


    tls->defer_sampling=false;

#ifndef NDEBUG
    IsCollectorDebugEnabled = (getenv("CBTF_DEBUG_COLLECTOR") != NULL);
#if defined (HAVE_OMPT)
    IsOMPTDebugEnabled = (getenv("CBTF_DEBUG_COLLECTOR_OMPT") != NULL);
#endif
#endif


/* testing this for performance improvements - dpm 8-17-2017 */
#if 0
    unw_set_caching_policy (unw_local_addr_space, UNW_CACHE_PER_THREAD);
#endif

    /* handle arguments */
    CBTF_usertime_start_sampling_args args;
    memset(&args, 0, sizeof(args));

    /* Access the environment-specified arguments */
    const char* sampling_rate = getenv("CBTF_USERTIME_RATE");
    args.sampling_rate = (sampling_rate != NULL) ? atoi(sampling_rate) : 35;


    /* Initialize the actual data blob */
    memcpy(&tls->header, header, sizeof(CBTF_DataHeader));
    initialize_data(tls);

    tls->data.interval = 
	(uint64_t)(1000000000) / (uint64_t)(args.sampling_rate);



    /* We can not assign mpi rank in the header at this point as it may not
     * be set yet. assign an integer tid value.  omp_tid is used regardless of
     * whether the application is using openmp threads.
     * libmonitor uses the same numbering scheme as openmp.
     */
    tls->header.omp_tid = monitor_get_thread_num();
    tls->header.id = strdup(cbtf_collector_unique_id);
    tls->header.time_begin = CBTF_GetTime();


#if defined (HAVE_OMPT)
    /* these are ompt specific.*/
    /* initialize the flags and counts for idle,wait_barrier.  */
    tls->thread_idle =  tls->thread_wait_barrier = tls->thread_barrier = false;
#endif

#if defined(CBTF_HANDLE_UNWIND_SEGV)
    memset((void *)tls->unwind_jmp, '\0', sizeof(tls->unwind_jmp));
    /* install segv handler */
    int rval = CBTF_Usertime_SetSEGVhandler();
    if (rval != 0) {
        fprintf(stderr,"No handler for unwind SIGSEGV\n");
    }
    tls->sample_count = 0;
#endif

    /* Begin sampling */
    CBTF_Timer(tls->data.interval, serviceTimerHandler);
}



/**
 * Called by the CBTF collector service in order to pause data collection.
 */
void cbtf_collector_pause()
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    if (tls == NULL)
	return;

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
}



/**
 * Called by the CBTF collector service in order to resume data collection.
 */
void cbtf_collector_resume()
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    if (tls == NULL)
	return;

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
}


#ifdef USE_EXPLICIT_TLS
void destroy_explicit_tls() {
    TLS* tls = CBTF_GetTLS(TLSKey);
    /* Destroy our thread-local storage */
    if (tls) {
        free(tls);
    }
    CBTF_SetTLS(TLSKey, NULL);
}
#endif


/**
 * Called by the CBTF collector service in order to stop data collection.
 */
void cbtf_collector_stop()
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);

    /* Stop sampling */
    CBTF_Timer(0, NULL);

    tls->header.time_end = CBTF_GetTime();

    /* Are there any unsent samples? */
    if(tls->data.stacktraces.stacktraces_len > 0) {
	/* Send these samples */
	send_samples(tls);
    }

#if defined(CBTF_HANDLE_UNWIND_SEGV)
#ifndef NDEBUG
    if (IsCollectorDebugEnabled) {
	if (tls->unwind_segvcount > 0) {
            fprintf(stderr,"[%ld:%d] usertime unwinder sample count:%d SIGSEGV count:%d\n",
		tls->header.pid, tls->header.omp_tid,
		tls->sample_count, tls->unwind_segvcount);
	}
    }
#endif
#endif

    /* Destroy our thread-local storage */
#ifdef CBTF_SERVICE_USE_EXPLICIT_TLS
    destroy_explicit_tls();
#endif
}
