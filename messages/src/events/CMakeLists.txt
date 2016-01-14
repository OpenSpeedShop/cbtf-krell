################################################################################
# Copyright (c) 2015-2016 Krell Institute. All Rights Reserved.
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

set(CBTF_MESSAGES_EVENTS_SOURCES
        ${CMAKE_CURRENT_BINARY_DIR}/EventHeader.h
        ${CMAKE_CURRENT_BINARY_DIR}/EventHeader.c
        ${CMAKE_CURRENT_BINARY_DIR}/OfflineEvents.h
        ${CMAKE_CURRENT_BINARY_DIR}/OfflineEvents.c
        ${CMAKE_CURRENT_BINARY_DIR}/LinkedObjectEvents.h
        ${CMAKE_CURRENT_BINARY_DIR}/LinkedObjectEvents.c
        ${CMAKE_CURRENT_BINARY_DIR}/ThreadEvents.h
        ${CMAKE_CURRENT_BINARY_DIR}/ThreadEvents.c
	EventHeader.x
	OfflineEvents.x
	LinkedObjectEvents.x
	ThreadEvents.x

    )
set(CBTF_MESSAGES_CONVERTERS_EVENTS_SOURCES
        ${CMAKE_CURRENT_BINARY_DIR}/EventHeader.h
        ${CMAKE_CURRENT_BINARY_DIR}/EventHeader.c
        ${CMAKE_CURRENT_BINARY_DIR}/OfflineEvents.h
        ${CMAKE_CURRENT_BINARY_DIR}/OfflineEvents.c
        ${CMAKE_CURRENT_BINARY_DIR}/LinkedObjectEvents.h
        ${CMAKE_CURRENT_BINARY_DIR}/LinkedObjectEvents.c
        ${CMAKE_CURRENT_BINARY_DIR}/ThreadEvents.h
        ${CMAKE_CURRENT_BINARY_DIR}/ThreadEvents.c
	Converters.cpp
    )

add_custom_command(
    OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/EventHeader.h
        ${CMAKE_CURRENT_BINARY_DIR}/EventHeader.c
    COMMAND rpcgen -h -o
        ${CMAKE_CURRENT_BINARY_DIR}/EventHeader.h
        ${CMAKE_CURRENT_BINARY_DIR}/EventHeader.x
    COMMAND rpcgen -c -o
        ${CMAKE_CURRENT_BINARY_DIR}/EventHeader.c
        ${CMAKE_CURRENT_BINARY_DIR}/EventHeader.x
    COMMAND cmake -E copy ${CMAKE_CURRENT_BINARY_DIR}/EventHeader.h ${CMAKE_CURRENT_BINARY_DIR}/KrellInstitute/Messages/EventHeader.h
    DEPENDS
        ${CMAKE_CURRENT_BINARY_DIR}/EventHeader.x
    )

add_custom_command(
    OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/OfflineEvents.h
        ${CMAKE_CURRENT_BINARY_DIR}/OfflineEvents.c
    COMMAND rpcgen -h -o
        ${CMAKE_CURRENT_BINARY_DIR}/OfflineEvents.h
        ${CMAKE_CURRENT_BINARY_DIR}/OfflineEvents.x
    COMMAND rpcgen -c -o
        ${CMAKE_CURRENT_BINARY_DIR}/OfflineEvents.c
        ${CMAKE_CURRENT_BINARY_DIR}/OfflineEvents.x
    COMMAND cmake -E copy ${CMAKE_CURRENT_BINARY_DIR}/OfflineEvents.h ${CMAKE_CURRENT_BINARY_DIR}/KrellInstitute/Messages/OfflineEvents.h
    DEPENDS
        ${CMAKE_CURRENT_BINARY_DIR}/OfflineEvents.x
    )

add_custom_command(
    OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/LinkedObjectEvents.h
        ${CMAKE_CURRENT_BINARY_DIR}/LinkedObjectEvents.c
    COMMAND rpcgen -h -o
        ${CMAKE_CURRENT_BINARY_DIR}/LinkedObjectEvents.h
        ${CMAKE_CURRENT_BINARY_DIR}/LinkedObjectEvents.x
    COMMAND rpcgen -c -o
        ${CMAKE_CURRENT_BINARY_DIR}/LinkedObjectEvents.c
        ${CMAKE_CURRENT_BINARY_DIR}/LinkedObjectEvents.x
    COMMAND cmake -E copy ${CMAKE_CURRENT_BINARY_DIR}/LinkedObjectEvents.h ${CMAKE_CURRENT_BINARY_DIR}/KrellInstitute/Messages/LinkedObjectEvents.h
    DEPENDS
        ${CMAKE_CURRENT_BINARY_DIR}/LinkedObjectEvents.x
    )

add_custom_command(
    OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/ThreadEvents.h
        ${CMAKE_CURRENT_BINARY_DIR}/ThreadEvents.c
    COMMAND rpcgen -h -o
        ${CMAKE_CURRENT_BINARY_DIR}/ThreadEvents.h
        ${CMAKE_CURRENT_BINARY_DIR}/ThreadEvents.x
    COMMAND rpcgen -c -o
        ${CMAKE_CURRENT_BINARY_DIR}/ThreadEvents.c
        ${CMAKE_CURRENT_BINARY_DIR}/ThreadEvents.x
    COMMAND cmake -E copy ${CMAKE_CURRENT_BINARY_DIR}/ThreadEvents.h ${CMAKE_CURRENT_BINARY_DIR}/KrellInstitute/Messages/ThreadEvents.h
    DEPENDS
        ${CMAKE_CURRENT_BINARY_DIR}/ThreadEvents.x
    )

