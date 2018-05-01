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

/** @file Definition of the CBTF collector service API. */

#include <stdbool.h>
#include <rpc/rpc.h>

#include "KrellInstitute/Messages/DataHeader.h"



/**
 * Called by the collector to send performance data. The necessary data header
 * is prepended to the actual data and both are XDR encoded before being sent.
 *
 * @param header     Performance data header to apply to this data.
 * @param xdrproc    XDR procedure for the passed data structure.
 * @param data       Pointer to the data structure to be sent.
 */
extern void cbtf_collector_send(const CBTF_DataHeader* header,
                                const xdrproc_t xdrproc, const void* data);



/**
 * A short string, provided by the collector and containing only lower-case
 * letters, that uniquely identifies this collector. E.g. "pcsamp".
 */
extern const char* const cbtf_collector_unique_id;



/**
 * Called by the CBTF collector service in order to start data collection.
 * The collector should perform whatever actions are necessary to begin the
 * collection of data for the active thread.
 *
 * @param header    Partial performance data header that should be applied
 *                  to all of this collector's data. The time_begin, time_end,
 *                  addr_begin, and addr_end fields will be zero initialized
 *                  and must be provided by the collector before sending data.
 */
extern void cbtf_collector_start(const CBTF_DataHeader const* header);



/**
 * Called by the CBTF collector service in order to pause data collection.
 * The collector should perform whatever actions are necessary to pause the
 * collection of data for the active thread. Any data collected up to this
 * point should be sent.
 */
extern void cbtf_collector_pause();



/**
 * Called by the CBTF collector service in order to resume data collection.
 * The collector should perform whatever actions are necessary ot resume the
 * collection of data for the active thread.
 */
extern void cbtf_collector_resume();



/**
 * Called by the CBTF collector service in order to stop data collection.
 * The collector should perform whatever actions are necessary to stop the
 * collection of data for the active thread. Any data collected up to this
 * point should be sent.
 */
extern void cbtf_collector_stop();


void cbtf_record_dsos();
void cbtf_timer_service_start_sampling(const char* arguments);
void cbtf_timer_service_stop_sampling(const char* arguments);

#if defined(CBTF_SERVICE_USE_MRNET)
bool connect_to_mrnet();
void send_attached_to_threads_message();
#endif

void set_mpi_flag(bool);
void set_ompt_flag(bool);
bool get_ompt_flag();
void set_ompt_thread_finished(bool);
void cbtf_collector_set_openmp_threadid(int32_t);
void set_threaded_flag(bool);
void set_threaded_mrnet_connection();
void cbtf_offline_sent_data(int sent_data);
void cbtf_set_connected_to_mrnet();
