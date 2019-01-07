################################################################################
# Copyright (c) 2014-2019 Krell Institute. All Rights Reserved.
# Copyright (c) 2017 Argo Navis Technologies. All Rights Reserved.
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

if(ENABLE_CBTF_MRNET_PLAYBACK)
    add_definitions(-DENABLE_CBTF_MRNET_PLAYBACK)
endif()

set(SERVICES_MRNET_SOURCES
	MRNet_Send.c
    playback.h playback.c
)

add_library(cbtf-services-mrnet SHARED
	${SERVICES_MRNET_SOURCES}
)

include_directories(
	${Libtirpc_INCLUDE_DIRS}
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${PROJECT_SOURCE_DIR}/services/include
	${PROJECT_SOURCE_DIR}/messages/include
	${CBTF_INCLUDE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/events
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/perfdata
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/base
	${LibMonitor_INCLUDE_DIRS}
	${MRNet_LW_INCLUDE_DIRS}
)

target_link_libraries(cbtf-services-mrnet
        -Wl,--no-as-needed
	cbtf-messages-events
	cbtf-messages-base
	${MRNet_LWR_SHARED_LIBRARIES}
	pthread
	${CMAKE_DL_LIBS}
)

set_target_properties(cbtf-services-mrnet PROPERTIES VERSION 1.1.0)
set_target_properties(cbtf-services-mrnet PROPERTIES 
        COMPILE_DEFINITIONS "${TLS_DEFINES};${MRNet_DEFINES}")
set_target_properties(cbtf-services-mrnet PROPERTIES POSITION_INDEPENDENT_CODE ON)

install(TARGETS cbtf-services-mrnet
	LIBRARY DESTINATION lib${LIB_SUFFIX}
)

add_library(cbtf-services-mrnet-static STATIC
	${SERVICES_MRNET_SOURCES}
)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${PROJECT_SOURCE_DIR}/services/include
	${PROJECT_SOURCE_DIR}/messages/include
	${CBTF_INCLUDE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/events
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/perfdata
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/base
	${LibMonitor_INCLUDE_DIRS}
	${MRNet_LW_INCLUDE_DIRS}
)

target_link_libraries(cbtf-services-mrnet-static
        -Wl,--no-as-needed
	cbtf-messages-events
	cbtf-messages-base
	${MRNet_LWR_SHARED_LIBRARIES}
	pthread
	${CMAKE_DL_LIBS}
)

set_target_properties(cbtf-services-mrnet-static PROPERTIES VERSION 1.1.0)
set_target_properties(cbtf-services-mrnet-static PROPERTIES OUTPUT_NAME cbtf-services-mrnet)
set_target_properties(cbtf-services-mrnet-static PROPERTIES 
        COMPILE_DEFINITIONS "${TLS_DEFINES};${MRNet_DEFINES}")
set_target_properties(cbtf-services-mrnet-static PROPERTIES POSITION_INDEPENDENT_CODE ON)

install(TARGETS cbtf-services-mrnet-static
	ARCHIVE DESTINATION lib${LIB_SUFFIX}
)
