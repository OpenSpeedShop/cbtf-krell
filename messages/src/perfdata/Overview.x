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
 * Specification of the Overview sampling collector's blobs.
 *
 * This should use the enumnerated types of data similar to cuda.
 * data for:
 * pc sample addresses (including any ompt marking)
 * hardware counter counts per sample.  Make per sample counts optional.
 * hardware counter counts total only (if not per sample).
 * mpip time (via simple wrappers from the mpiwrap tool).
 * omptp data (if low overhead and in lieu of ompt marking of samples)
 * posix IOP time via simple wrappers (if possible)
 * memory report via rusage
 * rusage report for any other useful items that are currently available.
 *
 * ultra lightweight:	no symbolic function or statement data. ie. per dso.
 * 			possibly no database?
 *
 * lightweight:		symbolic function and statements. no callstacks.
 *
 * normal:		symbolic function and statements. callstacks.
 *
 * Optionaly ensure darshawn can run where possible.
 * Optionaly support caliper.
 *
 */



/** Structure of the blob containing our parameters. */
struct CBTF_overview_parameters {
    uint64_t sampling_rate;	    /**< Sampling threshhold in hwc events. */
    /* is this buffer big enough for 6 events as strings? */
    char CBTF_overview_event[512];  /**< hwc event (PAPI event code) to sample*/
};



/** Structure of the blob containing overview_start_sampling()'s arguments. */
struct CBTF_overview_start_sampling_args {

    uint64_t sampling_rate;	    /**< Sampling threshhold in hwc events. */
    char CBTF_overview_event[512];  /**< hwc event (PAPI event code) to sample*/
    
    int experiment;  /**< Identifier of experiment to contain the data. */
    int collector;   /**< Identifier of collector gathering data. */
    
};
