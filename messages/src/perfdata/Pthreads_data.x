/*******************************************************************************
** Copyright (c) 2012 The KrellInstitute. All Rights Reserved.
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
 * Specification of the Pthread collector's data blobs.
 *
 */


enum CBTF_pthread_type {
    CBTF_PTHREAD_UNKNOWN=0,
    CBTF_PTHREAD_CREATE,
    CBTF_PTHREAD_JOIN,
    CBTF_PTHREAD_CANCEL,
    CBTF_PTHREAD_MUTEX_INIT,
    CBTF_PTHREAD_MUTEX_DESTROY,
    CBTF_PTHREAD_MUTEX_LOCK,
    CBTF_PTHREAD_MUTEX_UNLOCK,
    CBTF_PTHREAD_MUTEX_TRYLOCK,
    CBTF_PTHREAD_COND_INIT,
    CBTF_PTHREAD_COND_DESTROY,
    CBTF_PTHREAD_COND_SIGNAL,
    CBTF_PTHREAD_COND_BROADCAST,
    CBTF_PTHREAD_COND_WAIT,
    CBTF_PTHREAD_COND_TIMEDWAIT
};

/** Event structure describing a single I/O call. */
struct CBTF_pthreadt_event {
    uint64_t start_time;  /**< Start time of the call. */
    uint64_t stop_time;   /**< End time of the call. */
    uint64_t ptr1;        /**< pthread_mutex_t* ptr type args */
    uint64_t ptr2;   
    uint64_t ptr3;    
    int      retval;      /**< return values can be a void* */
    uint16_t stacktrace;  /**< Index of the stack trace. */
    CBTF_pthread_type pthread_type;
};


/** Structure of the blob containing extended trace performance data. */
struct CBTF_pthreads_exttrace_data {
    uint64_t stacktraces<>;  /**< Stack traces. */
    CBTF_pthreadt_event events<>;       /**< pthread call events. */
};
