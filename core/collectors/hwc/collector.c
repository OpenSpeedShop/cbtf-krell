/*******************************************************************************
** Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
** Copyright (c) 2007,2008 William Hachfeld. All Rights Reserved.
** Copyright (c) 2007-2012 Krell Institute.  All Rights Reserved.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

#include "KrellInstitute/Messages/DataHeader.h"
#include "KrellInstitute/Messages/Hwc.h"
#include "KrellInstitute/Messages/Hwc_data.h"
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

/** String uniquely identifying this collector. */
const char* const cbtf_collector_unique_id = "hwc";


/** Type defining the items stored in thread-local storage. */
typedef struct {

    CBTF_DataHeader header;  /**< Header for following data blob. */
    CBTF_hwc_data data;        /**< Actual data blob. */

    CBTF_PCData buffer;      /**< PC sampling data buffer. */

    bool_t defer_sampling;
    int EventSet;
} TLS;

static int hwc_papi_init_done = 0;

#ifdef USE_EXPLICIT_TLS

/**
 * Key used to look up our thread-local storage. This key <em>must</em> be
 * unique from any other key used by any of the CBTF services.
 */
static const uint32_t TLSKey = 0x00001EF5;

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

    tls->header.time_begin = CBTF_GetTime();
    tls->header.time_end = 0;
    tls->header.addr_begin = ~0;
    tls->header.addr_end = 0;
    
    /* Initialize the actual data blob */
    tls->data.pc.pc_val = tls->buffer.pc;
    tls->data.count.count_val = tls->buffer.count;

    /* Re-initialize the actual data blob */
    tls->data.pc.pc_len = 0;
    tls->data.count.count_len = 0;

    /* Re-initialize the sampling buffer */
    tls->buffer.addr_begin = ~0;
    tls->buffer.addr_end = 0;
    tls->buffer.length = 0;
    memset(tls->buffer.hash_table, 0, sizeof(tls->buffer.hash_table));
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

static void send_samples (TLS* tls)
{
    Assert(tls != NULL);

    tls->header.id = strdup(cbtf_collector_unique_id);
    tls->header.time_end = CBTF_GetTime();
    tls->header.addr_begin = tls->buffer.addr_begin;
    tls->header.addr_end = tls->buffer.addr_end;
    tls->data.pc.pc_len = tls->buffer.length;
    tls->data.count.count_len = tls->buffer.length;

#ifndef NDEBUG
    if (getenv("CBTF_DEBUG_COLLECTOR") != NULL) {
        fprintf(stderr,"HWC send_samples DATA:\n");
        fprintf(stderr,"time_end(%#lu) addr range [%#lx, %#lx] pc_len(%d)\n",
            tls->header.time_end,tls->header.addr_begin,
	    tls->header.addr_end,tls->data.pc.pc_len);
    }
#endif

    cbtf_collector_send(&tls->header, (xdrproc_t)xdr_CBTF_hwc_data, &tls->data);

    /* Re-initialize the data blob's header */
    initialize_data(tls);
}


/**
 * PAPI event handler.
 *
 * Called by PAPI each time a sample is to be taken. Takes the program counter
 * (PC) address passed by PAPI and places it into the sample buffer. When the
 * sample buffer is full, it is sent to the framework for storage in the
 * experiment's database.
 *
 * @note    
 * 
 * @param context    Thread context at papi overflow.
 */
static void
hwcPAPIHandler(int EventSet, void* pc, long_long overflow_vector, void* context)
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);

    if(tls->defer_sampling == TRUE) {
        return;
    }
    
    /* Update the sampling buffer and check if it has been filled */
    if(CBTF_UpdatePCData((uint64_t)pc, &tls->buffer)) {

	/* Send these samples */
	send_samples(tls);
    }
}


/**
 * Called by the CBTF collector service in order to start data collection.
 */
void cbtf_collector_start(const CBTF_DataHeader* const header)
{
/**
 * Start sampling.
 *
 * Starts hardware counter (HWC) sampling for the thread executing this
 * function. Initializes the appropriate thread-local data structures and
 * then enables the sampling counter.
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

    tls->defer_sampling=FALSE;

    /* Decode the passed function arguments */
    CBTF_hwc_start_sampling_args args;
    memset(&args, 0, sizeof(args));
 

    /* set defaults */ 
    int hwc_papithreshold = THRESHOLD*2;
    char* hwc_papi_event = "PAPI_TOT_CYC";

    char* hwc_event_param = getenv("CBTF_HWC_EVENT");
    if (hwc_event_param != NULL) {
        hwc_papi_event=hwc_event_param;
    }

    const char* sampling_rate = getenv("CBTF_HWC_THRESHOLD");
    if (sampling_rate != NULL) {
        hwc_papithreshold=atoi(sampling_rate);
    }
    tls->data.interval = hwc_papithreshold;

#if defined(CBTF_SERVICE_USE_FILEIO)
    CBTF_SetSendToFile(cbtf_collector_unique_id, "cbtf-data");
#endif

    /* Initialize the actual data blob */
    memcpy(&tls->header, header, sizeof(CBTF_DataHeader));
    initialize_data(tls);
    tls->header.time_begin = CBTF_GetTime();

    if(hwc_papi_init_done == 0) {
	CBTF_init_papi();
	tls->EventSet = PAPI_NULL;
	hwc_papi_init_done = 1;
    }

    unsigned papi_event_code = get_papi_eventcode(hwc_papi_event);

    /* PAPI SETUP */
    CBTF_Create_Eventset(&tls->EventSet);
    CBTF_AddEvent(tls->EventSet, papi_event_code);
    CBTF_Overflow(tls->EventSet, papi_event_code,
		    hwc_papithreshold, hwcPAPIHandler);

    /* Begin sampling */
    tls->header.time_begin = CBTF_GetTime();
    CBTF_Start(tls->EventSet);
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
    if (hwc_papi_init_done == 0 || tls == NULL)
	return;

    tls->defer_sampling=TRUE;
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
    if (hwc_papi_init_done == 0 || tls == NULL)
	return;

    tls->defer_sampling=FALSE;
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

    if (tls->EventSet == PAPI_NULL) {
	/*fprintf(stderr,"hwc_stop_sampling RETURNS - NO EVENTSET!\n");*/
	/* we are called before eny events are set in papi. just return */
        return;
    }

    /* Stop sampling */
    CBTF_Stop(tls->EventSet, NULL);

    tls->header.time_end = CBTF_GetTime();

    /* Are there any unsent samples? */
    if(tls->buffer.length > 0) {

#ifndef NDEBUG
        if (getenv("CBTF_DEBUG_COLLECTOR") != NULL) {
            fprintf(stderr, "hwcsamp collector stop calls send_samples.\n");
        }
#endif

	/* Send these samples */
	send_samples(tls);
    }

    /* Destroy our thread-local storage */
#ifdef CBTF_SERVICE_USE_EXPLICIT_TLS
    destroy_explicit_tls();
#endif
}



#if defined (CBTF_SERVICE_USE_OFFLINE)
void cbtf_offline_service_start_timer()
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    if (hwc_papi_init_done == 0 || tls == NULL)
	return;
    CBTF_Start(tls->EventSet);
}

void cbtf_offline_service_stop_timer()
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    if (hwc_papi_init_done == 0 || tls == NULL)
	return;
    CBTF_Stop(tls->EventSet, NULL);
}
#endif
