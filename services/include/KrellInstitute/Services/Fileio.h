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
 * Declaration of the CBTFW Runtime Send.
 *
 */

#ifndef _CBTF_Runtime_Fileio_
#define _CBTF_Runtime_Fileio_

#include "KrellInstitute/Services/Common.h"
#include <rpc/rpc.h>
#include "KrellInstitute/Messages/DataHeader.h"
#include "KrellInstitute/Messages/EventHeader.h"

void CBTF_EventSetSendToFile(CBTF_EventHeader*, const char*, const char*);
void CBTF_SetSendToFile(CBTF_DataHeader*, const char*, const char*);

int CBTF_SendToFile(const unsigned, const void*);

#endif
