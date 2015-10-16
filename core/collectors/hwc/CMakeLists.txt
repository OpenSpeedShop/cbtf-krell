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

if (NOT BUILD_TARGETED_FE)

if (LIBMONITOR_FOUND)

set(COLLECTOR_SOURCES
	collector.c
)

add_library(hwc-collector-monitor-fileio MODULE
	${COLLECTOR_SOURCES}
)

target_include_directories(hwc-collector-monitor-fileio PUBLIC
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

target_link_libraries(hwc-collector-monitor-fileio
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

set_target_properties(hwc-collector-monitor-fileio PROPERTIES PREFIX "")
#
# These defines apply only for this target module.  Note the OMPT defines are
# included here.  
# If present, will activate OpenMP runtime performance information gathering. 
#
set_target_properties(hwc-collector-monitor-fileio PROPERTIES
        COMPILE_DEFINITIONS "${TLS_DEFINES};CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_FILEIO")

install(TARGETS hwc-collector-monitor-fileio
        LIBRARY DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors
)

# Create and install static versions

add_library(hwc-collector-monitor-fileio-static STATIC
	${COLLECTOR_SOURCES}
)

target_include_directories(hwc-collector-monitor-fileio-static PUBLIC
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

target_link_libraries(hwc-collector-monitor-fileio-static
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

set_target_properties(hwc-collector-monitor-fileio-static PROPERTIES PREFIX "")
set_target_properties(hwc-collector-monitor-fileio-static PROPERTIES
        COMPILE_DEFINITIONS "${TLS_DEFINES};CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_FILEIO")


install(TARGETS hwc-collector-monitor-fileio-static
        ARCHIVE DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors
)

# Create and install static versions

add_library(hwc-monitor-fileio STATIC
	${COLLECTOR_SOURCES}
)

target_include_directories(hwc-monitor-fileio PUBLIC
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

target_link_libraries(hwc-monitor-fileio
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

set_target_properties(hwc-monitor-fileio PROPERTIES
        COMPILE_DEFINITIONS "${TLS_DEFINES};CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_FILEIO")

install(TARGETS hwc-monitor-fileio
        ARCHIVE DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors/static
)

if (MRNET_FOUND)

add_library(hwc-collector-monitor-mrnet MODULE
	${COLLECTOR_SOURCES}
)

target_include_directories(hwc-collector-monitor-mrnet PUBLIC
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

target_link_libraries(hwc-collector-monitor-mrnet
	cbtf-services-common
	cbtf-services-monitor
	cbtf-services-offline
	cbtf-services-timer
	cbtf-services-collector-monitor-mrnet
	${LibMonitor_SHARED_LIBRARIES}
	${MRNet_LWR_SHARED_LIBRARIES}
	pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(hwc-collector-monitor-mrnet PROPERTIES PREFIX "")
set_target_properties(hwc-collector-monitor-mrnet PROPERTIES
        COMPILE_DEFINITIONS "${TLS_DEFINES};CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_MRNET;${MRNet_DEFINES}")

install(TARGETS hwc-collector-monitor-mrnet
	LIBRARY DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors
)

# Create and install static versions

add_library(hwc-collector-monitor-mrnet-static STATIC
	${COLLECTOR_SOURCES}
)

target_include_directories(hwc-collector-monitor-mrnet-static PUBLIC
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

target_link_libraries(hwc-collector-monitor-mrnet-static
	cbtf-services-common-static
	cbtf-services-monitor-static
	cbtf-services-offline-static
	cbtf-services-timer-static
	cbtf-services-collector-monitor-mrnet-static
	${LibMonitor_STATIC_LIBRARIES}
	${MRNet_LWR_STATIC_LIBRARIES}
	pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(hwc-collector-monitor-mrnet-static PROPERTIES PREFIX "")
set_target_properties(hwc-collector-monitor-mrnet-static PROPERTIES
        COMPILE_DEFINITIONS "${TLS_DEFINES};CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_MRNET;${MRNet_DEFINES}")

install(TARGETS hwc-collector-monitor-mrnet-static
	ARCHIVE DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors
)

# Create and install static versions

add_library(hwc-monitor-mrnet STATIC
	${COLLECTOR_SOURCES}
)

target_include_directories(hwc-monitor-mrnet PUBLIC
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

target_link_libraries(hwc-monitor-mrnet
	cbtf-services-common-static
	cbtf-services-monitor-static
	cbtf-services-offline-static
	cbtf-services-timer-static
	cbtf-services-collector-monitor-mrnet-static
	${LibMonitor_STATIC_LIBRARIES}
	${MRNet_LWR_STATIC_LIBRARIES}
	pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(hwc-monitor-mrnet PROPERTIES
        COMPILE_DEFINITIONS "${TLS_DEFINES};CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_MRNET;${MRNet_DEFINES}")

install(TARGETS hwc-monitor-mrnet
	ARCHIVE DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors/static
)

add_library(hwc-collector-monitor-mrnet-mpi MODULE
	${COLLECTOR_SOURCES}
)

target_include_directories(hwc-collector-monitor-mrnet-mpi PUBLIC
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

target_link_libraries(hwc-collector-monitor-mrnet-mpi
	cbtf-services-common
	cbtf-services-monitor
	cbtf-services-offline
	cbtf-services-timer
	cbtf-services-data
	cbtf-services-collector-monitor-mrnet-mpi
	${LibMonitor_SHARED_LIBRARIES}
	${MRNet_LWR_SHARED_LIBRARIES}
	pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(hwc-collector-monitor-mrnet-mpi PROPERTIES PREFIX "")
set_target_properties(hwc-collector-monitor-mrnet-mpi PROPERTIES
        COMPILE_DEFINITIONS "${TLS_DEFINES};CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_MRNET;CBTF_SERVICE_USE_MRNET_MPI;${MRNet_DEFINES}")

install(TARGETS hwc-collector-monitor-mrnet-mpi
	LIBRARY DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors
)

# Create and install static versions

add_library(hwc-collector-monitor-mrnet-mpi-static STATIC
	${COLLECTOR_SOURCES}
)

target_include_directories(hwc-collector-monitor-mrnet-mpi-static PUBLIC
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

target_link_libraries(hwc-collector-monitor-mrnet-mpi-static
	cbtf-services-common-static
	cbtf-services-monitor-static
	cbtf-services-offline-static
	cbtf-services-timer-static
	cbtf-services-data-static
	cbtf-services-collector-monitor-mrnet-mpi-static
	${LibMonitor_STATIC_LIBRARIES}
	${MRNet_LWR_STATIC_LIBRARIES}
	pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(hwc-collector-monitor-mrnet-mpi-static PROPERTIES PREFIX "")
set_target_properties(hwc-collector-monitor-mrnet-mpi-static PROPERTIES
        COMPILE_DEFINITIONS "${TLS_DEFINES};CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_MRNET;CBTF_SERVICE_USE_MRNET_MPI;${MRNet_DEFINES}")

install(TARGETS hwc-collector-monitor-mrnet-mpi-static
	ARCHIVE DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors
)

# Create and install static versions

add_library(hwc-monitor-mrnet-mpi STATIC
	${COLLECTOR_SOURCES}
)

target_include_directories(hwc-monitor-mrnet-mpi PUBLIC
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

target_link_libraries(hwc-monitor-mrnet-mpi
	cbtf-services-common-static
	cbtf-services-monitor-static
	cbtf-services-offline-static
	cbtf-services-timer-static
	cbtf-services-data-static
	cbtf-services-collector-monitor-mrnet-mpi-static
	${LibMonitor_STATIC_LIBRARIES}
	${MRNet_LWR_STATIC_LIBRARIES}
	pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(hwc-monitor-mrnet-mpi PROPERTIES VERSION 1.1.0)
set_target_properties(hwc-monitor-mrnet-mpi PROPERTIES
        COMPILE_DEFINITIONS "${TLS_DEFINES};CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_MRNET;CBTF_SERVICE_USE_MRNET_MPI;${MRNet_DEFINES}")

install(TARGETS hwc-monitor-mrnet-mpi
	ARCHIVE DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors/static
)

endif()

endif()

if (NOT RUNTIME_ONLY)
    # If we are building the targeted runtimes/collectors, then don't build the xml
    # We will do that as part of the targeted build of the FE (BUILD_TARGETED_FE) 
    # See the else clause below: (This is the BUILD_TARGETED_FE section) where we build the
    # xml for the experiment.
    #
    # Immediately below is the normal cluster - non-targeted build of the xml file.
    #
    set(fe_component_location ${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}/KrellInstitute/Components)
    set(cn_component_location ${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}/KrellInstitute/Components)
    set(cbtflib_location ${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX})
    set(collector_name "hwc")
    configure_file(../template.xml.in hwc.xml @ONLY)

    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/hwc.xml
            DESTINATION share/KrellInstitute/xml)
endif()

else ()
    # This is the BUILD_TARGETED_FE section where we create the xml that includes both the traditional
    # frontend build based components for the FE node and the targeted compute node components for
    # the targeted commnode.
    set(fe_component_location ${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}/KrellInstitute/Components)
    set(cn_component_location ${CBTF_KRELL_CN_RUNTIME_DIR}/lib${LIB_SUFFIX}/KrellInstitute/Components)

    # Create and install xml for the experiment into the target cbtf-krell based location
    set(cbtflib_location ${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX})
    set(collector_name "hwc")
    configure_file(../template.xml.in hwc.xml @ONLY)

    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/hwc.xml
        DESTINATION ${CBTF_KRELL_CN_RUNTIME_DIR}/share/KrellInstitute/xml)

endif()

