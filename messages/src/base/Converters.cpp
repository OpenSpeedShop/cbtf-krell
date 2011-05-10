////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2011 Krell Institute. All Rights Reserved.
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

/** @file Declaration and definition of the XDR/MRNet conversion classes. */

#include <KrellInstitute/CBTF/XDR.hpp>

#include "Address.h"
#include "Blob.h"
#include "Error.h"
#include "File.h"
#include "StdIO.h"
#include "Thread.h"
#include "Time.h"


KRELL_INSTITUTE_CBTF_REGISTER_XDR_CONVERTERS(CBTF_Protocol_Address)
KRELL_INSTITUTE_CBTF_REGISTER_XDR_CONVERTERS(CBTF_Protocol_AddressRange)
KRELL_INSTITUTE_CBTF_REGISTER_XDR_CONVERTERS(CBTF_Protocol_Blob)
KRELL_INSTITUTE_CBTF_REGISTER_XDR_CONVERTERS(CBTF_Protocol_ReportError)
KRELL_INSTITUTE_CBTF_REGISTER_XDR_CONVERTERS(CBTF_Protocol_FileName)
KRELL_INSTITUTE_CBTF_REGISTER_XDR_CONVERTERS(CBTF_Protocol_StdErr)
KRELL_INSTITUTE_CBTF_REGISTER_XDR_CONVERTERS(CBTF_Protocol_StdIn)
KRELL_INSTITUTE_CBTF_REGISTER_XDR_CONVERTERS(CBTF_Protocol_StdOut)
KRELL_INSTITUTE_CBTF_REGISTER_XDR_CONVERTERS(CBTF_Protocol_ThreadName)
KRELL_INSTITUTE_CBTF_REGISTER_XDR_CONVERTERS(CBTF_Protocol_ThreadNameGroup)
KRELL_INSTITUTE_CBTF_REGISTER_XDR_CONVERTERS(CBTF_Protocol_ThreadState)
//KRELL_INSTITUTE_CBTF_REGISTER_XDR_CONVERTERS(CBTF_Protocol_Time)
