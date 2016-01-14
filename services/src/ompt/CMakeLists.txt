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

set(SERVICES_OMPT_SOURCES
	callbacks.c
)

add_library(cbtf-services-ompt SHARED
	${SERVICES_OMPT_SOURCES}
)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/events
	${LTDL_INCLUDE_DIR}
	${PROJECT_SOURCE_DIR}/services/include
	${CMAKE_INSTALL_PREFIX}/include
	${LibIomp_INCLUDE_DIRS}
)

target_link_libraries(cbtf-services-ompt
        -Wl,--no-as-needed
	${LibIomp_SHARED_LIBRARIES}
	${LTDL_LIBRARY_SHARED}
	${CMAKE_DL_LIBS}
)

set_target_properties(cbtf-services-ompt PROPERTIES VERSION 1.1.0)
set_target_properties(cbtf-services-ompt PROPERTIES COMPILE_DEFINITIONS "${TLS_DEFINES}")


install(TARGETS cbtf-services-ompt
	LIBRARY DESTINATION lib${LIB_SUFFIX}
)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")