# Do not build the converters if we are targeting the compute node build for Intel MIC
# which is a platform that runs the CBTF filters on a host processor not the
# compute node processors.
if ((RUNTIME_ONLY AND CBTF_FILTERS_RUN_ON MATCHES "native") OR NOT RUNTIME_ONLY)
    add_library(cbtf-messages-converters-events SHARED
        ${CBTF_MESSAGES_CONVERTERS_EVENTS_SOURCES}
    )
 
    target_include_directories(cbtf-messages-converters-events PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}../base
	${MRNet_INCLUDE_DIRS}
	${Boost_INCLUDE_DIRS}
	${CBTF_INCLUDE_DIRS}
	${LTDL_INCLUDE_DIR}
    )

    target_link_libraries(cbtf-messages-converters-events
        -Wl,--no-as-needed
	cbtf-messages-base
	${MRNet_LIBRARIES}
	pthread
	${LTDL_LIBRARY_SHARED}
	${CMAKE_DL_LIBS}
    )

    set_target_properties(cbtf-messages-converters-events PROPERTIES VERSION 1.1.0)
    set_target_properties(cbtf-messages-converters-events PROPERTIES 
        COMPILE_DEFINITIONS "${MRNet_DEFINES}")

    install(TARGETS cbtf-messages-converters-events
	LIBRARY DESTINATION lib${LIB_SUFFIX}
    )

# Install static version

    add_library(cbtf-messages-converters-events-static STATIC
        ${CBTF_MESSAGES_CONVERTERS_EVENTS_SOURCES}
    )
 
    target_include_directories(cbtf-messages-converters-events-static PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}../base
	${MRNet_INCLUDE_DIRS}
	${Boost_INCLUDE_DIRS}
	${CBTF_INCLUDE_DIRS}
	${LTDL_INCLUDE_DIR}
    )

    target_link_libraries(cbtf-messages-converters-events-static
        -Wl,--no-as-needed
	cbtf-messages-base
	${MRNet_LIBRARIES}
	pthread
	${LTDL_LIBRARY_SHARED}
	${CMAKE_DL_LIBS}
    )

    set_target_properties(cbtf-messages-converters-events-static PROPERTIES VERSION 1.1.0)
    set_target_properties(cbtf-messages-converters-events-static PROPERTIES 
        COMPILE_DEFINITIONS "${MRNet_DEFINES}")

    install(TARGETS cbtf-messages-converters-events-static
	ARCHIVE DESTINATION lib${LIB_SUFFIX}
    )
endif()

add_library(cbtf-messages-events SHARED
	${CBTF_MESSAGES_EVENTS_SOURCES}
)

target_include_directories(cbtf-messages-events PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../base
	${CMAKE_CURRENT_BINARY_DIR}
	${LTDL_INCLUDE_DIR}
)

target_link_libraries(cbtf-messages-events
        -Wl,--no-as-needed
	cbtf-messages-base
	${LTDL_LIBRARY_SHARED}
	${CMAKE_DL_LIBS}
)

set_target_properties(cbtf-messages-events PROPERTIES VERSION 1.1.0)
set_target_properties(cbtf-messages-events PROPERTIES 
        COMPILE_DEFINITIONS "${MRNet_DEFINES}")

install(TARGETS cbtf-messages-events 
        LIBRARY DESTINATION lib${LIB_SUFFIX}
)

# Install static version 

add_library(cbtf-messages-events-static STATIC
        ${CBTF_MESSAGES_EVENTS_SOURCES}
)

target_include_directories(cbtf-messages-events-static PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../base
	${CMAKE_CURRENT_BINARY_DIR}
	${LTDL_INCLUDE_DIR}
)

target_link_libraries(cbtf-messages-events-static
       -Wl,--no-as-needed
        ${LTDL_LIBRARY_SHARED}
        ${CMAKE_DL_LIBS}
)

set_target_properties(cbtf-messages-events-static PROPERTIES VERSION 1.1.0)
set_target_properties(cbtf-messages-events-static PROPERTIES 
        COMPILE_DEFINITIONS "${MRNet_DEFINES}")

install(TARGETS cbtf-messages-events-static
        ARCHIVE DESTINATION lib${LIB_SUFFIX}
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/EventHeader.h 
              ${CMAKE_CURRENT_BINARY_DIR}/OfflineEvents.h 
              ${CMAKE_CURRENT_BINARY_DIR}/LinkedObjectEvents.h 
              ${CMAKE_CURRENT_BINARY_DIR}/ThreadEvents.h 
        DESTINATION include/KrellInstitute/Messages
        PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)

# rpcgen processing done in CMAKE_CURRENT_BINARY_DIR..
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/EventHeader.x DESTINATION .)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/OfflineEvents.x DESTINATION .)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/LinkedObjectEvents.x DESTINATION .)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/ThreadEvents.x DESTINATION .)

