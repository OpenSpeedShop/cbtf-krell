/*******************************************************************************
** Copyright (c) 2015 The KrellInstitute. All Rights Reserved.
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
 * Specification of the Ompt collector's data blobs.
 *
 */


enum CBTF_ompt_type {
    CBTF_OMPT_UNKNOWN=0
};

/** Event structure describing a single OMPT event profile time. */
struct CBTF_omptp_event {
    uint64_t time;   /**< time of the call. */
    uint16_t stacktrace;  /**< Index of the stack trace. */
};

/** Event structure describing a single OMPT event. */
struct CBTF_ompt_event {
    uint64_t start_time;  /**< Start time of the call. */
    uint64_t stop_time;   /**< End time of the call. */
    uint32_t parID;	  /**< parallel region id. */
    uint32_t taskID;	  /**< task id. */
    uint16_t stacktrace;  /**< Index of the stack trace. */
    CBTF_ompt_type ompt_type;
};

/** Structure of the blob containing profile performance data. */
struct CBTF_ompt_profile_data {
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

/** Structure of the blob containing extended trace performance data. */
struct CBTF_ompt_exttrace_data {
    uint64_t stacktraces<>;   /**< Stack traces. */
    CBTF_ompt_event events<>; /**< ompt events. */
};
