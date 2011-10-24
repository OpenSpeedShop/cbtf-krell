////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2008 The Krell Institute. All Rights Reserved.
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
 * Declaration of the IOCollector TraceableFunctions
 *
 */

#ifndef _IOTraceableFunctions_
#define _IOTraceableFunctions_
 
    static const char* TraceableFunctions[] = {

        "malloc",
        "free",
        "memalign",
        "posix_memalign",
        "calloc",
        "realloc",
	/* End Of Table Entry */
	NULL
    };

#if defined(CBTF_SERVICE_USE_OFFLINE)
        static const char * traceable = \
	"malloc,free,memalign,posix_memalign,calloc,realloc";

#endif

#endif
