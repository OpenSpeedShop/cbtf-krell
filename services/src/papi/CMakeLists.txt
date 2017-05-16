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

set(SERVICES_PAPI_SOURCES
	PapiAPI.c
)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${PROJECT_SOURCE_DIR}/services/include
    ${Papi_INCLUDE_DIRS}
)

add_library(cbtf-services-papi SHARED
	${SERVICES_PAPI_SOURCES}
)

target_link_libraries(cbtf-services-papi
    -Wl,--no-as-needed
    ${Papi_SHARED_LIBRARIES}
    ${CMAKE_DL_LIBS}
)

set_target_properties(cbtf-services-papi PROPERTIES VERSION 1.1.0)
set_target_properties(cbtf-services-papi PROPERTIES POSITION_INDEPENDENT_CODE ON)

install(TARGETS cbtf-services-papi
    LIBRARY DESTINATION lib${LIB_SUFFIX}
)

# Install static version

add_library(cbtf-services-papi-static STATIC
	${SERVICES_PAPI_SOURCES}
)

target_link_libraries(cbtf-services-papi-static
    -Wl,--no-as-needed
    ${Papi_SHARED_LIBRARIES}
    ${CMAKE_DL_LIBS}
)

set_target_properties(cbtf-services-papi-static PROPERTIES VERSION 1.1.0)
set_target_properties(cbtf-services-papi-static PROPERTIES OUTPUT_NAME cbtf-services-papi)
set_target_properties(cbtf-services-papi-static PROPERTIES POSITION_INDEPENDENT_CODE ON)

install(TARGETS cbtf-services-papi-static
    ARCHIVE DESTINATION lib${LIB_SUFFIX}
)
