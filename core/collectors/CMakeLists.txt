################################################################################
# Copyright (c) 2014-2016 Krell Institute. All Rights Reserved.
#
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation; either version 2 of the License, or (at your option) any later
# version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# this program; if not, write to the Free Software Foundation, Inc., 59 Temple
# Place, Suite 330, Boston, MA  02111-1307  USA
################################################################################

if (RUNTIME_PLATFORM MATCHES "arm")
    # The unwind-tables entries where initially intended for arm and aarch64.
    # Only add these definitions when building for arm or aarch64
    # They help libunwind to unwind through callstacks more accurately
    add_definitions(
        -fasynchronous-unwind-tables
        -funwind-tables
    )
endif()

add_subdirectory(pcsamp)

if (LIBUNWIND_FOUND)
add_subdirectory(usertime)
if (NOT RUNTIME_PLATFORM MATCHES "arm")
add_subdirectory(io)
endif()
add_subdirectory(mem)
add_subdirectory(pthreads)
endif()

if (PAPI_FOUND)
add_subdirectory(hwc)
add_subdirectory(hwcsamp)
if (LIBUNWIND_FOUND)
add_subdirectory(hwctime)
endif()
endif()

if(HAVE_MPI)
if (LIBUNWIND_FOUND)
add_subdirectory(mpi)
endif()
endif()

if(LIBIOMP_FOUND)
if (LIBUNWIND_FOUND)
add_subdirectory(omptp)
endif()
endif()

add_subdirectory(overview)
