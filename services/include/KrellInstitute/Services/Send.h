/*******************************************************************************
** Copyright (c) 2010 The Krell Institute. All Rights Reserved.
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
 * Declaration of the CBTFW Runtime Send.
 *
 */

#ifndef _CBTF_Runtime_Send_
#define _CBTF_Runtime_Send_

#include "KrellInstitute/Services/Common.h"
#include <rpc/rpc.h>
#include "KrellInstitute/Messages/DataHeader.h"
#include "KrellInstitute/Messages/EventHeader.h"


void CBTF_Data_Send(const CBTF_DataHeader*, const xdrproc_t, const void*);
void CBTF_Event_Send(const CBTF_EventHeader*, const xdrproc_t, const void*);

#endif
