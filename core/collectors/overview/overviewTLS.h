/*******************************************************************************
** Copyright (c) 2017 The Krell Institute. All Rights Reserved.
**
** This program is free software; you can redistribute it and/or modify it under
** the terms of the GNU General Public License as published by the Free Software
** Foundation; either version 2 of the License, or (at your option) any later
** version.
**
** This program is distributed in the hope that it will be useful, but WITHOUT
** ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
** FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
** details.
**
** You should have received a copy of the GNU General Public License along with
** this program; if not, write to the Free Software Foundation, Inc., 59 Temple
** Place, Suite 330, Boston, MA  02111-1307  USA
*******************************************************************************/

/** @file Declaration of the TLS data structure and support functions. */

#pragma once

#include <inttypes.h>
#include <sys/types.h>
#include <sys/time.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>
#if defined(PAPI_FOUND)
#include <papi.h>
#endif

#include <KrellInstitute/Messages/Hwcsamp_data.h>
#include <KrellInstitute/Messages/IO_data.h>
#include <KrellInstitute/Messages/Mpi_data.h>
#include <KrellInstitute/Messages/Mem_data.h>
#include <KrellInstitute/Messages/Overview_data.h>
#include <KrellInstitute/Messages/DataHeader.h>
#include "KrellInstitute/Services/Data.h"

// FIXME: Likely this should be in a collector.h file
#define MaxFramesPerStackTrace 32 /* maximum number of frames per stacktrace */

/**
 * Maximum number of papi events to read. Based on hwcsamp value which
 * takes into account when we are not multi-plexing reads of counters.
 * Also note that on intel platforms the system may have enabled
 * hyperthreading which reduces the number of physical hw counters.
 * Papi itself may internally use physical hw counters further reducing
 * the actual countable counters.  With multiplexing on more counters can
 * be read but since Papi is using a timer, we can not multiplex counters
 * and use our timer based signal handlers at the same time.
 * When sampling is enable the maximum papi events we supported with the
 * hwcsamp experiment was 6 and even that may not be countable on some cpus.
 * papi_avail gives details on numbers of counters and papi_event_chooser
 * will verify (user needs to verify that is) if they wish to choose their
 * own papi or naive events.
 */
#define MAX_PAPI_EVENTS 12

/**
 * Maximum number of (CBTF_Protocol_Address) stack trace addresses contained
 * within each (CBTF_overview_data) performance data blob.
 *
 * @note    Currently there is no specific basis for the selection of this
 *          value other than a vague notion that it seems about right. In
 *          the future, performance testing should be done to determine an
 *          optimal value.
 */
#define MAX_STACK_ADDRESSES_PER_BLOB 1024

/**
 * Maximum number of individual (CBTF_overview_message) messages contained within
 * each (CBTF_overview_data) performance data blob. 
 *
 * @note    Currently there is no specific basis for the selection of this
 *          value other than a vague notion that it seems about right. In
 *          the future, performance testing should be done to determine an
 *          optimal value.
 */
#define MAX_MESSAGES_PER_BLOB 128

/** Overview MPI data type
 *  TODO: add data for all wrapped io calls seen. power of 4 binning.
 */
typedef struct {
    uint64_t total_mpi_time;
} overview_mpi_t;

/** Overview PAPI data type */
typedef struct {
    CBTF_overview_hwc_sample_data hwc_samp_data;  /**< hwcsamp data. */
    CBTF_HWCPCData buffer;   /**< PC sampling data buffer with event counts. */
    long long evalues[MAX_PAPI_EVENTS];
    int EventSet;
} overview_papi_t;

/** Overview POSIX IO data type
 *  TODO: add data for all wrapped io calls seen. power of 4 binning.
 */
typedef struct {
    uint64_t total_time;
    uint64_t total_read_time;
    uint64_t total_write_time;
    uint64_t total_bytes_read;
    uint64_t total_bytes_written;
} overview_posixio_t;

/** Overview mem data type
 *  TODO: add data for all wrapped mem calls seen. power of 4 binning.
 */
