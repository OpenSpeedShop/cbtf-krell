/*******************************************************************************
** Copyright (c) 2010-13 The Krell Institute. All Rights Reserved.
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
 * Declaration of the CBTFW Runtime Offline.
 *
 */

#ifndef _CBTF_Runtime_Offline_
#define _CBTF_Runtime_Offline_

#include "KrellInstitute/Messages/OfflineEvents.h"

typedef struct cbtf_dlinfo_t cbtf_dlinfo;
typedef struct cbtf_dlinfoList_t cbtf_dlinfoList;

struct cbtf_dlinfo_t {
    uint64_t load_time;
    uint64_t unload_time;
    uint64_t addr_begin;
    uint64_t addr_end;
    char *name;
    void *handle;
};

struct cbtf_dlinfoList_t {
    cbtf_dlinfo cbtf_dlinfo_entry;
    cbtf_dlinfoList *cbtf_dlinfo_next;
};


/* Size of buffer (dso blob) to hold the dsos loaded into victim addressspace.
 * Computed in collector offline code as:
 * number of dso objects * sizeof dso object + the string lengths of the dso paths.
 * The collectors will send the current dsos buffer (blob) when ever
 * CBTF_OBJBufferSize is exceeded and start a new buffer.
 */
#define CBTF_OBJBufferSize (8*1024)
#define CBTF_MAXLINKEDOBJECTS 512

void cbtf_offline_start_sampling(const char* arguments);
void cbtf_offline_stop_sampling(const char* arguments, const bool finished);
void cbtf_offline_record_dso(const char* dsoname, uint64_t begin, uint64_t end,
                        uint8_t is_dlopen);
void cbtf_offline_defer_sampling(const int flag);
void cbtf_offline_sampling_status(CBTF_Monitor_Event_Type event, CBTF_Monitor_Status status);
void cbtf_offline_service_sampling_control(CBTF_Monitor_Status);
void cbtf_offline_notify_event(CBTF_Monitor_Event_Type event);

int CBTF_GetDLInfo(pid_t pid, char *path, uint64_t b_time, uint64_t e_time);
void CBTF_InitializeParameters (CBTF_Protocol_Offline_Parameters *info);

#endif /*_CBTF_Runtime_Offline_*/
