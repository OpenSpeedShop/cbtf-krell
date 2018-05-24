/*******************************************************************************
** Copyright (c) 2017 The Krell Institute. All Rights Reserved.
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
 * Specification of the Overview sampling collector data blobs.
 *
 */

%#include "KrellInstitute/Messages/Address.h"
%#include "KrellInstitute/Messages/Time.h"

/**
 * Enumeration of the different types of messages that are encapsulated within
 * this collector's blobs. See the note on CBTF_cuda_data for more information.
 */
enum CBTF_Overview_MessageTypes
{
    Samples = 0,
    SamplingConfig = 8
};


struct CBTF_overview_hwc_event {
    uint64_t hwccounts[6];
};


/** Event structure describing a single MPI call profile time. */
struct CBTF_overview_mpip_event {
    uint64_t time;	  /**< time of the call. */
    uint16_t stacktrace;  /**< Index of the stack trace. */
};

/** Structure of the blob containing mpi profile data. */
struct CBTF_overview_mpi_profile_data {
    uint64_t stacktraces<>;  /**< Stack traces. */
    uint64_t time<>;      /**< Total time for stack trace.*/
    uint8_t count<>;      /**< Count for stack trace. Entries with a positive */
			  /**< count value represent the top of stack for a */
			  /**< specifc stack. If stack count exceeds 255 */
			  /**< a new entry is made in the sample buffer. */
			  /**< Positive entries the count buffer represent */
			  /**< the index into the address buffer (bt) for a */
			  /**< specifc stack */
};


#if 0
/** Event structure describing a single I/O call profile time. */
struct CBTF_overview_iop_event {
    uint64_t time;	  /**< time of the call. */
    uint16_t stacktrace;  /**< Index of the stack trace. */
};
#endif

/** Structure of the blob containing posix IO profile data. */
struct CBTF_overview_io_profile_data {
    uint64_t stacktraces<>;  /**< Stack traces. */
    uint64_t time<>;      /**< Total time for stack trace.*/
    uint8_t count<>;      /**< Count for stack trace. Entries with a positive */
			  /**< count value represent the top of stack for a */
			  /**< specifc stack. If stack count exceeds 255 */
			  /**< a new entry is made in the sample buffer. */
			  /**< Positive entries the count buffer represent */
			  /**< the index into the address buffer (bt) for a */
			  /**< specifc stack */
};

/** TODO: define the fields we will record. How often etc etc etc. */


enum CBTF_overview_ompt_type {
    CBTF_OMPT_UNKNOWN=0,
    CBTF_OMPT_IDLE=1,
    CBTF_OMPT_BARRIER=2,
    CBTF_OMPT_WAIT_BARRIER=3,
    CBTF_OMPT_REGION=4,
    CBTF_OMPT_TASK=5
};

/** Event structure describing a single OMPT event profile time. */
/* NOTE: is this used or usable? */
struct CBTF_overview_omptp_event {
    uint64_t time;		/**< omp idel time. */
    uint16_t stacktrace;	/**< Index of the stack trace. */
    CBTF_overview_ompt_type  type;
};


/** Structure of the blob containing ompt profile performance data. */
struct CBTF_overview_ompt_profile_data {
    uint64_t stacktraces<>;  /**< Stack traces. */
    uint64_t time<>;      /**< Total time for stack trace.*/
    uint8_t count<>;      /**< Count for stack trace. Entries with a positive */
                          /**< count value represent the top of stack for a */
                          /**< specifc stack. If stack count exceeds 255 */
                          /**< a new entry is made in the sample buffer. */
                          /**< Positive entries the count buffer represent */
                          /**< the index into the address buffer (bt) for a */
                          /**< specifc stack */
};

/** Structure of the blob containing sample data for pc and hwc counters. */
struct CBTF_overview_hwc_sample_data {
    uint64_t interval;    /**< Sampling interval in nanoseconds. */
    uint64_t pc<>;        /**< Program counter (PC) addresses. */
    uint8_t count<>;      /**< Sample counts at those addresses. */    
    float  clock_mhz;
    CBTF_overview_hwc_event hwc_events<>;
};

/**
 * Message containing the event sampling configuration.
 */
struct CBTF_Overview_SamplingConfig
{
    /**
     * Sampling interval in nanoseconds. This is the time between consecutive
     * periodic (in time) samples.
     */
    uint64_t interval;

    /** Descriptions of the sampled events. */
    /**CUDA_EventDescription events<>; */
};

/**
 * Union of the different types of messages that are encapsulated within this
 * collector's blobs. See the note on CBTF_cuda_data for more information.
 */
union CBTF_overview_message switch (unsigned type)
{
    case  Samples: CBTF_overview_hwc_sample_data sample_data;
    case  SamplingConfig:  CBTF_Overview_SamplingConfig sampling_config;

    default: void;
};

/**
 * Structure of the blob containing our performance data.
 *
 * @note    This collector generates multiple types of performance data, which
 *          leads to its blobs being significantly more complex than those of
 *          the typical collector. All of this data is "packed" into a single
 *          performance data blob type via a XDR discriminated union in order
 *          to facilitate maximum reuse of existing collector infrastructure.
 *
 * @sa http://en.wikipedia.org/wiki/Tagged_union
 */
struct CBTF_overview_data
{
    /** Individual messages containing data gathered by this collector. */
    CBTF_overview_message messages<>;

    /**
     * Unique, null-terminated, stack traces referenced by the messages.
     *
     * @note    Because calls to the CUDA driver typically occur from a limited
     *          set of call sites, grouping them together in this way can result
     *          in significant data compression.
     */
    CBTF_Protocol_Address stack_traces<>;
};
