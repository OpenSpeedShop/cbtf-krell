/*******************************************************************************
** Copyright (c) 2011-2015 The KrellInstitute. All Rights Reserved.
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
    CBTF_MEM_UNKNOWN = 0,
    CBTF_MEM_MALLOC,
    CBTF_MEM_CALLOC,
    CBTF_MEM_REALLOC,
    CBTF_MEM_FREE,
    CBTF_MEM_MEMALIGN,
    CBTF_MEM_POSIX_MEMALIGN
};

enum CBTF_mem_reason {
    CBTF_MEM_REASON_STILLALLOCATED = 0,	     /**< Memory still allocated from this event */
    CBTF_MEM_REASON_UNIQUE_CALLPATH,         /**< Unique call path. */
    CBTF_MEM_REASON_DURATION_OF_ALLOCATION,  /**< Duration time of allocation exceeds threshold */
    CBTF_MEM_REASON_MAX_ALLOCATION,	     /**< Max allocation for event callstack */
    CBTF_MEM_REASON_MIN_ALLOCATION,	     /**< Min allocation for event callstack */
    CBTF_MEM_REASON_HIGHWATER_SET,           /**< Set the highwater mark */
    CBTF_MEM_REASON_ALLOCATION_ADDRESS,      /**< Unique allocation address*/
    CBTF_MEM_REASON_UNKNOWN
};

/** Event structure describing a single mem call with details. */
/*  NOTE: There are numerous events traced with this collector.
 *  The collector will send these events to be filtered for events
 *  of interest.  Ultimately the blobs sent to the OSS database will
 *  will be filtered from these events and resent as new reduced blobs.
 *  Therefore the view code for the mem experiment will need to use
 *  the new more detailed CBTF_mem_event_of_interest event and not
 *  default to metrics based on time spent in the calls.
 */
struct CBTF_memt_event {
    uint64_t start_time;	/**< Start time of the call. */
    uint64_t stop_time;		/**< End time of the call. */
    uint64_t retval;		/**< return values can be a void* */
    uint64_t ptr;		/**< void* ptr type args */
    uint64_t size1;		/**< first size_t arg */
    uint64_t size2;		/**< second size_t arg */
    uint64_t total_allocation;  /**< current total allocation size at this time */
    uint64_t max;		/**< max allocation size seen*/
    uint64_t min;		/**< min allocation size seen*/
    uint32_t count;             /**< id of event as int. can serve as count. */
    uint16_t stacktrace;	/**< Index of the stack trace. */
    CBTF_mem_type mem_type;	/**< Memory call type */
    CBTF_mem_reason reason;	/**< Reason for interest */
};

/**
 * represents the previous mem event data.
 */
struct CBTF_mem_event {
    uint64_t start_time;        /**< Start time of the call. */
    uint64_t stop_time;         /**< End time of the call. */
    uint16_t stacktrace;        /**< Index of the stack trace. */
    uint64_t retval;            /**< return values can be a void* */
    uint64_t ptr;               /**< void* ptr type args */
    int      size1;             /**< first size_t arg */
    int      size2;             /**< second size_t arg */
    CBTF_mem_type mem_type;     /**< Memory call type */
};

/**
 * represents the previous mem experiment data.
 */
struct CBTF_mem_trace_data {
    uint64_t stacktraces<>;    /**< Stack traces. */
    CBTF_mem_event events<>;   /**< Mem call events. */
};


/** Structure of the blob containing extended trace performance data. */
struct CBTF_mem_exttrace_data {
    uint64_t stacktraces<>;    /**< Stack traces. */
    CBTF_memt_event events<>;  /**< Mem call events with details. */
};
