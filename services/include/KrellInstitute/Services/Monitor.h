/*******************************************************************************
** Copyright (c) 2010 The Krell Institue. All Rights Reserved.
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
 * Declaration of Offline Monitor specifc utility functions
 *
 */

#ifndef _CBTF_Runtime_Monitor_
#define _CBTF_Runtime_Monitor_

#include "monitor.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum {
    CBTF_Monitor_Proc,
    CBTF_Monitor_Thread,
    CBTF_Monitor_Default
} CBTF_Monitor_Type;

typedef enum {
    CBTF_Monitor_Started = 1,
    CBTF_Monitor_Finished,
    CBTF_Monitor_Paused,
    CBTF_Monitor_Resumed
} CBTF_Monitor_Status;

#ifdef  __cplusplus
}
#endif
#endif /*_CBTF_Runtime_Monitor_*/
