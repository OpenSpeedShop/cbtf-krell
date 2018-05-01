/*******************************************************************************
** Copyright (c) 2018 The Krell Institute. All Rights Reserved.
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
 * Declaration of the CBTFW Runtime MRNet.
 *
 */

#ifndef _CBTF_Runtime_MRNet_
#define _CBTF_Runtime_MRNet_

#include <rpc/rpc.h>

void CBTF_Waitfor_MRNet_Shutdown();
void CBTF_MRNet_Send(const int tag,
                 const xdrproc_t xdrproc, const void* data);
int CBTF_MRNet_LW_connect (const int con_rank);
void CBTF_MRNet_Send_PerfData(const CBTF_DataHeader* header,
                              const xdrproc_t xdrproc, const void* data);

#endif
