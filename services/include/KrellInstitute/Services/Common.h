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
 * Declaration of the Common includes.
 *
 */

#ifndef _CBTF_Runtime_Common_
#define _CBTF_Runtime_Common_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Assert.h"
#ifdef HAVE_INTTYPES_H
#include <inttypes.h>
#endif
#include <rpc/rpc.h>
#include <stdbool.h>

#include <ucontext.h>


#if defined(__linux) && (defined( __powerpc64__ ) || defined( __powerpc__ ))
#define CBTF_BlobSizeFactor 8
#else
#define CBTF_BlobSizeFactor 15
#endif


uint64_t CBTF_GetAddressOfFunction(const void*);
const char* CBTF_GetExecutablePath();
uint64_t CBTF_GetPCFromContext(const ucontext_t*);
void CBTF_SetPCInContext(uint64_t, ucontext_t*);
int CBTF_GetInstrLength(uint64_t);
uint64_t CBTF_GetTime();

#endif
