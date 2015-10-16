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

set(SERVICES_MRNET_SOURCES
	MRNet_Send.c
	MRNet_Senders.c
)

add_library(cbtf-services-mrnet SHARED
	${SERVICES_MRNET_SOURCES}
)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${LTDL_INCLUDE_DIR}
	${PROJECT_SOURCE_DIR}/services/include
	${PROJECT_SOURCE_DIR}/messages/include
	${CBTF_INCLUDE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/events
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/perfdata
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/base
	${CMAKE_INSTALL_PREFIX}/include
	${MRNet_INCLUDE_DIRS}
)

target_link_libraries(cbtf-services-mrnet
	cbtf-messages-events
	cbtf-messages-base
	${MRNet_LWR_SHARED_LIBRARIES}
	${LTDL_LIBRARY_SHARED}
	pthread
	${CMAKE_DL_LIBS}
)

set_target_properties(cbtf-services-mrnet PROPERTIES VERSION 1.1.0)
set_target_properties(cbtf-services-mrnet PROPERTIES 
        COMPILE_DEFINITIONS "${TLS_DEFINES};${MRNet_DEFINES}")

install(TARGETS cbtf-services-mrnet
	LIBRARY DESTINATION lib${LIB_SUFFIX}
)

add_library(cbtf-services-mrnet-static STATIC
	${SERVICES_MRNET_SOURCES}
)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${LTDL_INCLUDE_DIR}
	${PROJECT_SOURCE_DIR}/services/include
	${PROJECT_SOURCE_DIR}/messages/include
	${CBTF_INCLUDE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/events
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/perfdata
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/base
	${CMAKE_INSTALL_PREFIX}/include
	${MRNet_INCLUDE_DIRS}
)

target_link_libraries(cbtf-services-mrnet-static
	cbtf-messages-events
	cbtf-messages-base
	${MRNet_LWR_SHARED_LIBRARIES}
	${LTDL_LIBRARY_SHARED}
	pthread
	${CMAKE_DL_LIBS}
)

set_target_properties(cbtf-services-mrnet-static PROPERTIES VERSION 1.1.0)
set_target_properties(cbtf-services-mrnet-static PROPERTIES 
        COMPILE_DEFINITIONS "${TLS_DEFINES};${MRNet_DEFINES}")

install(TARGETS cbtf-services-mrnet-static
	ARCHIVE DESTINATION lib${LIB_SUFFIX}
)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")

