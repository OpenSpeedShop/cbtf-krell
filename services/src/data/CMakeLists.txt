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

set(SERVICES_DATA_SOURCES
	InitializeDataHeader.c
	InitializeEventHeader.c
	UpdateHWCPCData.c
	UpdatePCData.c
	UpdateStackTraceBuffer.c
)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${LTDL_INCLUDE_DIR}
	${PROJECT_SOURCE_DIR}/services/include
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/perfdata
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/events
)

add_library(cbtf-services-data SHARED
	${SERVICES_DATA_SOURCES}
)

target_link_libraries(cbtf-services-data
        -Wl,--no-as-needed
	cbtf-messages-base
	cbtf-messages-events
	cbtf-messages-perfdata
	${LTDL_LIBRARY_SHARED}
	${CMAKE_DL_LIBS}
)

set_target_properties(cbtf-services-data PROPERTIES VERSION 1.1.0)

install(TARGETS cbtf-services-data
        LIBRARY DESTINATION lib${LIB_SUFFIX}
)

# Install static version

add_library(cbtf-services-data-static STATIC
	${SERVICES_DATA_SOURCES}
)

target_link_libraries(cbtf-services-data-static
        -Wl,--no-as-needed
	cbtf-messages-base
	cbtf-messages-events
	cbtf-messages-perfdata
	${LTDL_LIBRARY_SHARED}
	${CMAKE_DL_LIBS}
)

set_target_properties(cbtf-services-data-static PROPERTIES VERSION 1.1.0)
set_target_properties(cbtf-services-data-static PROPERTIES OUTPUT_NAME cbtf-services-data)

install(TARGETS cbtf-services-data-static
        ARCHIVE DESTINATION lib${LIB_SUFFIX}
)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")

