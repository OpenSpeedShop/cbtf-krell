/*******************************************************************************
** Copyright (c) 2017-2018 The KrellInstitute. All Rights Reserved.
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

/** @file Definition of the overview TLS support functions. */

#define _GNU_SOURCE
#include <errno.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <pthread.h>
#include <libgen.h>
#include <malloc.h>
#include <monitor.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <KrellInstitute/Services/Assert.h>
#include <KrellInstitute/Services/Collector.h>
#include "KrellInstitute/Services/Common.h"
#include <KrellInstitute/Services/Data.h>
#include "KrellInstitute/Services/Fileio.h"
#include <KrellInstitute/Services/TLS.h>
#include <KrellInstitute/Services/Unwind.h>

#include <papi.h>
#include "collector.h"
#include "monitor.h"
#include "overviewTLS.h"

/* FIXME: find include files for these externs. */
extern bool cbtf_connected_to_mrnet();
extern overview_rusage_t get_usage();
extern overview_papi_dmem_t get_papi_dmem_info();
extern void send_samples (TLS*);

#if defined(USE_EXPLICIT_TLS)
/**
 * Key used to look up our thread-local storage. This key <em>must</em> be
 * unique from any other key used by any of the CBTF services.
 */
static const uint32_t Key = 0xBADC0FDA;
/* is wrapping mem calls, malloc is called before we are setup. */
#else
/** Implicit thread-local storage. */
static __thread TLS Implicit;
#endif

// record the executable path once here.
static char* executable_path = NULL;

/**
 * Is the performance data blob in the given thread-local storage already
 * full? I.e. does it already contain the maximum number of messages?
 *
 * @param tls    Thread-local storage to be tested.
 * @return       Boolean flag indicating if the performance data blob is full.
 */
static bool is_full(TLS* tls)
{
    Assert(tls != NULL);

    u_int max_messages_per_blob = MAX_MESSAGES_PER_BLOB;

    /* overview blobs checks here */
    return tls->data.messages.messages_len == max_messages_per_blob;
}



/**
 * Allocate and zero-initialize the thread-local storage for the current thread.
 * This function <em>must</em> be called by a thread before that thread attempts
 * to call any of this file's other functions or memory corruption will result!
 */
void TLS_initialize()
{
#if defined(USE_EXPLICIT_TLS)
    TLS* tls = malloc(sizeof(TLS));
    Assert(tls != NULL);
    CBTF_SetTLS(Key, tls);
    //fprintf(stderr,"[%d,%d] TLS_initalize tls=%p\n",getpid(),monitor_get_thread_num(),tls);
#else
    TLS* tls = &Implicit;
#endif
    Assert(tls != NULL);
    memset(tls, 0, sizeof(TLS));

    tls->nesting_depth = 0;

    //fprintf(stderr,"[%d,%d] TLS_initialize called. tls:%p\n",getpid(), monitor_get_thread_num(),tls);
    /* these are only inititalized at collector start up */
    tls->total_posixio_time = 0;
    tls->total_posixio_read_time = 0;
    tls->total_posixio_write_time = 0;
    tls->total_posixio_bytes_read = 0;
    tls->total_posixio_bytes_written = 0;
    tls->collector_start_time = 0;

    /* papi values */
    memset(tls->evalues,0,sizeof(tls->evalues));

    /* total thread time */
    tls->thread_ttime=0;
    /*collector end time */
    tls->collector_etime = 0;

    /* openmp */
    tls->region_ttime = 0;
    tls->idle_ttime = 0;
    tls->barrier_ttime = 0;
    tls->wbarrier_ttime = 0;
    tls->itask_ttime = 0;
    tls->serial_ttime = 0;
    tls->serial_btime = CBTF_GetTime();
    tls->idle_btime = tls->serial_btime;
    tls->thread_btime = tls->serial_btime;
    tls->lock_btime = 0;
    tls->lock_time = 0;

}



/**
 * Destroy the thread-local storage for the current thread.
 */
void TLS_destroy()
{
#if defined(USE_EXPLICIT_TLS)
    TLS* tls = CBTF_GetTLS(Key);
    Assert(tls != NULL);
    free(tls);
    CBTF_SetTLS(Key, NULL);
#endif
}



/**
 * Access the thread-local storage for the current thread.
 *
 * @return    Thread-local storage for the current thread.
 */
