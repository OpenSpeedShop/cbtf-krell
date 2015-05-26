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

find_library(CBTF_SERVICES_BINUTILS_SHARED_LIBRARY
    NAMES libcbtf-services-binutils.so
    HINTS $ENV{CBTF_KRELL_DIR}
    PATH_SUFFIXES lib lib64
    )

find_library(CBTF_SERVICES_BINUTILS_STATIC_LIBRARY
    NAMES libcbtf-services-binutils.a libcbtf-services-binutils-static.a
    HINTS $ENV{CBTF_KRELL_DIR}
    PATH_SUFFIXES lib lib64
    )

find_library(CBTF_SERVICES_COLLECTOR_MONITOR_FILEIO_SHARED_LIBRARY
    NAMES libcbtf-services-collector-monitor-fileio.so
    HINTS $ENV{CBTF_KRELL_DIR}
    PATH_SUFFIXES lib lib64
    )

find_library(CBTF_SERVICES_COLLECTOR_MONITOR_FILEIO_STATIC_LIBRARY
    NAMES libcbtf-services-collector-monitor-fileio.a libcbtf-services-collector-monitor-fileio-static.a
    HINTS $ENV{CBTF_KRELL_DIR}
    PATH_SUFFIXES lib lib64
    )

find_library(CBTF_SERVICES_COLLECTOR_MONITOR_MRNET_SHARED_LIBRARY
    NAMES libcbtf-services-collector-monitor-mrnet.so
    HINTS $ENV{CBTF_KRELL_DIR}
    PATH_SUFFIXES lib lib64
    )

find_library(CBTF_SERVICES_COLLECTOR_MONITOR_MRNET_STATIC_LIBRARY
    NAMES libcbtf-services-collector-monitor-mrnet.a libcbtf-services-collector-monitor-mrnet-static.a
    HINTS $ENV{CBTF_KRELL_DIR}
    PATH_SUFFIXES lib lib64
    )

find_library(CBTF_SERVICES_COLLECTOR_MONITOR_MRNET_MPI_SHARED_LIBRARY
    NAMES libcbtf-services-collector-monitor-mrnet-mpi.so
    HINTS $ENV{CBTF_KRELL_DIR}
    PATH_SUFFIXES lib lib64
    )

find_library(CBTF_SERVICES_COLLECTOR_MONITOR_MRNET_MPI_STATIC_LIBRARY
    NAMES libcbtf-services-collector-monitor-mrnet-mpi.a libcbtf-services-collector-monitor-mrnet-mpi-static.a
    HINTS $ENV{CBTF_KRELL_DIR}
    PATH_SUFFIXES lib lib64
    )

find_library(CBTF_SERVICES_COMMON_SHARED_LIBRARY
    NAMES libcbtf-services-common.so
    HINTS $ENV{CBTF_KRELL_DIR}
    PATH_SUFFIXES lib lib64
    )

find_library(CBTF_SERVICES_COMMON_STATIC_LIBRARY
    NAMES libcbtf-services-common.a libcbtf-services-common-static.a
    HINTS $ENV{CBTF_KRELL_DIR}
    PATH_SUFFIXES lib lib64
    )

find_library(CBTF_SERVICES_DATA_SHARED_LIBRARY
    NAMES libcbtf-services-data.so
    HINTS $ENV{CBTF_KRELL_DIR}
    PATH_SUFFIXES lib lib64
    )

find_library(CBTF_SERVICES_DATA_STATIC_LIBRARY
    NAMES libcbtf-services-data.a libcbtf-services-data-static.a
    HINTS $ENV{CBTF_KRELL_DIR}
    PATH_SUFFIXES lib lib64
    )

find_library(CBTF_SERVICES_FILEIO_SHARED_LIBRARY
    NAMES libcbtf-services-fileio.so
    HINTS $ENV{CBTF_KRELL_DIR}
    PATH_SUFFIXES lib lib64
    )

find_library(CBTF_SERVICES_FILEIO_STATIC_LIBRARY
    NAMES libcbtf-services-fileio.a libcbtf-services-fileio-static.a
    HINTS $ENV{CBTF_KRELL_DIR}
    PATH_SUFFIXES lib lib64
    )

find_library(CBTF_SERVICES_FPE_SHARED_LIBRARY
    NAMES libcbtf-services-fpe.so
    HINTS $ENV{CBTF_KRELL_DIR}
    PATH_SUFFIXES lib lib64
    )

find_library(CBTF_SERVICES_FPE_STATIC_LIBRARY
    NAMES libcbtf-services-fpe.a libcbtf-services-fpe-static.a
    HINTS $ENV{CBTF_KRELL_DIR}
    PATH_SUFFIXES lib lib64
    )