typedef struct {
    uint64_t total_allocation_time;
    uint64_t total_allocation_calls;
    uint64_t total_free_time;
    uint64_t total_free_calls;
    uint64_t total_bytes_allocated;
} overview_mem_t;

/** Overview OpenMP data type */
typedef struct {
    uint64_t region_begin_time;
    uint64_t region_total_time;
    uint64_t region_count;
    uint64_t idle_begin_time;
    uint64_t idle_total_time;
    uint64_t barrier_begin_time;
    uint64_t barrier_total_time;
    uint64_t waitbarrier_begin_time;
    uint64_t waitbarrier_total_time;
    uint64_t itask_begin_time;
    uint64_t itask_total_time;
    uint64_t serial_begin_time;
    uint64_t serial_total_time;
    uint64_t lock_begin_time;
    uint64_t lock_time;
    bool thread_idle, thread_wait_barrier, thread_barrier;
    uint32_t ompTid;
} overview_omp_t;

/** Overview rusage data type */
typedef struct {
    struct timeval ru_utime;
    struct timeval ru_stime;
    long   ru_maxrss;
} overview_rusage_t;

/** Overview PAPI_dmem data type */
typedef struct {
    long long size;
    long long resident;
    long long high_water_mark;
    long long shared;
    long long heap;
} overview_papi_dmem_t;


/** Type defining the data stored in thread-local storage. */
typedef struct {

    /** Flag indicating if data collection is paused. */
    bool paused;

    /** Flag indicating if data collection was ever started. */
    bool started;
    
    /**
     * Performance data header to be applied to this thread's performance data.
     * All of the fields except [addr|time]_[begin|end] are constant throughout
     * data collection. These exceptions are updated dynamically by the various
     * collection routines.
     */
    CBTF_DataHeader data_header;

    /** Sampling and HWC */
    /**
     * The hwc data here is sampled counters and pc addresses
     * same as used by hwcsamp.
     */
    CBTF_overview_hwc_sample_data hwc_samp_data;  /**< hwcsamp data. */
    CBTF_HWCPCData buffer;   /**< PC sampling data buffer with event counts. */

    /* TODO: currently set for multiplexed events.
     * For sampling we do not multiplex and are restricted to available
     * physical counter number.  It may be safe to just leave this as
     * is and use same strategy of adding events until this is full or
     * the events can not be added due to counter limits.
     */
    long long evalues[MAX_PAPI_EVENTS];
    int EventSet;

    /** OMPT */
#if defined (HAVE_OMPT)
    /* these are ompt specific. */
    bool thread_idle, thread_wait_barrier, thread_barrier;
    uint32_t ompTid;
#endif

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
    CBTF_overview_omptp_event region_event;
    CBTF_overview_omptp_event task_event;

    /** MPI  TODO: use the overview_mpi_t type here.*/
    uint64_t total_mpi_time;

    /** POSIX IO  TODO: use the overview_posixio_t type here.*/
    uint64_t total_posixio_time;
    uint64_t total_posixio_read_time;
    uint64_t total_posixio_write_time;
    uint64_t total_posixio_bytes_read;
    uint64_t total_posixio_bytes_written;

    /** MEM */
    overview_mem_t mem_data;

    /** Collector start time*/
    uint64_t collector_start_time;

    /** nesting depth of wrappers. TODO: should these be specific? */
    unsigned nesting_depth;

    /* debug flags */
    bool debug_collector;
    bool defer_sampling;

    /** Store the path to the csv file per thread */
    char csv_path[PATH_MAX];

    /**
     * Current performance data blob for this thread. Messages are added by the
     * various collection routines. It is sent when full, or upon completion of
     * data collection.
     */
    CBTF_overview_data data;

    /**
     * Individual messages containing data gathered by this collector. Pointed
     * to by the performance data blob above.
     */
    CBTF_overview_message messages[MAX_MESSAGES_PER_BLOB];

    /**
     * Unique, null-terminated, stack traces referenced by the messages. Pointed
     * to by the performance data blob above.
     */
    CBTF_Protocol_Address stack_traces[MAX_STACK_ADDRESSES_PER_BLOB];
   
} TLS;