TLS* TLS_get()
{
    /* Access our thread-local storage */
#if defined(USE_EXPLICIT_TLS)
    TLS* tls = CBTF_GetTLS(Key);
#else
    TLS* tls = &Implicit;
#endif
    return tls;
}



/**
 * Initialize the performance data header and blob contained within the given
 * thread-local storage. This function <em>must</em> be called before any of
 * the collection routines attempts to add a message.
 *
 * @param tls    Thread-local storage to be initialized.
 */
void TLS_initialize_data(TLS* tls)
{
    Assert(tls != NULL);

    tls->data_header.time_begin = ~0;
    tls->data_header.time_end = 0;
    tls->data_header.addr_begin = ~0;
    tls->data_header.addr_end = 0;

    /* overview init here */
    tls->data.messages.messages_len = 0;
    tls->data.messages.messages_val = tls->messages;
    tls->nesting_depth = 0;
}



/**
 * Send the performance data blob contained within the given thread-local
 * storage. The blob is re-initialized (cleared) after being sent. Nothing
 * is sent if the blob is empty.
 *
 * @param tls    Thread-local storage containing data to be sent.
 */
void TLS_send_data(TLS* tls)
{
    Assert(tls != NULL);

    bool send = (tls->data.messages.messages_len > 0);

    /* handle overview messages here */
    
    if (send) {
#if !defined(NDEBUG)
        if (IsDebugEnabled) {
            printf("[%d:%d] TLS_send_data(): "
                   "sending CBTF_overview_data message (%u msg, %u pc)\n",
                   getpid(), monitor_get_thread_num(),
                   tls->data.messages.messages_len,
                   tls->data.stack_traces.stack_traces_len);
        }
#endif

        /*
         * At the point when cbtf_collector_start() is called the process has
         * not had a chance to call MPI_Init() yet. Thus the process does not
         * yet have a MPI rank. But by the time a performance data blob is to
         * be sent, MPI_Init() almost certainly has been called, so obtain the
         * MPI and OpenMP ranks and put them in the performance data header.
         */

        tls->data_header.rank = monitor_mpi_comm_rank();

        if (tls->data_header.omp_tid != -1) {
            tls->data_header.omp_tid = monitor_get_thread_num();
        }
        
        cbtf_collector_send(
            &tls->data_header, (xdrproc_t)xdr_CBTF_overview_data, &tls->data
            );
        TLS_initialize_data(tls);
    }
}



/**
 * Add a new message to the performance data blob contained within the given
 * thread-local storage. The current blob is sent and re-initialized (cleared)
 * if it is already full.
 *
 * @param tls    Thread-local storage to which a message is to be added.
 * @return       Pointer to the new message to be filled in by the caller.
 */
CBTF_overview_message* TLS_add_message(TLS* tls)
{
    Assert(tls != NULL);

    if (is_full(tls)) {
        TLS_send_data(tls);
    }
    
    return &(tls->messages[tls->data.messages.messages_len++]);
}



/**
 * Update the performance data header contained within the given thread-local
 * storage with the specified time. Insures that the time interval defined by
 * time_begin and time_end contain the specified time.
 *
 * @param tls     Thread-local storage to be updated.
 * @param time    Time with which to update.
 */
void TLS_update_header_with_time(TLS* tls, CBTF_Protocol_Time time)
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
void TLS_update_header_with_address(TLS* tls, CBTF_Protocol_Address addr)
{
    Assert(tls != NULL);

    if (addr < tls->data_header.addr_begin) {
        tls->data_header.addr_begin = addr;
    }

    if (addr >= tls->data_header.addr_end) {
        tls->data_header.addr_end = addr + 1;
    }
}