find_library(CBTF_SERVICES_MONITOR_SHARED_LIBRARY
    NAMES libcbtf-services-monitor.so
    HINTS $ENV{CBTF_KRELL_DIR}
    PATH_SUFFIXES lib lib64
    )

find_library(CBTF_SERVICES_MONITOR_STATIC_LIBRARY
    NAMES libcbtf-services-monitor.a libcbtf-services-monitor-static.a
    HINTS $ENV{CBTF_KRELL_DIR}
    PATH_SUFFIXES lib lib64
    )

find_library(CBTF_SERVICES_MRNET_SHARED_LIBRARY
    NAMES libcbtf-services-mrnet.so
    HINTS $ENV{CBTF_KRELL_DIR}
    PATH_SUFFIXES lib lib64
    )

find_library(CBTF_SERVICES_MRNET_STATIC_LIBRARY
    NAMES libcbtf-services-mrnet.a libcbtf-services-mrnet-static.a
    HINTS $ENV{CBTF_KRELL_DIR}
    PATH_SUFFIXES lib lib64
    )

find_library(CBTF_SERVICES_OFFLINE_SHARED_LIBRARY
    NAMES libcbtf-services-offline.so
    HINTS $ENV{CBTF_KRELL_DIR}
    PATH_SUFFIXES lib lib64
    )

find_library(CBTF_SERVICES_OFFLINE_STATIC_LIBRARY
    NAMES libcbtf-services-offline.a libcbtf-services-offline-static.a
    HINTS $ENV{CBTF_KRELL_DIR}
    PATH_SUFFIXES lib lib64
    )

find_library(CBTF_SERVICES_PAPI_SHARED_LIBRARY
    NAMES libcbtf-services-papi.so
    HINTS $ENV{CBTF_KRELL_DIR}
    PATH_SUFFIXES lib lib64
    )

find_library(CBTF_SERVICES_PAPI_STATIC_LIBRARY
    NAMES libcbtf-services-papi.a libcbtf-services-papi-static.a
    HINTS $ENV{CBTF_KRELL_DIR}
    PATH_SUFFIXES lib lib64
    )

find_library(CBTF_SERVICES_SEND_SHARED_LIBRARY
    NAMES libcbtf-services-send.so
    HINTS $ENV{CBTF_KRELL_DIR}
    PATH_SUFFIXES lib lib64
    )

find_library(CBTF_SERVICES_SEND_STATIC_LIBRARY
    NAMES libcbtf-services-send.a libcbtf-services-send-static.a
    HINTS $ENV{CBTF_KRELL_DIR}
    PATH_SUFFIXES lib lib64
    )

find_library(CBTF_SERVICES_TIMER_SHARED_LIBRARY
    NAMES libcbtf-services-timer.so
    HINTS $ENV{CBTF_KRELL_DIR}
    PATH_SUFFIXES lib lib64
    )

find_library(CBTF_SERVICES_TIMER_STATIC_LIBRARY
    NAMES libcbtf-services-timer.a libcbtf-services-timer-static.a
    HINTS $ENV{CBTF_KRELL_DIR}
    PATH_SUFFIXES lib lib64
    )

find_library(CBTF_SERVICES_UNWIND_SHARED_LIBRARY
    NAMES libcbtf-services-unwind.so
    HINTS $ENV{CBTF_KRELL_DIR}
    PATH_SUFFIXES lib lib64
    )

find_library(CBTF_SERVICES_UNWIND_STATIC_LIBRARY
    NAMES libcbtf-services-unwind.a libcbtf-services-unwind-static.a
    HINTS $ENV{CBTF_KRELL_DIR}
    PATH_SUFFIXES lib lib64
    )

find_path(CBTF_SERVICES_INCLUDE_DIR
    KrellInstitute/Services/Assert.h
    HINTS $ENV{CBTF_KRELL_DIR}
    PATH_SUFFIXES include
    )

