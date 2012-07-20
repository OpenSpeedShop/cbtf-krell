/*******************************************************************************
** Copyright (c) 2012 Argo Navis Technologies. All Rights Reserved.
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

/** @file Implementation of the CUDA collector. */

#include <cupti.h>
#include <inttypes.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "KrellInstitute/Services/Assert.h"
#include "KrellInstitute/Services/Collector.h"
#include "KrellInstitute/Services/Context.h"
#include "KrellInstitute/Services/Data.h"
#include "KrellInstitute/Services/Time.h"
#include "KrellInstitute/Services/TLS.h"
#include "KrellInstitute/Services/Unwind.h"

#include "CUDA_data.h"



/**
 * Maximum number of individual (CBTF_cuda_message) messages contained within
 * each (CBTF_cuda_data) performance data blob. 
 *
 * @note    Currently there is no specific basis for the selection of this
 *          value other than a vague notion that is seems about right. In
 *          the future, performance testing should be done to determine an
 *          optimal value.
 */
#define MAX_MESSAGES_PER_BLOB  128

/**
 * Maximum number of (CBTF_Protocol_Address) addresses contained within each
 * (CBTF_cuda_data) performance data blob.
 *
 * @note    Currently there is no specific basis for the selection of this
 *          value other than a vague notion that is seems about right. In
 *          the future, performance testing should be done to determine an
 *          optimal value.
 */
#define MAX_ADDRESSES_PER_BLOB  1024



/**
 * Checks that the given CUPTI function call returns the value "CUPTI_SUCCESS".
 * If the call was unsuccessful, the returned error is reported on the standard
 * error stream and the application is aborted.
 *
 * @param x    CUPTI function call to be checked.
 */
