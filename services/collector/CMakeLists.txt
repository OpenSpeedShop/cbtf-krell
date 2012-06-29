################################################################################
# Copyright (c) 2012 Argo Navis Technologies. All Rights Reserved.
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

set(SOURCES KrellInstitute/Services/Collector.h collector.c monitor.c)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CBTF_TOOLS_INCLUDE_DIR})

link_directories(${CBTF_TOOLS_LIBRARY_DIR})

add_library(cbtf-services-collector-monitor-fileio-static STATIC ${SOURCES})
add_library(cbtf-services-collector-monitor-mrnet-static STATIC ${SOURCES})
add_library(cbtf-services-collector-monitor-mrnet-mpi-static STATIC ${SOURCES})

add_library(cbtf-services-collector-monitor-fileio-shared SHARED ${SOURCES})
add_library(cbtf-services-collector-monitor-mrnet-shared SHARED ${SOURCES})
add_library(cbtf-services-collector-monitor-mrnet-mpi-shared SHARED ${SOURCES})

target_link_libraries(cbtf-services-collector-monitor-fileio-shared
    libcbtf-services-common.so
    libcbtf-services-fileio.so
    libcbtf-services-monitor.so
    libcbtf-services-offline.so
    )

target_link_libraries(cbtf-services-collector-monitor-mrnet-shared
    libcbtf-services-common.so
    libcbtf-services-monitor.so
    libcbtf-services-mrnet.so
    libcbtf-services-offline.so
    )

target_link_libraries(cbtf-services-collector-monitor-mrnet-mpi-shared
    libcbtf-services-common.so
    libcbtf-services-monitor.so
    libcbtf-services-mrnet.so
    libcbtf-services-offline.so
    )

set_target_properties(cbtf-services-collector-monitor-fileio-static
    PROPERTIES
        COMPILE_FLAGS "-DCBTF_SERVICE_USE_OFFLINE -DCBTF_SERVICE_USE_FILEIO"
        OUTPUT_NAME cbtf-services-collector-monitor-fileio
    )

set_target_properties(cbtf-services-collector-monitor-mrnet-static
    PROPERTIES
        COMPILE_FLAGS "-DCBTF_SERVICE_USE_OFFLINE -DCBTF_SERVICE_USE_MRNET"
        OUTPUT_NAME cbtf-services-collector-monitor-mrnet
    )

set_target_properties(cbtf-services-collector-monitor-mrnet-mpi-static
    PROPERTIES
        COMPILE_FLAGS "-DCBTF_SERVICE_USE_OFFLINE -DCBTF_SERVICE_USE_MRNET -DCBTF_SERVICE_USE_MRNET_MPI"
        OUTPUT_NAME cbtf-services-collector-monitor-mrnet-mpi
    )

set_target_properties(cbtf-services-collector-monitor-fileio-shared
    PROPERTIES
        COMPILE_FLAGS "-DCBTF_SERVICE_USE_OFFLINE -DCBTF_SERVICE_USE_FILEIO"
        OUTPUT_NAME cbtf-services-collector-monitor-fileio
        VERSION 0.0.0
    )

set_target_properties(cbtf-services-collector-monitor-mrnet-shared
    PROPERTIES
        COMPILE_FLAGS "-DCBTF_SERVICE_USE_OFFLINE -DCBTF_SERVICE_USE_MRNET"
        OUTPUT_NAME cbtf-services-collector-monitor-mrnet
        VERSION 0.0.0
    )

set_target_properties(cbtf-services-collector-monitor-mrnet-mpi-shared
    PROPERTIES
        COMPILE_FLAGS "-DCBTF_SERVICE_USE_OFFLINE -DCBTF_SERVICE_USE_MRNET -DCBTF_SERVICE_USE_MRNET_MPI"
        OUTPUT_NAME cbtf-services-collector-monitor-mrnet-mpi
        VERSION 0.0.0
    )

install(DIRECTORY KrellInstitute DESTINATION include)
 
install(
    TARGETS
        cbtf-services-collector-monitor-fileio-static
        cbtf-services-collector-monitor-mrnet-static
        cbtf-services-collector-monitor-mrnet-mpi-static
        cbtf-services-collector-monitor-fileio-shared
        cbtf-services-collector-monitor-mrnet-shared
        cbtf-services-collector-monitor-mrnet-mpi-shared
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib${LIB_SUFFIX}
    ARCHIVE DESTINATION lib${LIB_SUFFIX}
    )
