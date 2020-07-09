////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2014 The Krell Institue. All Rights Reserved.
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

/** @file
 *
 * Definition of the  PerfData handlers and aggregators
 *
 */
#ifndef _KrellInsitute_Core_PerfData_
#define _KrellInsitute_Core_PerfData_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "KrellInstitute/Messages/Blob.h"
#include "KrellInstitute/Messages/DataHeader.h"
#include "KrellInstitute/Messages/Address.h"
#include "KrellInstitute/Messages/PCSamp_data.h"
#include "KrellInstitute/Messages/Usertime_data.h"
#include "KrellInstitute/Messages/Hwc_data.h"
#include "KrellInstitute/Messages/Hwcsamp_data.h"
#include "KrellInstitute/Messages/Hwctime_data.h"
#include "KrellInstitute/Messages/IO_data.h"
#include "KrellInstitute/Messages/Mem_data.h"
#include "KrellInstitute/Messages/Mpi_data.h"
#include "KrellInstitute/Messages/Ompt_data.h"
#include "KrellInstitute/Messages/Pthreads_data.h"
#include "KrellInstitute/Messages/ThreadEvents.h"

#include "KrellInstitute/Core/AddressBuffer.hpp"
#include "KrellInstitute/Core/Blob.hpp"
#include "KrellInstitute/Core/Address.hpp"
#include "KrellInstitute/Core/AddressEntry.hpp"
#include "KrellInstitute/Core/PCData.hpp"
#include "KrellInstitute/Core/StackTrace.hpp"
#include "KrellInstitute/Core/StacktraceData.hpp"
#include "KrellInstitute/Core/Time.hpp"
#include "KrellInstitute/Core/TimeInterval.hpp"
#include "KrellInstitute/Core/MemEventMetrics.hpp"


namespace KrellInstitute { namespace Core {

    class PerfData {

	public:
	   int aggregate(const Blob&, AddressBuffer& buf);
	   int memMetrics(const Blob&, MemMetrics&);


	private:

    };

} }
#endif
