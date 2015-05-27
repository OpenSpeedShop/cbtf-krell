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

set(CBTF_MESSAGES_THREAD_SOURCES
        ${CMAKE_CURRENT_BINARY_DIR}/ThreadControl.h
        ${CMAKE_CURRENT_BINARY_DIR}/ThreadControl.c
	ThreadControl.x
)

set(CBTF_MESSAGES_CONVERTERS_THREAD_SOURCES
        ${CMAKE_CURRENT_BINARY_DIR}/ThreadControl.h
	Converters.cpp
)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/ThreadControl.h ${CMAKE_CURRENT_BINARY_DIR}/ThreadControl.c
    COMMAND rpcgen -h -o ${CMAKE_CURRENT_BINARY_DIR}/ThreadControl.h ${CMAKE_CURRENT_BINARY_DIR}/ThreadControl.x
    COMMAND rpcgen -c -o ${CMAKE_CURRENT_BINARY_DIR}/ThreadControl.c ${CMAKE_CURRENT_BINARY_DIR}/ThreadControl.x
    COMMAND cmake -E copy ${CMAKE_CURRENT_BINARY_DIR}/ThreadControl.h ${CMAKE_CURRENT_BINARY_DIR}/KrellInstitute/Messages/ThreadControl.h
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/ThreadControl.x
)

add_library(cbtf-messages-converters-thread SHARED
        ${CBTF_MESSAGES_CONVERTERS_THREAD_SOURCES}
)
  
target_include_directories(cbtf-messages-converters-thread PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../base
	${MRNet_INCLUDE_DIRS}
	${Boost_INCLUDE_DIRS}
	${CBTF_INCLUDE_DIRS}
	${LTDL_INCLUDE_DIR}
)

target_link_libraries(cbtf-messages-converters-thread
	${MRNet_LIBRARIES}
	pthread
	${LTDL_LIBRARY_SHARED}
	${CMAKE_DL_LIBS}
)

set_target_properties(cbtf-messages-converters-thread PROPERTIES VERSION 1.1.0)
set_target_properties(cbtf-messages-converters-thread PROPERTIES 
        COMPILE_DEFINITIONS "${MRNet_DEFINES}")

install(TARGETS cbtf-messages-converters-thread
	LIBRARY DESTINATION lib${LIB_SUFFIX}
)

# Install static version

add_library(cbtf-messages-converters-thread-static STATIC
        ${CBTF_MESSAGES_CONVERTERS_THREAD_SOURCES}
    )

target_include_directories(cbtf-messages-converters-thread-static PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../base
	${MRNet_INCLUDE_DIRS}
	${Boost_INCLUDE_DIRS}
	${CBTF_INCLUDE_DIRS}
	${LTDL_INCLUDE_DIR}
)

target_link_libraries(cbtf-messages-converters-thread-static
	${MRNet_LIBRARIES}
	pthread
	${LTDL_LIBRARY_SHARED}
	${CMAKE_DL_LIBS}
)

set_target_properties(cbtf-messages-converters-thread-static PROPERTIES VERSION 1.1.0)
set_target_properties(cbtf-messages-converters-thread-static PROPERTIES 
        COMPILE_DEFINITIONS "${MRNet_DEFINES}")

install(TARGETS cbtf-messages-converters-thread-static
	ARCHIVE DESTINATION lib${LIB_SUFFIX}
)


add_library(cbtf-messages-thread SHARED
	${CBTF_MESSAGES_THREAD_SOURCES}
)

target_include_directories(cbtf-messages-thread PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../base
	${LTDL_INCLUDE_DIR}
)

target_link_libraries(cbtf-messages-thread
	cbtf-messages-base
	${LTDL_LIBRARY_SHARED}
	${CMAKE_DL_LIBS}
)

set_target_properties(cbtf-messages-thread PROPERTIES VERSION 1.1.0)

install(TARGETS cbtf-messages-thread
	LIBRARY DESTINATION lib${LIB_SUFFIX}
)

# Install static version

add_library(cbtf-messages-thread-static STATIC
	${CBTF_MESSAGES_THREAD_SOURCES}
)

target_include_directories(cbtf-messages-thread-static PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../base
	${LTDL_INCLUDE_DIR}
)

target_link_libraries(cbtf-messages-thread-static
	cbtf-messages-base
	${LTDL_LIBRARY_SHARED}
	${CMAKE_DL_LIBS}
)

set_target_properties(cbtf-messages-thread-static PROPERTIES VERSION 1.1.0)

install(TARGETS cbtf-messages-thread-static
	ARCHIVE DESTINATION lib${LIB_SUFFIX}
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/ThreadControl.h
        DESTINATION include/KrellInstitute/Messages
        PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
)

# rpcgen processing done in CMAKE_CURRENT_BINARY_DIR..
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/ThreadControl.x DESTINATION .)
