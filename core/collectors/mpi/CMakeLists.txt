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

set(WRAPPER_SOURCES
	wrappers.c wrappers-fortran.c
)

set(COLLECTOR_SOURCES
	collector.c
)

if (HAVE_OPENMPI)
#
# mpi experiment libraries for openmpi
#
add_library(mpi-openmpi-collector-monitor-fileio MODULE
	${COLLECTOR_SOURCES}
	${WRAPPER_SOURCES}
)

target_include_directories(mpi-openmpi-collector-monitor-fileio PUBLIC
	${LibMonitor_INCLUDE_DIRS}
	${LibUnwind_INCLUDE_DIRS}
	${OpenMPI_INCLUDE_DIRS}
	${PROJECT_SOURCE_DIR}/messages/include
	${PROJECT_SOURCE_DIR}/services/include
	${PROJECT_SOURCE_DIR}/services/collector
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/perfdata
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/events
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/base
)

target_link_libraries(mpi-openmpi-collector-monitor-fileio
	cbtf-messages-events
	cbtf-messages-base
	cbtf-services-common
	cbtf-services-monitor
	cbtf-services-offline
	cbtf-services-timer
	cbtf-services-unwind
	cbtf-services-collector-monitor-fileio
	${LibMonitor_SHARED_LIBRARIES}
	${LibUnwind_SHARED_LIBRARIES}
	${OpenMPI_SHARED_LIBRARIES}
	pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(mpi-openmpi-collector-monitor-fileio PROPERTIES PREFIX "")
set_target_properties(mpi-openmpi-collector-monitor-fileio PROPERTIES
        COMPILE_DEFINITIONS "${TLS_DEFINES};CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_FILEIO;${OpenMPI_DEFINES}")

install(TARGETS mpi-openmpi-collector-monitor-fileio
	LIBRARY DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors
)

# Create and install static versions

add_library(mpi-openmpi-monitor-fileio STATIC
	${COLLECTOR_SOURCES}
	${WRAPPER_SOURCES}
)

target_include_directories(mpi-openmpi-monitor-fileio PUBLIC
	${LibMonitor_INCLUDE_DIRS}
	${LibUnwind_INCLUDE_DIRS}
	${OpenMPI_INCLUDE_DIRS}
	${PROJECT_SOURCE_DIR}/messages/include
	${PROJECT_SOURCE_DIR}/services/include
	${PROJECT_SOURCE_DIR}/services/collector
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/perfdata
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/events
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/base
)

target_link_libraries(mpi-openmpi-monitor-fileio
	cbtf-messages-events-static
	cbtf-messages-base-static
	cbtf-services-common-static
	cbtf-services-monitor-static
	cbtf-services-offline-static
	cbtf-services-timer-static
	cbtf-services-unwind-static
	cbtf-services-collector-monitor-fileio-static
	${LibMonitor_STATIC_LIBRARIES}
	${LibUnwind_STATIC_LIBRARIES}
	${OpenMPI_STATIC_LIBRARIES}
	pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(mpi-openmpi-monitor-fileio PROPERTIES
        COMPILE_DEFINITIONS "${TLS_DEFINES};CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_FILEIO;CBTF_STATIC;${OpenMPI_DEFINES}")

install(TARGETS mpi-openmpi-monitor-fileio
	ARCHIVE DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors/static
)
#
# mpit experiment libraries for openmpi
#
add_library(mpit-openmpi-collector-monitor-fileio MODULE
	${COLLECTOR_SOURCES}
	${WRAPPER_SOURCES}
)

target_include_directories(mpit-openmpi-collector-monitor-fileio PUBLIC
	${LibMonitor_INCLUDE_DIRS}
	${LibUnwind_INCLUDE_DIRS}
	${OpenMPI_INCLUDE_DIRS}
	${PROJECT_SOURCE_DIR}/messages/include
	${PROJECT_SOURCE_DIR}/services/include
	${PROJECT_SOURCE_DIR}/services/collector
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/perfdata
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/events
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/base
)

target_link_libraries(mpit-openmpi-collector-monitor-fileio
	cbtf-messages-events
	cbtf-messages-base
	cbtf-services-common
	cbtf-services-monitor
	cbtf-services-offline
	cbtf-services-timer
	cbtf-services-unwind
	cbtf-services-collector-monitor-fileio
	${LibMonitor_SHARED_LIBRARIES}
	${LibUnwind_SHARED_LIBRARIES}
	${OpenMPI_SHARED_LIBRARIES}
	pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(mpit-openmpi-collector-monitor-fileio PROPERTIES PREFIX "")
set_target_properties(mpit-openmpi-collector-monitor-fileio PROPERTIES
        COMPILE_DEFINITIONS "${TLS_DEFINES};CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_FILEIO;EXTENDEDTRACE;${OpenMPI_DEFINES}")

install(TARGETS mpit-openmpi-collector-monitor-fileio
LIBRARY DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors
)

# Create and install static versions

add_library(mpit-openmpi-monitor-fileio STATIC
	${COLLECTOR_SOURCES}
	${WRAPPER_SOURCES}
)

target_include_directories(mpit-openmpi-monitor-fileio PUBLIC
	${LibMonitor_INCLUDE_DIRS}
	${LibUnwind_INCLUDE_DIRS}
	${OpenMPI_INCLUDE_DIRS}
	${PROJECT_SOURCE_DIR}/messages/include
	${PROJECT_SOURCE_DIR}/services/include
	${PROJECT_SOURCE_DIR}/services/collector
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/perfdata
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/events
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/base
)

target_link_libraries(mpit-openmpi-monitor-fileio
	cbtf-messages-events-static
	cbtf-messages-base-static
	cbtf-services-common-static
	cbtf-services-monitor-static
	cbtf-services-offline-static
	cbtf-services-timer-static
	cbtf-services-unwind-static
	cbtf-services-collector-monitor-fileio-static
	${LibMonitor_STATIC_LIBRARIES}
	${LibUnwind_STATIC_LIBRARIES}
	${OpenMPI_STATIC_LIBRARIES}
	pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(mpit-openmpi-monitor-fileio PROPERTIES
        COMPILE_DEFINITIONS "${TLS_DEFINES};CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_FILEIO;CBTF_STATIC;EXTENDEDTRACE;${OpenMPI_DEFINES}")

install(TARGETS mpit-openmpi-monitor-fileio
	ARCHIVE DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors/static
)
#
# mpip experiment libraries for openmpi
#
add_library(mpip-openmpi-collector-monitor-fileio MODULE
	${COLLECTOR_SOURCES}
	${WRAPPER_SOURCES}
)

target_include_directories(mpip-openmpi-collector-monitor-fileio PUBLIC
	${LibMonitor_INCLUDE_DIRS}
	${LibUnwind_INCLUDE_DIRS}
	${OpenMPI_INCLUDE_DIRS}
	${PROJECT_SOURCE_DIR}/messages/include
	${PROJECT_SOURCE_DIR}/services/include
	${PROJECT_SOURCE_DIR}/services/collector
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/perfdata
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/events
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/base
)

target_link_libraries(mpip-openmpi-collector-monitor-fileio
	cbtf-messages-events
	cbtf-messages-base
	cbtf-services-common
	cbtf-services-monitor
	cbtf-services-offline
	cbtf-services-timer
	cbtf-services-unwind
	cbtf-services-collector-monitor-fileio
	${LibMonitor_SHARED_LIBRARIES}
	${LibUnwind_SHARED_LIBRARIES}
	${OpenMPI_SHARED_LIBRARIES}
	pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(mpip-openmpi-collector-monitor-fileio PROPERTIES PREFIX "")
set_target_properties(mpip-openmpi-collector-monitor-fileio PROPERTIES
        COMPILE_DEFINITIONS "${TLS_DEFINES};CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_FILEIO;PROFILE;${OpenMPI_DEFINES}")

install(TARGETS mpip-openmpi-collector-monitor-fileio
	LIBRARY DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors
)

# Create and install static versions

add_library(mpip-openmpi-monitor-fileio STATIC
	${COLLECTOR_SOURCES}
	${WRAPPER_SOURCES}
)

target_include_directories(mpip-openmpi-monitor-fileio PUBLIC
	${LibMonitor_INCLUDE_DIRS}
	${LibUnwind_INCLUDE_DIRS}
	${OpenMPI_INCLUDE_DIRS}
	${PROJECT_SOURCE_DIR}/messages/include
	${PROJECT_SOURCE_DIR}/services/include
	${PROJECT_SOURCE_DIR}/services/collector
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/perfdata
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/events
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/base
)

target_link_libraries(mpip-openmpi-monitor-fileio
	cbtf-messages-events-static
	cbtf-messages-base-static
	cbtf-services-common-static
	cbtf-services-monitor-static
	cbtf-services-offline-static
	cbtf-services-timer-static
	cbtf-services-unwind-static
	cbtf-services-collector-monitor-fileio-static
	${LibMonitor_STATIC_LIBRARIES}
	${LibUnwind_STATIC_LIBRARIES}
	${OpenMPI_STATIC_LIBRARIES}
	pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(mpip-openmpi-monitor-fileio PROPERTIES
        COMPILE_DEFINITIONS "${TLS_DEFINES};CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_FILEIO;CBTF_STATIC;PROFILE;${OpenMPI_DEFINES}")

install(TARGETS mpip-openmpi-monitor-fileio
	ARCHIVE DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors/static
)

if (MRNET_FOUND)
#
# mpi experiment libraries for openmpi
#
add_library(mpi-openmpi-collector-monitor-mrnet-mpi MODULE
	${COLLECTOR_SOURCES}
	${WRAPPER_SOURCES}
)

target_include_directories(mpi-openmpi-collector-monitor-mrnet-mpi PUBLIC
	${LibMonitor_INCLUDE_DIRS}
	${LibUnwind_INCLUDE_DIRS}
	${OpenMPI_INCLUDE_DIRS}
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

target_link_libraries(mpi-openmpi-collector-monitor-mrnet-mpi
	cbtf-messages-events
	cbtf-messages-base
	cbtf-services-common
	cbtf-services-monitor
	cbtf-services-offline
	cbtf-services-unwind
	cbtf-services-timer
	cbtf-services-data
	cbtf-services-collector-monitor-mrnet-mpi
	${LibMonitor_SHARED_LIBRARIES}
	${LibUnwind_SHARED_LIBRARIES}
	${MRNet_MRNET_LWR_SHARED_LIBRARIES}
	${OpenMPI_SHARED_LIBRARIES}
	pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(mpi-openmpi-collector-monitor-mrnet-mpi PROPERTIES PREFIX "")
set_target_properties(mpi-openmpi-collector-monitor-mrnet-mpi PROPERTIES
        COMPILE_DEFINITIONS "${TLS_DEFINES};CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_MRNET;CBTF_SERVICE_USE_MRNET_MPI;${OpenMPI_DEFINES};${MRNet_DEFINES}")

install(TARGETS mpi-openmpi-collector-monitor-mrnet-mpi
	LIBRARY DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors
)

# Create and install static versions

add_library(mpi-openmpi-monitor-mrnet-mpi STATIC
	${COLLECTOR_SOURCES}
	${WRAPPER_SOURCES}
)

target_include_directories(mpi-openmpi-monitor-mrnet-mpi PUBLIC
	${LibMonitor_INCLUDE_DIRS}
	${LibUnwind_INCLUDE_DIRS}
	${OpenMPI_INCLUDE_DIRS}
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

target_link_libraries(mpi-openmpi-monitor-mrnet-mpi
	cbtf-messages-events-static
	cbtf-messages-base-static
	cbtf-services-common-static
	cbtf-services-monitor-static
	cbtf-services-offline-static
	cbtf-services-timer-static
	cbtf-services-data-static
	cbtf-services-unwind-static
	cbtf-services-collector-monitor-mrnet-mpi
	${LibMonitor_STATIC_LIBRARIES}
	${LibUnwind_STATIC_LIBRARIES}
	${MRNet_MRNET_LWR_STATIC_LIBRARIES}
	${OpenMPI_STATIC_LIBRARIES}
	pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(mpi-openmpi-monitor-mrnet-mpi PROPERTIES VERSION 1.1.0)
set_target_properties(mpi-openmpi-monitor-mrnet-mpi PROPERTIES
        COMPILE_DEFINITIONS "${TLS_DEFINES};CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_MRNET;CBTF_SERVICE_USE_MRNET_MPI;CBTF_STATIC;${OpenMPI_DEFINES};${MRNet_DEFINES}")

install(TARGETS mpi-openmpi-monitor-mrnet-mpi
	ARCHIVE DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors/static
)
#
# mpip experiment libraries for openmpi
#
add_library(mpip-openmpi-collector-monitor-mrnet-mpi MODULE
	${COLLECTOR_SOURCES}
	${WRAPPER_SOURCES}
)

target_include_directories(mpip-openmpi-collector-monitor-mrnet-mpi PUBLIC
	${LibMonitor_INCLUDE_DIRS}
	${LibUnwind_INCLUDE_DIRS}
	${OpenMPI_INCLUDE_DIRS}
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

target_link_libraries(mpip-openmpi-collector-monitor-mrnet-mpi
	cbtf-messages-events
	cbtf-messages-base
	cbtf-services-common
	cbtf-services-monitor
	cbtf-services-offline
	cbtf-services-unwind
	cbtf-services-timer
	cbtf-services-data
	cbtf-services-collector-monitor-mrnet-mpi
	${LibMonitor_SHARED_LIBRARIES}
	${LibUnwind_SHARED_LIBRARIES}
	${MRNet_MRNET_LWR_SHARED_LIBRARIES}
	${OpenMPI_SHARED_LIBRARIES}
	pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(mpip-openmpi-collector-monitor-mrnet-mpi PROPERTIES PREFIX "")
set_target_properties(mpip-openmpi-collector-monitor-mrnet-mpi PROPERTIES
        COMPILE_DEFINITIONS "${TLS_DEFINES};CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_MRNET;CBTF_SERVICE_USE_MRNET_MPI;PROFILE;${OpenMPI_DEFINES};${MRNet_DEFINES}")

install(TARGETS mpip-openmpi-collector-monitor-mrnet-mpi
	LIBRARY DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors
)


add_library(mpip-openmpi-monitor-mrnet-mpi STATIC
	${COLLECTOR_SOURCES}
	${WRAPPER_SOURCES}
)

target_include_directories(mpip-openmpi-monitor-mrnet-mpi PUBLIC
	${LibMonitor_INCLUDE_DIRS}
	${LibUnwind_INCLUDE_DIRS}
	${OpenMPI_INCLUDE_DIRS}
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

target_link_libraries(mpip-openmpi-monitor-mrnet-mpi
	cbtf-messages-events-static
	cbtf-messages-base-static
	cbtf-services-common-static
	cbtf-services-monitor-static
	cbtf-services-offline-static
	cbtf-services-timer-static
	cbtf-services-data-static
	cbtf-services-unwind-static
	cbtf-services-collector-monitor-mrnet-mpi-static
	${LibMonitor_STATIC_LIBRARIES}
	${LibUnwind_STATIC_LIBRARIES}
	${MRNet_MRNET_LWR_STATIC_LIBRARIES}
	${OpenMPI_STATIC_LIBRARIES}
	pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(mpip-openmpi-monitor-mrnet-mpi PROPERTIES VERSION 1.1.0)
set_target_properties(mpip-openmpi-monitor-mrnet-mpi PROPERTIES
        COMPILE_DEFINITIONS "${TLS_DEFINES};CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_MRNET;CBTF_SERVICE_USE_MRNET_MPI;CBTF_STATIC;PROFILE;${OpenMPI_DEFINES};${MRNet_DEFINES}")

install(TARGETS mpip-openmpi-monitor-mrnet-mpi
	ARCHIVE DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors/static
)
#
# mpit experiment libraries for openmpi
#
add_library(mpit-openmpi-collector-monitor-mrnet-mpi MODULE
	${COLLECTOR_SOURCES}
	${WRAPPER_SOURCES}
)

target_include_directories(mpit-openmpi-collector-monitor-mrnet-mpi PUBLIC
	${LibMonitor_INCLUDE_DIRS}
	${LibUnwind_INCLUDE_DIRS}
	${OpenMPI_INCLUDE_DIRS}
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

target_link_libraries(mpit-openmpi-collector-monitor-mrnet-mpi
	cbtf-messages-events
	cbtf-messages-base
	cbtf-services-common
	cbtf-services-monitor
	cbtf-services-offline
	cbtf-services-timer
	cbtf-services-data
	cbtf-services-unwind
	cbtf-services-collector-monitor-mrnet-mpi
	${LibMonitor_SHARED_LIBRARIES}
	${LibUnwind_SHARED_LIBRARIES}
	${MRNet_MRNET_LWR_SHARED_LIBRARIES}
	${OpenMPI_SHARED_LIBRARIES}
	pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(mpit-openmpi-collector-monitor-mrnet-mpi PROPERTIES PREFIX "")
set_target_properties(mpit-openmpi-collector-monitor-mrnet-mpi PROPERTIES
        COMPILE_DEFINITIONS "${TLS_DEFINES};CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_MRNET;CBTF_SERVICE_USE_MRNET_MPI;EXTENDEDTRACE;${OpenMPI_DEFINES};${MRNet_DEFINES}")

install(TARGETS mpit-openmpi-collector-monitor-mrnet-mpi
	LIBRARY DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors
)


add_library(mpit-openmpi-monitor-mrnet-mpi STATIC
	${COLLECTOR_SOURCES}
	${WRAPPER_SOURCES}
)

target_include_directories(mpit-openmpi-monitor-mrnet-mpi PUBLIC
	${LibMonitor_INCLUDE_DIRS}
	${LibUnwind_INCLUDE_DIRS}
	${OpenMPI_INCLUDE_DIRS}
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

target_link_libraries(mpit-openmpi-monitor-mrnet-mpi
	cbtf-messages-events-static
	cbtf-messages-base-static
	cbtf-services-common-static
	cbtf-services-monitor-static
	cbtf-services-offline-static
	cbtf-services-timer-static
	cbtf-services-data-static
	cbtf-services-unwind-static
	cbtf-services-collector-monitor-mrnet-mpi-static
	${LibMonitor_STATIC_LIBRARIES}
	${LibUnwind_STATIC_LIBRARIES}
	${MRNet_MRNET_LWR_STATIC_LIBRARIES}
	${OpenMPI_STATIC_LIBRARIES}
	pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(mpit-openmpi-monitor-mrnet-mpi PROPERTIES VERSION 1.1.0)
set_target_properties(mpit-openmpi-monitor-mrnet-mpi PROPERTIES
        COMPILE_DEFINITIONS "${TLS_DEFINES};CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_MRNET;CBTF_SERVICE_USE_MRNET_MPI;CBTF_STATIC;EXTENDEDTRACE;${OpenMPI_DEFINES};${MRNet_DEFINES}")

install(TARGETS mpit-openmpi-monitor-mrnet-mpi
	ARCHIVE DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors/static
)

endif() # if (MRNET_FOUND)

endif() # if (HAVE_OPENMPI)

if (HAVE_MPICH)
#
# mpi experiment libraries for mpich
#
add_library(mpi-mpich-collector-monitor-fileio MODULE
	${COLLECTOR_SOURCES}
	${WRAPPER_SOURCES}
)

target_include_directories(mpi-mpich-collector-monitor-fileio PUBLIC
	${LibMonitor_INCLUDE_DIRS}
	${LibUnwind_INCLUDE_DIRS}
	${Mpich_INCLUDE_DIRS}
	${PROJECT_SOURCE_DIR}/messages/include
	${PROJECT_SOURCE_DIR}/services/include
	${PROJECT_SOURCE_DIR}/services/collector
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/perfdata
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/events
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/base
)

target_link_libraries(mpi-mpich-collector-monitor-fileio
	cbtf-messages-events
	cbtf-messages-base
	cbtf-services-common
	cbtf-services-monitor
	cbtf-services-offline
	cbtf-services-timer
	cbtf-services-unwind
	cbtf-services-collector-monitor-fileio
	${LibMonitor_SHARED_LIBRARIES}
	${LibUnwind_SHARED_LIBRARIES}
	${Mpich_SHARED_LIBRARIES}
	pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(mpi-mpich-collector-monitor-fileio PROPERTIES PREFIX "")
set_target_properties(mpi-mpich-collector-monitor-fileio PROPERTIES
        COMPILE_DEFINITIONS "${TLS_DEFINES};CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_FILEIO;${Mpich_DEFINES}")

install(TARGETS mpi-mpich-collector-monitor-fileio
	LIBRARY DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors
)

add_library(mpi-mpich-monitor-fileio STATIC
	${COLLECTOR_SOURCES}
	${WRAPPER_SOURCES}
)

target_include_directories(mpi-mpich-monitor-fileio PUBLIC
	${LibMonitor_INCLUDE_DIRS}
	${LibUnwind_INCLUDE_DIRS}
	${Mpich_INCLUDE_DIRS}
	${PROJECT_SOURCE_DIR}/messages/include
	${PROJECT_SOURCE_DIR}/services/include
	${PROJECT_SOURCE_DIR}/services/collector
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/perfdata
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/events
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/base
)

target_link_libraries(mpi-mpich-monitor-fileio
	cbtf-messages-events-static
	cbtf-messages-base-static
	cbtf-services-common-static
	cbtf-services-monitor-static
	cbtf-services-offline-static
	cbtf-services-timer-static
	cbtf-services-unwind-static
	cbtf-services-collector-monitor-fileio-static
	${LibMonitor_STATIC_LIBRARIES}
	${LibUnwind_STATIC_LIBRARIES}
	${Mpich_STATIC_LIBRARIES}
	pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(mpi-mpich-monitor-fileio PROPERTIES
        COMPILE_DEFINITIONS "${TLS_DEFINES};CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_FILEIO;CBTF_STATIC;${Mpich_DEFINES}")

install(TARGETS mpi-mpich-monitor-fileio
	ARCHIVE DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors/static
)

#
# mpit experiment libraries for mpich
#
add_library(mpit-mpich-collector-monitor-fileio MODULE
	${COLLECTOR_SOURCES}
	${WRAPPER_SOURCES}
)

target_include_directories(mpit-mpich-collector-monitor-fileio PUBLIC
	${LibMonitor_INCLUDE_DIRS}
	${LibUnwind_INCLUDE_DIRS}
	${Mpich_INCLUDE_DIRS}
	${PROJECT_SOURCE_DIR}/messages/include
	${PROJECT_SOURCE_DIR}/services/include
	${PROJECT_SOURCE_DIR}/services/collector
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/perfdata
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/events
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/base
)

target_link_libraries(mpit-mpich-collector-monitor-fileio
	cbtf-messages-events
	cbtf-messages-base
	cbtf-services-common
	cbtf-services-monitor
	cbtf-services-offline
	cbtf-services-timer
	cbtf-services-unwind
	cbtf-services-collector-monitor-fileio
	${LibMonitor_SHARED_LIBRARIES}
	${LibUnwind_SHARED_LIBRARIES}
	${Mpich_SHARED_LIBRARIES}
	pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(mpit-mpich-collector-monitor-fileio PROPERTIES PREFIX "")
set_target_properties(mpit-mpich-collector-monitor-fileio PROPERTIES
        COMPILE_DEFINITIONS "${TLS_DEFINES};CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_FILEIO;EXTENDEDTRACE;${Mpich_DEFINES}")

install(TARGETS mpit-mpich-collector-monitor-fileio
	LIBRARY DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors
)

add_library(mpit-mpich-monitor-fileio STATIC
	${COLLECTOR_SOURCES}
	${WRAPPER_SOURCES}
)

target_include_directories(mpit-mpich-monitor-fileio PUBLIC
	${LibMonitor_INCLUDE_DIRS}
	${LibUnwind_INCLUDE_DIRS}
	${Mpich_INCLUDE_DIRS}
	${PROJECT_SOURCE_DIR}/messages/include
	${PROJECT_SOURCE_DIR}/services/include
	${PROJECT_SOURCE_DIR}/services/collector
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/perfdata
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/events
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/base
)

target_link_libraries(mpit-mpich-monitor-fileio
	cbtf-messages-events-static
	cbtf-messages-base-static
	cbtf-services-common-static
	cbtf-services-monitor-static
	cbtf-services-offline-static
	cbtf-services-timer-static
	cbtf-services-unwind-static
	cbtf-services-collector-monitor-fileio-static
	${LibMonitor_STATIC_LIBRARIES}
	${LibUnwind_STATIC_LIBRARIES}
	${Mpich_STATIC_LIBRARIES}
	pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(mpit-mpich-monitor-fileio PROPERTIES
        COMPILE_DEFINITIONS "${TLS_DEFINES};CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_FILEIO;CBTF_STATIC;EXTENDEDTRACE;${Mpich_DEFINES}")

install(TARGETS mpit-mpich-monitor-fileio
	ARCHIVE DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors/static
)
#
# mpip experiment libraries for mpich
#
add_library(mpip-mpich-collector-monitor-fileio MODULE
	${COLLECTOR_SOURCES}
	${WRAPPER_SOURCES}
)

target_include_directories(mpip-mpich-collector-monitor-fileio PUBLIC
	${LibMonitor_INCLUDE_DIRS}
	${LibUnwind_INCLUDE_DIRS}
	${Mpich_INCLUDE_DIRS}
	${PROJECT_SOURCE_DIR}/messages/include
	${PROJECT_SOURCE_DIR}/services/include
	${PROJECT_SOURCE_DIR}/services/collector
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/perfdata
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/events
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/base
)

target_link_libraries(mpip-mpich-collector-monitor-fileio
	cbtf-messages-events
	cbtf-messages-base
	cbtf-services-common
	cbtf-services-monitor
	cbtf-services-offline
	cbtf-services-timer
	cbtf-services-unwind
	cbtf-services-collector-monitor-fileio
	${LibMonitor_SHARED_LIBRARIES}
	${LibUnwind_SHARED_LIBRARIES}
	${Mpich_SHARED_LIBRARIES}
	pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(mpip-mpich-collector-monitor-fileio PROPERTIES PREFIX "")
set_target_properties(mpip-mpich-collector-monitor-fileio PROPERTIES
        COMPILE_DEFINITIONS "${TLS_DEFINES};CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_FILEIO;PROFILE;${Mpich_DEFINES}")

install(TARGETS mpip-mpich-collector-monitor-fileio
	LIBRARY DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors
)

add_library(mpip-mpich-monitor-fileio STATIC
	${COLLECTOR_SOURCES}
	${WRAPPER_SOURCES}
)

target_include_directories(mpip-mpich-monitor-fileio PUBLIC
	${LibMonitor_INCLUDE_DIRS}
	${LibUnwind_INCLUDE_DIRS}
	${Mpich_INCLUDE_DIRS}
	${PROJECT_SOURCE_DIR}/messages/include
	${PROJECT_SOURCE_DIR}/services/include
	${PROJECT_SOURCE_DIR}/services/collector
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/perfdata
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/events
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/base
)

target_link_libraries(mpip-mpich-monitor-fileio
	cbtf-messages-events-static
	cbtf-messages-base-static
	cbtf-services-common-static
	cbtf-services-monitor-static
	cbtf-services-offline-static
	cbtf-services-timer-static
	cbtf-services-unwind-static
	cbtf-services-collector-monitor-fileio-static
	${LibMonitor_STATIC_LIBRARIES}
	${LibUnwind_STATIC_LIBRARIES}
	${Mpich_STATIC_LIBRARIES}
	pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(mpip-mpich-monitor-fileio PROPERTIES
        COMPILE_DEFINITIONS "${TLS_DEFINES};CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_FILEIO;CBTF_STATIC;PROFILE;${Mpich_DEFINES}")

install(TARGETS mpip-mpich-monitor-fileio
	ARCHIVE DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors/static
)

if (MRNET_FOUND)
#
# mpi experiment libraries for mpich
#
add_library(mpi-mpich-collector-monitor-mrnet-mpi MODULE
	${COLLECTOR_SOURCES}
	${WRAPPER_SOURCES}
)

target_include_directories(mpi-mpich-collector-monitor-mrnet-mpi PUBLIC
	${LibMonitor_INCLUDE_DIRS}
	${LibUnwind_INCLUDE_DIRS}
	${Mpich_INCLUDE_DIRS}
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

target_link_libraries(mpi-mpich-collector-monitor-mrnet-mpi
	cbtf-messages-events
	cbtf-messages-base
	cbtf-services-common
	cbtf-services-monitor
	cbtf-services-offline
	cbtf-services-timer
	cbtf-services-data
	cbtf-services-unwind
	cbtf-services-collector-monitor-mrnet-mpi
	${LibMonitor_SHARED_LIBRARIES}
	${LibUnwind_SHARED_LIBRARIES}
	${MRNet_MRNET_LWR_SHARED_LIBRARIES}
	${Mpich_SHARED_LIBRARIES}
	pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(mpi-mpich-collector-monitor-mrnet-mpi PROPERTIES PREFIX "")
set_target_properties(mpi-mpich-collector-monitor-mrnet-mpi PROPERTIES
        COMPILE_DEFINITIONS "${TLS_DEFINES};CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_MRNET;CBTF_SERVICE_USE_MRNET_MPI;${MRNet_DEFINES};${Mpich_DEFINES}")

install(TARGETS mpi-mpich-collector-monitor-mrnet-mpi
	LIBRARY DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors
)


add_library(mpi-mpich-monitor-mrnet-mpi STATIC
	${COLLECTOR_SOURCES}
	${WRAPPER_SOURCES}
)

target_include_directories(mpi-mpich-monitor-mrnet-mpi PUBLIC
	${LibMonitor_INCLUDE_DIRS}
	${LibUnwind_INCLUDE_DIRS}
	${Mpich_INCLUDE_DIRS}
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

target_link_libraries(mpi-mpich-monitor-mrnet-mpi
	cbtf-messages-events-static
	cbtf-messages-base-static
	cbtf-services-common-static
	cbtf-services-monitor-static
	cbtf-services-offline-static
	cbtf-services-timer-static
	cbtf-services-data-static
	cbtf-services-unwind-static
	cbtf-services-collector-monitor-mrnet-mpi-static
	${LibMonitor_STATIC_LIBRARIES}
	${LibUnwind_STATIC_LIBRARIES}
	${MRNet_MRNET_LWR_STATIC_LIBRARIES}
	${Mpich_STATIC_LIBRARIES}
	pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(mpi-mpich-monitor-mrnet-mpi PROPERTIES VERSION 1.1.0)
set_target_properties(mpi-mpich-monitor-mrnet-mpi PROPERTIES
        COMPILE_DEFINITIONS "${TLS_DEFINES};CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_MRNET;CBTF_SERVICE_USE_MRNET_MPI;CBTF_STATIC;${MRNet_DEFINES};${Mpich_DEFINES}")

install(TARGETS mpi-mpich-monitor-mrnet-mpi
	ARCHIVE DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors/static
)
#
# mpip experiment libraries for mpich
#
add_library(mpip-mpich-collector-monitor-mrnet-mpi MODULE
	${COLLECTOR_SOURCES}
	${WRAPPER_SOURCES}
)

target_include_directories(mpip-mpich-collector-monitor-mrnet-mpi PUBLIC
	${LibMonitor_INCLUDE_DIRS}
	${LibUnwind_INCLUDE_DIRS}
	${Mpich_INCLUDE_DIRS}
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

target_link_libraries(mpip-mpich-collector-monitor-mrnet-mpi
	cbtf-messages-events
	cbtf-messages-base
	cbtf-services-common
	cbtf-services-monitor
	cbtf-services-offline
	cbtf-services-timer
	cbtf-services-data
	cbtf-services-unwind
	cbtf-services-collector-monitor-mrnet-mpi
	${LibMonitor_SHARED_LIBRARIES}
	${LibUnwind_SHARED_LIBRARIES}
	${MRNet_MRNET_LWR_SHARED_LIBRARIES}
	${Mpich_SHARED_LIBRARIES}
	pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(mpip-mpich-collector-monitor-mrnet-mpi PROPERTIES PREFIX "")
set_target_properties(mpip-mpich-collector-monitor-mrnet-mpi PROPERTIES
        COMPILE_DEFINITIONS "${TLS_DEFINES};CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_MRNET;CBTF_SERVICE_USE_MRNET_MPI;PROFILE;${MRNet_DEFINES};${Mpich_DEFINES}")

install(TARGETS mpip-mpich-collector-monitor-mrnet-mpi
	LIBRARY DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors
)


add_library(mpip-mpich-monitor-mrnet-mpi STATIC
	${COLLECTOR_SOURCES}
	${WRAPPER_SOURCES}
)

target_include_directories(mpip-mpich-monitor-mrnet-mpi PUBLIC
	${LibMonitor_INCLUDE_DIRS}
	${LibUnwind_INCLUDE_DIRS}
	${Mpich_INCLUDE_DIRS}
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

target_link_libraries(mpip-mpich-monitor-mrnet-mpi
	cbtf-messages-events-static
	cbtf-messages-base-static
	cbtf-services-common-static
	cbtf-services-monitor-static
	cbtf-services-offline-static
	cbtf-services-timer-static
	cbtf-services-data-static
	cbtf-services-unwind-static
	cbtf-services-collector-monitor-mrnet-mpi-static
	${LibMonitor_STATIC_LIBRARIES}
	${LibUnwind_STATIC_LIBRARIES}
	${MRNet_MRNET_LWR_STATIC_LIBRARIES}
	${Mpich_STATIC_LIBRARIES}
	pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(mpip-mpich-monitor-mrnet-mpi PROPERTIES VERSION 1.1.0)
set_target_properties(mpip-mpich-monitor-mrnet-mpi PROPERTIES
        COMPILE_DEFINITIONS "${TLS_DEFINES};CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_MRNET;CBTF_SERVICE_USE_MRNET_MPI;CBTF_STATIC;PROFILE;${MRNet_DEFINES};${Mpich_DEFINES}")

install(TARGETS mpip-mpich-monitor-mrnet-mpi
	ARCHIVE DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors/static
)
#
# mpit experiment libraries for mpich
#
add_library(mpit-mpich-collector-monitor-mrnet-mpi MODULE
	${COLLECTOR_SOURCES}
	${WRAPPER_SOURCES}
)

target_include_directories(mpit-mpich-collector-monitor-mrnet-mpi PUBLIC
	${LibMonitor_INCLUDE_DIRS}
	${LibUnwind_INCLUDE_DIRS}
	${Mpich_INCLUDE_DIRS}
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

target_link_libraries(mpit-mpich-collector-monitor-mrnet-mpi
	cbtf-messages-events
	cbtf-messages-base
	cbtf-services-common
	cbtf-services-monitor
	cbtf-services-offline
	cbtf-services-timer
	cbtf-services-data
	cbtf-services-unwind
	cbtf-services-collector-monitor-mrnet-mpi
	${LibMonitor_SHARED_LIBRARIES}
	${LibUnwind_SHARED_LIBRARIES}
	${MRNet_MRNET_LWR_SHARED_LIBRARIES}
	${Mpich_SHARED_LIBRARIES}
	pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(mpit-mpich-collector-monitor-mrnet-mpi PROPERTIES PREFIX "")
set_target_properties(mpit-mpich-collector-monitor-mrnet-mpi PROPERTIES
        COMPILE_DEFINITIONS "${TLS_DEFINES};CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_MRNET;CBTF_SERVICE_USE_MRNET_MPI;EXTENDEDTRACE;${MRNet_DEFINES};${Mpich_DEFINES}")

install(TARGETS mpit-mpich-collector-monitor-mrnet-mpi
	LIBRARY DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors
)


add_library(mpit-mpich-monitor-mrnet-mpi STATIC
	${COLLECTOR_SOURCES}
	${WRAPPER_SOURCES}
)

target_include_directories(mpit-mpich-monitor-mrnet-mpi PUBLIC
	${LibMonitor_INCLUDE_DIRS}
	${LibUnwind_INCLUDE_DIRS}
	${Mpich_INCLUDE_DIRS}
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

target_link_libraries(mpit-mpich-monitor-mrnet-mpi
	cbtf-messages-events-static
	cbtf-messages-base-static
	cbtf-services-common-static
	cbtf-services-monitor-static
	cbtf-services-offline-static
	cbtf-services-timer-static
	cbtf-services-data-static
	cbtf-services-unwind-static
	cbtf-services-collector-monitor-mrnet-mpi-static
	${LibMonitor_STATIC_LIBRARIES}
	${LibUnwind_STATIC_LIBRARIES}
	${MRNet_MRNET_LWR_STATIC_LIBRARIES}
	${Mpich_STATIC_LIBRARIES}
	pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(mpit-mpich-monitor-mrnet-mpi PROPERTIES VERSION 1.1.0)
set_target_properties(mpit-mpich-monitor-mrnet-mpi PROPERTIES
        COMPILE_DEFINITIONS "${TLS_DEFINES};CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_MRNET;CBTF_SERVICE_USE_MRNET_MPI;CBTF_STATIC;EXTENDEDTRACE;${MRNet_DEFINES};${Mpich_DEFINES}")

install(TARGETS mpit-mpich-monitor-mrnet-mpi
	ARCHIVE DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors/static
)

endif() # if (MRNET_FOUND)

endif() # if (HAVE_MPICH)

if (HAVE_MPICH2)

#add_definitions(
#	${Mpich2_DEFINES}
#)

#
# mpi experiment libraries for mpich2
#
add_library(mpi-mpich2-collector-monitor-fileio MODULE
	${COLLECTOR_SOURCES}
	${WRAPPER_SOURCES}
)

target_include_directories(mpi-mpich2-collector-monitor-fileio PUBLIC
	${LibMonitor_INCLUDE_DIRS}
	${LibUnwind_INCLUDE_DIRS}
	${Mpich2_INCLUDE_DIRS}
	${PROJECT_SOURCE_DIR}/messages/include
	${PROJECT_SOURCE_DIR}/services/include
	${PROJECT_SOURCE_DIR}/services/collector
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/perfdata
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/events
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/base
)

target_link_libraries(mpi-mpich2-collector-monitor-fileio
	cbtf-messages-events
	cbtf-messages-base
	cbtf-services-common
	cbtf-services-monitor
	cbtf-services-offline
	cbtf-services-timer
	cbtf-services-unwind
	cbtf-services-collector-monitor-fileio
	${LibMonitor_SHARED_LIBRARIES}
	${LibUnwind_SHARED_LIBRARIES}
	${Mpich2_SHARED_LIBRARIES}
	pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(mpi-mpich2-collector-monitor-fileio PROPERTIES PREFIX "")
set_target_properties(mpi-mpich2-collector-monitor-fileio PROPERTIES
        COMPILE_DEFINITIONS "${TLS_DEFINES};CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_FILEIO;${Mpich2_DEFINES}")

install(TARGETS mpi-mpich2-collector-monitor-fileio
	LIBRARY DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors
)

add_library(mpi-mpich2-monitor-fileio STATIC
	${COLLECTOR_SOURCES}
	${WRAPPER_SOURCES}
)

target_include_directories(mpi-mpich2-monitor-fileio PUBLIC
	${LibMonitor_INCLUDE_DIRS}
	${LibUnwind_INCLUDE_DIRS}
	${Mpich2_INCLUDE_DIRS}
	${PROJECT_SOURCE_DIR}/messages/include
	${PROJECT_SOURCE_DIR}/services/include
	${PROJECT_SOURCE_DIR}/services/collector
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/perfdata
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/events
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/base
)

target_link_libraries(mpi-mpich2-monitor-fileio
	cbtf-messages-events-static
	cbtf-messages-base-static
	cbtf-services-common-static
	cbtf-services-monitor-static
	cbtf-services-offline-static
	cbtf-services-timer-static
	cbtf-services-unwind-static
	cbtf-services-collector-monitor-fileio-static
	${LibMonitor_STATIC_LIBRARIES}
	${LibUnwind_STATIC_LIBRARIES}
	${Mpich2_STATIC_LIBRARIES}
	pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(mpi-mpich2-monitor-fileio PROPERTIES
        COMPILE_DEFINITIONS "${TLS_DEFINES};CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_FILEIO;CBTF_STATIC;${Mpich2_DEFINES}")

install(TARGETS mpi-mpich2-monitor-fileio
	ARCHIVE DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors/static
)

#
# mpit experiment libraries for mpich2
#
add_library(mpit-mpich2-collector-monitor-fileio MODULE
	${COLLECTOR_SOURCES}
	${WRAPPER_SOURCES}
)

target_include_directories(mpit-mpich2-collector-monitor-fileio PUBLIC
	${LibMonitor_INCLUDE_DIRS}
	${LibUnwind_INCLUDE_DIRS}
	${Mpich2_INCLUDE_DIRS}
	${PROJECT_SOURCE_DIR}/messages/include
	${PROJECT_SOURCE_DIR}/services/include
	${PROJECT_SOURCE_DIR}/services/collector
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/perfdata
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/events
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/base
)

target_link_libraries(mpit-mpich2-collector-monitor-fileio
	cbtf-messages-events
	cbtf-messages-base
	cbtf-services-common
	cbtf-services-monitor
	cbtf-services-offline
	cbtf-services-timer
	cbtf-services-unwind
	cbtf-services-collector-monitor-fileio
	${LibMonitor_SHARED_LIBRARIES}
	${LibUnwind_SHARED_LIBRARIES}
	${Mpich2_SHARED_LIBRARIES}
	pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(mpit-mpich2-collector-monitor-fileio PROPERTIES PREFIX "")
set_target_properties(mpit-mpich2-collector-monitor-fileio PROPERTIES
        COMPILE_DEFINITIONS "${TLS_DEFINES};CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_FILEIO;EXTENDEDTRACE;${Mpich2_DEFINES}")

install(TARGETS mpit-mpich2-collector-monitor-fileio
	LIBRARY DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors
)

add_library(mpit-mpich2-monitor-fileio STATIC
	${COLLECTOR_SOURCES}
	${WRAPPER_SOURCES}
)

target_include_directories(mpit-mpich2-monitor-fileio PUBLIC
	${LibMonitor_INCLUDE_DIRS}
	${LibUnwind_INCLUDE_DIRS}
	${Mpich2_INCLUDE_DIRS}
	${PROJECT_SOURCE_DIR}/messages/include
	${PROJECT_SOURCE_DIR}/services/include
	${PROJECT_SOURCE_DIR}/services/collector
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/perfdata
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/events
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/base
)

target_link_libraries(mpit-mpich2-monitor-fileio
	cbtf-messages-events-static
	cbtf-messages-base-static
	cbtf-services-common-static
	cbtf-services-monitor-static
	cbtf-services-offline-static
	cbtf-services-timer-static
	cbtf-services-unwind-static
	cbtf-services-collector-monitor-fileio-static
	${LibMonitor_STATIC_LIBRARIES}
	${LibUnwind_STATIC_LIBRARIES}
	${Mpich2_STATIC_LIBRARIES}
	pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(mpit-mpich2-monitor-fileio PROPERTIES
        COMPILE_DEFINITIONS "${TLS_DEFINES};CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_FILEIO;CBTF_STATIC;EXTENDEDTRACE;${Mpich2_DEFINES}")

install(TARGETS mpit-mpich2-monitor-fileio
	ARCHIVE DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors/static
)
#
# mpip experiment libraries for mpich2
#
add_library(mpip-mpich2-collector-monitor-fileio MODULE
	${COLLECTOR_SOURCES}
	${WRAPPER_SOURCES}
)

target_include_directories(mpip-mpich2-collector-monitor-fileio PUBLIC
	${LibMonitor_INCLUDE_DIRS}
	${LibUnwind_INCLUDE_DIRS}
	${Mpich2_INCLUDE_DIRS}
	${PROJECT_SOURCE_DIR}/messages/include
	${PROJECT_SOURCE_DIR}/services/include
	${PROJECT_SOURCE_DIR}/services/collector
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/perfdata
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/events
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/base
)

target_link_libraries(mpip-mpich2-collector-monitor-fileio
	cbtf-messages-events
	cbtf-messages-base
	cbtf-services-common
	cbtf-services-monitor
	cbtf-services-offline
	cbtf-services-timer
	cbtf-services-unwind
	cbtf-services-collector-monitor-fileio
	${LibMonitor_SHARED_LIBRARIES}
	${LibUnwind_SHARED_LIBRARIES}
	${Mpich2_SHARED_LIBRARIES}
	pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(mpip-mpich2-collector-monitor-fileio PROPERTIES PREFIX "")
set_target_properties(mpip-mpich2-collector-monitor-fileio PROPERTIES
        COMPILE_DEFINITIONS "${TLS_DEFINES};CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_FILEIO;PROFILE;${Mpich2_DEFINES}")

install(TARGETS mpip-mpich2-collector-monitor-fileio
	LIBRARY DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors
)

add_library(mpip-mpich2-monitor-fileio STATIC
	${COLLECTOR_SOURCES}
	${WRAPPER_SOURCES}
)

target_include_directories(mpip-mpich2-monitor-fileio PUBLIC
	${LibMonitor_INCLUDE_DIRS}
	${LibUnwind_INCLUDE_DIRS}
	${Mpich2_INCLUDE_DIRS}
	${PROJECT_SOURCE_DIR}/messages/include
	${PROJECT_SOURCE_DIR}/services/include
	${PROJECT_SOURCE_DIR}/services/collector
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/perfdata
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/events
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/base
)

target_link_libraries(mpip-mpich2-monitor-fileio
	cbtf-messages-events-static
	cbtf-messages-base-static
	cbtf-services-common-static
	cbtf-services-monitor-static
	cbtf-services-offline-static
	cbtf-services-timer-static
	cbtf-services-unwind-static
	cbtf-services-collector-monitor-fileio-static
	${LibMonitor_STATIC_LIBRARIES}
	${LibUnwind_STATIC_LIBRARIES}
	${Mpich2_STATIC_LIBRARIES}
	pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(mpip-mpich2-monitor-fileio PROPERTIES
        COMPILE_DEFINITIONS "${TLS_DEFINES};CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_FILEIO;CBTF_STATIC;PROFILE;${Mpich2_DEFINES}")

install(TARGETS mpip-mpich2-monitor-fileio
	ARCHIVE DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors/static
)

if (MRNET_FOUND)
#
# mpi experiment libraries for mpich2
#
add_library(mpi-mpich2-collector-monitor-mrnet-mpi MODULE
	${COLLECTOR_SOURCES}
	${WRAPPER_SOURCES}
)

target_include_directories(mpi-mpich2-collector-monitor-mrnet-mpi PUBLIC
	${LibMonitor_INCLUDE_DIRS}
	${LibUnwind_INCLUDE_DIRS}
	${Mpich2_INCLUDE_DIRS}
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

target_link_libraries(mpi-mpich2-collector-monitor-mrnet-mpi
	cbtf-messages-events
	cbtf-messages-base
	cbtf-services-common
	cbtf-services-monitor
	cbtf-services-offline
	cbtf-services-timer
	cbtf-services-data
	cbtf-services-unwind
	cbtf-services-collector-monitor-mrnet-mpi
	${LibMonitor_SHARED_LIBRARIES}
	${LibUnwind_SHARED_LIBRARIES}
	${MRNet_MRNET_LWR_SHARED_LIBRARIES}
	${Mpich2_SHARED_LIBRARIES}
	pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(mpi-mpich2-collector-monitor-mrnet-mpi PROPERTIES PREFIX "")
set_target_properties(mpi-mpich2-collector-monitor-mrnet-mpi PROPERTIES
        COMPILE_DEFINITIONS "${TLS_DEFINES};CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_MRNET;CBTF_SERVICE_USE_MRNET_MPI;${MRNet_DEFINES};${Mpich2_DEFINES}")

install(TARGETS mpi-mpich2-collector-monitor-mrnet-mpi
	LIBRARY DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors
)


add_library(mpi-mpich2-monitor-mrnet-mpi STATIC
	${COLLECTOR_SOURCES}
	${WRAPPER_SOURCES}
)

target_include_directories(mpi-mpich2-monitor-mrnet-mpi PUBLIC
	${LibMonitor_INCLUDE_DIRS}
	${LibUnwind_INCLUDE_DIRS}
	${Mpich2_INCLUDE_DIRS}
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

target_link_libraries(mpi-mpich2-monitor-mrnet-mpi
	cbtf-messages-events-static
	cbtf-messages-base-static
	cbtf-services-common-static
	cbtf-services-monitor-static
	cbtf-services-offline-static
	cbtf-services-timer-static
	cbtf-services-data-static
	cbtf-services-unwind-static
	cbtf-services-collector-monitor-mrnet-mpi-static
	${LibMonitor_STATIC_LIBRARIES}
	${LibUnwind_STATIC_LIBRARIES}
	${MRNet_MRNET_LWR_STATIC_LIBRARIES}
	${Mpich2_STATIC_LIBRARIES}
	pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(mpi-mpich2-monitor-mrnet-mpi PROPERTIES VERSION 1.1.0)
set_target_properties(mpi-mpich2-monitor-mrnet-mpi PROPERTIES
        COMPILE_DEFINITIONS "${TLS_DEFINES};CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_MRNET;CBTF_SERVICE_USE_MRNET_MPI;CBTF_STATIC;${MRNet_DEFINES};${Mpich2_DEFINES}")

install(TARGETS mpi-mpich2-monitor-mrnet-mpi
	ARCHIVE DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors/static
)
#
# mpip experiment libraries for mpich2
#
add_library(mpip-mpich2-collector-monitor-mrnet-mpi MODULE
	${COLLECTOR_SOURCES}
	${WRAPPER_SOURCES}
)

target_include_directories(mpip-mpich2-collector-monitor-mrnet-mpi PUBLIC
	${LibMonitor_INCLUDE_DIRS}
	${LibUnwind_INCLUDE_DIRS}
	${Mpich2_INCLUDE_DIRS}
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

target_link_libraries(mpip-mpich2-collector-monitor-mrnet-mpi
	cbtf-messages-events
	cbtf-messages-base
	cbtf-services-common
	cbtf-services-monitor
	cbtf-services-offline
	cbtf-services-timer
	cbtf-services-data
	cbtf-services-unwind
	cbtf-services-collector-monitor-mrnet-mpi
	${LibMonitor_SHARED_LIBRARIES}
	${LibUnwind_SHARED_LIBRARIES}
	${MRNet_MRNET_LWR_SHARED_LIBRARIES}
	${Mpich2_SHARED_LIBRARIES}
	pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(mpip-mpich2-collector-monitor-mrnet-mpi PROPERTIES PREFIX "")
set_target_properties(mpip-mpich2-collector-monitor-mrnet-mpi PROPERTIES
        COMPILE_DEFINITIONS "${TLS_DEFINES};CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_MRNET;CBTF_SERVICE_USE_MRNET_MPI;PROFILE;${MRNet_DEFINES};${Mpich2_DEFINES}")

install(TARGETS mpip-mpich2-collector-monitor-mrnet-mpi
	LIBRARY DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors
)


add_library(mpip-mpich2-monitor-mrnet-mpi STATIC
	${COLLECTOR_SOURCES}
	${WRAPPER_SOURCES}
)

target_include_directories(mpip-mpich2-monitor-mrnet-mpi PUBLIC
	${LibMonitor_INCLUDE_DIRS}
	${LibUnwind_INCLUDE_DIRS}
	${Mpich2_INCLUDE_DIRS}
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

target_link_libraries(mpip-mpich2-monitor-mrnet-mpi
	cbtf-messages-events-static
	cbtf-messages-base-static
	cbtf-services-common-static
	cbtf-services-monitor-static
	cbtf-services-offline-static
	cbtf-services-timer-static
	cbtf-services-data-static
	cbtf-services-unwind-static
	cbtf-services-collector-monitor-mrnet-mpi-static
	${LibMonitor_STATIC_LIBRARIES}
	${LibUnwind_STATIC_LIBRARIES}
	${MRNet_MRNET_LWR_STATIC_LIBRARIES}
	${Mpich2_STATIC_LIBRARIES}
	pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(mpip-mpich2-monitor-mrnet-mpi PROPERTIES VERSION 1.1.0)
set_target_properties(mpip-mpich2-monitor-mrnet-mpi PROPERTIES
        COMPILE_DEFINITIONS "${TLS_DEFINES};CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_MRNET;CBTF_SERVICE_USE_MRNET_MPI;CBTF_STATIC;PROFILE;${MRNet_DEFINES};${Mpich2_DEFINES}")

install(TARGETS mpip-mpich2-monitor-mrnet-mpi
	ARCHIVE DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors/static
)
#
# mpit experiment libraries for mpich2
#
add_library(mpit-mpich2-collector-monitor-mrnet-mpi MODULE
	${COLLECTOR_SOURCES}
	${WRAPPER_SOURCES}
)

target_include_directories(mpit-mpich2-collector-monitor-mrnet-mpi PUBLIC
	${LibMonitor_INCLUDE_DIRS}
	${LibUnwind_INCLUDE_DIRS}
	${Mpich2_INCLUDE_DIRS}
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

target_link_libraries(mpit-mpich2-collector-monitor-mrnet-mpi
	cbtf-messages-events
	cbtf-messages-base
	cbtf-services-common
	cbtf-services-monitor
	cbtf-services-offline
	cbtf-services-timer
	cbtf-services-data
	cbtf-services-unwind
	cbtf-services-collector-monitor-mrnet-mpi
	${LibMonitor_SHARED_LIBRARIES}
	${LibUnwind_SHARED_LIBRARIES}
	${MRNet_MRNET_LWR_SHARED_LIBRARIES}
	${Mpich2_SHARED_LIBRARIES}
	pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(mpit-mpich2-collector-monitor-mrnet-mpi PROPERTIES PREFIX "")
set_target_properties(mpit-mpich2-collector-monitor-mrnet-mpi PROPERTIES
        COMPILE_DEFINITIONS "${TLS_DEFINES};CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_MRNET;CBTF_SERVICE_USE_MRNET_MPI;EXTENDEDTRACE;${MRNet_DEFINES};${Mpich2_DEFINES}")

install(TARGETS mpit-mpich2-collector-monitor-mrnet-mpi
	LIBRARY DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors
)


add_library(mpit-mpich2-monitor-mrnet-mpi STATIC
	${COLLECTOR_SOURCES}
	${WRAPPER_SOURCES}
)

target_include_directories(mpit-mpich2-monitor-mrnet-mpi PUBLIC
	${LibMonitor_INCLUDE_DIRS}
	${LibUnwind_INCLUDE_DIRS}
	${Mpich2_INCLUDE_DIRS}
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

target_link_libraries(mpit-mpich2-monitor-mrnet-mpi
	cbtf-messages-events-static
	cbtf-messages-base-static
	cbtf-services-common-static
	cbtf-services-monitor-static
	cbtf-services-offline-static
	cbtf-services-timer-static
	cbtf-services-data-static
	cbtf-services-unwind-static
	cbtf-services-collector-monitor-mrnet-mpi-static
	${LibMonitor_STATIC_LIBRARIES}
	${LibUnwind_STATIC_LIBRARIES}
	${MRNet_MRNET_LWR_STATIC_LIBRARIES}
	${Mpich2_STATIC_LIBRARIES}
	pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(mpit-mpich2-monitor-mrnet-mpi PROPERTIES VERSION 1.1.0)
set_target_properties(mpit-mpich2-monitor-mrnet-mpi PROPERTIES
        COMPILE_DEFINITIONS "${TLS_DEFINES};CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_MRNET;CBTF_SERVICE_USE_MRNET_MPI;CBTF_STATIC;EXTENDEDTRACE;${MRNet_DEFINES};${Mpich2_DEFINES}")

install(TARGETS mpit-mpich2-monitor-mrnet-mpi
	ARCHIVE DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors/static
)

endif() # if (MRNET_FOUND)

endif() # if (HAVE_MPICH2)

if (HAVE_MVAPICH)

#add_definitions(
#	${Mvapich_DEFINES}
#)

#
# mpi experiment libraries for mvapich
#
add_library(mpi-mvapich-collector-monitor-fileio MODULE
	${COLLECTOR_SOURCES}
	${WRAPPER_SOURCES}
)

target_include_directories(mpi-mvapich-collector-monitor-fileio PUBLIC
	${LibMonitor_INCLUDE_DIRS}
	${LibUnwind_INCLUDE_DIRS}
	${Mvapich_INCLUDE_DIRS}
	${PROJECT_SOURCE_DIR}/messages/include
	${PROJECT_SOURCE_DIR}/services/include
	${PROJECT_SOURCE_DIR}/services/collector
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/perfdata
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/events
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/base
)

target_link_libraries(mpi-mvapich-collector-monitor-fileio
	cbtf-messages-events
	cbtf-messages-base
	cbtf-services-common
	cbtf-services-monitor
	cbtf-services-offline
	cbtf-services-timer
	cbtf-services-unwind
	cbtf-services-collector-monitor-fileio
	${LibMonitor_SHARED_LIBRARIES}
	${LibUnwind_SHARED_LIBRARIES}
	${Mvapich_SHARED_LIBRARIES}
	pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(mpi-mvapich-collector-monitor-fileio PROPERTIES PREFIX "")
set_target_properties(mpi-mvapich-collector-monitor-fileio PROPERTIES
        COMPILE_DEFINITIONS "${TLS_DEFINES};CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_FILEIO;${Mvapich_DEFINES}")

install(TARGETS mpi-mvapich-collector-monitor-fileio
	LIBRARY DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors
)

add_library(mpi-mvapich-monitor-fileio STATIC
	${COLLECTOR_SOURCES}
	${WRAPPER_SOURCES}
)

target_include_directories(mpi-mvapich-monitor-fileio PUBLIC
	${LibMonitor_INCLUDE_DIRS}
	${LibUnwind_INCLUDE_DIRS}
	${Mvapich_INCLUDE_DIRS}
	${PROJECT_SOURCE_DIR}/messages/include
	${PROJECT_SOURCE_DIR}/services/include
	${PROJECT_SOURCE_DIR}/services/collector
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/perfdata
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/events
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/base
)

target_link_libraries(mpi-mvapich-monitor-fileio
	cbtf-messages-events-static
	cbtf-messages-base-static
	cbtf-services-common-static
	cbtf-services-monitor-static
	cbtf-services-offline-static
	cbtf-services-timer-static
	cbtf-services-unwind-static
	cbtf-services-collector-monitor-fileio-static
	${LibMonitor_STATIC_LIBRARIES}
	${LibUnwind_STATIC_LIBRARIES}
	${Mvapich_STATIC_LIBRARIES}
	pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(mpi-mvapich-monitor-fileio PROPERTIES
        COMPILE_DEFINITIONS "${TLS_DEFINES};CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_FILEIO;CBTF_STATIC;${Mvapich_DEFINES}")

install(TARGETS mpi-mvapich-monitor-fileio
	ARCHIVE DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors/static
)

#
# mpit experiment libraries for mvapich
#
add_library(mpit-mvapich-collector-monitor-fileio MODULE
	${COLLECTOR_SOURCES}
	${WRAPPER_SOURCES}
)

target_include_directories(mpit-mvapich-collector-monitor-fileio PUBLIC
	${LibMonitor_INCLUDE_DIRS}
	${LibUnwind_INCLUDE_DIRS}
	${Mvapich_INCLUDE_DIRS}
	${PROJECT_SOURCE_DIR}/messages/include
	${PROJECT_SOURCE_DIR}/services/include
	${PROJECT_SOURCE_DIR}/services/collector
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/perfdata
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/events
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/base
)

target_link_libraries(mpit-mvapich-collector-monitor-fileio
	cbtf-messages-events
	cbtf-messages-base
	cbtf-services-common
	cbtf-services-monitor
	cbtf-services-offline
	cbtf-services-timer
	cbtf-services-unwind
	cbtf-services-collector-monitor-fileio
	${LibMonitor_SHARED_LIBRARIES}
	${LibUnwind_SHARED_LIBRARIES}
	${Mvapich_SHARED_LIBRARIES}
	pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(mpit-mvapich-collector-monitor-fileio PROPERTIES PREFIX "")
set_target_properties(mpit-mvapich-collector-monitor-fileio PROPERTIES
        COMPILE_DEFINITIONS "${TLS_DEFINES};CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_FILEIO;EXTENDEDTRACE;${Mvapich_DEFINES}")

install(TARGETS mpit-mvapich-collector-monitor-fileio
	LIBRARY DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors
)

add_library(mpit-mvapich-monitor-fileio STATIC
	${COLLECTOR_SOURCES}
	${WRAPPER_SOURCES}
)

target_include_directories(mpit-mvapich-monitor-fileio PUBLIC
	${LibMonitor_INCLUDE_DIRS}
	${LibUnwind_INCLUDE_DIRS}
	${Mvapich_INCLUDE_DIRS}
	${PROJECT_SOURCE_DIR}/messages/include
	${PROJECT_SOURCE_DIR}/services/include
	${PROJECT_SOURCE_DIR}/services/collector
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/perfdata
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/events
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/base
)

target_link_libraries(mpit-mvapich-monitor-fileio
	cbtf-messages-events-static
	cbtf-messages-base-static
	cbtf-services-common-static
	cbtf-services-monitor-static
	cbtf-services-offline-static
	cbtf-services-timer-static
	cbtf-services-unwind-static
	cbtf-services-collector-monitor-fileio-static
	${LibMonitor_STATIC_LIBRARIES}
	${LibUnwind_STATIC_LIBRARIES}
	${Mvapich_STATIC_LIBRARIES}
	pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(mpit-mvapich-monitor-fileio PROPERTIES
        COMPILE_DEFINITIONS "${TLS_DEFINES};CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_FILEIO;CBTF_STATIC;EXTENDEDTRACE;${Mvapich_DEFINES}")

install(TARGETS mpit-mvapich-monitor-fileio
	ARCHIVE DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors/static
)
#
# mpip experiment libraries for mvapich
#
add_library(mpip-mvapich-collector-monitor-fileio MODULE
	${COLLECTOR_SOURCES}
	${WRAPPER_SOURCES}
)

target_include_directories(mpip-mvapich-collector-monitor-fileio PUBLIC
	${LibMonitor_INCLUDE_DIRS}
	${LibUnwind_INCLUDE_DIRS}
	${Mvapich_INCLUDE_DIRS}
	${PROJECT_SOURCE_DIR}/messages/include
	${PROJECT_SOURCE_DIR}/services/include
	${PROJECT_SOURCE_DIR}/services/collector
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/perfdata
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/events
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/base
)

target_link_libraries(mpip-mvapich-collector-monitor-fileio
	cbtf-messages-events
	cbtf-messages-base
	cbtf-services-common
	cbtf-services-monitor
	cbtf-services-offline
	cbtf-services-timer
	cbtf-services-unwind
	cbtf-services-collector-monitor-fileio
	${LibMonitor_SHARED_LIBRARIES}
	${LibUnwind_SHARED_LIBRARIES}
	${Mvapich_SHARED_LIBRARIES}
	pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(mpip-mvapich-collector-monitor-fileio PROPERTIES PREFIX "")
set_target_properties(mpip-mvapich-collector-monitor-fileio PROPERTIES
        COMPILE_DEFINITIONS "${TLS_DEFINES};CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_FILEIO;PROFILE;${Mvapich_DEFINES}")

install(TARGETS mpip-mvapich-collector-monitor-fileio
	LIBRARY DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors
)

add_library(mpip-mvapich-monitor-fileio STATIC
	${COLLECTOR_SOURCES}
	${WRAPPER_SOURCES}
)

target_include_directories(mpip-mvapich-monitor-fileio PUBLIC
	${LibMonitor_INCLUDE_DIRS}
	${LibUnwind_INCLUDE_DIRS}
	${Mvapich_INCLUDE_DIRS}
	${PROJECT_SOURCE_DIR}/messages/include
	${PROJECT_SOURCE_DIR}/services/include
	${PROJECT_SOURCE_DIR}/services/collector
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/perfdata
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/events
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/base
)

target_link_libraries(mpip-mvapich-monitor-fileio
	cbtf-messages-events-static
	cbtf-messages-base-static
	cbtf-services-common-static
	cbtf-services-monitor-static
	cbtf-services-offline-static
	cbtf-services-timer-static
	cbtf-services-unwind-static
	cbtf-services-collector-monitor-fileio-static
	${LibMonitor_STATIC_LIBRARIES}
	${LibUnwind_STATIC_LIBRARIES}
	${Mvapich_STATIC_LIBRARIES}
	pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(mpip-mvapich-monitor-fileio PROPERTIES
        COMPILE_DEFINITIONS "${TLS_DEFINES};CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_FILEIO;CBTF_STATIC;PROFILE;${Mvapich_DEFINES}")

install(TARGETS mpip-mvapich-monitor-fileio
	ARCHIVE DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors/static
)

if (MRNET_FOUND)
#
# mpi experiment libraries for mvapich
#
add_library(mpi-mvapich-collector-monitor-mrnet-mpi MODULE
	${COLLECTOR_SOURCES}
	${WRAPPER_SOURCES}
)

target_include_directories(mpi-mvapich-collector-monitor-mrnet-mpi PUBLIC
	${LibMonitor_INCLUDE_DIRS}
	${LibUnwind_INCLUDE_DIRS}
	${Mvapich_INCLUDE_DIRS}
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

target_link_libraries(mpi-mvapich-collector-monitor-mrnet-mpi
	cbtf-messages-events
	cbtf-messages-base
	cbtf-services-common
	cbtf-services-monitor
	cbtf-services-offline
	cbtf-services-timer
	cbtf-services-data
	cbtf-services-unwind
	cbtf-services-collector-monitor-mrnet-mpi
	${LibMonitor_SHARED_LIBRARIES}
	${LibUnwind_SHARED_LIBRARIES}
	${MRNet_MRNET_LWR_SHARED_LIBRARIES}
	${Mvapich_SHARED_LIBRARIES}
	pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(mpi-mvapich-collector-monitor-mrnet-mpi PROPERTIES PREFIX "")
set_target_properties(mpi-mvapich-collector-monitor-mrnet-mpi PROPERTIES
        COMPILE_DEFINITIONS "${TLS_DEFINES};CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_MRNET;CBTF_SERVICE_USE_MRNET_MPI;${MRNet_DEFINES};${Mvapich_DEFINES}")

install(TARGETS mpi-mvapich-collector-monitor-mrnet-mpi
	LIBRARY DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors
)


add_library(mpi-mvapich-monitor-mrnet-mpi STATIC
	${COLLECTOR_SOURCES}
	${WRAPPER_SOURCES}
)

target_include_directories(mpi-mvapich-monitor-mrnet-mpi PUBLIC
	${LibMonitor_INCLUDE_DIRS}
	${LibUnwind_INCLUDE_DIRS}
	${Mvapich_INCLUDE_DIRS}
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

target_link_libraries(mpi-mvapich-monitor-mrnet-mpi
	cbtf-messages-events-static
	cbtf-messages-base-static
	cbtf-services-common-static
	cbtf-services-monitor-static
	cbtf-services-offline-static
	cbtf-services-timer-static
	cbtf-services-data-static
	cbtf-services-unwind-static
	cbtf-services-collector-monitor-mrnet-mpi-static
	${LibMonitor_STATIC_LIBRARIES}
	${LibUnwind_STATIC_LIBRARIES}
	${MRNet_MRNET_LWR_STATIC_LIBRARIES}
	${Mvapich_STATIC_LIBRARIES}
	pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(mpi-mvapich-monitor-mrnet-mpi PROPERTIES VERSION 1.1.0)
set_target_properties(mpi-mvapich-monitor-mrnet-mpi PROPERTIES
        COMPILE_DEFINITIONS "${TLS_DEFINES};CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_MRNET;CBTF_SERVICE_USE_MRNET_MPI;CBTF_STATIC;${MRNet_DEFINES};${Mvapich_DEFINES}")

install(TARGETS mpi-mvapich-monitor-mrnet-mpi
	ARCHIVE DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors/static
)
#
# mpip experiment libraries for mvapich
#
add_library(mpip-mvapich-collector-monitor-mrnet-mpi MODULE
	${COLLECTOR_SOURCES}
	${WRAPPER_SOURCES}
)

target_include_directories(mpip-mvapich-collector-monitor-mrnet-mpi PUBLIC
	${LibMonitor_INCLUDE_DIRS}
	${LibUnwind_INCLUDE_DIRS}
	${Mvapich_INCLUDE_DIRS}
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

target_link_libraries(mpip-mvapich-collector-monitor-mrnet-mpi
	cbtf-messages-events
	cbtf-messages-base
	cbtf-services-common
	cbtf-services-monitor
	cbtf-services-offline
	cbtf-services-timer
	cbtf-services-data
	cbtf-services-unwind
	cbtf-services-collector-monitor-mrnet-mpi
	${LibMonitor_SHARED_LIBRARIES}
	${LibUnwind_SHARED_LIBRARIES}
	${MRNet_MRNET_LWR_SHARED_LIBRARIES}
	${Mvapich_SHARED_LIBRARIES}
	pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(mpip-mvapich-collector-monitor-mrnet-mpi PROPERTIES PREFIX "")
set_target_properties(mpip-mvapich-collector-monitor-mrnet-mpi PROPERTIES
        COMPILE_DEFINITIONS "${TLS_DEFINES};CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_MRNET;CBTF_SERVICE_USE_MRNET_MPI;PROFILE;${MRNet_DEFINES};${Mvapich_DEFINES}")

install(TARGETS mpip-mvapich-collector-monitor-mrnet-mpi
	LIBRARY DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors
)


add_library(mpip-mvapich-monitor-mrnet-mpi STATIC
	${COLLECTOR_SOURCES}
	${WRAPPER_SOURCES}
)

target_include_directories(mpip-mvapich-monitor-mrnet-mpi PUBLIC
	${LibMonitor_INCLUDE_DIRS}
	${LibUnwind_INCLUDE_DIRS}
	${Mvapich_INCLUDE_DIRS}
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

target_link_libraries(mpip-mvapich-monitor-mrnet-mpi
	cbtf-messages-events-static
	cbtf-messages-base-static
	cbtf-services-common-static
	cbtf-services-monitor-static
	cbtf-services-offline-static
	cbtf-services-timer-static
	cbtf-services-data-static
	cbtf-services-unwind-static
	cbtf-services-collector-monitor-mrnet-mpi-static
	${LibMonitor_STATIC_LIBRARIES}
	${LibUnwind_STATIC_LIBRARIES}
	${MRNet_MRNET_LWR_STATIC_LIBRARIES}
	${Mvapich_STATIC_LIBRARIES}
	pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(mpip-mvapich-monitor-mrnet-mpi PROPERTIES VERSION 1.1.0)
set_target_properties(mpip-mvapich-monitor-mrnet-mpi PROPERTIES
        COMPILE_DEFINITIONS "${TLS_DEFINES};CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_MRNET;CBTF_SERVICE_USE_MRNET_MPI;CBTF_STATIC;PROFILE;${MRNet_DEFINES};${Mvapich_DEFINES}")

install(TARGETS mpip-mvapich-monitor-mrnet-mpi
	ARCHIVE DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors/static
)
#
# mpit experiment libraries for mvapich
#
add_library(mpit-mvapich-collector-monitor-mrnet-mpi MODULE
	${COLLECTOR_SOURCES}
	${WRAPPER_SOURCES}
)

target_include_directories(mpit-mvapich-collector-monitor-mrnet-mpi PUBLIC
	${LibMonitor_INCLUDE_DIRS}
	${LibUnwind_INCLUDE_DIRS}
	${Mvapich_INCLUDE_DIRS}
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

target_link_libraries(mpit-mvapich-collector-monitor-mrnet-mpi
	cbtf-messages-events
	cbtf-messages-base
	cbtf-services-common
	cbtf-services-monitor
	cbtf-services-offline
	cbtf-services-timer
	cbtf-services-data
	cbtf-services-unwind
	cbtf-services-collector-monitor-mrnet-mpi
	${LibMonitor_SHARED_LIBRARIES}
	${LibUnwind_SHARED_LIBRARIES}
	${MRNet_MRNET_LWR_SHARED_LIBRARIES}
	${Mvapich_SHARED_LIBRARIES}
	pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(mpit-mvapich-collector-monitor-mrnet-mpi PROPERTIES PREFIX "")
set_target_properties(mpit-mvapich-collector-monitor-mrnet-mpi PROPERTIES
        COMPILE_DEFINITIONS "${TLS_DEFINES};CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_MRNET;CBTF_SERVICE_USE_MRNET_MPI;EXTENDEDTRACE;${MRNet_DEFINES};${Mvapich_DEFINES}")

install(TARGETS mpit-mvapich-collector-monitor-mrnet-mpi
	LIBRARY DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors
)


add_library(mpit-mvapich-monitor-mrnet-mpi STATIC
	${COLLECTOR_SOURCES}
	${WRAPPER_SOURCES}
)

target_include_directories(mpit-mvapich-monitor-mrnet-mpi PUBLIC
	${LibMonitor_INCLUDE_DIRS}
	${LibUnwind_INCLUDE_DIRS}
	${Mvapich_INCLUDE_DIRS}
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

target_link_libraries(mpit-mvapich-monitor-mrnet-mpi
	cbtf-messages-events-static
	cbtf-messages-base-static
	cbtf-services-common-static
	cbtf-services-monitor-static
	cbtf-services-offline-static
	cbtf-services-timer-static
	cbtf-services-data-static
	cbtf-services-unwind-static
	cbtf-services-collector-monitor-mrnet-mpi-static
	${LibMonitor_STATIC_LIBRARIES}
	${LibUnwind_STATIC_LIBRARIES}
	${MRNet_MRNET_LWR_STATIC_LIBRARIES}
	${Mvapich_STATIC_LIBRARIES}
	pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(mpit-mvapich-monitor-mrnet-mpi PROPERTIES VERSION 1.1.0)
set_target_properties(mpit-mvapich-monitor-mrnet-mpi PROPERTIES
        COMPILE_DEFINITIONS "${TLS_DEFINES};CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_MRNET;CBTF_SERVICE_USE_MRNET_MPI;CBTF_STATIC;EXTENDEDTRACE;${MRNet_DEFINES};${Mvapich_DEFINES}")

install(TARGETS mpit-mvapich-monitor-mrnet-mpi
	ARCHIVE DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors/static
)

endif() # if (MRNET_FOUND)

endif() # if (HAVE_MVAPICH)

if (HAVE_MVAPICH2)

#add_definitions(
#	${Mvapich2_DEFINES}
#)

#
# mpi experiment libraries for mvapich2
#
add_library(mpi-mvapich2-collector-monitor-fileio MODULE
	${COLLECTOR_SOURCES}
	${WRAPPER_SOURCES}
)

target_include_directories(mpi-mvapich2-collector-monitor-fileio PUBLIC
	${LibMonitor_INCLUDE_DIRS}
	${LibUnwind_INCLUDE_DIRS}
	${Mvapich2_INCLUDE_DIRS}
	${PROJECT_SOURCE_DIR}/messages/include
	${PROJECT_SOURCE_DIR}/services/include
	${PROJECT_SOURCE_DIR}/services/collector
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/perfdata
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/events
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/base
)

target_link_libraries(mpi-mvapich2-collector-monitor-fileio
	cbtf-messages-events
	cbtf-messages-base
	cbtf-services-common
	cbtf-services-monitor
	cbtf-services-offline
	cbtf-services-timer
	cbtf-services-unwind
	cbtf-services-collector-monitor-fileio
	${LibMonitor_SHARED_LIBRARIES}
	${LibUnwind_SHARED_LIBRARIES}
	${Mvapich2_SHARED_LIBRARIES}
	pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(mpi-mvapich2-collector-monitor-fileio PROPERTIES PREFIX "")
set_target_properties(mpi-mvapich2-collector-monitor-fileio PROPERTIES
        COMPILE_DEFINITIONS "${TLS_DEFINES};CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_FILEIO;${Mvapich2_DEFINES}")

install(TARGETS mpi-mvapich2-collector-monitor-fileio
	LIBRARY DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors
)

add_library(mpi-mvapich2-monitor-fileio STATIC
	${COLLECTOR_SOURCES}
	${WRAPPER_SOURCES}
)

target_include_directories(mpi-mvapich2-monitor-fileio PUBLIC
	${LibMonitor_INCLUDE_DIRS}
	${LibUnwind_INCLUDE_DIRS}
	${Mvapich2_INCLUDE_DIRS}
	${PROJECT_SOURCE_DIR}/messages/include
	${PROJECT_SOURCE_DIR}/services/include
	${PROJECT_SOURCE_DIR}/services/collector
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/perfdata
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/events
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/base
)

target_link_libraries(mpi-mvapich2-monitor-fileio
	cbtf-messages-events-static
	cbtf-messages-base-static
	cbtf-services-common-static
	cbtf-services-monitor-static
	cbtf-services-offline-static
	cbtf-services-timer-static
	cbtf-services-unwind-static
	cbtf-services-collector-monitor-fileio-static
	${LibMonitor_STATIC_LIBRARIES}
	${LibUnwind_STATIC_LIBRARIES}
	${Mvapich2_STATIC_LIBRARIES}
	pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(mpi-mvapich2-monitor-fileio PROPERTIES
        COMPILE_DEFINITIONS "${TLS_DEFINES};CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_FILEIO;CBTF_STATIC;${Mvapich2_DEFINES}")

install(TARGETS mpi-mvapich2-monitor-fileio
	ARCHIVE DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors/static
)

#
# mpit experiment libraries for mvapich2
#
add_library(mpit-mvapich2-collector-monitor-fileio MODULE
	${COLLECTOR_SOURCES}
	${WRAPPER_SOURCES}
)

target_include_directories(mpit-mvapich2-collector-monitor-fileio PUBLIC
	${LibMonitor_INCLUDE_DIRS}
	${LibUnwind_INCLUDE_DIRS}
	${Mvapich2_INCLUDE_DIRS}
	${PROJECT_SOURCE_DIR}/messages/include
	${PROJECT_SOURCE_DIR}/services/include
	${PROJECT_SOURCE_DIR}/services/collector
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/perfdata
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/events
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/base
)

target_link_libraries(mpit-mvapich2-collector-monitor-fileio
	cbtf-messages-events
	cbtf-messages-base
	cbtf-services-common
	cbtf-services-monitor
	cbtf-services-offline
	cbtf-services-timer
	cbtf-services-unwind
	cbtf-services-collector-monitor-fileio
	${LibMonitor_SHARED_LIBRARIES}
	${LibUnwind_SHARED_LIBRARIES}
	${Mvapich2_SHARED_LIBRARIES}
	pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(mpit-mvapich2-collector-monitor-fileio PROPERTIES PREFIX "")
set_target_properties(mpit-mvapich2-collector-monitor-fileio PROPERTIES
        COMPILE_DEFINITIONS "${TLS_DEFINES};CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_FILEIO;EXTENDEDTRACE;${Mvapich2_DEFINES}")

install(TARGETS mpit-mvapich2-collector-monitor-fileio
	LIBRARY DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors
)

add_library(mpit-mvapich2-monitor-fileio STATIC
	${COLLECTOR_SOURCES}
	${WRAPPER_SOURCES}
)

target_include_directories(mpit-mvapich2-monitor-fileio PUBLIC
	${LibMonitor_INCLUDE_DIRS}
	${LibUnwind_INCLUDE_DIRS}
	${Mvapich2_INCLUDE_DIRS}
	${PROJECT_SOURCE_DIR}/messages/include
	${PROJECT_SOURCE_DIR}/services/include
	${PROJECT_SOURCE_DIR}/services/collector
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/perfdata
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/events
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/base
)

target_link_libraries(mpit-mvapich2-monitor-fileio
	cbtf-messages-events-static
	cbtf-messages-base-static
	cbtf-services-common-static
	cbtf-services-monitor-static
	cbtf-services-offline-static
	cbtf-services-timer-static
	cbtf-services-unwind-static
	cbtf-services-collector-monitor-fileio-static
	${LibMonitor_STATIC_LIBRARIES}
	${LibUnwind_STATIC_LIBRARIES}
	${Mvapich2_STATIC_LIBRARIES}
	pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(mpit-mvapich2-monitor-fileio PROPERTIES
        COMPILE_DEFINITIONS "${TLS_DEFINES};CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_FILEIO;CBTF_STATIC;EXTENDEDTRACE;${Mvapich2_DEFINES}")

install(TARGETS mpit-mvapich2-monitor-fileio
	ARCHIVE DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors/static
)
#
# mpip experiment libraries for mvapich2
#
add_library(mpip-mvapich2-collector-monitor-fileio MODULE
	${COLLECTOR_SOURCES}
	${WRAPPER_SOURCES}
)

target_include_directories(mpip-mvapich2-collector-monitor-fileio PUBLIC
	${LibMonitor_INCLUDE_DIRS}
	${LibUnwind_INCLUDE_DIRS}
	${Mvapich2_INCLUDE_DIRS}
	${PROJECT_SOURCE_DIR}/messages/include
	${PROJECT_SOURCE_DIR}/services/include
	${PROJECT_SOURCE_DIR}/services/collector
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/perfdata
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/events
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/base
)

target_link_libraries(mpip-mvapich2-collector-monitor-fileio
	cbtf-messages-events
	cbtf-messages-base
	cbtf-services-common
	cbtf-services-monitor
	cbtf-services-offline
	cbtf-services-timer
	cbtf-services-unwind
	cbtf-services-collector-monitor-fileio
	${LibMonitor_SHARED_LIBRARIES}
	${LibUnwind_SHARED_LIBRARIES}
	${Mvapich2_SHARED_LIBRARIES}
	pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(mpip-mvapich2-collector-monitor-fileio PROPERTIES PREFIX "")
set_target_properties(mpip-mvapich2-collector-monitor-fileio PROPERTIES
        COMPILE_DEFINITIONS "${TLS_DEFINES};CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_FILEIO;PROFILE;${Mvapich2_DEFINES}")

install(TARGETS mpip-mvapich2-collector-monitor-fileio
	LIBRARY DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors
)

add_library(mpip-mvapich2-monitor-fileio STATIC
	${COLLECTOR_SOURCES}
	${WRAPPER_SOURCES}
)

target_include_directories(mpip-mvapich2-monitor-fileio PUBLIC
	${LibMonitor_INCLUDE_DIRS}
	${LibUnwind_INCLUDE_DIRS}
	${Mvapich2_INCLUDE_DIRS}
	${PROJECT_SOURCE_DIR}/messages/include
	${PROJECT_SOURCE_DIR}/services/include
	${PROJECT_SOURCE_DIR}/services/collector
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/perfdata
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/events
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/base
)

target_link_libraries(mpip-mvapich2-monitor-fileio
	cbtf-messages-events-static
	cbtf-messages-base-static
	cbtf-services-common-static
	cbtf-services-monitor-static
	cbtf-services-offline-static
	cbtf-services-timer-static
	cbtf-services-unwind-static
	cbtf-services-collector-monitor-fileio-static
	${LibMonitor_STATIC_LIBRARIES}
	${LibUnwind_STATIC_LIBRARIES}
	${Mvapich2_STATIC_LIBRARIES}
	pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(mpip-mvapich2-monitor-fileio PROPERTIES
        COMPILE_DEFINITIONS "${TLS_DEFINES};CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_FILEIO;CBTF_STATIC;PROFILE;${Mvapich2_DEFINES}")

install(TARGETS mpip-mvapich2-monitor-fileio
	ARCHIVE DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors/static
)

if (MRNET_FOUND)
#
# mpi experiment libraries for mvapich2
#
add_library(mpi-mvapich2-collector-monitor-mrnet-mpi MODULE
	${COLLECTOR_SOURCES}
	${WRAPPER_SOURCES}
)

target_include_directories(mpi-mvapich2-collector-monitor-mrnet-mpi PUBLIC
	${LibMonitor_INCLUDE_DIRS}
	${LibUnwind_INCLUDE_DIRS}
	${Mvapich2_INCLUDE_DIRS}
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

target_link_libraries(mpi-mvapich2-collector-monitor-mrnet-mpi
	cbtf-messages-events
	cbtf-messages-base
	cbtf-services-common
	cbtf-services-monitor
	cbtf-services-offline
	cbtf-services-timer
	cbtf-services-data
	cbtf-services-unwind
	cbtf-services-collector-monitor-mrnet-mpi
	${LibMonitor_SHARED_LIBRARIES}
	${LibUnwind_SHARED_LIBRARIES}
	${MRNet_MRNET_LWR_SHARED_LIBRARIES}
	${Mvapich2_SHARED_LIBRARIES}
	pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(mpi-mvapich2-collector-monitor-mrnet-mpi PROPERTIES PREFIX "")
set_target_properties(mpi-mvapich2-collector-monitor-mrnet-mpi PROPERTIES
        COMPILE_DEFINITIONS "${TLS_DEFINES};CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_MRNET;CBTF_SERVICE_USE_MRNET_MPI;${MRNet_DEFINES};${Mvapich2_DEFINES}")

install(TARGETS mpi-mvapich2-collector-monitor-mrnet-mpi
	LIBRARY DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors
)


add_library(mpi-mvapich2-monitor-mrnet-mpi STATIC
	${COLLECTOR_SOURCES}
	${WRAPPER_SOURCES}
)

target_include_directories(mpi-mvapich2-monitor-mrnet-mpi PUBLIC
	${LibMonitor_INCLUDE_DIRS}
	${LibUnwind_INCLUDE_DIRS}
	${Mvapich2_INCLUDE_DIRS}
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

target_link_libraries(mpi-mvapich2-monitor-mrnet-mpi
	cbtf-messages-events-static
	cbtf-messages-base-static
	cbtf-services-common-static
	cbtf-services-monitor-static
	cbtf-services-offline-static
	cbtf-services-timer-static
	cbtf-services-data-static
	cbtf-services-unwind-static
	cbtf-services-collector-monitor-mrnet-mpi-static
	${LibMonitor_STATIC_LIBRARIES}
	${LibUnwind_STATIC_LIBRARIES}
	${MRNet_MRNET_LWR_STATIC_LIBRARIES}
	${Mvapich2_STATIC_LIBRARIES}
	pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(mpi-mvapich2-monitor-mrnet-mpi PROPERTIES VERSION 1.1.0)
set_target_properties(mpi-mvapich2-monitor-mrnet-mpi PROPERTIES
        COMPILE_DEFINITIONS "${TLS_DEFINES};CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_MRNET;CBTF_SERVICE_USE_MRNET_MPI;CBTF_STATIC;${MRNet_DEFINES};${Mvapich2_DEFINES}")

install(TARGETS mpi-mvapich2-monitor-mrnet-mpi
	ARCHIVE DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors/static
)
#
# mpip experiment libraries for mvapich2
#
add_library(mpip-mvapich2-collector-monitor-mrnet-mpi MODULE
	${COLLECTOR_SOURCES}
	${WRAPPER_SOURCES}
)

target_include_directories(mpip-mvapich2-collector-monitor-mrnet-mpi PUBLIC
	${LibMonitor_INCLUDE_DIRS}
	${LibUnwind_INCLUDE_DIRS}
	${Mvapich2_INCLUDE_DIRS}
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

target_link_libraries(mpip-mvapich2-collector-monitor-mrnet-mpi
	cbtf-messages-events
	cbtf-messages-base
	cbtf-services-common
	cbtf-services-monitor
	cbtf-services-offline
	cbtf-services-timer
	cbtf-services-data
	cbtf-services-unwind
	cbtf-services-collector-monitor-mrnet-mpi
	${LibMonitor_SHARED_LIBRARIES}
	${LibUnwind_SHARED_LIBRARIES}
	${MRNet_MRNET_LWR_SHARED_LIBRARIES}
	${Mvapich2_SHARED_LIBRARIES}
	pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(mpip-mvapich2-collector-monitor-mrnet-mpi PROPERTIES PREFIX "")
set_target_properties(mpip-mvapich2-collector-monitor-mrnet-mpi PROPERTIES
        COMPILE_DEFINITIONS "${TLS_DEFINES};CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_MRNET;CBTF_SERVICE_USE_MRNET_MPI;PROFILE;${MRNet_DEFINES};${Mvapich2_DEFINES}")

install(TARGETS mpip-mvapich2-collector-monitor-mrnet-mpi
	LIBRARY DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors
)


add_library(mpip-mvapich2-monitor-mrnet-mpi STATIC
	${COLLECTOR_SOURCES}
	${WRAPPER_SOURCES}
)

target_include_directories(mpip-mvapich2-monitor-mrnet-mpi PUBLIC
	${LibMonitor_INCLUDE_DIRS}
	${LibUnwind_INCLUDE_DIRS}
	${Mvapich2_INCLUDE_DIRS}
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

target_link_libraries(mpip-mvapich2-monitor-mrnet-mpi
	cbtf-messages-events-static
	cbtf-messages-base-static
	cbtf-services-common-static
	cbtf-services-monitor-static
	cbtf-services-offline-static
	cbtf-services-timer-static
	cbtf-services-data-static
	cbtf-services-unwind-static
	cbtf-services-collector-monitor-mrnet-mpi-static
	${LibMonitor_STATIC_LIBRARIES}
	${LibUnwind_STATIC_LIBRARIES}
	${MRNet_MRNET_LWR_STATIC_LIBRARIES}
	${Mvapich2_STATIC_LIBRARIES}
	pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(mpip-mvapich2-monitor-mrnet-mpi PROPERTIES VERSION 1.1.0)
set_target_properties(mpip-mvapich2-monitor-mrnet-mpi PROPERTIES
        COMPILE_DEFINITIONS "${TLS_DEFINES};CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_MRNET;CBTF_SERVICE_USE_MRNET_MPI;CBTF_STATIC;PROFILE;${MRNet_DEFINES};${Mvapich2_DEFINES}")

install(TARGETS mpip-mvapich2-monitor-mrnet-mpi
	ARCHIVE DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors/static
)
#
# mpit experiment libraries for mvapich2
#
add_library(mpit-mvapich2-collector-monitor-mrnet-mpi MODULE
	${COLLECTOR_SOURCES}
	${WRAPPER_SOURCES}
)

target_include_directories(mpit-mvapich2-collector-monitor-mrnet-mpi PUBLIC
	${LibMonitor_INCLUDE_DIRS}
	${LibUnwind_INCLUDE_DIRS}
	${Mvapich2_INCLUDE_DIRS}
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

target_link_libraries(mpit-mvapich2-collector-monitor-mrnet-mpi
	cbtf-messages-events
	cbtf-messages-base
	cbtf-services-common
	cbtf-services-monitor
	cbtf-services-offline
	cbtf-services-timer
	cbtf-services-data
	cbtf-services-unwind
	cbtf-services-collector-monitor-mrnet-mpi
	${LibMonitor_SHARED_LIBRARIES}
	${LibUnwind_SHARED_LIBRARIES}
	${MRNet_MRNET_LWR_SHARED_LIBRARIES}
	${Mvapich2_SHARED_LIBRARIES}
	pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(mpit-mvapich2-collector-monitor-mrnet-mpi PROPERTIES PREFIX "")
set_target_properties(mpit-mvapich2-collector-monitor-mrnet-mpi PROPERTIES
        COMPILE_DEFINITIONS "${TLS_DEFINES};CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_MRNET;CBTF_SERVICE_USE_MRNET_MPI;EXTENDEDTRACE;${MRNet_DEFINES};${Mvapich2_DEFINES}")

install(TARGETS mpit-mvapich2-collector-monitor-mrnet-mpi
	LIBRARY DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors
)


add_library(mpit-mvapich2-monitor-mrnet-mpi STATIC
	${COLLECTOR_SOURCES}
	${WRAPPER_SOURCES}
)

target_include_directories(mpit-mvapich2-monitor-mrnet-mpi PUBLIC
	${LibMonitor_INCLUDE_DIRS}
	${LibUnwind_INCLUDE_DIRS}
	${Mvapich2_INCLUDE_DIRS}
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

target_link_libraries(mpit-mvapich2-monitor-mrnet-mpi
	cbtf-messages-events-static
	cbtf-messages-base-static
	cbtf-services-common-static
	cbtf-services-monitor-static
	cbtf-services-offline-static
	cbtf-services-timer-static
	cbtf-services-data-static
	cbtf-services-unwind-static
	cbtf-services-collector-monitor-mrnet-mpi-static
	${LibMonitor_STATIC_LIBRARIES}
	${LibUnwind_STATIC_LIBRARIES}
	${MRNet_MRNET_LWR_STATIC_LIBRARIES}
	${Mvapich2_STATIC_LIBRARIES}
	pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(mpit-mvapich2-monitor-mrnet-mpi PROPERTIES VERSION 1.1.0)
set_target_properties(mpit-mvapich2-monitor-mrnet-mpi PROPERTIES
        COMPILE_DEFINITIONS "${TLS_DEFINES};CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_MRNET;CBTF_SERVICE_USE_MRNET_MPI;CBTF_STATIC;EXTENDEDTRACE;${MRNet_DEFINES};${Mvapich2_DEFINES}")

install(TARGETS mpit-mvapich2-monitor-mrnet-mpi
	ARCHIVE DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors/static
)

endif() # if (MRNET_FOUND)

endif() # if (HAVE_MVAPICH2)

if (HAVE_MPT)

#add_definitions(
#	${Mpt_DEFINES}
#)

#
# mpi experiment libraries for mpt
#
add_library(mpi-mpt-collector-monitor-fileio MODULE
	${COLLECTOR_SOURCES}
	${WRAPPER_SOURCES}
)

target_include_directories(mpi-mpt-collector-monitor-fileio PUBLIC
	${LibMonitor_INCLUDE_DIRS}
	${LibUnwind_INCLUDE_DIRS}
	${Mpt_INCLUDE_DIRS}
	${PROJECT_SOURCE_DIR}/messages/include
	${PROJECT_SOURCE_DIR}/services/include
	${PROJECT_SOURCE_DIR}/services/collector
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/perfdata
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/events
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/base
)

target_link_libraries(mpi-mpt-collector-monitor-fileio
	cbtf-messages-events
	cbtf-messages-base
	cbtf-services-common
	cbtf-services-monitor
	cbtf-services-offline
	cbtf-services-timer
	cbtf-services-unwind
	cbtf-services-collector-monitor-fileio
	${LibMonitor_SHARED_LIBRARIES}
	${LibUnwind_SHARED_LIBRARIES}
	${Mpt_SHARED_LIBRARIES}
	pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(mpi-mpt-collector-monitor-fileio PROPERTIES PREFIX "")
set_target_properties(mpi-mpt-collector-monitor-fileio PROPERTIES
        COMPILE_DEFINITIONS "${TLS_DEFINES};CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_FILEIO;${Mpt_DEFINES}")

install(TARGETS mpi-mpt-collector-monitor-fileio
	LIBRARY DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors
)

add_library(mpi-mpt-monitor-fileio STATIC
	${COLLECTOR_SOURCES}
	${WRAPPER_SOURCES}
)

target_include_directories(mpi-mpt-monitor-fileio PUBLIC
	${LibMonitor_INCLUDE_DIRS}
	${LibUnwind_INCLUDE_DIRS}
	${Mpt_INCLUDE_DIRS}
	${PROJECT_SOURCE_DIR}/messages/include
	${PROJECT_SOURCE_DIR}/services/include
	${PROJECT_SOURCE_DIR}/services/collector
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/perfdata
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/events
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/base
)

target_link_libraries(mpi-mpt-monitor-fileio
	cbtf-messages-events-static
	cbtf-messages-base-static
	cbtf-services-common-static
	cbtf-services-monitor-static
	cbtf-services-offline-static
	cbtf-services-timer-static
	cbtf-services-unwind-static
	cbtf-services-collector-monitor-fileio-static
	${LibMonitor_STATIC_LIBRARIES}
	${LibUnwind_STATIC_LIBRARIES}
	${Mpt_STATIC_LIBRARIES}
	pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(mpi-mpt-monitor-fileio PROPERTIES
        COMPILE_DEFINITIONS "${TLS_DEFINES};CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_FILEIO;CBTF_STATIC;${Mpt_DEFINES}")

install(TARGETS mpi-mpt-monitor-fileio
	ARCHIVE DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors/static
)

#
# mpit experiment libraries for mpt
#
add_library(mpit-mpt-collector-monitor-fileio MODULE
	${COLLECTOR_SOURCES}
	${WRAPPER_SOURCES}
)

target_include_directories(mpit-mpt-collector-monitor-fileio PUBLIC
	${LibMonitor_INCLUDE_DIRS}
	${LibUnwind_INCLUDE_DIRS}
	${Mpt_INCLUDE_DIRS}
	${PROJECT_SOURCE_DIR}/messages/include
	${PROJECT_SOURCE_DIR}/services/include
	${PROJECT_SOURCE_DIR}/services/collector
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/perfdata
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/events
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/base
)

target_link_libraries(mpit-mpt-collector-monitor-fileio
	cbtf-messages-events
	cbtf-messages-base
	cbtf-services-common
	cbtf-services-monitor
	cbtf-services-offline
	cbtf-services-timer
	cbtf-services-unwind
	cbtf-services-collector-monitor-fileio
	${LibMonitor_SHARED_LIBRARIES}
	${LibUnwind_SHARED_LIBRARIES}
	${Mpt_SHARED_LIBRARIES}
	pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(mpit-mpt-collector-monitor-fileio PROPERTIES PREFIX "")
set_target_properties(mpit-mpt-collector-monitor-fileio PROPERTIES
        COMPILE_DEFINITIONS "${TLS_DEFINES};CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_FILEIO;EXTENDEDTRACE;${Mpt_DEFINES}")

install(TARGETS mpit-mpt-collector-monitor-fileio
	LIBRARY DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors
)

add_library(mpit-mpt-monitor-fileio STATIC
	${COLLECTOR_SOURCES}
	${WRAPPER_SOURCES}
)

target_include_directories(mpit-mpt-monitor-fileio PUBLIC
	${LibMonitor_INCLUDE_DIRS}
	${LibUnwind_INCLUDE_DIRS}
	${Mpt_INCLUDE_DIRS}
	${PROJECT_SOURCE_DIR}/messages/include
	${PROJECT_SOURCE_DIR}/services/include
	${PROJECT_SOURCE_DIR}/services/collector
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/perfdata
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/events
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/base
)

target_link_libraries(mpit-mpt-monitor-fileio
	cbtf-messages-events-static
	cbtf-messages-base-static
	cbtf-services-common-static
	cbtf-services-monitor-static
	cbtf-services-offline-static
	cbtf-services-timer-static
	cbtf-services-unwind-static
	cbtf-services-collector-monitor-fileio-static
	${LibMonitor_STATIC_LIBRARIES}
	${LibUnwind_STATIC_LIBRARIES}
	${Mpt_STATIC_LIBRARIES}
	pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(mpit-mpt-monitor-fileio PROPERTIES
        COMPILE_DEFINITIONS "${TLS_DEFINES};CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_FILEIO;CBTF_STATIC;EXTENDEDTRACE;${Mpt_DEFINES}")

install(TARGETS mpit-mpt-monitor-fileio
	ARCHIVE DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors/static
)
#
# mpip experiment libraries for mpt
#
add_library(mpip-mpt-collector-monitor-fileio MODULE
	${COLLECTOR_SOURCES}
	${WRAPPER_SOURCES}
)

target_include_directories(mpip-mpt-collector-monitor-fileio PUBLIC
	${LibMonitor_INCLUDE_DIRS}
	${LibUnwind_INCLUDE_DIRS}
	${Mpt_INCLUDE_DIRS}
	${PROJECT_SOURCE_DIR}/messages/include
	${PROJECT_SOURCE_DIR}/services/include
	${PROJECT_SOURCE_DIR}/services/collector
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/perfdata
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/events
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/base
)

target_link_libraries(mpip-mpt-collector-monitor-fileio
	cbtf-messages-events
	cbtf-messages-base
	cbtf-services-common
	cbtf-services-monitor
	cbtf-services-offline
	cbtf-services-timer
	cbtf-services-unwind
	cbtf-services-collector-monitor-fileio
	${LibMonitor_SHARED_LIBRARIES}
	${LibUnwind_SHARED_LIBRARIES}
	${Mpt_SHARED_LIBRARIES}
	pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(mpip-mpt-collector-monitor-fileio PROPERTIES PREFIX "")
set_target_properties(mpip-mpt-collector-monitor-fileio PROPERTIES
        COMPILE_DEFINITIONS "${TLS_DEFINES};CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_FILEIO;PROFILE;${Mpt_DEFINES}")

install(TARGETS mpip-mpt-collector-monitor-fileio
	LIBRARY DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors
)

add_library(mpip-mpt-monitor-fileio STATIC
	${COLLECTOR_SOURCES}
	${WRAPPER_SOURCES}
)

target_include_directories(mpip-mpt-monitor-fileio PUBLIC
	${LibMonitor_INCLUDE_DIRS}
	${LibUnwind_INCLUDE_DIRS}
	${Mpt_INCLUDE_DIRS}
	${PROJECT_SOURCE_DIR}/messages/include
	${PROJECT_SOURCE_DIR}/services/include
	${PROJECT_SOURCE_DIR}/services/collector
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/perfdata
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/events
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/base
)

target_link_libraries(mpip-mpt-monitor-fileio
	cbtf-messages-events-static
	cbtf-messages-base-static
	cbtf-services-common-static
	cbtf-services-monitor-static
	cbtf-services-offline-static
	cbtf-services-timer-static
	cbtf-services-unwind-static
	cbtf-services-collector-monitor-fileio-static
	${LibMonitor_STATIC_LIBRARIES}
	${LibUnwind_STATIC_LIBRARIES}
	${Mpt_STATIC_LIBRARIES}
	pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(mpip-mpt-monitor-fileio PROPERTIES
        COMPILE_DEFINITIONS "${TLS_DEFINES};CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_FILEIO;CBTF_STATIC;PROFILE;${Mpt_DEFINES}")

install(TARGETS mpip-mpt-monitor-fileio
	ARCHIVE DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors/static
)

if (MRNET_FOUND)
#
# mpi experiment libraries for mpt
#
add_library(mpi-mpt-collector-monitor-mrnet-mpi MODULE
	${COLLECTOR_SOURCES}
	${WRAPPER_SOURCES}
)

target_include_directories(mpi-mpt-collector-monitor-mrnet-mpi PUBLIC
	${LibMonitor_INCLUDE_DIRS}
	${LibUnwind_INCLUDE_DIRS}
	${Mpt_INCLUDE_DIRS}
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

target_link_libraries(mpi-mpt-collector-monitor-mrnet-mpi
	cbtf-messages-events
	cbtf-messages-base
	cbtf-services-common
	cbtf-services-monitor
	cbtf-services-offline
	cbtf-services-timer
	cbtf-services-data
	cbtf-services-unwind
	cbtf-services-collector-monitor-mrnet-mpi
	${LibMonitor_SHARED_LIBRARIES}
	${LibUnwind_SHARED_LIBRARIES}
	${MRNet_MRNET_LWR_SHARED_LIBRARIES}
	${Mpt_SHARED_LIBRARIES}
	pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(mpi-mvapich2-collector-monitor-mrnet-mpi PROPERTIES PREFIX "")
set_target_properties(mpi-mpt-collector-monitor-mrnet-mpi PROPERTIES
        COMPILE_DEFINITIONS "${TLS_DEFINES};CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_MRNET;CBTF_SERVICE_USE_MRNET_MPI;${Mpt_DEFINES};${MRNet_DEFINES}")

install(TARGETS mpi-mpt-collector-monitor-mrnet-mpi
	LIBRARY DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors
)


add_library(mpi-mpt-monitor-mrnet-mpi STATIC
	${COLLECTOR_SOURCES}
	${WRAPPER_SOURCES}
)

target_include_directories(mpi-mpt-monitor-mrnet-mpi PUBLIC
	${LibMonitor_INCLUDE_DIRS}
	${LibUnwind_INCLUDE_DIRS}
	${Mpt_INCLUDE_DIRS}
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

target_link_libraries(mpi-mpt-monitor-mrnet-mpi
	cbtf-messages-events-static
	cbtf-messages-base-static
	cbtf-services-common-static
	cbtf-services-monitor-static
	cbtf-services-offline-static
	cbtf-services-timer-static
	cbtf-services-data-static
	cbtf-services-unwind-static
	cbtf-services-collector-monitor-mrnet-mpi-static
	${LibMonitor_STATIC_LIBRARIES}
	${LibUnwind_STATIC_LIBRARIES}
	${MRNet_MRNET_LWR_STATIC_LIBRARIES}
	${Mpt_STATIC_LIBRARIES}
	pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(mpi-mpt-monitor-mrnet-mpi PROPERTIES VERSION 1.1.0)
set_target_properties(mpi-mpt-monitor-mrnet-mpi PROPERTIES
        COMPILE_DEFINITIONS "${TLS_DEFINES};CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_MRNET;CBTF_SERVICE_USE_MRNET_MPI;CBTF_STATIC;${Mpt_DEFINES};${MRNet_DEFINES}")

install(TARGETS mpi-mpt-monitor-mrnet-mpi
	ARCHIVE DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors/static
)
#
# mpip experiment libraries for mpt
#
add_library(mpip-mpt-collector-monitor-mrnet-mpi MODULE
	${COLLECTOR_SOURCES}
	${WRAPPER_SOURCES}
)

target_include_directories(mpip-mpt-collector-monitor-mrnet-mpi PUBLIC
	${LibMonitor_INCLUDE_DIRS}
	${LibUnwind_INCLUDE_DIRS}
	${Mpt_INCLUDE_DIRS}
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

target_link_libraries(mpip-mpt-collector-monitor-mrnet-mpi
	cbtf-messages-events
	cbtf-messages-base
	cbtf-services-common
	cbtf-services-monitor
	cbtf-services-offline
	cbtf-services-timer
	cbtf-services-data
	cbtf-services-unwind
	cbtf-services-collector-monitor-mrnet-mpi
	${LibMonitor_SHARED_LIBRARIES}
	${LibUnwind_SHARED_LIBRARIES}
	${MRNet_MRNET_LWR_SHARED_LIBRARIES}
	${Mpt_SHARED_LIBRARIES}
	pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(mpip-mpt-collector-monitor-mrnet-mpi PROPERTIES PREFIX "")
set_target_properties(mpip-mpt-collector-monitor-mrnet-mpi PROPERTIES
        COMPILE_DEFINITIONS "${TLS_DEFINES};CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_MRNET;CBTF_SERVICE_USE_MRNET_MPI;PROFILE;${Mpt_DEFINES};${MRNet_DEFINES}")

install(TARGETS mpip-mpt-collector-monitor-mrnet-mpi
	LIBRARY DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors
)


add_library(mpip-mpt-monitor-mrnet-mpi STATIC
	${COLLECTOR_SOURCES}
	${WRAPPER_SOURCES}
)

target_include_directories(mpip-mpt-monitor-mrnet-mpi PUBLIC
	${LibMonitor_INCLUDE_DIRS}
	${LibUnwind_INCLUDE_DIRS}
	${Mpt_INCLUDE_DIRS}
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

target_link_libraries(mpip-mpt-monitor-mrnet-mpi
	cbtf-messages-events-static
	cbtf-messages-base-static
	cbtf-services-common-static
	cbtf-services-monitor-static
	cbtf-services-offline-static
	cbtf-services-timer-static
	cbtf-services-data-static
	cbtf-services-unwind-static
	cbtf-services-collector-monitor-mrnet-mpi-static
	${LibMonitor_STATIC_LIBRARIES}
	${LibUnwind_STATIC_LIBRARIES}
	${MRNet_MRNET_LWR_STATIC_LIBRARIES}
	${Mpt_STATIC_LIBRARIES}
	pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(mpip-mpt-monitor-mrnet-mpi PROPERTIES VERSION 1.1.0)
set_target_properties(mpip-mpt-monitor-mrnet-mpi PROPERTIES
        COMPILE_DEFINITIONS "${TLS_DEFINES};CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_MRNET;CBTF_SERVICE_USE_MRNET_MPI;CBTF_STATIC;PROFILE;${Mpt_DEFINES};${MRNet_DEFINES}")

install(TARGETS mpip-mpt-monitor-mrnet-mpi
	ARCHIVE DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors/static
)
#
# mpit experiment libraries for mpt
#
add_library(mpit-mpt-collector-monitor-mrnet-mpi MODULE
	${COLLECTOR_SOURCES}
	${WRAPPER_SOURCES}
)

target_include_directories(mpit-mpt-collector-monitor-mrnet-mpi PUBLIC
	${LibMonitor_INCLUDE_DIRS}
	${LibUnwind_INCLUDE_DIRS}
	${Mpt_INCLUDE_DIRS}
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

target_link_libraries(mpit-mpt-collector-monitor-mrnet-mpi
	cbtf-messages-events
	cbtf-messages-base
	cbtf-services-common
	cbtf-services-monitor
	cbtf-services-offline
	cbtf-services-timer
	cbtf-services-data
	cbtf-services-unwind
	cbtf-services-collector-monitor-mrnet-mpi
	${LibMonitor_SHARED_LIBRARIES}
	${LibUnwind_SHARED_LIBRARIES}
	${MRNet_MRNET_LWR_SHARED_LIBRARIES}
	${Mpt_SHARED_LIBRARIES}
	pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(mpit-mpt-collector-monitor-mrnet-mpi PROPERTIES PREFIX "")
set_target_properties(mpit-mpt-collector-monitor-mrnet-mpi PROPERTIES
        COMPILE_DEFINITIONS "${TLS_DEFINES};CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_MRNET;CBTF_SERVICE_USE_MRNET_MPI;EXTENDEDTRACE;${Mpt_DEFINES};${MRNet_DEFINES}")

install(TARGETS mpit-mpt-collector-monitor-mrnet-mpi
	LIBRARY DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors
)


add_library(mpit-mpt-monitor-mrnet-mpi STATIC
	${COLLECTOR_SOURCES}
	${WRAPPER_SOURCES}
)

target_include_directories(mpit-mpt-monitor-mrnet-mpi PUBLIC
	${LibMonitor_INCLUDE_DIRS}
	${LibUnwind_INCLUDE_DIRS}
	${Mpt_INCLUDE_DIRS}
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

target_link_libraries(mpit-mpt-monitor-mrnet-mpi
	cbtf-messages-events-static-static
	cbtf-messages-base-static-static
	cbtf-services-common-static-static
	cbtf-services-monitor-static-static
	cbtf-services-offline-static-static
	cbtf-services-timer-static-static
	cbtf-services-data-static-static
	cbtf-services-unwind-static-static
	cbtf-services-collector-monitor-mrnet-mpi-static-static
	${LibMonitor_STATIC_LIBRARIES}
	${LibUnwind_STATIC_LIBRARIES}
	${MRNet_MRNET_LWR_STATIC_LIBRARIES}
	${Mpt_STATIC_LIBRARIES}
	pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(mpit-mpt-monitor-mrnet-mpi PROPERTIES VERSION 1.1.0)
set_target_properties(mpit-mpt-monitor-mrnet-mpi PROPERTIES
        COMPILE_DEFINITIONS "${TLS_DEFINES};CBTF_SERVICE_USE_OFFLINE;CBTF_SERVICE_USE_MRNET;CBTF_SERVICE_USE_MRNET_MPI;CBTF_STATIC;EXTENDEDTRACE;${Mpt_DEFINES};${MRNet_DEFINES}")

install(TARGETS mpit-mpt-monitor-mrnet-mpi
	ARCHIVE DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors/static
)

endif() # if (MRNET_FOUND)

endif() # if (HAVE_MPT)

endif() # if (LIBMONITOR_FOUND)

set(component_location ${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}/KrellInstitute/Components)
set(cbtflib_location ${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX})

set(collector_name "mpit")
configure_file(../template.xml.in mpit.xml @ONLY)

set(collector_name "mpip")
configure_file(../template.xml.in mpip.xml @ONLY)

set(collector_name "mpi")
configure_file(../template.xml.in mpi.xml @ONLY)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/mpit.xml
        DESTINATION share/KrellInstitute/xml)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/mpip.xml
        DESTINATION share/KrellInstitute/xml)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/mpi.xml
        DESTINATION share/KrellInstitute/xml)
