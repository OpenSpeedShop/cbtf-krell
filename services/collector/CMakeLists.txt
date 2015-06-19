################################################################################
# Copyright (c) 2014-2015 Krell Institute. All Rights Reserved.
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

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")

if (LIBMONITOR_FOUND)

set(OFFLINE_SOURCES
	monitor.c
)

add_library(cbtf-services-collector-monitor-fileio SHARED
	${OFFLINE_SOURCES}
	collector.c
)

target_include_directories(cbtf-services-collector-monitor-fileio PUBLIC
	${PROJECT_SOURCE_DIR}/services/include
	${PROJECT_SOURCE_DIR}/messages/include
	${CMAKE_INSTALL_PREFIX}/include
	${LibMonitor_INCLUDE_DIRS}
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../../messages/src/events
	${CMAKE_CURRENT_BINARY_DIR}/../../messages/src/perfdata
	${CMAKE_CURRENT_BINARY_DIR}/../../messages/src/base
)

target_link_libraries(cbtf-services-collector-monitor-fileio
	${LibMonitor_SHARED_LIBRARIES}
	pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(cbtf-services-collector-monitor-fileio PROPERTIES VERSION 1.1.0)
set_target_properties(cbtf-services-collector-monitor-fileio PROPERTIES
	COMPILE_DEFINITIONS "${TLS_DEFINES};CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_FILEIO") 

install(TARGETS cbtf-services-collector-monitor-fileio
	LIBRARY DESTINATION lib${LIB_SUFFIX}
)

# Create and install static version

add_library(cbtf-services-collector-monitor-fileio-static STATIC
	${OFFLINE_SOURCES}
	collector.c
)

target_include_directories(cbtf-services-collector-monitor-fileio-static PUBLIC
	${PROJECT_SOURCE_DIR}/services/include
	${PROJECT_SOURCE_DIR}/messages/include
	${CMAKE_INSTALL_PREFIX}/include
	${LibMonitor_INCLUDE_DIRS}
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../../messages/src/events
	${CMAKE_CURRENT_BINARY_DIR}/../../messages/src/perfdata
	${CMAKE_CURRENT_BINARY_DIR}/../../messages/src/base
)

target_link_libraries(cbtf-services-collector-monitor-fileio-static
	${LibMonitor_SHARED_LIBRARIES}
	pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(cbtf-services-collector-monitor-fileio-static PROPERTIES VERSION 1.1.0)
set_target_properties(cbtf-services-collector-monitor-fileio-static PROPERTIES
	COMPILE_DEFINITIONS "${TLS_DEFINES};CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_FILEIO") 

install(TARGETS cbtf-services-collector-monitor-fileio-static
	ARCHIVE DESTINATION lib${LIB_SUFFIX}
)

if (MRNET_FOUND)

add_library(cbtf-services-collector-monitor-mrnet SHARED
	${OFFLINE_SOURCES}
	collector.c
)

target_include_directories(cbtf-services-collector-monitor-mrnet PUBLIC
	${PROJECT_SOURCE_DIR}/services/include
	${PROJECT_SOURCE_DIR}/messages/include
	${CMAKE_INSTALL_PREFIX}/include
	${LibMonitor_INCLUDE_DIRS}
	${MRNet_INCLUDE_DIRS}
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../../messages/src/events
	${CMAKE_CURRENT_BINARY_DIR}/../../messages/src/perfdata
	${CMAKE_CURRENT_BINARY_DIR}/../../messages/src/base
)

target_link_libraries(cbtf-services-collector-monitor-mrnet
	${LibMonitor_SHARED_LIBRARIES}
	${MRNet_XPLAT_LWR_SHARED_LIBRARY}
	${MRNet_MRNET_LWR_SHARED_LIBRARY}
        pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(cbtf-services-collector-monitor-mrnet PROPERTIES VERSION 1.1.0)
set_target_properties(cbtf-services-collector-monitor-mrnet PROPERTIES
	COMPILE_DEFINITIONS "${TLS_DEFINES};CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_MRNET;${MRNet_DEFINES}") 

install(TARGETS cbtf-services-collector-monitor-mrnet
	LIBRARY DESTINATION lib${LIB_SUFFIX}
)

# Create and install static version

add_library(cbtf-services-collector-monitor-mrnet-static STATIC
	${OFFLINE_SOURCES}
	collector.c
)

target_include_directories(cbtf-services-collector-monitor-mrnet-static PUBLIC
	${PROJECT_SOURCE_DIR}/services/include
	${PROJECT_SOURCE_DIR}/messages/include
	${CMAKE_INSTALL_PREFIX}/include
	${LibMonitor_INCLUDE_DIRS}
	${MRNet_INCLUDE_DIRS}
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../../messages/src/events
	${CMAKE_CURRENT_BINARY_DIR}/../../messages/src/perfdata
	${CMAKE_CURRENT_BINARY_DIR}/../../messages/src/base
)

target_link_libraries(cbtf-services-collector-monitor-mrnet-static
	${LibMonitor_SHARED_LIBRARIES}
	${MRNet_XPLAT_LWR_SHARED_LIBRARY}
	${MRNet_MRNET_LWR_SHARED_LIBRARY}
        pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(cbtf-services-collector-monitor-mrnet-static PROPERTIES VERSION 1.1.0)
set_target_properties(cbtf-services-collector-monitor-mrnet-static PROPERTIES
	COMPILE_DEFINITIONS "${TLS_DEFINES};CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_MRNET;${MRNet_DEFINES}") 

install(TARGETS cbtf-services-collector-monitor-mrnet-static
	ARCHIVE DESTINATION lib${LIB_SUFFIX}
)

add_library(cbtf-services-collector-monitor-mrnet-mpi SHARED
	${OFFLINE_SOURCES}
	collector.c
)

target_include_directories(cbtf-services-collector-monitor-mrnet-mpi PUBLIC
	${PROJECT_SOURCE_DIR}/services/include
	${PROJECT_SOURCE_DIR}/messages/include
	${CMAKE_INSTALL_PREFIX}/include
	${LibMonitor_INCLUDE_DIRS}
	${MRNet_INCLUDE_DIRS}
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../../messages/src/events
	${CMAKE_CURRENT_BINARY_DIR}/../../messages/src/perfdata
	${CMAKE_CURRENT_BINARY_DIR}/../../messages/src/base
)

target_link_libraries(cbtf-services-collector-monitor-mrnet-mpi
	${LibMonitor_SHARED_LIBRARIES}
	${MRNet_XPLAT_LWR_SHARED_LIBRARY}
	${MRNet_MRNET_LWR_SHARED_LIBRARY}
        pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(cbtf-services-collector-monitor-mrnet-mpi PROPERTIES VERSION 1.1.0)
set_target_properties(cbtf-services-collector-monitor-mrnet-mpi PROPERTIES
	COMPILE_DEFINITIONS "${TLS_DEFINES};CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_MRNET;CBTF_SERVICE_USE_MRNET_MPI;${MRNet_DEFINES}") 

install(TARGETS cbtf-services-collector-monitor-mrnet-mpi
	LIBRARY DESTINATION lib${LIB_SUFFIX}
)

# Create and install static version

add_library(cbtf-services-collector-monitor-mrnet-mpi-static STATIC
	${OFFLINE_SOURCES}
	collector.c
)

target_include_directories(cbtf-services-collector-monitor-mrnet-mpi-static PUBLIC
	${PROJECT_SOURCE_DIR}/services/include
	${PROJECT_SOURCE_DIR}/messages/include
	${CMAKE_INSTALL_PREFIX}/include
	${LibMonitor_INCLUDE_DIRS}
	${MRNet_INCLUDE_DIRS}
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../../messages/src/events
	${CMAKE_CURRENT_BINARY_DIR}/../../messages/src/perfdata
	${CMAKE_CURRENT_BINARY_DIR}/../../messages/src/base
)

target_link_libraries(cbtf-services-collector-monitor-mrnet-mpi-static
	${LibMonitor_SHARED_LIBRARIES}
	${MRNet_XPLAT_LWR_SHARED_LIBRARY}
	${MRNet_MRNET_LWR_SHARED_LIBRARY}
        pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(cbtf-services-collector-monitor-mrnet-mpi-static PROPERTIES VERSION 1.1.0)
set_target_properties(cbtf-services-collector-monitor-mrnet-mpi-static PROPERTIES
	COMPILE_DEFINITIONS "${TLS_DEFINES};CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_MRNET;CBTF_SERVICE_USE_MRNET_MPI;${MRNet_DEFINES}") 

install(TARGETS cbtf-services-collector-monitor-mrnet-mpi-static
	ARCHIVE DESTINATION lib${LIB_SUFFIX}
)

endif()

endif()


install (FILES ${PROJECT_SOURCE_DIR}/services/collector/KrellInstitute/Services/Collector.h
	DESTINATION include/KrellInstitute/Services
)