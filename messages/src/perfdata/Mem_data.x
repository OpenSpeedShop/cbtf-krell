/*******************************************************************************
** Copyright (c) 2011 The KrellInstitute. All Rights Reserved.
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
 * Specification of the Mem collector's data blobs.
 *
 */


enum CBTF_mem_type {
    CBTF_MEM_UNKNOWN=0,
    CBTF_MEM_MALLOC,
    CBTF_MEM_CALLOC,
    CBTF_MEM_REALLOC,
    CBTF_MEM_FREE,
    CBTF_MEM_MEMALIGN,
    CBTF_MEM_POSIX_MEMALIGN
};

/** Event structure describing a single I/O call. */
struct CBTF_mem_event {
    uint64_t start_time;  /**< Start time of the call. */
    uint64_t stop_time;   /**< End time of the call. */
    uint16_t stacktrace;  /**< Index of the stack trace. */
    CBTF_mem_type mem_type;
};

/** Event structure describing a single I/O call. */
struct CBTF_memt_event {
    uint64_t start_time;  /**< Start time of the call. */
    uint64_t stop_time;   /**< End time of the call. */
    uint16_t stacktrace;  /**< Index of the stack trace. */
    uint64_t retval;      /**< return values can be a void* */
    uint64_t ptr;         /**< void* ptr type args */
    int      size1;       /**< first size_t arg */
    int      size2;       /**< second size_t arg */
    CBTF_mem_type mem_type;
};


/** Structure of the blob containing trace performance data. */
struct CBTF_mem_trace_data {
    uint64_t stacktraces<>;  /**< Stack traces. */
    CBTF_mem_event events<>;       /**< IO call events. */
};

/** Structure of the blob containing extended trace performance data. */
struct CBTF_mem_exttrace_data {
    uint64_t stacktraces<>;  /**< Stack traces. */
    CBTF_memt_event events<>;       /**< IO call events. */
};

/** Structure of the blob containing profile performance data. */
struct CBTF_mem_profile_data {
    uint64_t stacktraces<>;  /**< Stack traces. */
    uint64_t time<>;         /**< time spent in this stack trace. */
    uint8_t count<>;      /**< Count for stack trace. Entries with a positive */
			  /**< count value represent the top of stack for a */
			  /**< specifc stack. If stack count exceeds 255 */
			  /**< a new entry is made in the sample buffer. */
			  /**< Positive entries the count buffer represent */
			  /**< the index into the address buffer (bt) for a */
			  /**< specifc stack */
};
