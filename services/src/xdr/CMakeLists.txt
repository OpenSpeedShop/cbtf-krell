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

set(SERVICES_XDR_SOURCES
        Send.c
)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/events
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/perfdata
	${LTDL_INCLUDE_DIR}
	${PROJECT_SOURCE_DIR}/services/include
	${PROJECT_SOURCE_DIR}/services/src/data
)

add_library(cbtf-services-send SHARED
	${SERVICES_XDR_SOURCES}
)

target_link_libraries(cbtf-services-send
        -Wl,--no-as-needed
	${LTDL_LIBRARY_SHARED}
	cbtf-messages-perfdata
	${CMAKE_DL_LIBS}
)

set_target_properties(cbtf-services-send PROPERTIES VERSION 1.1.0)

install(TARGETS cbtf-services-send
	LIBRARY DESTINATION lib${LIB_SUFFIX}
)

# Install static version

add_library(cbtf-services-send-static STATIC
	${SERVICES_XDR_SOURCES}
)

target_link_libraries(cbtf-services-send-static
        -Wl,--no-as-needed
	${LTDL_LIBRARY_SHARED}
	cbtf-messages-perfdata
	${CMAKE_DL_LIBS}
)

set_target_properties(cbtf-services-send-static PROPERTIES VERSION 1.1.0)
set_target_properties(cbtf-services-send-static PROPERTIES OUTPUT_NAME cbtf-services-send)

install(TARGETS cbtf-services-send-static
	ARCHIVE DESTINATION lib${LIB_SUFFIX}
)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")


