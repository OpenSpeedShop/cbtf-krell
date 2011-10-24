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

#include "DataHeader.h"
#include "PCSamp_data.h"
#include "PCSamp.h"
#include "Usertime_data.h"
#include "Usertime.h"
#include "IO_data.h"
#include "IO.h"
#include "Mem_data.h"
#include "Mem.h"
#include "Hwc_data.h"
#include "Hwc.h"

KRELL_INSTITUTE_CBTF_REGISTER_XDR_CONVERTERS(CBTF_DataHeader)
KRELL_INSTITUTE_CBTF_REGISTER_XDR_CONVERTERS(CBTF_pcsamp_data)
KRELL_INSTITUTE_CBTF_REGISTER_XDR_CONVERTERS(CBTF_pcsamp_parameters)
KRELL_INSTITUTE_CBTF_REGISTER_XDR_CONVERTERS(CBTF_pcsamp_start_sampling_args)
KRELL_INSTITUTE_CBTF_REGISTER_XDR_CONVERTERS(CBTF_usertime_data)
KRELL_INSTITUTE_CBTF_REGISTER_XDR_CONVERTERS(CBTF_usertime_parameters)
KRELL_INSTITUTE_CBTF_REGISTER_XDR_CONVERTERS(CBTF_usertime_start_sampling_args)
KRELL_INSTITUTE_CBTF_REGISTER_XDR_CONVERTERS(CBTF_io_trace_data)
KRELL_INSTITUTE_CBTF_REGISTER_XDR_CONVERTERS(CBTF_io_profile_data)
KRELL_INSTITUTE_CBTF_REGISTER_XDR_CONVERTERS(CBTF_io_parameters)
KRELL_INSTITUTE_CBTF_REGISTER_XDR_CONVERTERS(CBTF_io_start_sampling_args)
KRELL_INSTITUTE_CBTF_REGISTER_XDR_CONVERTERS(CBTF_hwc_data)
KRELL_INSTITUTE_CBTF_REGISTER_XDR_CONVERTERS(CBTF_hwc_parameters)
KRELL_INSTITUTE_CBTF_REGISTER_XDR_CONVERTERS(CBTF_hwc_start_sampling_args)
KRELL_INSTITUTE_CBTF_REGISTER_XDR_CONVERTERS(CBTF_mem_trace_data)
KRELL_INSTITUTE_CBTF_REGISTER_XDR_CONVERTERS(CBTF_mem_profile_data)
KRELL_INSTITUTE_CBTF_REGISTER_XDR_CONVERTERS(CBTF_mem_parameters)
KRELL_INSTITUTE_CBTF_REGISTER_XDR_CONVERTERS(CBTF_mem_start_sampling_args)
