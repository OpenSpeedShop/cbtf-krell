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

if (LIBMONITOR_FOUND)

set(COLLECTOR_SOURCES
	collector.c
)

add_library(hwcsamp-collector-monitor-fileio MODULE
	${COLLECTOR_SOURCES}
)

target_include_directories(hwcsamp-collector-monitor-fileio PUBLIC
	${LibMonitor_INCLUDE_DIRS}
	${Papi_INCLUDE_DIRS}
	${PROJECT_SOURCE_DIR}/messages/include
	${PROJECT_SOURCE_DIR}/services/include
	${PROJECT_SOURCE_DIR}/services/collector
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/perfdata
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/events
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/base
)

target_link_libraries(hwcsamp-collector-monitor-fileio
	cbtf-services-common
	cbtf-services-monitor
	cbtf-services-offline
	cbtf-services-timer
	cbtf-services-collector-monitor-fileio
	${LibMonitor_SHARED_LIBRARIES}
	pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(hwcsamp-collector-monitor-fileio PROPERTIES PREFIX "")
#
# These defines apply only for this target module.  Note the OMPT defines are
# included here.  
# If present, will activate OpenMP runtime performance information gathering. 
#
set_target_properties(hwcsamp-collector-monitor-fileio PROPERTIES
        COMPILE_DEFINITIONS "CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_FILEIO;${OMPT_DEFINES}")

install(TARGETS hwcsamp-collector-monitor-fileio
        LIBRARY DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors
)

# Create and install static versions

add_library(hwcsamp-collector-monitor-fileio-static STATIC
	${COLLECTOR_SOURCES}
)

target_include_directories(hwcsamp-collector-monitor-fileio-static PUBLIC
	${LibMonitor_INCLUDE_DIRS}
	${Papi_INCLUDE_DIRS}
	${PROJECT_SOURCE_DIR}/messages/include
	${PROJECT_SOURCE_DIR}/services/include
	${PROJECT_SOURCE_DIR}/services/collector
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/perfdata
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/events
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/base
)

target_link_libraries(hwcsamp-collector-monitor-fileio-static
	cbtf-services-common-static
	cbtf-services-monitor-static
	cbtf-services-offline-static
	cbtf-services-timer-static
	cbtf-services-collector-monitor-fileio-static
	${LibMonitor_STATIC_LIBRARIES}
	pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(hwcsamp-collector-monitor-fileio-static PROPERTIES PREFIX "")
set_target_properties(hwcsamp-collector-monitor-fileio-static PROPERTIES
        COMPILE_DEFINITIONS "CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_FILEIO;${OMPT_DEFINES}")


install(TARGETS hwcsamp-collector-monitor-fileio-static
        ARCHIVE DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors
)

# Create and install static versions

add_library(hwcsamp-monitor-fileio STATIC
	${COLLECTOR_SOURCES}
)

target_include_directories(hwcsamp-monitor-fileio PUBLIC
	${LibMonitor_INCLUDE_DIRS}
	${Papi_INCLUDE_DIRS}
	${PROJECT_SOURCE_DIR}/messages/include
	${PROJECT_SOURCE_DIR}/services/include
	${PROJECT_SOURCE_DIR}/services/collector
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/perfdata
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/events
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/base
)

target_link_libraries(hwcsamp-monitor-fileio
	cbtf-services-common-static
	cbtf-services-monitor-static
	cbtf-services-offline-static
	cbtf-services-timer-static
	cbtf-services-collector-monitor-fileio-static
	${LibMonitor_STATIC_LIBRARIES}
	pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(hwcsamp-monitor-fileio PROPERTIES
        COMPILE_DEFINITIONS "CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_FILEIO;${OMPT_DEFINES}")

install(TARGETS hwcsamp-monitor-fileio
        ARCHIVE DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors/static
)

if (MRNET_FOUND)

add_library(hwcsamp-collector-monitor-mrnet MODULE
	${COLLECTOR_SOURCES}
)

target_include_directories(hwcsamp-collector-monitor-mrnet PUBLIC
	${LibMonitor_INCLUDE_DIRS}
	${Papi_INCLUDE_DIRS}
	${PROJECT_SOURCE_DIR}/messages/include
	${PROJECT_SOURCE_DIR}/services/include
	${PROJECT_SOURCE_DIR}/services/collector
	${MRNet_INCLUDE_DIRS}
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/perfdata
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/events
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/base
)

target_link_libraries(hwcsamp-collector-monitor-mrnet
	cbtf-services-common
	cbtf-services-monitor
	cbtf-services-offline
	cbtf-services-timer
	cbtf-services-collector-monitor-mrnet
	${LibMonitor_SHARED_LIBRARIES}
	${MRNet_MRNET_LWR_SHARED_LIBRARIES}
	pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(hwcsamp-collector-monitor-mrnet PROPERTIES PREFIX "")
set_target_properties(hwcsamp-collector-monitor-mrnet PROPERTIES
        COMPILE_DEFINITIONS "CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_MRNET;${MRNet_DEFINES};${OMPT_DEFINES}")

install(TARGETS hwcsamp-collector-monitor-mrnet
	LIBRARY DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors
)

# Create and install static versions

add_library(hwcsamp-collector-monitor-mrnet-static STATIC
	${COLLECTOR_SOURCES}
)

target_include_directories(hwcsamp-collector-monitor-mrnet-static PUBLIC
	${LibMonitor_INCLUDE_DIRS}
	${Papi_INCLUDE_DIRS}
	${PROJECT_SOURCE_DIR}/messages/include
	${PROJECT_SOURCE_DIR}/services/include
	${PROJECT_SOURCE_DIR}/services/collector
	${MRNet_INCLUDE_DIRS}
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/perfdata
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/events
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/base
)

target_link_libraries(hwcsamp-collector-monitor-mrnet-static
	cbtf-services-common-static
	cbtf-services-monitor-static
	cbtf-services-offline-static
	cbtf-services-timer-static
	cbtf-services-collector-monitor-mrnet-static
	${LibMonitor_STATIC_LIBRARIES}
	${MRNet_MRNET_LWR_STATIC_LIBRARIES}
	pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(hwcsamp-collector-monitor-mrnet-static PROPERTIES PREFIX "")
set_target_properties(hwcsamp-collector-monitor-mrnet-static PROPERTIES
        COMPILE_DEFINITIONS "CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_MRNET;${MRNet_DEFINES};${OMPT_DEFINES}")

install(TARGETS hwcsamp-collector-monitor-mrnet-static
	ARCHIVE DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors
)

# Create and install static versions

add_library(hwcsamp-monitor-mrnet STATIC
	${COLLECTOR_SOURCES}
)

target_include_directories(hwcsamp-monitor-mrnet PUBLIC
	${LibMonitor_INCLUDE_DIRS}
	${Papi_INCLUDE_DIRS}
	${PROJECT_SOURCE_DIR}/messages/include
	${PROJECT_SOURCE_DIR}/services/include
	${PROJECT_SOURCE_DIR}/services/collector
	${MRNet_INCLUDE_DIRS}
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/perfdata
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/events
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/base
)

target_link_libraries(hwcsamp-monitor-mrnet
	cbtf-services-common-static
	cbtf-services-monitor-static
	cbtf-services-offline-static
	cbtf-services-timer-static
	cbtf-services-collector-monitor-mrnet-static
	${LibMonitor_STATIC_LIBRARIES}
	${MRNet_MRNET_LWR_STATIC_LIBRARIES}
	pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(hwcsamp-monitor-mrnet PROPERTIES
        COMPILE_DEFINITIONS "CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_MRNET;${MRNet_DEFINES};${OMPT_DEFINES}")

install(TARGETS hwcsamp-monitor-mrnet
	ARCHIVE DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors/static
)

add_library(hwcsamp-collector-monitor-mrnet-mpi MODULE
	${COLLECTOR_SOURCES}
)

target_include_directories(hwcsamp-collector-monitor-mrnet-mpi PUBLIC
	${LibMonitor_INCLUDE_DIRS}
	${Papi_INCLUDE_DIRS}
	${PROJECT_SOURCE_DIR}/messages/include
	${PROJECT_SOURCE_DIR}/services/include
	${PROJECT_SOURCE_DIR}/services/collector
	${MRNet_INCLUDE_DIRS}
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/perfdata
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/events
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/base
)

target_link_libraries(hwcsamp-collector-monitor-mrnet-mpi
	cbtf-services-common
	cbtf-services-monitor
	cbtf-services-offline
	cbtf-services-timer
	cbtf-services-data
	cbtf-services-collector-monitor-mrnet-mpi
	${LibMonitor_SHARED_LIBRARIES}
	${MRNet_MRNET_LWR_SHARED_LIBRARIES}
	pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(hwcsamp-collector-monitor-mrnet-mpi PROPERTIES PREFIX "")
set_target_properties(hwcsamp-collector-monitor-mrnet-mpi PROPERTIES
        COMPILE_DEFINITIONS "CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_MRNET;CBTF_SERVICE_USE_MRNET_MPI;${MRNet_DEFINES};${OMPT_DEFINES}")

install(TARGETS hwcsamp-collector-monitor-mrnet-mpi
	LIBRARY DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors
)

# Create and install static versions

add_library(hwcsamp-collector-monitor-mrnet-mpi-static STATIC
	${COLLECTOR_SOURCES}
)

target_include_directories(hwcsamp-collector-monitor-mrnet-mpi-static PUBLIC
	${LibMonitor_INCLUDE_DIRS}
	${Papi_INCLUDE_DIRS}
	${PROJECT_SOURCE_DIR}/messages/include
	${PROJECT_SOURCE_DIR}/services/include
	${PROJECT_SOURCE_DIR}/services/collector
	${MRNet_INCLUDE_DIRS}
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/perfdata
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/events
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/base
)

target_link_libraries(hwcsamp-collector-monitor-mrnet-mpi-static
	cbtf-services-common-static
	cbtf-services-monitor-static
	cbtf-services-offline-static
	cbtf-services-timer-static
	cbtf-services-data-static
	cbtf-services-collector-monitor-mrnet-mpi-static
	${LibMonitor_STATIC_LIBRARIES}
	${MRNet_MRNET_LWR_STATIC_LIBRARIES}
	pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(hwcsamp-collector-monitor-mrnet-mpi-static PROPERTIES PREFIX "")
set_target_properties(hwcsamp-collector-monitor-mrnet-mpi-static PROPERTIES
        COMPILE_DEFINITIONS "CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_MRNET;CBTF_SERVICE_USE_MRNET_MPI;${MRNet_DEFINES};${OMPT_DEFINES}")

install(TARGETS hwcsamp-collector-monitor-mrnet-mpi-static
	ARCHIVE DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors
)

# Create and install static versions

add_library(hwcsamp-monitor-mrnet-mpi STATIC
	${COLLECTOR_SOURCES}
)

target_include_directories(hwcsamp-monitor-mrnet-mpi PUBLIC
	${LibMonitor_INCLUDE_DIRS}
	${Papi_INCLUDE_DIRS}
	${PROJECT_SOURCE_DIR}/messages/include
	${PROJECT_SOURCE_DIR}/services/include
	${PROJECT_SOURCE_DIR}/services/collector
	${MRNet_INCLUDE_DIRS}
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/perfdata
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/events
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/base
)

target_link_libraries(hwcsamp-monitor-mrnet-mpi
	cbtf-services-common-static
	cbtf-services-monitor-static
	cbtf-services-offline-static
	cbtf-services-timer-static
	cbtf-services-data-static
	cbtf-services-collector-monitor-mrnet-mpi-static
	${LibMonitor_STATIC_LIBRARIES}
	${MRNet_MRNET_LWR_STATIC_LIBRARIES}
	pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(hwcsamp-monitor-mrnet-mpi PROPERTIES VERSION 1.1.0)
set_target_properties(hwcsamp-monitor-mrnet-mpi PROPERTIES
        COMPILE_DEFINITIONS "CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_MRNET;CBTF_SERVICE_USE_MRNET_MPI;${MRNet_DEFINES};${OMPT_DEFINES}")

install(TARGETS hwcsamp-monitor-mrnet-mpi
	ARCHIVE DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors/static
)

endif()


endif()

set(component_location ${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}/KrellInstitute/Components)
set(cbtflib_location ${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX})
set(collector_name "hwcsamp")
configure_file(../template.xml.in hwcsamp.xml @ONLY)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/hwcsamp.xml
        DESTINATION share/KrellInstitute/xml)
