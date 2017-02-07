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

include(CheckIncludeFile)

set(SERVICES_MONITOR_SOURCES
	Monitor.c
)

if (CMAKE_HAVE_PTHREAD_CREATE)
   add_definitions(
        -DHAVE_TARGET_POSIX_THREADS
        )
endif()
if (HAVE_DLOPEN)
   add_definitions(
        -DHAVE_TARGET_DLOPEN
        )
endif()
if (HAVE_FORK)
   add_definitions(
        -DHAVE_TARGET_FORK
        )
endif()

add_library(cbtf-services-monitor SHARED
	${SERVICES_MONITOR_SOURCES}
)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/events
	${LTDL_INCLUDE_DIR}
	${PROJECT_SOURCE_DIR}/services/include
	${LibMonitor_INCLUDE_DIRS}
)

target_link_libraries(cbtf-services-monitor
        -Wl,--no-as-needed
	${LibMonitor_SHARED_LIBRARIES}
	${LTDL_LIBRARY_SHARED}
	${CMAKE_DL_LIBS}
)

set_target_properties(cbtf-services-monitor PROPERTIES VERSION 1.1.0)
set_target_properties(cbtf-services-monitor PROPERTIES COMPILE_DEFINITIONS "${TLS_DEFINES}")


install(TARGETS cbtf-services-monitor
	LIBRARY DESTINATION lib${LIB_SUFFIX}
)

add_library(cbtf-services-monitor-static STATIC
	${SERVICES_MONITOR_SOURCES}
)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/events
	${LTDL_INCLUDE_DIR}
	${PROJECT_SOURCE_DIR}/services/include
	${LibMonitor_INCLUDE_DIRS}
)

target_link_libraries(cbtf-services-monitor-static
        -Wl,--no-as-needed
	${LibMonitor_SHARED_LIBRARIES}
	${LTDL_LIBRARY_SHARED}
	${CMAKE_DL_LIBS}
)

set_target_properties(cbtf-services-monitor-static PROPERTIES VERSION 1.1.0)
set_target_properties(cbtf-services-monitor-static PROPERTIES OUTPUT_NAME cbtf-services-monitor)
set_target_properties(cbtf-services-monitor-static PROPERTIES COMPILE_DEFINITIONS "${TLS_DEFINES}")

install(TARGETS cbtf-services-monitor-static
	ARCHIVE DESTINATION lib${LIB_SUFFIX}
)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")

# Create Monitor.c.o and install as cbtf_monitor.o into Collector directory

add_custom_command(
    POST_BUILD
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/cbtf_monitor.o
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}/KrellInstitute
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}/KrellInstitute/Collectors
    COMMAND ar -x ${CMAKE_CURRENT_BINARY_DIR}/libcbtf-services-monitor.a
    COMMAND cp Monitor.c.o cbtf_monitor.o
    DEPENDS cbtf-services-monitor-static
    COMMENT "Copying cbtf_monitor.o to the install directory for static linking via cbtflink..."
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)
add_custom_target(runcbtfmonitor ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/cbtf_monitor.o)
install (FILES ${CMAKE_CURRENT_BINARY_DIR}/cbtf_monitor.o
        DESTINATION ${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}/KrellInstitute/Collectors)


