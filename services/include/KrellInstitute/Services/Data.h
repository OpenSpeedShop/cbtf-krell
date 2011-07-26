/*******************************************************************************
** Copyright (c) 2010 The Krell Institute. All Rights Reserved.
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
 * Declaration of the PCData.
 *
 */

#ifndef _CBTF_PCData_
#define _CBTF_PCData_

#include "Common.h"
#include <stdbool.h>

/** Number of entries in the sample buffer. */
#define CBTF_PCBufferSize (1024 * CBTF_BlobSizeFactor)
/** Number of entries in the hardware counter sample buffer. */
#define CBTF_HWCPCBufferSize (128 * CBTF_BlobSizeFactor)

/** Number of entries in the hash table. */
#define CBTF_PCHashTableSize (CBTF_PCBufferSize + (CBTF_PCBufferSize / 4))
/** Number of entries in the hardware counter hash table. */
#define CBTF_HWCPCHashTableSize (CBTF_HWCPCBufferSize + (CBTF_HWCPCBufferSize / 4))

/** Type representing PC sampling data (PCs and their respective counts). */
typedef struct {
    uint64_t addr_begin;  /**< Beginning of gathered data's address range. */
    uint64_t addr_end;    /**< End of gathered data's address range. */

    uint16_t length;  /**< Actual used length of the PC and count arrays. */

    uint64_t pc[CBTF_PCBufferSize];    /**< Program counter (PC) addresses. */
    uint8_t count[CBTF_PCBufferSize];  /**< Sample count at each address. */

    /** Hash table mapping PC addresses to their array index. */
    unsigned hash_table[CBTF_PCHashTableSize];

} CBTF_PCData;


/** Type representing PC sampling data (PCs and their respective hwc counts). */
typedef uint64_t CBTF_evcounts[6];
typedef struct {

    uint64_t addr_begin;  /**< Beginning of gathered data's address range. */
    uint64_t addr_end;    /**< End of gathered data's address range. */

    uint16_t length;  /**< Actual used length of the PC and count arrays. */

    uint64_t pc[CBTF_HWCPCBufferSize];    /**< Program counter (PC) addresses. */
    uint8_t count[CBTF_HWCPCBufferSize];  /**< Sample count at each address. */
    CBTF_evcounts hwccounts[CBTF_HWCPCBufferSize];

    /** Hash table mapping PC addresses to their array index. */
    unsigned hash_table[CBTF_HWCPCHashTableSize];

} CBTF_HWCPCData;

/** Type representing StackTrace sampling data. */
#define CBTF_ST_BufferSize  1024
#define CBTF_ST_MAXFRAMES 32
typedef struct {
    uint64_t addr_begin;  /**< Beginning of gathered data's address range. */
    uint64_t addr_end;    /**< End of gathered data's address range. */

    uint64_t bt[CBTF_ST_BufferSize];    /**< Stack trace (PC) addresses. */
    uint8_t  count[CBTF_ST_BufferSize]; /**< count value greater than 0 is top */
                                        /**< of stack. A count of 255 indicates */
                                /**< another instance of this stack may */
                                /**< exist in buffer bt. */

} CBTF_StackTraceData;

bool CBTF_UpdatePCData(uint64_t, CBTF_PCData*);
bool CBTF_UpdateHWCPCData(uint64_t, CBTF_HWCPCData*, long long* );

#endif