#define CUPTI_CHECK(x)                                              \
    do {                                                            \
        CUptiResult retval = x;                                     \
        if (retval != CUPTI_SUCCESS)                                \
        {                                                           \
            const char* description = NULL;                         \
            if (cuptiGetResultString(retval, &description) ==       \
                CUPTI_SUCCESS)                                      \
            {                                                       \
                fprintf(stderr, "[CBTF/CUDA] %s(): %s = %d (%s)\n", \
                        __func__, #x, retval, description);         \
            }                                                       \
            else                                                    \
            {                                                       \
                fprintf(stderr, "[CBTF/CUDA] %s(): %s = %d\n",      \
                        __func__, #x, retval);                      \
            }                                                       \
            fflush(stderr);                                         \
            abort();                                                \
        }                                                           \
    } while (0)



/**
 * Checks that the given pthread function call returns the value "0". If the
 * call was unsuccessful, the returned error is reported on the standard error
 * stream and the application is aborted.
 *
 * @param x    Pthread function call to be checked.
 */
#define PTHREAD_CHECK(x)                                   \
    do {                                                   \
        int retval = x;                                    \
        if (retval != 0)                                   \
        {                                                  \
            fprintf(stderr, "[CBTF/CUDA] %s(): %s = %d\n", \
                    __func__, #x, retval);                 \
            fflush(stderr);                                \
            abort();                                       \
        }                                                  \
    } while (0)



/** String uniquely identifying this collector. */
const char* const cbtf_collector_unique_id = "cuda";

/**
 * The number of threads for which are are collecting data (actively or not).
 * This value is atomically incremented in cbtf_collector_start(), decremented
 * in cbtf_collector_stop(), and is used by those functions to determine when
 * to perform process-wide initialization and finalization.
 */
static struct {
    int value;
    pthread_mutex_t mutex;
} thread_count = { 0, PTHREAD_MUTEX_INITIALIZER };

/** Flag indicating if debugging is enabled. */
static bool debug = FALSE;

/** CUPTI subscriber handle for this collector. */
static CUpti_SubscriberHandle cupti_subscriber_handle;



/** Type defining the data stored in thread-local storage. */
typedef struct {

    /** Flag indicating if data collection is paused. */
    bool paused;

    /**
     * Performance data header to be applied to this thread's performance data.
     * All of the fields except [addr|time]_[begin|end] are constant throughout
     * data collection. These exceptions are updated dynamically by the various
     * collection routines.
     */
    CBTF_DataHeader data_header;

    /**
     * Current performance data blob for this thread. Messages are added by the
     * various collection routines. It is sent when full, or upon completion of
     * data collection.
     */
    CBTF_cuda_data data;

    /**
     * Individual messages containing data gathered by this collector. Pointed
     * to by the performance data blob above.
     */
    CBTF_cuda_message messages[MAX_MESSAGES_PER_BLOB];

    /**
     * Unique, null-terminated, stack traces referenced by the messages. Pointed
     * to by the performance data blob above.
     */
    CBTF_Protocol_Address stack_traces[MAX_ADDRESSES_PER_BLOB];
    
} TLS;

#if defined(USE_EXPLICIT_TLS)

/**
 * Key used to look up our thread-local storage. This key <em>must</em> be
 * unique from any other key used by any of the CBTF services.
 */
static const uint32_t TLSKey = 0xBADC00DA;

#else

/** Thread-local storage. */
static __thread TLS the_tls;

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

    tls->data_header.time_begin = ~0;
    tls->data_header.time_end = 0;
    tls->data_header.addr_begin = ~0;
    tls->data_header.addr_end = 0;
    
    tls->data.messages.messages_len = 0;
    tls->data.messages.messages_val = tls->messages;
    
    tls->data.stack_traces.stack_traces_len = 0;
    tls->data.stack_traces.stack_traces_val = tls->stack_traces;
    
    memset(tls->stack_traces, 0, sizeof(tls->stack_traces));
}



/**
 * Send the performance data blob contained within the given thread-local
 * storage. The blob is re-initialized (cleared) after being sent. Nothing
 * is sent if the blob is empty.
 *
 * @param tls    Thread-local storage containing data to be sent.
 */
static void send_data(TLS* tls)
{
    Assert(tls != NULL);

    if (tls->data.messages.messages_len > 0)
    {
        cbtf_collector_send(
            &tls->data_header, (xdrproc_t)xdr_CBTF_cuda_data, &tls->data
            );
        initialize_data(tls);
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
static CBTF_cuda_message* add_message(TLS* tls)
{
    Assert(tls != NULL);

    if (tls->data.messages.messages_len == MAX_MESSAGES_PER_BLOB)
    {
        send_data(tls);
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
inline void update_header_with_time(TLS* tls, CBTF_Protocol_Time time)
{
    Assert(tls != NULL);

    if (time < tls->data_header.time_begin)
    {
        tls->data_header.time_begin = time;
    }
    if (time >= tls->data_header.time_end)
    {
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
inline void update_header_with_address(TLS* tls, CBTF_Protocol_Address addr)
{
    Assert(tls != NULL);

    if (addr < tls->data_header.addr_begin)
    {
        tls->data_header.addr_begin = addr;
    }
    if (addr >= tls->data_header.addr_end)
    {
        tls->data_header.addr_end = addr + 1;
    }
}



/**
 * Add a new stack trace for the current call site to the performance data
 * blob contained within the given thread-local storage.
 *
 * @param tls    Thread-local storage to which the stack trace is to be added.
 * @return       Index of this call site within the performance data blob.
 */
static uint32_t add_current_call_site(TLS* tls)
{
    Assert(tls != NULL);

    /* Get the stack trace for the current call site */

    ucontext_t context;
    int frame_count = 0;
    uint64_t frame_buffer[CBTF_ST_MAXFRAMES];
    
    CBTF_GetContext(&context);

    CBTF_GetStackTraceFromContext(
        &context, FALSE, 1, CBTF_ST_MAXFRAMES, &frame_count, frame_buffer
        );
    
    /* Search for this stack trace amongst the existing stack traces */
    
    int i, j;
    
    /* Iterate over the addresses in the existing stack traces */
    for (i = 0, j = 0; i < MAX_ADDRESSES_PER_BLOB; ++i)
    {
        /* Is this the terminating null of an existing stack trace? */
        if (tls->stack_traces[i] == 0)
        {
            /*
             * Terminate the search if a complete match has been found between
             * this stack trace and the existing stack trace. Otherwise check
             * for a null in the first or last entry, or for consecutive nulls,
             * all of which indicate the end of the existing stack traces, and
             * the need to add this stack trace to the existing stack traces.
             */
            if (j == frame_count)
            {
                break;
            }
            else if ((i == 0) || 
                     (i == (MAX_ADDRESSES_PER_BLOB - 1)) ||
                     (tls->stack_traces[i - 1] == 0))
            {
                /*
                 * Send performance data for this thread if there isn't enough
                 * room in the existing stack traces to add this stack trace.
                 * Doing so frees up enough space for this stack trace.
                 */
                if ((i + frame_count) >= MAX_ADDRESSES_PER_BLOB)
                {
                    send_data(tls);
                    i = 0;
                }

                /* Add this stack trace to the existing stack traces */
                for (j = 0; j < frame_count; ++j, ++i)
                {
                    tls->stack_traces[i] = frame_buffer[j];
                }
                tls->stack_traces[i++] = 0;
                
                break;
            }
        }
        else
        {
            /*
             * Advance the pointer within this stack trace if the current
             * address within this stack trace matches the current address
             * within the existing stack trace. Otherwise reset the pointer
             * to zero.
             */
            if (frame_buffer[j] == tls->stack_traces[i])
            {
                ++j;
            }
            else
            {
                j = 0;
            } 
        }
    }

    /* Return the index of this stack trace within the existing stack traces */
    return i - frame_count;
}



/**
 * Callback invoked by CUPTI every time a CUDA event occurs for which we have
 * a subscription. Subscriptions are setup within cbtf_collector_start(), and
 * are setup once for the entire process.
 *
 * @param userdata    User data supplied at subscription of the callback.
 * @param domain      Domain of the callback.
 * @param id          ID of the callback.
 * @param data        Data passed to the callback.
 */
static void cupti_callback(void* userdata,
                           CUpti_CallbackDomain domain,
                           CUpti_CallbackId id,
                           const void* data)
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);

    /* Do nothing if data collection is paused for this thread */
    if (tls->paused)
    {
        return;
    }

    /* Determine the CUDA event that has occurred and handle it */
    switch (domain)
    {

    case CUPTI_CB_DOMAIN_DRIVER_API:
        {
            const CUpti_CallbackData* const cbdata = (CUpti_CallbackData*)data;
            
            switch (id)
            {



            case CUPTI_DRIVER_TRACE_CBID_cuLaunchKernel :
                if (cbdata->callbackSite == CUPTI_API_ENTER)
                {
                    const cuLaunchKernel_params* const params =
                        (cuLaunchKernel_params*)cbdata->functionParams;

#if !defined(NDEBUG)
                    if (debug)
                    {
                        printf("[CBTF/CUDA] enter cuLaunchKernel()\n");
                    }
#endif
                    
                    /* Add a message for this event */
                    
                    CBTF_cuda_message* raw_message = add_message(tls);
                    Assert(raw_message != NULL);
                    raw_message->type = EnqueueRequest;
                    
                    CUDA_EnqueueRequest* message = 
                        &raw_message->CBTF_cuda_message_u.enqueue_request;
                    
                    message->type = LaunchKernel;
                    message->time = CBTF_GetTime();

                    CUPTI_CHECK(cuptiGetStreamId(cbdata->context,
                                                 params->hStream,
                                                 &message->stream));

                    message->call_site = add_current_call_site(tls);
                    
                    update_header_with_time(tls, message->time);
                }
                break;
                


            case CUPTI_DRIVER_TRACE_CBID_cuModuleGetFunction :
                if (cbdata->callbackSite == CUPTI_API_EXIT)
                {
                    const cuModuleGetFunction_params* const params =
                        (cuModuleGetFunction_params*)cbdata->functionParams;
                    
#if !defined(NDEBUG)
                    if (debug)
                    {
                        printf("[CBTF/CUDA] exit cuModuleGetFunction()\n");
                    }
#endif
                    
                    /* Add a message for this event */
                    
                    CBTF_cuda_message* raw_message = add_message(tls);
                    Assert(raw_message != NULL);
                    raw_message->type = ResolvedFunction;
                    
                    CUDA_ResolvedFunction* message = 
                        &raw_message->CBTF_cuda_message_u.resolved_function;
                    
                    message->time = CBTF_GetTime();
                    message->module_handle = 
                        (CBTF_Protocol_Address)params->hmod;
                    message->function = (char*)params->name;
                    message->handle = (CBTF_Protocol_Address)*(params->hfunc);
                    
                    update_header_with_time(tls, message->time);
                }
                break;



            case CUPTI_DRIVER_TRACE_CBID_cuModuleLoad :
                if (cbdata->callbackSite == CUPTI_API_EXIT)
                {
                    const cuModuleLoad_params* const params =
                        (cuModuleLoad_params*)cbdata->functionParams;
                    
#if !defined(NDEBUG)
                    if (debug)
                    {
                        printf("[CBTF/CUDA] exit cuModuleLoad()\n");
                    }
#endif
                    
                    /* Add a message for this event */
                    
                    CBTF_cuda_message* raw_message = add_message(tls);
                    Assert(raw_message != NULL);
                    raw_message->type = LoadedModule;
                    
                    CUDA_LoadedModule* message =  
                        &raw_message->CBTF_cuda_message_u.loaded_module;
                    
                    message->time = CBTF_GetTime();
                    message->module.path = (char*)(params->fname);
                    message->module.checksum = 0;
                    message->handle = (CBTF_Protocol_Address)*(params->module);
                    
                    update_header_with_time(tls, message->time);
                }
                break;



            case CUPTI_DRIVER_TRACE_CBID_cuModuleLoadData :
                if (cbdata->callbackSite == CUPTI_API_EXIT)
                {
                    const cuModuleLoadData_params* const params =
                        (cuModuleLoadData_params*)cbdata->functionParams;
                    
#if !defined(NDEBUG)
                    if (debug)
                    {
                        printf("[CBTF/CUDA] exit cuModuleLoadData()\n");
                    }
#endif
                        
                    /* Add a message for this event */
                    
                    CBTF_cuda_message* raw_message = add_message(tls);
                    Assert(raw_message != NULL);
                    raw_message->type = LoadedModule;
                    
                    CUDA_LoadedModule* message =  
                        &raw_message->CBTF_cuda_message_u.loaded_module;
                    
                    static char* const kModulePath = 
                        "<Module from Embedded Data>";
                    
                    message->time = CBTF_GetTime();
                    message->module.path = kModulePath;
                    message->module.checksum = 0;
                    message->handle = (CBTF_Protocol_Address)*(params->module);
                        
                    update_header_with_time(tls, message->time);
                }
                break;
                
                
                
            case CUPTI_DRIVER_TRACE_CBID_cuModuleLoadDataEx :
                if (cbdata->callbackSite == CUPTI_API_EXIT)
                {
                    const cuModuleLoadDataEx_params* const params =
                        (cuModuleLoadDataEx_params*)cbdata->functionParams;
                    
#if !defined(NDEBUG)
                    if (debug)
                    {
                        printf("[CBTF/CUDA] exit cuModuleLoadDataEx()\n");
                    }
#endif
                    
                    /* Add a message for this event */
                    
                    CBTF_cuda_message* raw_message = add_message(tls);
                    Assert(raw_message != NULL);
                    raw_message->type = LoadedModule;
                    
                    CUDA_LoadedModule* message =  
                        &raw_message->CBTF_cuda_message_u.loaded_module;
                    
                    static char* const kModulePath = 
                        "<Module from Embedded Data>";
  
                    message->time = CBTF_GetTime();
                    message->module.path = kModulePath;
                    message->module.checksum = 0;
                    message->handle = (CBTF_Protocol_Address)*(params->module);
                        
                    update_header_with_time(tls, message->time);
                }
                break;
                
                

            case CUPTI_DRIVER_TRACE_CBID_cuModuleLoadFatBinary :
                if (cbdata->callbackSite == CUPTI_API_EXIT)
                {
                    const cuModuleLoadFatBinary_params* const params =
                        (cuModuleLoadFatBinary_params*)cbdata->functionParams;
                    
#if !defined(NDEBUG)
                    if (debug)
                    {
                        printf("[CBTF/CUDA] exit cuModuleLoadFatBinary()\n");
                    }
#endif
                    
                    /* Add a message for this event */
                    
                    CBTF_cuda_message* raw_message = add_message(tls);
                    Assert(raw_message != NULL);
                    raw_message->type = LoadedModule;
                    
                    CUDA_LoadedModule* message =  
                        &raw_message->CBTF_cuda_message_u.loaded_module;
                        
                    static char* const kModulePath = "<Module from Fat Binary>";
                        
                    message->time = CBTF_GetTime();
                    message->module.path = kModulePath;
                    message->module.checksum = 0;
                    message->handle = (CBTF_Protocol_Address)*(params->module);
                        
                    update_header_with_time(tls, message->time);
                }
                break;
                
                
                
            case CUPTI_DRIVER_TRACE_CBID_cuModuleUnload :
                {
                    if (cbdata->callbackSite == CUPTI_API_EXIT)
                    {
                        const cuModuleUnload_params* const params =
                            (cuModuleUnload_params*)cbdata->functionParams;
                        
#if !defined(NDEBUG)
                        if (debug)
                        {
                            printf("[CBTF/CUDA] exit cuModuleUnload()\n");
                        }
#endif
                        
                        /* Add a message for this event */
                        
                        CBTF_cuda_message* raw_message = add_message(tls);
                        Assert(raw_message != NULL);
                        raw_message->type = UnloadedModule;
                        
                        CUDA_UnloadedModule* message =  
                            &raw_message->CBTF_cuda_message_u.unloaded_module;
                        
                        message->time = CBTF_GetTime();
                        message->handle = (CBTF_Protocol_Address)params->hmod;
                        
                        update_header_with_time(tls, message->time);                                 }
                }
                break;
                


            }
        }
        break;

    default:
        break;        
    }
}



/**
 * Parse the configuration string that was passed into this collector.
 *
 * @param configuration    Configuration string passed into this collector.
 */
static void parse_configuration(const char* const configuration)
{
#if !defined(NDEBUG)
    if (debug)
    {
        printf("[CBTF/CUDA] parse_configuration(\"%s\")\n", configuration);
    }
#endif

    /* ... parse options for future CPU/GPU hardware counter sampling ... */
}



/**
 * Called by the CBTF collector service in order to start data collection.
 */
void cbtf_collector_start(const CBTF_DataHeader* const header)
{
#if !defined(NDEBUG)
    if (debug)
    {
        printf("[CBTF/CUDA] cbtf_collector_start()\n");
    }
#endif
    
    /* Atomically increment the active thread count */

    PTHREAD_CHECK(pthread_mutex_lock(&thread_count.mutex));
    
#if !defined(NDEBUG)
    if (debug)
    {
        printf("[CBTF/CUDA] cbtf_collector_start(): "
               "thread_count.value = %d --> %d\n",
               thread_count.value, thread_count.value + 1);
    }
#endif
    
    if (thread_count.value == 0)
    {
        /* Should debugging be enabled? */
        debug = (getenv("CBTF_DEBUG_COLLECTOR") == NULL);
        
#if !defined(NDEBUG)
        if (debug)
        {
            printf("[CBTF/CUDA] cbtf_collector_start()\n");
        }
#endif

        /** Obtain our configuration string from the environment and parse it */
        const char* const configuration = getenv("CBTF_CUDA_CONFIG");
        if (configuration != NULL)
        {
            parse_configuration(configuration);
        }

        /* Subscribe to the CUPTI callbacks of interest */
        
        CUPTI_CHECK(cuptiSubscribe(&cupti_subscriber_handle,
                                   cupti_callback, NULL));

        CUPTI_CHECK(cuptiEnableCallback(
                        1, cupti_subscriber_handle,
                        CUPTI_CB_DOMAIN_DRIVER_API,
                        CUPTI_DRIVER_TRACE_CBID_cuLaunchKernel
                        ));

        CUPTI_CHECK(cuptiEnableCallback(
                        1, cupti_subscriber_handle,
                        CUPTI_CB_DOMAIN_DRIVER_API,
                        CUPTI_DRIVER_TRACE_CBID_cuModuleGetFunction
                        ));

        CUPTI_CHECK(cuptiEnableCallback(
                        1, cupti_subscriber_handle,
                        CUPTI_CB_DOMAIN_DRIVER_API,
                        CUPTI_DRIVER_TRACE_CBID_cuModuleLoad
                        ));

        CUPTI_CHECK(cuptiEnableCallback(
                        1, cupti_subscriber_handle,
                        CUPTI_CB_DOMAIN_DRIVER_API,
                        CUPTI_DRIVER_TRACE_CBID_cuModuleLoadData
                        ));

        CUPTI_CHECK(cuptiEnableCallback(
                        1, cupti_subscriber_handle,
                        CUPTI_CB_DOMAIN_DRIVER_API,
                        CUPTI_DRIVER_TRACE_CBID_cuModuleLoadDataEx
                        ));

        CUPTI_CHECK(cuptiEnableCallback(
                        1, cupti_subscriber_handle,
                        CUPTI_CB_DOMAIN_DRIVER_API,
                        CUPTI_DRIVER_TRACE_CBID_cuModuleLoadFatBinary
                        ));

        CUPTI_CHECK(cuptiEnableCallback(
                        1, cupti_subscriber_handle,
                        CUPTI_CB_DOMAIN_DRIVER_API,
                        CUPTI_DRIVER_TRACE_CBID_cuModuleUnload
                        ));
    }

    thread_count.value++;

    PTHREAD_CHECK(pthread_mutex_unlock(&thread_count.mutex));
    
    /* Create, zero-initialize, and access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = malloc(sizeof(TLS));
    Assert(tls != NULL);
    OpenSS_SetTLS(TLSKey, tls);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);
    memset(tls, 0, sizeof(TLS));

    /* Copy the header into our thread-local storage for future use */
    memcpy(&tls->data_header, header, sizeof(CBTF_DataHeader));

    /* Initialize our performance data header and blob */
    initialize_data(tls);

    /* ... enable future CPU/GPU hardware counter sampling ... */
    
    /* Resume (start) data collection for this thread */
    tls->paused = FALSE;
}



/**
 * Called by the CBTF collector service in order to pause data collection.
 */
void cbtf_collector_pause()
{
#if !defined(NDEBUG)
    if (debug)
    {
        printf("[CBTF/CUDA] cbtf_collector_pause()\n");
    }
#endif
                
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);

    /* ... disable future CPU/GPU hardware counter sampling ... */

    /* Pause data collection for this thread */
    tls->paused = TRUE;
}



/**
 * Called by the CBTF collector service in order to resume data collection.
 */
void cbtf_collector_resume()
{
#if !defined(NDEBUG)
    if (debug)
    {
        printf("[CBTF/CUDA] cbtf_collector_resume()\n");
    }
#endif

    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);

    /* ... enable future CPU/GPU hardware counter sampling ... */

    /* Resume data collection for this thread */
    tls->paused = FALSE;
}



/**
 * Called by the CBTF collector service in order to stop data collection.
 */
void cbtf_collector_stop()
{
#if !defined(NDEBUG)
    if (debug)
    {
        printf("[CBTF/CUDA] cbtf_collector_stop()\n");
    }
#endif
    
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);

    /* ... disable future CPU/GPU hardware counter sampling ... */

    /* Pause (stop) data collection for this thread */
    tls->paused = TRUE;

    /* Send any remaining performance data for this thread */
    send_data(tls);
    
    /* Destroy our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    free(tls);
    OpenSS_SetTLS(TLSKey, NULL);
#endif
    
    /* Atomically decrement the active thread count */
    
    PTHREAD_CHECK(pthread_mutex_lock(&thread_count.mutex));
    
#if !defined(NDEBUG)
    if (debug)
    {
        printf("[CBTF/CUDA] cbtf_collector_stop(): "
               "thread_count.value = %d --> %d\n",
               thread_count.value, thread_count.value - 1);
    }
#endif
    
    thread_count.value--;

    if (thread_count.value == 0)
    {
        /* Unsubscribe from all CUPTI callbacks */
        CUPTI_CHECK(cuptiUnsubscribe(cupti_subscriber_handle));
    }
    
    PTHREAD_CHECK(pthread_mutex_unlock(&thread_count.mutex));
}
