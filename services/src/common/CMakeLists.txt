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

add_definitions( -D_GNU_SOURCE )

set(SERVICES_COMMON_SOURCES
	GetPCFromContext.c
	SetPCInContext.c
	GetAddressOfFunction.c
	GetTime.c
	GetExecutablePath.c
	TLS.c
)

include_directories(
	${Libtirpc_INCLUDE_DIRS}
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${PROJECT_SOURCE_DIR}/services/include
)

add_library(cbtf-services-common SHARED
	${SERVICES_COMMON_SOURCES}
)

target_link_libraries(cbtf-services-common
        -Wl,--no-as-needed
	${CMAKE_DL_LIBS}
)

set_target_properties(cbtf-services-common PROPERTIES VERSION 1.1.0)
set_target_properties(cbtf-services-common PROPERTIES POSITION_INDEPENDENT_CODE ON)

install(TARGETS cbtf-services-common
	LIBRARY DESTINATION lib${LIB_SUFFIX}
)

# Install static version

add_library(cbtf-services-common-static STATIC
	${SERVICES_COMMON_SOURCES}
)

target_link_libraries(cbtf-services-common-static
        -Wl,--no-as-needed
	${CMAKE_DL_LIBS}
)

set_target_properties(cbtf-services-common-static PROPERTIES VERSION 1.1.0)
set_target_properties(cbtf-services-common-static PROPERTIES OUTPUT_NAME cbtf-services-common)
set_target_properties(cbtf-services-common-static PROPERTIES POSITION_INDEPENDENT_CODE ON)

install(TARGETS cbtf-services-common-static
	ARCHIVE DESTINATION lib${LIB_SUFFIX}
)