/**
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
uint32_t TLS_add_current_call_site(TLS* tls)
{
    Assert(tls != NULL);

    /*
     * Send performance data for this thread if there isn't enough room
     * to hold another message. See the note in this function's header.
     */

    if (is_full(tls))
    {
        TLS_send_data(tls);
    }

    /* Get the stack trace for the current call site */

    unsigned frame_count = 0;
    uint64_t frame_buffer[CBTF_ST_MAXFRAMES];
    
    CBTF_GetStackTraceFromContext(
        NULL, FALSE, 0, CBTF_ST_MAXFRAMES, &frame_count, frame_buffer
        );

    /* Search for this stack trace amongst the existing stack traces */
    
    int i, j;
    
    /* Iterate over the addresses in the existing stack traces */
    for (i = 0, j = 0; i < MAX_STACK_ADDRESSES_PER_BLOB; ++i)
    {
        /* Is this the terminating null of an existing stack trace? */
        if (tls->stack_traces[i] == 0)
        {
            /*
             * Terminate the search if a complete match has been found
             * between this stack trace and the existing stack trace.
             */
            if (j == frame_count)
            {
                break;
            }

            /*
             * Otherwise check for a null in the first or last entry, or
             * for consecutive nulls, all of which indicate the end of the
             * existing stack traces, and the need to add this stack trace
             * to the existing stack traces.
             */
            else if ((i == 0) || 
                     (i == (MAX_STACK_ADDRESSES_PER_BLOB - 1)) ||
                     (tls->stack_traces[i - 1] == 0))
            {
                /*
                 * Send performance data for this thread if there isn't enough
                 * room in the existing stack traces to add this stack trace.
                 * Doing so frees up enough space for this stack trace.
                 */
                if ((i + frame_count) >= MAX_STACK_ADDRESSES_PER_BLOB)
                {
                    TLS_send_data(tls);
                    i = 0;
                }

                /* Add this stack trace to the existing stack traces */
                for (j = 0; j < frame_count; ++j, ++i)
                {
                    tls->stack_traces[i] = frame_buffer[j];
                    TLS_update_header_with_address(tls, tls->stack_traces[i]);
                }
                tls->stack_traces[i] = 0;
                tls->data.stack_traces.stack_traces_len = i + 1;
              
                break;
            }
            
            /* Otherwise reset the pointer within this stack trace to zero */
            else
            {
                j = 0;
            }
        }
        else
        {
            /*
             * Advance the pointer within this stack trace if the current
             * address within this stack trace matches the current address
             * within the existing stack traces. Otherwise reset the pointer
             * to zero.
             */
            j = (frame_buffer[j] == tls->stack_traces[i]) ? (j + 1) : 0;
        }
    }

    /* Return the index of this stack trace within the existing stack traces */
    return i - frame_count;
}

// Handle hwc samples.
void TLS_add_hwc_sample(TLS* tls, CBTF_Protocol_Address pc)
{
    /* Update the sampling buffer and check if it has been filled */
    if(CBTF_UpdateHWCPCData(pc, &tls->buffer,tls->evalues)) {
        /* Send these samples */
        /* NOTE: use the TLS_send method */
        send_samples(tls);
    }
    /* reset our papi event values */
    memset(tls->evalues,0,sizeof(tls->evalues));
}


void TLS_start_io_event(CBTF_overview_iop_event* event)
{
    /* Access our thread-local storage */
    TLS* tls = TLS_get();
    if (!tls) {
	//fprintf(stderr,"[%d,%d] TLS_record_io_event called with no TLS\n",getpid(),monitor_get_thread_num());
	return;
    }

    if (tls->defer_sampling) {
	return;
    }
    

    /* Increment the IO function wrapper nesting depth */
    //++tls->nesting_depth;

    /* Initialize the event record. */
    memset(event, 0, sizeof(CBTF_overview_iop_event));
}

void TLS_record_io_event(const CBTF_overview_iop_event* event, uint64_t addr)
{
    /* Access our thread-local storage */
    TLS* tls = TLS_get();
    if (!tls) {
	//fprintf(stderr,"[%d,%d] TLS_record_io_event called with no TLS\n",getpid(),monitor_get_thread_num());
	return;
    }

    if (tls->defer_sampling) {
	return;
    }

    tls->total_posixio_time += event->time;
    if (event->kind == Read) {
	tls->total_posixio_read_time += event->time;
	tls->total_posixio_bytes_read += event->bytes;
    } else if (event->kind == Write) {
	tls->total_posixio_write_time += event->time;
	tls->total_posixio_bytes_written += event->bytes;
    }
    /** record stacktrace */
}

void TLS_start_mpi_event(CBTF_mpip_event* event)
{
    /* Initialize the event record. */
    memset(event, 0, sizeof(CBTF_mpip_event));
}

// TODO: handle individual events.
void TLS_record_mpi_event(const CBTF_mpip_event* event, uint64_t addr)
{
    /* Access our thread-local storage */
    TLS* tls = TLS_get();

    if (!tls) return;
    tls->total_mpi_time += event->time;
    /** record stacktrace */
}

void TLS_start_mem_event(CBTF_memt_event* event)
{
    /* Initialize the event record. */
    memset(event, 0, sizeof(CBTF_memt_event));
    //if (tls) ++tls->nesting_depth;
}

