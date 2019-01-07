################################################################################
# Copyright (c) 2014-2019 Krell Institute. All Rights Reserved.
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


include(CheckIncludeFile)

if (LIBMONITOR_FOUND)

set(OFFLINE_SOURCES
	monitor.c
)

add_library(cbtf-services-collector-monitor-fileio SHARED
	${OFFLINE_SOURCES}
	collector.c
)

target_include_directories(cbtf-services-collector-monitor-fileio PUBLIC
	${Libtirpc_INCLUDE_DIRS}
	${PROJECT_SOURCE_DIR}/services/include
	${PROJECT_SOURCE_DIR}/messages/include
	${LibMonitor_INCLUDE_DIRS}
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../../messages/src/events
	${CMAKE_CURRENT_BINARY_DIR}/../../messages/src/perfdata
	${CMAKE_CURRENT_BINARY_DIR}/../../messages/src/base
)

target_link_libraries(cbtf-services-collector-monitor-fileio
        -Wl,--no-as-needed
	${LibMonitor_SHARED_LIBRARIES}
	pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(cbtf-services-collector-monitor-fileio PROPERTIES VERSION 1.1.0)
set_target_properties(cbtf-services-collector-monitor-fileio PROPERTIES
	COMPILE_DEFINITIONS "${TLS_DEFINES};CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_FILEIO") 
set_target_properties(cbtf-services-collector-monitor-fileio PROPERTIES POSITION_INDEPENDENT_CODE ON)

install(TARGETS cbtf-services-collector-monitor-fileio
	LIBRARY DESTINATION lib${LIB_SUFFIX}
)

# Create and install static version

add_library(cbtf-services-collector-monitor-fileio-static STATIC
	${OFFLINE_SOURCES}
	collector.c
)

target_include_directories(cbtf-services-collector-monitor-fileio-static PUBLIC
	${Libtirpc_INCLUDE_DIRS}
	${PROJECT_SOURCE_DIR}/services/include
	${PROJECT_SOURCE_DIR}/messages/include
	${LibMonitor_INCLUDE_DIRS}
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../../messages/src/events
	${CMAKE_CURRENT_BINARY_DIR}/../../messages/src/perfdata
	${CMAKE_CURRENT_BINARY_DIR}/../../messages/src/base
)

target_link_libraries(cbtf-services-collector-monitor-fileio-static
        -Wl,--no-as-needed
	${LibMonitor_SHARED_LIBRARIES}
	pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(cbtf-services-collector-monitor-fileio-static PROPERTIES VERSION 1.1.0)
set_target_properties(cbtf-services-collector-monitor-fileio-static PROPERTIES OUTPUT_NAME cbtf-services-collector-monitor-fileio)
set_target_properties(cbtf-services-collector-monitor-fileio-static PROPERTIES
	COMPILE_DEFINITIONS "${TLS_DEFINES};CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_FILEIO") 
set_target_properties(cbtf-services-collector-monitor-fileio-static PROPERTIES POSITION_INDEPENDENT_CODE ON)

install(TARGETS cbtf-services-collector-monitor-fileio-static
	ARCHIVE DESTINATION lib${LIB_SUFFIX}
)

if (MRNET_FOUND)

add_library(cbtf-services-collector-monitor-mrnet SHARED
	${OFFLINE_SOURCES}
	collector.c
)

target_include_directories(cbtf-services-collector-monitor-mrnet PUBLIC
	${Libtirpc_INCLUDE_DIRS}
	${PROJECT_SOURCE_DIR}/services/include
	${PROJECT_SOURCE_DIR}/messages/include
	${LibMonitor_INCLUDE_DIRS}
	${MRNet_INCLUDE_DIRS}
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../../messages/src/events
	${CMAKE_CURRENT_BINARY_DIR}/../../messages/src/perfdata
	${CMAKE_CURRENT_BINARY_DIR}/../../messages/src/base
)

target_link_libraries(cbtf-services-collector-monitor-mrnet
        -Wl,--no-as-needed
	${LibMonitor_SHARED_LIBRARIES}
	${MRNet_LWR_SHARED_LIBRARIES}
        pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(cbtf-services-collector-monitor-mrnet PROPERTIES VERSION 1.1.0)
set_target_properties(cbtf-services-collector-monitor-mrnet PROPERTIES
	COMPILE_DEFINITIONS "${TLS_DEFINES};CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_MRNET;${MRNet_DEFINES}") 
set_target_properties(cbtf-services-collector-monitor-mrnet PROPERTIES POSITION_INDEPENDENT_CODE ON)

install(TARGETS cbtf-services-collector-monitor-mrnet
	LIBRARY DESTINATION lib${LIB_SUFFIX}
)

# Create and install static version

add_library(cbtf-services-collector-monitor-mrnet-static STATIC
	${OFFLINE_SOURCES}
	collector.c
)

target_include_directories(cbtf-services-collector-monitor-mrnet-static PUBLIC
	${Libtirpc_INCLUDE_DIRS}
	${PROJECT_SOURCE_DIR}/services/include
	${PROJECT_SOURCE_DIR}/messages/include
	${LibMonitor_INCLUDE_DIRS}
	${MRNet_INCLUDE_DIRS}
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../../messages/src/events
	${CMAKE_CURRENT_BINARY_DIR}/../../messages/src/perfdata
	${CMAKE_CURRENT_BINARY_DIR}/../../messages/src/base
)

target_link_libraries(cbtf-services-collector-monitor-mrnet-static
        -Wl,--no-as-needed
	${LibMonitor_SHARED_LIBRARIES}
	${MRNet_LWR_SHARED_LIBRARIES}
        pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(cbtf-services-collector-monitor-mrnet-static PROPERTIES VERSION 1.1.0)
set_target_properties(cbtf-services-collector-monitor-mrnet-static PROPERTIES OUTPUT_NAME cbtf-services-collector-monitor-mrnet)
set_target_properties(cbtf-services-collector-monitor-mrnet-static PROPERTIES
	COMPILE_DEFINITIONS "${TLS_DEFINES};CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_MRNET;${MRNet_DEFINES}") 
set_target_properties(cbtf-services-collector-monitor-mrnet-static PROPERTIES POSITION_INDEPENDENT_CODE ON)

install(TARGETS cbtf-services-collector-monitor-mrnet-static
	ARCHIVE DESTINATION lib${LIB_SUFFIX}
)

add_library(cbtf-services-collector-monitor-mrnet-mpi SHARED
	${OFFLINE_SOURCES}
	collector.c
)

target_include_directories(cbtf-services-collector-monitor-mrnet-mpi PUBLIC
	${Libtirpc_INCLUDE_DIRS}
	${PROJECT_SOURCE_DIR}/services/include
	${PROJECT_SOURCE_DIR}/messages/include
	${LibMonitor_INCLUDE_DIRS}
	${MRNet_INCLUDE_DIRS}
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../../messages/src/events
	${CMAKE_CURRENT_BINARY_DIR}/../../messages/src/perfdata
	${CMAKE_CURRENT_BINARY_DIR}/../../messages/src/base
)

target_link_libraries(cbtf-services-collector-monitor-mrnet-mpi
        -Wl,--no-as-needed
	${MRNet_LWR_SHARED_LIBRARIES}
	${LibMonitor_SHARED_LIBRARIES}
        pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(cbtf-services-collector-monitor-mrnet-mpi PROPERTIES VERSION 1.1.0)
set_target_properties(cbtf-services-collector-monitor-mrnet-mpi PROPERTIES
	COMPILE_DEFINITIONS "${TLS_DEFINES};CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_MRNET;CBTF_SERVICE_USE_MRNET_MPI;${MRNet_DEFINES}") 
set_target_properties(cbtf-services-collector-monitor-mrnet-mpi PROPERTIES POSITION_INDEPENDENT_CODE ON)

install(TARGETS cbtf-services-collector-monitor-mrnet-mpi
	LIBRARY DESTINATION lib${LIB_SUFFIX}
)

# Create and install static version

add_library(cbtf-services-collector-monitor-mrnet-mpi-static STATIC
	${OFFLINE_SOURCES}
	collector.c
)

target_include_directories(cbtf-services-collector-monitor-mrnet-mpi-static PUBLIC
	${Libtirpc_INCLUDE_DIRS}
	${PROJECT_SOURCE_DIR}/services/include
	${PROJECT_SOURCE_DIR}/messages/include
	${LibMonitor_INCLUDE_DIRS}
	${MRNet_INCLUDE_DIRS}
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../../messages/src/events
	${CMAKE_CURRENT_BINARY_DIR}/../../messages/src/perfdata
	${CMAKE_CURRENT_BINARY_DIR}/../../messages/src/base
)

target_link_libraries(cbtf-services-collector-monitor-mrnet-mpi-static
        -Wl,--no-as-needed
	${LibMonitor_SHARED_LIBRARIES}
	${MRNet_LWR_SHARED_LIBRARIES}
        pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(cbtf-services-collector-monitor-mrnet-mpi-static PROPERTIES VERSION 1.1.0)
set_target_properties(cbtf-services-collector-monitor-mrnet-mpi-static PROPERTIES OUTPUT_NAME cbtf-services-collector-monitor-mrnet-mpi)
set_target_properties(cbtf-services-collector-monitor-mrnet-mpi-static PROPERTIES
	COMPILE_DEFINITIONS "${TLS_DEFINES};CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_MRNET;CBTF_SERVICE_USE_MRNET_MPI;${MRNet_DEFINES}") 
set_target_properties(cbtf-services-collector-monitor-mrnet-mpi-static PROPERTIES POSITION_INDEPENDENT_CODE ON)

install(TARGETS cbtf-services-collector-monitor-mrnet-mpi-static
	ARCHIVE DESTINATION lib${LIB_SUFFIX}
)

endif()

endif()


install (FILES ${PROJECT_SOURCE_DIR}/services/collector/KrellInstitute/Services/Collector.h
	DESTINATION include/KrellInstitute/Services
)