/*
 * Allocate and zero-initialize the thread-local storage for the current thread.
 * This function <em>must</em> be called by a thread before that thread attempts
 * to call any of this file's other functions or memory corruption will result!
 */
void TLS_initialize();

/* Destroy the thread-local storage for the current thread. */
void TLS_destroy();

/*
 * Access the thread-local storage for the current thread.
 *
 * @return    Thread-local storage for the current thread.
 */
TLS* TLS_get();

/*
 * Initialize the performance data header and blob contained within the given
 * thread-local storage. This function <em>must</em> be called before any of
 * the collection routines attempts to add a message.
 *
 * @param tls    Thread-local storage to be initialized.
 */
void TLS_initialize_data(TLS* tls);

/*
 * Send the performance data blob contained within the given thread-local
 * storage. The blob is re-initialized (cleared) after being sent. Nothing
 * is sent if the blob is empty.
 *
 * @param tls    Thread-local storage containing data to be sent.
 */
void TLS_send_data(TLS* tls);

void TLS_print_data(TLS* tls);

/*
 * Add a new message to the performance data blob contained within the given
 * thread-local storage. The current blob is sent and re-initialized (cleared)
 * if it is already full.
 *
 * @param tls    Thread-local storage to which a message is to be added.
 * @return       Pointer to the new message to be filled in by the caller.
 */
CBTF_overview_message* TLS_add_message(TLS* tls);

/*
 * Update the performance data header contained within the given thread-local
 * storage with the specified time. Insures that the time interval defined by
 * time_begin and time_end contain the specified time.
 *
 * @param tls     Thread-local storage to be updated.
 * @param time    Time with which to update.
 */
void TLS_update_header_with_time(TLS* tls, CBTF_Protocol_Time time);

/*
 * Update the performance data header contained within the given thread-local
 * storage with the specified address. Insures that the address range defined
 * by addr_begin and addr_end contain the specified address.
 *
 * @param tls     Thread-local storage to be updated.
 * @param addr    Address with which to update.
 */
void TLS_update_header_with_address(TLS* tls, CBTF_Protocol_Address addr);

/*
 * Add a new stack trace for the current call site to the performance data
 * blob contained within the given thread-local storage.
 *
 * @param tls    Thread-local storage to which the stack trace is to be added.
 * @return       Index of this call site within the performance data blob.
 *
 * @note    Call sites are always referenced by a message. And since cross-blob
 *          references aren't supported, a crash is all but certain if the call
 *          site and its referencing message were to be split across two blobs.
 *          So this function also insures there is room in the current blob for
 *          at least one more message before adding the call site.
 */
uint32_t TLS_add_current_call_site(TLS* tls);

/*
 * Various callbacks to setup or record data from individual
 * sub collection services for overview/summary
 */
void TLS_add_hwc_sample(TLS* tls, CBTF_Protocol_Address addr);
void TLS_record_io_event(const CBTF_overview_iop_event*, uint64_t);
void TLS_start_io_event(CBTF_overview_iop_event*);
void TLS_record_mpi_event(const CBTF_mpip_event*, uint64_t);
void TLS_start_mpi_event(CBTF_mpip_event*);
void TLS_record_mem_event(const CBTF_memt_event*, uint64_t);
void TLS_start_mem_event(CBTF_memt_event*);
void TLS_record_ompt_event(const CBTF_overview_omptp_event*, uint64_t);
void TLS_start_ompt_event(CBTF_overview_omptp_event*);
void TLS_ompt_set_collector_active(bool flag);
void TLS_update_ompt_task_begin(uint64_t);
void TLS_update_ompt_task_totals(uint64_t);
void TLS_update_ompt_idle_init();
void TLS_update_ompt_idle_begin(uint64_t);
void TLS_update_ompt_idle_totals(uint64_t);
