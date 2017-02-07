/*******************************************************************************
** Copyright (c) 2016  The Krell Institute. All Rights Reserved.
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
 * omptp collector support
 *
 **/

#include <stdbool.h> /* C99 std */
#include <inttypes.h>
#include "KrellInstitute/Messages/Ompt.h"
#include "KrellInstitute/Messages/Ompt_data.h"

#define MAX_REGIONS 8		  /* maximum number of nested regions */
#define MaxFramesPerStackTrace 32 /* maximum number of frames per stacktrace */

typedef struct CBTF_omptp_region {
  uint64_t id;
  uint64_t stacktrace[MaxFramesPerStackTrace];
  unsigned stacktrace_size;
} CBTF_omptp_region;

// these external calls are expected in the cbtf collectors either
// as implementations or as empty functions.
//
// pause and resume collection.
extern void cbtf_collector_pause();
extern void cbtf_collector_resume();

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
extern void IDLE(bool);
extern void BARRIER(bool);
extern void WAIT_BARRIER(bool);

extern void omptp_record_event(const CBTF_omptp_event*, uint64_t*, unsigned);
extern void omptp_start_event(CBTF_omptp_event*, uint64_t, uint64_t*, unsigned*);