void TLS_record_mem_event(const CBTF_memt_event* event, uint64_t addr)
{
    /* Access our thread-local storage */
    TLS* tls = TLS_get();
    if (!tls) {
	//fprintf(stderr,"[%d,%d] TLS_record_mem_event called with no TLS\n",getpid(),monitor_get_thread_num());
	return;
    }

    if (tls->defer_sampling) {
	//fprintf(stderr,"[%d,%d] TLS_record_mem_event returns early. DEFER\n",getpid(),monitor_get_thread_num());
	return;
    }

    /* Decrement the Mem function wrapper nesting depth */
    //--tls->nesting_depth;

    /*
     * Don't record events for any recursive calls to our Mem function wrappers.
     * The place where this occurs is when the Mem implemetnation calls itself.
     * We don't record that data here because we are primarily interested in
     * direct calls by the application to the Mem library - not in the internal
     * implementation details of that library.
     */
    if(tls->nesting_depth > 0) {
        return;
    }


    if (event->mem_type == CBTF_MEM_MALLOC) {
	//fprintf(stderr,"[%d,%d] TLS_record_mem_event MALLOC\n",getpid(),monitor_get_thread_num());
	tls->mem_data.total_allocation_calls++;
	tls->mem_data.total_allocation_time += event->stop_time - event->start_time;
	tls->mem_data.total_bytes_allocated += event->size1;
    } else if (event->mem_type == CBTF_MEM_CALLOC) {
	//fprintf(stderr,"[%d,%d] TLS_record_mem_event CALLOC\n",getpid(),monitor_get_thread_num());
	tls->mem_data.total_allocation_calls++;
	tls->mem_data.total_allocation_time += event->stop_time - event->start_time;
	tls->mem_data.total_bytes_allocated += event->size1 * event->size2;
    } else if (event->mem_type == CBTF_MEM_REALLOC) {
	//fprintf(stderr,"[%d,%d] TLS_record_mem_event REALLOC\n",getpid(),monitor_get_thread_num());
	// if size is 0, realloc acts as a free.
	if (event->size1 == 0) {
	    tls->mem_data.total_free_time += event->stop_time - event->start_time;
	    tls->mem_data.total_free_calls++;
	} else {
	    tls->mem_data.total_allocation_time += event->stop_time - event->start_time;
	    tls->mem_data.total_allocation_calls++;
	    tls->mem_data.total_bytes_allocated += event->size1;
	}
    } else if (event->mem_type == CBTF_MEM_FREE) {
	//fprintf(stderr,"[%d,%d] TLS_record_mem_event FREE\n",getpid(),monitor_get_thread_num());
	tls->mem_data.total_free_time += event->stop_time - event->start_time;
	tls->mem_data.total_free_calls++;
    }
    /** record stacktrace */
}

void TLS_ompt_set_collector_active(bool flag)
{
    /* Access our thread-local storage */
    TLS* tls = TLS_get();
    if (!tls) {
	//fprintf(stderr,"[%d,%d] TLS_ompt_set_collector_active called with no TLS\n",getpid(),monitor_get_thread_num());
	return;
    }
    tls->collector_active=flag;
    tls->collector_etime=CBTF_GetTime();
}

// update ompt task begin. maintain serial total time.
void TLS_update_ompt_task_begin(uint64_t task_btime)
{
    /* Access our thread-local storage */
    TLS* tls = TLS_get();
    /* with amg2013 we see a task end AFTER monitor has notified that
     * the underlying thread has been ended.
     */
    if (!tls) {
	return;
    }
    tls->itask_btime = task_btime;
    tls->serial_ttime += tls->itask_btime - tls->serial_btime;
}

// TODO: could pass context address here for symbolic resolution later...
void TLS_update_ompt_task_totals(uint64_t task_etime)
{
    /* Access our thread-local storage */
    TLS* tls = TLS_get();

    /* with amg2013 we see a task end AFTER monitor has notified that
     * the underlying thread has been ended.
     */
    if (!tls) {
	return;
    }
    tls->serial_btime = task_etime;
    tls->itask_ttime += task_etime - tls->itask_btime;
}

void TLS_update_ompt_idle_init()
{
    /* Access our thread-local storage */
    TLS* tls = TLS_get();
    /* with amg2013 we see a task end AFTER monitor has notified that
     * the underlying thread has been ended.
     */
    if (!tls) {
	return;
    }
    tls->idle_ttime = 0;
}

