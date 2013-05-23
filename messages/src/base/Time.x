/*******************************************************************************
** Copyright (c) 2007,2008 William Hachfeld. All Rights Reserved.
** Copyright (c) 2011,2013 Krell Institute. All Rights Reserved.
**
** This program is free software; you can redistribute it and/or modify it under
** the terms of the GNU General Public License as published by the Free Software
** Foundation; either version 2 of the License, or (at your option) any later
** version.
**
** This program is distributed in the hope that it will be useful, but WITHOUT
** ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
** FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
** details.
**
** You should have received a copy of the GNU General Public License along with
** this program; if not, write to the Free Software Foundation, Inc., 59 Temple
** Place, Suite 330, Boston, MA  02111-1307  USA
*******************************************************************************/

/** @file
 *
 * Specification of the Time communication protocol.
 *
 */



/**
 * Time.
 *
 * All time values are represented using a single 64-bit unsigned integer.
 * These integers are interpreted as the number of nanoseconds that have
 * passed since midnight (00:00) Coordinated Universal Time (UTC), on
 * Janaury 1, 1970. This system gives nanosecond resolution for representing
 * times while not running out the clock until sometime in the year 2554.
 */
typedef uint64_t CBTF_Protocol_Time;



/**
 * Time interval.
 *
 * A single, open-ended, time interval: [begin, end). Used in many different
 * places for representing a single contiguous period of time.
 */
struct CBTF_Protocol_TimeInterval
{
    CBTF_Protocol_Time begin;  /**< Beginning of the time interval. */
    CBTF_Protocol_Time end;    /**< End of the time interval. */
};
