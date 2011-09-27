/*******************************************************************************
** Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
** Copyright (c) 2007 William Hachfeld. All Rights Reserved.
** Copyright (c) 2011 The Krell Institute. All Rights Reserved.
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
 * Specification of the HWC sampling collector's blobs.
 *
 */



/** Structure of the blob containing our parameters. */
struct CBTF_hwc_parameters {
    uint64_t sampling_rate;  /**< Sampling threshhold in hwc events. */
    char CBTF_hwc_event[128];      /**< hwc event (PAPI event code) to sample*/
};



/** Structure of the blob containing hwc_start_sampling()'s arguments. */
struct CBTF_hwc_start_sampling_args {

    uint64_t sampling_rate;  /**< Sampling threshhold in hwc events. */
    char CBTF_hwc_event[128];            /**< hwc event (PAPI event code) to sample*/
    
    int experiment;  /**< Identifier of experiment to contain the data. */
    int collector;   /**< Identifier of collector gathering data. */
    
};
