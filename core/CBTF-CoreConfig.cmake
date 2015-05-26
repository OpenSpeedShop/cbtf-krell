################################################################################
# Copyright (c) 2012 Argo Navis Technologies. All Rights Reserved.
# Copyright (c) 2012-2015 Krell Institute. All Rights Reserved.
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

include(FindPackageHandleStandardArgs)

find_library(CBTF_CORE_SHARED_LIBRARY
    NAMES libcbtf-core.so
    HINTS ${CBTF_KRELL_DIR} $ENV{CBTF_KRELL_DIR}
    PATH_SUFFIXES lib lib64
    )

find_library(CBTF_CORE_STATIC_LIBRARY
    NAMES libcbtf-core.a libcbtf-core-static.a
    HINTS ${CBTF_KRELL_DIR} $ENV{CBTF_KRELL_DIR}
    PATH_SUFFIXES lib lib64
    )

find_library(CBTF_CORE_SYMTABAPI_SHARED_LIBRARY
    NAMES libcbtf-core-symtabapi.so
    HINTS ${CBTF_KRELL_DIR} $ENV{CBTF_KRELL_DIR}
    PATH_SUFFIXES lib lib64
    )

find_library(CBTF_CORE_SYMTABAPI_STATIC_LIBRARY
    NAMES libcbtf-core-symtabapi.a libcbtf-core-symtabapi-static.a
    HINTS ${CBTF_KRELL_DIR} $ENV{CBTF_KRELL_DIR}
    PATH_SUFFIXES lib lib64
    )

find_library(CBTF_CORE_BFD_SHARED_LIBRARY
    NAMES libcbtf-core-bfd.so
    HINTS ${CBTF_KRELL_DIR} $ENV{CBTF_KRELL_DIR}
    PATH_SUFFIXES lib lib64
    )

find_library(CBTF_CORE_BFD_STATIC_LIBRARY
    NAMES libcbtf-core-bfd.a libcbtf-core-bfd-static.a
    HINTS ${CBTF_KRELL_DIR} $ENV{CBTF_KRELL_DIR}
    PATH_SUFFIXES lib lib64
    )

find_library(CBTF_CORE_MRNET_SHARED_LIBRARY
    NAMES libcbtf-core-mrnet.so
    HINTS ${CBTF_KRELL_DIR} $ENV{CBTF_KRELL_DIR}
    PATH_SUFFIXES lib lib64
    )

find_path(CBTF_CORE_INCLUDE_DIR
    KrellInstitute/Core/Assert.hpp
    HINTS ${CBTF_KRELL_DIR} $ENV{CBTF_KRELL_DIR}
    PATH_SUFFIXES include
    )

find_library(CBTF_CORE_MRNET_STATIC_LIBRARY
    NAMES libcbtf-core-mrnet.a libcbtf-core-mrnet-static.a
    HINTS ${CBTF_KRELL_DIR} $ENV{CBTF_KRELL_DIR}
    PATH_SUFFIXES lib lib64
    )

find_package_handle_standard_args(
    CBTF-Core DEFAULT_MSG
    CBTF_CORE_SHARED_LIBRARY CBTF_CORE_INCLUDE_DIR
    )

set(CBTF_CORE_LIBRARIES ${CBTF_CORE_SHARED_LIBRARY})
set(CBTF_CORE_STATIC_LIBRARIES ${CBTF_CORE_STATIC_LIBRARY} )
set(CBTF_CORE_INCLUDE_DIRS ${CBTF_CORE_INCLUDE_DIR})

mark_as_advanced(CBTF_CORE_SHARED_LIBRARY CBTF_CORE_INCLUDE_DIR CBTF_CORE_SYMTABAPI_SHARED_LIBRARY CBTF_CORE_BFD_SHARED_LIBRARY CBTF_CORE_MRNET_SHARED_LIBRARY)
mark_as_advanced(CBTF_CORE_STATIC_LIBRARY CBTF_CORE_SYMTABAPI_STATIC_LIBRARY CBTF_CORE_BFD_STATIC_LIBRARY CBTF_CORE_MRNET_STATIC_LIBRARY)