find_package_handle_standard_args(
    CBTF-Services DEFAULT_MSG
    CBTF_SERVICES_BINUTILS_SHARED_LIBRARY
    CBTF_SERVICES_BINUTILS_STATIC_LIBRARY
    CBTF_SERVICES_COLLECTOR_MONITOR_FILEIO_SHARED_LIBRARY
    CBTF_SERVICES_COLLECTOR_MONITOR_FILEIO_STATIC_LIBRARY
    CBTF_SERVICES_COLLECTOR_MONITOR_MRNET_SHARED_LIBRARY
    CBTF_SERVICES_COLLECTOR_MONITOR_MRNET_STATIC_LIBRARY
    CBTF_SERVICES_COLLECTOR_MONITOR_MRNET_MPI_SHARED_LIBRARY
    CBTF_SERVICES_COLLECTOR_MONITOR_MRNET_MPI_STATIC_LIBRARY
    CBTF_SERVICES_COMMON_SHARED_LIBRARY
    CBTF_SERVICES_COMMON_STATIC_LIBRARY
    CBTF_SERVICES_DATA_SHARED_LIBRARY
    CBTF_SERVICES_DATA_STATIC_LIBRARY
    CBTF_SERVICES_FILEIO_SHARED_LIBRARY
    CBTF_SERVICES_FILEIO_STATIC_LIBRARY
    CBTF_SERVICES_FPE_SHARED_LIBRARY
    CBTF_SERVICES_FPE_STATIC_LIBRARY
    CBTF_SERVICES_MONITOR_SHARED_LIBRARY
    CBTF_SERVICES_MONITOR_STATIC_LIBRARY
    CBTF_SERVICES_MRNET_SHARED_LIBRARY
    CBTF_SERVICES_MRNET_STATIC_LIBRARY
    CBTF_SERVICES_OFFLINE_SHARED_LIBRARY
    CBTF_SERVICES_OFFLINE_STATIC_LIBRARY
    CBTF_SERVICES_PAPI_SHARED_LIBRARY
    CBTF_SERVICES_PAPI_STATIC_LIBRARY
    CBTF_SERVICES_SEND_SHARED_LIBRARY
    CBTF_SERVICES_SEND_STATIC_LIBRARY
    CBTF_SERVICES_TIMER_SHARED_LIBRARY
    CBTF_SERVICES_TIMER_STATIC_LIBRARY
    CBTF_SERVICES_UNWIND_SHARED_LIBRARY
    CBTF_SERVICES_UNWIND_STATIC_LIBRARY
    CBTF_SERVICES_INCLUDE_DIR
    )

set(CBTF_SERVICES_INCLUDE_DIRS ${CBTF_SERVICES_INCLUDE_DIR})

mark_as_advanced(
    CBTF_SERVICES_BINUTILS_SHARED_LIBRARY
    CBTF_SERVICES_BINUTILS_STATIC_LIBRARY
    CBTF_SERVICES_COLLECTOR_MONITOR_FILEIO_SHARED_LIBRARY
    CBTF_SERVICES_COLLECTOR_MONITOR_FILEIO_STATIC_LIBRARY
    CBTF_SERVICES_COLLECTOR_MONITOR_MRNET_SHARED_LIBRARY
    CBTF_SERVICES_COLLECTOR_MONITOR_MRNET_STATIC_LIBRARY
    CBTF_SERVICES_COLLECTOR_MONITOR_MRNET_MPI_SHARED_LIBRARY
    CBTF_SERVICES_COLLECTOR_MONITOR_MRNET_MPI_STATIC_LIBRARY
    CBTF_SERVICES_COMMON_SHARED_LIBRARY
    CBTF_SERVICES_COMMON_STATIC_LIBRARY
    CBTF_SERVICES_DATA_SHARED_LIBRARY
    CBTF_SERVICES_DATA_STATIC_LIBRARY
    CBTF_SERVICES_FILEIO_SHARED_LIBRARY
    CBTF_SERVICES_FILEIO_STATIC_LIBRARY
    CBTF_SERVICES_FPE_SHARED_LIBRARY
    CBTF_SERVICES_FPE_STATIC_LIBRARY
    CBTF_SERVICES_MONITOR_SHARED_LIBRARY
    CBTF_SERVICES_MONITOR_STATIC_LIBRARY
    CBTF_SERVICES_MRNET_SHARED_LIBRARY
    CBTF_SERVICES_MRNET_STATIC_LIBRARY
    CBTF_SERVICES_OFFLINE_SHARED_LIBRARY
    CBTF_SERVICES_OFFLINE_STATIC_LIBRARY
    CBTF_SERVICES_PAPI_SHARED_LIBRARY
    CBTF_SERVICES_PAPI_STATIC_LIBRARY
    CBTF_SERVICES_SEND_SHARED_LIBRARY
    CBTF_SERVICES_SEND_STATIC_LIBRARY
    CBTF_SERVICES_TIMER_SHARED_LIBRARY
    CBTF_SERVICES_TIMER_STATIC_LIBRARY
    CBTF_SERVICES_UNWIND_SHARED_LIBRARY
    CBTF_SERVICES_UNWIND_STATIC_LIBRARY
    CBTF_SERVICES_INCLUDE_DIR
    )
