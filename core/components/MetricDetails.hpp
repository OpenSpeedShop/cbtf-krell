////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2012 The Krell Institute. All Rights Reserved.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License as published by the Free
// Software Foundation; either version 2.1 of the License, or (at your option)
// any later version.
//
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
// details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, write to the Free Software Foundation, Inc.,
// 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
////////////////////////////////////////////////////////////////////////////////

/** @file
 *
 * Declaration and definition of the various metric Detail structures.
 *
 */

#ifndef _MetricDetail_
#define _MetricDetail_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "KrellInstitute/Core/TotallyOrdered.hpp"


#define NUMCOUNTERS 6

namespace KrellInstitute { namespace Core {

    /**
     *
     */
    struct SampleDetail :
	public TotallyOrdered<SampleDetail>
    {
	uint64_t dm_count;  /**< Number of samples at the stack trace. */
	double dm_time;     /**< Time attributable to the stack trace. */

	/** Operator "<" defined for two SampleDetail objects. */
	bool operator<(const SampleDetail& other) const
	{
	    if(dm_count < other.dm_count)
                return true;
            if(dm_count > other.dm_count)
                return false;
	    return dm_time < other.dm_time;
        }

	/** Operator "+=" defined for two SampleDetail objects. */
	SampleDetail& operator+=(const SampleDetail& other)
	{
	    dm_count += other.dm_count;
	    dm_time += other.dm_time;
	    return *this;
	}

    };



    /**
     * Hardware time sampling collector detail.
     *
     * Encapsulate the detail metric (inclusive or exclusive) for the hardware
     * time sampling collector.
     */
    struct HWCSampDetail :
	public TotallyOrdered<HWCSampDetail>
    {
	uint64_t dm_count;   /**< Number of samples at the address. */
	double dm_time;   /**< Time attributable to the address. */
	uint64_t dm_event_values[NUMCOUNTERS];   /**< Number of samples at the address. */

	/* Initialize all fields. */
        HWCSampDetail()
	{
          dm_count = 0;
          dm_time = 0.0;
	  for (int i = 0; i < NUMCOUNTERS; i++) {
	    dm_event_values[i]  = 0;
	  }
	}
	

	/** Operator "<" defined for two HWCSampDetail objects. */
	bool operator<(const HWCSampDetail& other) const
	{
	  for (int i = 0; i < NUMCOUNTERS; i++) {
	    if(dm_event_values[i] < other.dm_event_values[i])
                return true;
            if(dm_event_values[i] > other.dm_event_values[i])
                return false;
	  }
          return dm_time < other.dm_time;
        }

	/** Operator "+=" defined for two HWCSampDetail objects. */
	HWCSampDetail& operator+=(const HWCSampDetail& other)
	{
          dm_time += other.dm_time;
	  for (int i = 0; i < NUMCOUNTERS; i++) {
	    dm_event_values[i] += other.dm_event_values[i];
	  }
	  return *this;
	}

    };

    /**
     * Hardware counter sampling collector detail.
     *
     * Encapsulate the detail metric (inclusive or exclusive) for the hardware
     * counter sampling collector.
     */
    struct HWCDetail :
	public TotallyOrdered<HWCDetail>
    {
	uint64_t dm_count;   /**< Number of samples at the stack trace. */
	uint64_t dm_events;  /**< Events attributable to the stack trace. */

	/** Operator "<" defined for two HWCDetail objects. */
	bool operator<(const HWCDetail& other) const
	{
	    if(dm_count < other.dm_count)
                return true;
            if(dm_count > other.dm_count)
                return false;
	    return dm_events < other.dm_events;
        }

	/** Operator "+=" defined for two HWCDetail objects. */
	HWCDetail& operator+=(const HWCDetail& other)
	{
	    dm_count += other.dm_count;
	    dm_events += other.dm_events;
	    return *this;
	}

    };

    /**
     * Simple event tracing collector details. 
     *
     * Encapsulate the details metric (inclusive or exclusive) for the simple event
     * tracing collectors.
     */
    struct TraceDetail :
	public TotallyOrdered<TraceDetail>
    {
	TimeInterval dm_interval;  /**< Begin/End time of the call. */
	double dm_time;            /**< Time spent in the call. */	

	/** Operator "<" defined for two TraceDetail objects. */
	bool operator<(const TraceDetail& other) const
	{
	    if(dm_interval < other.dm_interval)
                return true;
            if(dm_interval > other.dm_interval)
                return false;
	    return dm_time < other.dm_time;
        }

    };

    /**
     * MPI extended event tracing collector details. 
     *
     * Encapsulate the details metric (inclusive or exclusive) for the MPI
     * extended event tracing collector.
     */
    struct MPITraceDetail :
	public TotallyOrdered<MPITraceDetail>
    {
	TimeInterval dm_interval;  /**< Begin/End time of the call. */
	double dm_time;            /**< Time spent in the call. */	

        int dm_source;             /**< Source rank (in MPI_COMM_WORLD). */
        int dm_destination;        /**< Destination rank (in MPI_COMM_WORLD). */
        uint64_t dm_size;          /**< Number of bytes sent. */
        int dm_tag;                /**< Tag of the message (if any). */
        int dm_communicator;       /**< Communicator used. */
        int dm_datatype;           /**< Data type of the message. */
        int dm_retval;             /**< Enumerated return value. */

	/** Operator "<" defined for two MPITraceDetail objects. */
	bool operator<(const MPITraceDetail& other) const
	{
	    if(dm_interval < other.dm_interval)
                return true;
            if(dm_interval > other.dm_interval)
                return false;
	    if(dm_time < other.dm_time)
                return true;
            if(dm_time > other.dm_time)
                return false;
	    if(dm_source < other.dm_source)
                return true;
            if(dm_source > other.dm_source)
                return false;
	    if(dm_destination < other.dm_destination)
                return true;
            if(dm_destination > other.dm_destination)
                return false;
	    if(dm_size < other.dm_size)
                return true;
            if(dm_size > other.dm_size)
                return false;
	    if(dm_tag < other.dm_tag)
                return true;
            if(dm_tag > other.dm_tag)
                return false;
	    if(dm_communicator < other.dm_communicator)
                return true;
            if(dm_communicator > other.dm_communicator)
                return false;
	    if(dm_datatype < other.dm_datatype)
                return true;
            if(dm_datatype > other.dm_datatype)
                return false;
	    return dm_retval < other.dm_retval;
        }

    };

    /**
     * I/O extended event tracing collector details. 
     *
     * Encapsulate the details metric (inclusive or exclusive) for the I/O event
     * tracing collector.
     */
    struct IOTraceDetail :
	public TotallyOrdered<IOTraceDetail>
    {
	TimeInterval dm_interval;  /**< Begin/End time of the call. */
	double dm_time;            /**< Time spent in the call. */	
	int dm_syscallno;          /**< Which syscallno is this */
	int dm_nsysargs;           /**< Number of args for this syscall*/
	int dm_retval;             /**< Enumerated return value. */
	int dm_sysargs[4];         /**< sysargs. */
        std::string dm_pathname;   /**< pathname buffer of characters representing the path name */
	int pathindex;		   /**< index into pathnames buffer. */

	/** Operator "<" defined for two IOTraceDetail objects. */
	bool operator<(const IOTraceDetail& other) const
	{
	    if(dm_interval < other.dm_interval)
                return true;
            if(dm_interval > other.dm_interval)
                return false;
// FIXME: Need to complete the compare rules here.
            if(dm_retval < other.dm_retval)
                return true;
	    return dm_time < other.dm_time;
        }

    };
    
} }



#endif