void TLS_update_ompt_idle_begin(uint64_t t)
{
    /* Access our thread-local storage */
    TLS* tls = TLS_get();
    /* with amg2013 we see a task end AFTER monitor has notified that
     * the underlying thread has been ended.
     */
    if (!tls) {
	return;
    }
    tls->idle_btime = t;
    //fprintf(stderr,"[%d,%ld] TLS_update_ompt_idle_begin active:%d idle_btime:%ld tls->idle_ttime:%ld\n",
    //           getpid(),omp_get_thread_num(), tls->collector_active,tls->idle_btime,(tls->idle_ttime > 0)?tls->idle_ttime:0);
}

void TLS_update_ompt_idle_totals(uint64_t etime)
{
    /* Access our thread-local storage */
    TLS* tls = TLS_get();
    /* with amg2013 we see a task end AFTER monitor has notified that
     * the underlying thread has been ended.
     */
    if (!tls) {
	return;
    }

    uint64_t t;
    if (tls->collector_active) {
        t = etime - tls->idle_btime;
    } else {
        t = tls->collector_etime - tls->idle_btime;
    }
    //fprintf(stderr,"[%d,%ld] TLS_update_ompt_idle_totals active:%d etime:%ld tls->idle_btime:%ld\n",
    //           getpid(),omp_get_thread_num(), tls->collector_active,etime,tls->idle_btime);
    tls->idle_ttime += t;
    //fprintf(stderr,"[%d,%ld] TLS_update_ompt_idle_totals active:%d idle:%f idle_ttime:%f\n",
    //           getpid(),omp_get_thread_num(), tls->collector_active,(float)t/1000000000, (float)tls->idle_ttime/1000000000);
}

void TLS_start_ompt_event(CBTF_overview_omptp_event* event)
{
    /* Initialize the event record. */
    memset(event, 0, sizeof(CBTF_overview_omptp_event));
}

void TLS_record_ompt_event(const CBTF_overview_omptp_event* event, uint64_t addr)
{
    /** record event like omptp */
}

/*
 * Helper function to create a path.
 * TODO: Move this to services?
 */
static int mkpath(char *dir, mode_t mode)
{
    struct stat sb;

    if (!dir) {
        errno = EINVAL;
        return 1;
    }

    if (!stat(dir, &sb))
        return 0;

    mkpath(dirname(strdupa(dir)), mode);

    return mkdir(dir, mode);
}

/*
 * Create a folder for csv files and setup csv for each thread.
 */
