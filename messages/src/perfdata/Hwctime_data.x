/*******************************************************************************
** Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
** Copyright (c) 2007 William Hachfeld. All Rights Reserved.
** Copyright (c) 2011-2012 The Krell Institute. All Rights Reserved.
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
 * Specification of the HwcTime data collector's blobs.
 *
 */



/** Structure of the blob containing our performance data. */
struct CBTF_hwctime_data {
    uint64_t interval;    /**< Sampling interval in nanoseconds. */

    uint64_t stacktraces<>;        /**< Stack traces. */

    uint8_t count<>;      /**< Count for stack trace. Entries with a positive */
			  /**< count value represent the top of stack for a */
			  /**< specifc stack. If stack count exceeds 255 */
			  /**< a new entry is made in the sample buffer. */
			  /**< Positive entries the count buffer represent */
			  /**< the index into the address buffer (bt) for a */
			  /**< specifc stack */
};
