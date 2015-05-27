################################################################################
# Copyright (c) 2015 Krell Institute. All Rights Reserved.
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


set(CBTF_MESSAGES_COLLECTOR_SOURCES
        ${CMAKE_CURRENT_BINARY_DIR}/Collector.h
        ${CMAKE_CURRENT_BINARY_DIR}/Collector.c
	Collector.x
)

set(CBTF_MESSAGES_CONVERTERS_COLLECTOR_SOURCES
	Converters.cpp
)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/Collector.h ${CMAKE_CURRENT_BINARY_DIR}/Collector.c
    COMMAND rpcgen -h -o ${CMAKE_CURRENT_BINARY_DIR}/Collector.h ${CMAKE_CURRENT_BINARY_DIR}/Collector.x
    COMMAND rpcgen -c -o ${CMAKE_CURRENT_BINARY_DIR}/Collector.c ${CMAKE_CURRENT_BINARY_DIR}/Collector.x
    COMMAND cmake -E copy ${CMAKE_CURRENT_BINARY_DIR}/Collector.h ${CMAKE_CURRENT_BINARY_DIR}/KrellInstitute/Messages/Collector.h
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/Collector.x
)

add_library(cbtf-messages-converters-collector SHARED
        ${CBTF_MESSAGES_CONVERTERS_COLLECTOR_SOURCES}
)
  
target_include_directories(cbtf-messages-converters-collector PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${MRNet_INCLUDE_DIRS}
	${Boost_INCLUDE_DIRS}
	${CBTF_INCLUDE_DIRS}
	${LTDL_INCLUDE_DIR}
)

target_link_libraries(cbtf-messages-converters-collector
	${MRNet_LIBRARIES}
	pthread
	${CMAKE_DL_LIBS}
	${LTDL_LIBRARY_SHARED}
)

set_target_properties(cbtf-messages-converters-collector PROPERTIES VERSION 1.1.0)
set_target_properties(cbtf-messages-converters-collector PROPERTIES
	COMPILE_DEFINITIONS "${MRNet_DEFINES}")


install(TARGETS cbtf-messages-converters-collector
	LIBRARY DESTINATION lib${LIB_SUFFIX}
)

# Install static versions

add_library(cbtf-messages-converters-collector-static STATIC
        ${CBTF_MESSAGES_CONVERTERS_COLLECTOR_SOURCES}
)
  
target_include_directories(cbtf-messages-converters-collector-static PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${MRNet_INCLUDE_DIRS}
	${Boost_INCLUDE_DIRS}
	${CBTF_INCLUDE_DIRS}
	${LTDL_INCLUDE_DIR}
)

target_link_libraries(cbtf-messages-converters-collector-static
	${MRNet_LIBRARIES}
	pthread
	${CMAKE_DL_LIBS}
	${LTDL_LIBRARY_SHARED}
)

set_target_properties(cbtf-messages-converters-collector-static PROPERTIES VERSION 1.1.0)
set_target_properties(cbtf-messages-converters-collector-static PROPERTIES
	COMPILE_DEFINITIONS "${MRNet_DEFINES}")

install(TARGETS cbtf-messages-converters-collector-static
	ARCHIVE DESTINATION lib${LIB_SUFFIX}
)

add_library(cbtf-messages-collector SHARED
	${CBTF_MESSAGES_COLLECTOR_SOURCES}
)

target_include_directories(cbtf-messages-collector PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${LTDL_INCLUDE_DIR}
)

target_link_libraries(cbtf-messages-collector
	${LTDL_LIBRARY_SHARED}
	${CMAKE_DL_LIBS}
)

set_target_properties(cbtf-messages-collector PROPERTIES VERSION 1.1.0)

install(TARGETS cbtf-messages-collector
	LIBRARY DESTINATION lib${LIB_SUFFIX}
)


# Install static version

add_library(cbtf-messages-collector-static STATIC
	${CBTF_MESSAGES_COLLECTOR_SOURCES}
)

target_include_directories(cbtf-messages-collector-static PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${LTDL_INCLUDE_DIR}
)

target_link_libraries(cbtf-messages-collector-static
	${LTDL_LIBRARY_SHARED}
	${CMAKE_DL_LIBS}
)

set_target_properties(cbtf-messages-collector-static PROPERTIES VERSION 1.1.0)

install(TARGETS cbtf-messages-collector-static
	ARCHIVE DESTINATION lib${LIB_SUFFIX}
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/Collector.h
        DESTINATION include/KrellInstitute/Messages
        PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
)

# rpcgen processing done in CMAKE_CURRENT_BINARY_DIR..
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Collector.x DESTINATION .)