static void SetCSVFile(TLS* tls, const char* unique_id,
			const char* suffix)
{
    int fd;
    char* cbtf_csvdata_dir = NULL;
    char* user_name = NULL;
    char dir_path[PATH_MAX] = {0};
    char cwd[PATH_MAX];

    if (getcwd(cwd, sizeof(cwd)) == NULL) {
	sprintf(cwd, "%s", "/tmp");
    }
    

    cbtf_csvdata_dir = getenv("CBTF_CSVDATA_DIR");
    user_name = getenv("USER");
    char *exename = basename(executable_path);

    // TODO: add jobid from any resource manager found.
    if (cbtf_csvdata_dir != NULL) {
	sprintf(dir_path, "%s", cbtf_csvdata_dir);
    } else if (strcmp(cwd,"/tmp") == 0) {
	sprintf(dir_path, "%s/%s/%s-csvdata",
	    cwd, (user_name != NULL) ? user_name : "/unknownuser",exename);
    } else {
	sprintf(dir_path, "%s/%s-csvdata",
	    cwd,exename);
    }

    if (tls->data_header.rank < 0) {
	sprintf(dir_path, "%s/%s-%lu",
	    dir_path, tls->data_header.host,tls->data_header.pid);
    } else {
	sprintf(dir_path, "%s/%s-%u",
	    dir_path, tls->data_header.host,tls->data_header.rank);
    }

    {
	sprintf(tls->csv_path,"");
	if(tls->data_header.posix_tid == 0) {
            if (tls->data_header.rank < 0) {
		sprintf(tls->csv_path, "%s/%s-%lu", dir_path, exename, tls->data_header.pid);
	    } else {
		sprintf(tls->csv_path, "%s/%s-%u", dir_path, exename, tls->data_header.rank);
	    }
	} else {
            if (tls->data_header.rank < 0) {
		sprintf(tls->csv_path, "%s/%s-%lu-%u", dir_path, exename, tls->data_header.pid, tls->data_header.omp_tid);
	    } else {
		sprintf(tls->csv_path, "%s/%s-%u-%u", dir_path, exename, tls->data_header.rank, tls->data_header.omp_tid);
	    }
	}

	sprintf(tls->csv_path, "%s.%s", tls->csv_path, suffix);
   }

#if !defined(NDEBUG)
    if (IsDebugEnabled) {
    fprintf(stderr,"[OVERVIEW %ld,%d] CSV PATH %s\n",
	tls->data_header.pid, tls->data_header.omp_tid,
	tls->csv_path);
    }
#endif

    /* Insure the directory path to contain the file exists */
    struct stat st;
    if (stat(dir_path, &st) == 0 && S_ISDIR (st.st_mode)) {
	if ( (getenv("CBTF_DEBUG_FILEIO_SERVICE") != NULL)) {
	    fprintf(stderr,"SetCSVFile pathname %s exists and is a directory\n", dir_path);
	}
    } else {

        /* The directory does not exist.
	 * Try to make the directory in this section of code.
	 * Use a while loop to test the status of mkdir in case
	 * it fails for some reason. On a cluster running nvidia-nmi
	 * under the watch command (run command by default every 2 secs)
	 * the mkdir on an NFS directory was interupted.
	 * TODO: if some threshold of trys is exceeded, abort.
	 */
        int status = -1;
        int save_errno = 0;
        int try_count = 0;
        while (status != 0 ) {
   	   status = mkpath(dir_path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
           save_errno = errno;
           try_count = try_count + 1;
        }
	if ( (getenv("CBTF_DEBUG_FILEIO_SERVICE") != NULL)) {
	    if (try_count > 0) {
            fprintf(stderr,"SetCSVFile mkdir dir_path:%s status=%d errno:%d try_count:%d\n",
		dir_path, status, save_errno, try_count);
	    }
        }
    } 
        
    /* Insure the file itself exists */
    fd = open(tls->csv_path, O_CREAT | O_APPEND,
	      S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
    if(fd >= 0)
	close(fd);
}

/*
 * This function currectly prints debug to stderr and creates
 * a csv file per thread of execution.
 */
void TLS_print_data(TLS* tls)
{
    bool print_to_stdout = getenv("CBTF_SHOW_CSVDATA") ? true : false;
    /* Get our executable path */
    if (executable_path == NULL) {
        executable_path = strdup(CBTF_GetExecutablePath());
    }
    char *exename = basename(executable_path);

    /* header is created before mpi rank is known so fill it
     * in here to make sure it is accurate
     */
    tls->data_header.rank = monitor_mpi_comm_rank();

    SetCSVFile(tls, cbtf_collector_unique_id, "csv");

    /* Open the file for writing */
    FILE *csvfileptr = fopen(tls->csv_path, "a");

    // Provenance.
    char provenance_csv_header[64] = {0};
    char provenance_csv_values[1024] = {0};
    strcat(provenance_csv_header,"host,pid,rank,tid,posix_tid,executable,total_time_seconds");
    sprintf(provenance_csv_values, "%s,%ld,%d,%d,%ld,%s,%f",
	tls->data_header.host, tls->data_header.pid,
	tls->data_header.rank, tls->data_header.omp_tid,
	tls->data_header.posix_tid, exename,
	(float)tls->thread_ttime/1000000000
	);
    fprintf(csvfileptr,"%s\n",provenance_csv_header);
    fprintf(csvfileptr,"%s\n",provenance_csv_values);

    if (print_to_stdout) {
    fprintf(stdout,"[%ld,%d] %s\n",
	tls->data_header.pid, tls->data_header.omp_tid, provenance_csv_header);
    fprintf(stdout,"[%ld,%d] %s\n",
	tls->data_header.pid, tls->data_header.omp_tid, provenance_csv_values);
    }


    // Rusage.
    char rusage_csv_header[48] = {0};
    char rusage_csv_values[128] = {0};
    strcat(rusage_csv_header,"maxrss_kB,utime_seconds,stime_seconds");
    overview_rusage_t ov_rusage = get_usage();
    sprintf(rusage_csv_values, "%ld,%lu.%06u,%lu.%06u",
	ov_rusage.ru_maxrss,
        ov_rusage.ru_utime.tv_sec,
	(unsigned int) ov_rusage.ru_utime.tv_usec,
	ov_rusage.ru_stime.tv_sec,
	(unsigned int) ov_rusage.ru_stime.tv_usec);
    fprintf(csvfileptr,"%s\n",rusage_csv_header);
    fprintf(csvfileptr,"%s\n",rusage_csv_values);

    if (print_to_stdout) {
    fprintf(stdout,"[%ld,%d] %s\n",
	tls->data_header.pid, tls->data_header.omp_tid, rusage_csv_header);
    fprintf(stdout,"[%ld,%d] %s\n",
	tls->data_header.pid, tls->data_header.omp_tid, rusage_csv_values);
    }

    // PAPI_dmem.
    char papi_dmem_csv_header[80] = {0};
    char papi_dmem_csv_values[128] = {0};
    strcat(papi_dmem_csv_header,"dmem_size_kB,dmem_resident_kB,dmem_high_water_mark_kB,dmem_shared_kB,dmem_heap_kB");
    overview_papi_dmem_t ov_dmem = get_papi_dmem_info();
    sprintf(papi_dmem_csv_values, "%lld,%lld,%lld,%lld,%lld",
	ov_dmem.size, ov_dmem.resident, ov_dmem.high_water_mark, ov_dmem.shared, ov_dmem.heap);
    fprintf(csvfileptr,"%s\n",papi_dmem_csv_header);
    fprintf(csvfileptr,"%s\n",papi_dmem_csv_values);

    if (print_to_stdout) {
    fprintf(stdout,"[%ld,%d] %s\n",
	tls->data_header.pid, tls->data_header.omp_tid, papi_dmem_csv_header);
    fprintf(stdout,"[%ld,%d] %s\n",
	tls->data_header.pid, tls->data_header.omp_tid, papi_dmem_csv_values);
    }

    // POSIX IO
    if (tls->total_posixio_time > 0) {
	char posixio_csv_header[64] = {0};
	char posixio_csv_values[64] = {0};
	strcat(posixio_csv_header,"io_total_time_seconds,read_time_seconds,write_time_seconds,read_bytes,write_bytes");
	sprintf(posixio_csv_values, "%f,%f,%f,%ld,%ld",
	    (tls->total_posixio_time > 0)?(float)tls->total_posixio_time/1000000000:0,
	    (tls->total_posixio_read_time > 0)?(float)tls->total_posixio_read_time/1000000000:0,
	    (tls->total_posixio_write_time > 0)?(float)tls->total_posixio_write_time/1000000000:0,
	    tls->total_posixio_bytes_read,
	    tls->total_posixio_bytes_written
	);
	fprintf(csvfileptr,"%s\n",posixio_csv_header);
	fprintf(csvfileptr,"%s\n",posixio_csv_values);

	if (print_to_stdout) {
	fprintf(stdout,"[%ld,%d] %s\n",
	tls->data_header.pid, tls->data_header.omp_tid, posixio_csv_header);
	fprintf(stdout,"[%ld,%d] %s\n",
	tls->data_header.pid, tls->data_header.omp_tid, posixio_csv_values);
	}
    }

    // MEM
    if (tls->mem_data.total_allocation_calls > 0) {
	char mem_alloc_csv_header[64] = {0};
	char mem_alloc_csv_values[64] = {0};
	strcat(mem_alloc_csv_header,"allocation_time_seconds,allocation_calls,allocation_bytes");
	sprintf(mem_alloc_csv_values, "%f,%ld,%ld",
		(tls->mem_data.total_allocation_time > 0)?(float)tls->mem_data.total_allocation_time/1000000000:0,
		(tls->mem_data.total_allocation_calls > 0)?tls->mem_data.total_allocation_calls:0,
		(tls->mem_data.total_bytes_allocated > 0)?tls->mem_data.total_bytes_allocated:0);
	fprintf(csvfileptr,"%s\n",mem_alloc_csv_header);
	fprintf(csvfileptr,"%s\n",mem_alloc_csv_values);

	if (print_to_stdout) {
	fprintf(stdout,"[%ld,%d] %s\n",
	tls->data_header.pid, tls->data_header.omp_tid,
	mem_alloc_csv_header);
	fprintf(stdout,"[%ld,%d] %s\n",
	tls->data_header.pid, tls->data_header.omp_tid,
	mem_alloc_csv_values);
	}
    }

    if (tls->mem_data.total_free_calls > 0) {
	char mem_free_csv_header[64] = {0};
	char mem_free_csv_values[64] = {0};
	strcat(mem_free_csv_header,"free_time_seconds,free_calls");
	sprintf(mem_free_csv_values, "%f,%ld",
		(tls->mem_data.total_free_time > 0)?(float)tls->mem_data.total_free_time/1000000000:0,
		(tls->mem_data.total_free_calls > 0)?tls->mem_data.total_free_calls:0);
	fprintf(csvfileptr,"%s\n",mem_free_csv_header);
	fprintf(csvfileptr,"%s\n",mem_free_csv_values);

	if (print_to_stdout) {
	fprintf(stdout,"[%ld,%d] %s\n",
	tls->data_header.pid, tls->data_header.omp_tid, mem_free_csv_header);
	fprintf(stdout,"[%ld,%d] %s\n",
	tls->data_header.pid, tls->data_header.omp_tid, mem_free_csv_values);
	}

    }

    // MPI: TODO: handle all calls seen.
    if (tls->total_mpi_time > 0) {
	char mpi_csv_header[64] = {0};
	char mpi_csv_values[64] = {0};
	strcat(mpi_csv_header,"total_mpi_time_seconds");
	sprintf(mpi_csv_values, "%f",
	    (tls->total_mpi_time > 0)?(float)tls->total_mpi_time/1000000000:0);

	fprintf(csvfileptr,"%s\n",mpi_csv_header);
	fprintf(csvfileptr,"%s\n",mpi_csv_values);

	if (print_to_stdout) {
	fprintf(stdout,"[%ld,%d] %s\n",
	tls->data_header.pid, tls->data_header.omp_tid, mpi_csv_header);
	fprintf(stdout,"[%ld,%d] %s\n",
	tls->data_header.pid, tls->data_header.omp_tid, mpi_csv_values);
	}
    }

    // PAPI
    char evName[256];
    char papi_csv_header[8192] = {0};
    char papi_csv_values[4096] = {0};
    int pNumEvents = PAPI_num_events(tls->EventSet);
    int PAPI_events[MAX_PAPI_EVENTS];
    int rval = PAPI_list_events(tls->EventSet, PAPI_events, &pNumEvents);
    int i;
    for (i = 0; i < pNumEvents; i++) {
	if (!PAPI_event_code_to_name(PAPI_events[i], evName)) {

	    strcat(papi_csv_header,evName);
	    char str[20];
	    sprintf(str, "%lld", tls->evalues[i]);
	    strcat(papi_csv_values,str);

	    if (i < pNumEvents-1) {
		strcat(papi_csv_header,",");
		strcat(papi_csv_values,",");
	    }
	}
    }

    fprintf(csvfileptr,"%s\n",papi_csv_header);
    fprintf(csvfileptr,"%s\n",papi_csv_values);

    if (print_to_stdout) {
    fprintf(stdout,"[%ld,%d] %s\n",
	tls->data_header.pid, tls->data_header.omp_tid, papi_csv_header);
    fprintf(stdout,"[%ld,%d] %s\n",
	tls->data_header.pid, tls->data_header.omp_tid, papi_csv_values);
    }

    // OMPT
    if (tls->itask_ttime > 0) {
	char ompt_csv_header[128] = {0};
	char ompt_csv_values[128] = {0};
	strcat(ompt_csv_header,"implicit_task_time_seconds,serial_time_seconds,barrier_time_seconds,wait_barrier_time_seconds,idle_time_seconds");
	sprintf(ompt_csv_values, "%f,%f,%f,%f,%f",
            (float)tls->itask_ttime/1000000000,
            (float)tls->serial_ttime/1000000000,
            (float)tls->barrier_ttime/1000000000,
            (float)tls->wbarrier_ttime/1000000000,
            (float)tls->idle_ttime/1000000000);

	fprintf(csvfileptr,"%s\n",ompt_csv_header);
	fprintf(csvfileptr,"%s\n",ompt_csv_values);

        if (print_to_stdout) {
	fprintf(stdout,"[%ld,%d] %s\n",
	tls->data_header.pid, tls->data_header.omp_tid, ompt_csv_header);
	fprintf(stdout,"[%ld,%d] %s\n",
	tls->data_header.pid, tls->data_header.omp_tid, ompt_csv_values);
	}
    }

    /* Close the file */
    fclose(csvfileptr);
}
