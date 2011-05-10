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

#include "Instrument.h"
#include "Job.h"
#include "MpiJob.h"

KRELL_INSTITUTE_CBTF_REGISTER_XDR_CONVERTERS(CBTF_Protocol_ExecuteNow)
KRELL_INSTITUTE_CBTF_REGISTER_XDR_CONVERTERS(CBTF_Protocol_ExecuteAtEntryOrExit)
KRELL_INSTITUTE_CBTF_REGISTER_XDR_CONVERTERS(CBTF_Protocol_ExecuteInPlaceOf)
KRELL_INSTITUTE_CBTF_REGISTER_XDR_CONVERTERS(CBTF_Protocol_GetGlobalInteger)
KRELL_INSTITUTE_CBTF_REGISTER_XDR_CONVERTERS(CBTF_Protocol_GetGlobalString)
KRELL_INSTITUTE_CBTF_REGISTER_XDR_CONVERTERS(CBTF_Protocol_GetMPICHProcTable)
KRELL_INSTITUTE_CBTF_REGISTER_XDR_CONVERTERS(CBTF_Protocol_GlobalIntegerValue)
KRELL_INSTITUTE_CBTF_REGISTER_XDR_CONVERTERS(CBTF_Protocol_GlobalJobValue)
KRELL_INSTITUTE_CBTF_REGISTER_XDR_CONVERTERS(CBTF_Protocol_GlobalStringValue)
KRELL_INSTITUTE_CBTF_REGISTER_XDR_CONVERTERS(CBTF_Protocol_Instrumented)
KRELL_INSTITUTE_CBTF_REGISTER_XDR_CONVERTERS(CBTF_Protocol_JobEntry)
KRELL_INSTITUTE_CBTF_REGISTER_XDR_CONVERTERS(CBTF_Protocol_Job)
KRELL_INSTITUTE_CBTF_REGISTER_XDR_CONVERTERS(CBTF_Protocol_MPIStartup)
KRELL_INSTITUTE_CBTF_REGISTER_XDR_CONVERTERS(CBTF_Protocol_SetGlobalInteger)
KRELL_INSTITUTE_CBTF_REGISTER_XDR_CONVERTERS(CBTF_Protocol_StopAtEntryOrExit)
KRELL_INSTITUTE_CBTF_REGISTER_XDR_CONVERTERS(CBTF_Protocol_Uninstrument)
