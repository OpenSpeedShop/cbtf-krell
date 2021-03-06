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

set(SERVICES_OMPT_SOURCES
	callbacks.c
)

set(SERVICES_OMPT50_SOURCES
	callbacks-50.c
)

if (${HAVE_OMPT50_TOOL})
add_library(cbtf-services-ompt SHARED
	${SERVICES_OMPT50_SOURCES}
)
else()
add_library(cbtf-services-ompt SHARED
	${SERVICES_OMPT_SOURCES}
)
endif()

include_directories(
	${Libtirpc_INCLUDE_DIRS}
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/events
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/perfdata
	${PROJECT_SOURCE_DIR}/services/include
	${PROJECT_SOURCE_DIR}/services/collector
	${LibIomp_INCLUDE_DIRS}
	${LibMonitor_INCLUDE_DIRS}
)

target_link_libraries(cbtf-services-ompt
        -Wl,--no-as-needed
	${LibIomp_SHARED_LIBRARIES}
	${LibMonitor_SHARED_LIBRARIES}
	${CMAKE_DL_LIBS}
)

set_target_properties(cbtf-services-ompt PROPERTIES VERSION 1.1.0)
set_target_properties(cbtf-services-ompt PROPERTIES COMPILE_DEFINITIONS "${TLS_DEFINES};${LibIomp_DEFINES}")
set_target_properties(cbtf-services-ompt PROPERTIES POSITION_INDEPENDENT_CODE ON)


install(TARGETS cbtf-services-ompt
	LIBRARY DESTINATION lib${LIB_SUFFIX}
)
