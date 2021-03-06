################################################################################
# Copyright (c) 2015-2019 Krell Institute. All Rights Reserved.
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

# Adding the rpcgen .h files to converters sources to ensure
# they are created.
set(CBTF_MESSAGES_CONVERTERS_THREAD_SOURCES
    ${CMAKE_CURRENT_BINARY_DIR}/ThreadControl.h
    Converters.cpp
)

INCLUDE_DIRECTORIES(SYSTEM ${Libtirpc_INCLUDE_DIRS})

add_custom_command(
    OUTPUT
	${CMAKE_CURRENT_BINARY_DIR}/ThreadControl.h
	${CMAKE_CURRENT_BINARY_DIR}/ThreadControl.c
    COMMAND rpcgen -h -o
	${CMAKE_CURRENT_BINARY_DIR}/ThreadControl.h
	${CMAKE_CURRENT_BINARY_DIR}/ThreadControl.x
    COMMAND rpcgen -c -i 0 -M -o
	${CMAKE_CURRENT_BINARY_DIR}/ThreadControl.c
	${CMAKE_CURRENT_BINARY_DIR}/ThreadControl.x
    COMMAND cmake -E copy ${CMAKE_CURRENT_BINARY_DIR}/ThreadControl.h ${CMAKE_CURRENT_BINARY_DIR}/KrellInstitute/Messages/ThreadControl.h
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/ThreadControl.x
)




# Do not build the converters if we are targeting the compute node build for Intel MIC
# which is a platform that runs the CBTF filters on a host processor not the
# compute node processors.
if ((RUNTIME_ONLY AND CBTF_FILTERS_RUN_ON MATCHES "native") OR NOT RUNTIME_ONLY)
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
    )

    target_link_libraries(cbtf-messages-converters-thread
        -Wl,--no-as-needed
	cbtf-messages-base
	${MRNet_LIBRARIES}
	${CMAKE_DL_LIBS}
    )

    set_target_properties(cbtf-messages-converters-thread PROPERTIES VERSION 1.1.0)
    set_target_properties(cbtf-messages-converters-thread PROPERTIES COMPILE_DEFINITIONS "${MRNet_DEFINES}")
    set_target_properties(cbtf-messages-converters-thread PROPERTIES POSITION_INDEPENDENT_CODE ON)

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
    )

    target_link_libraries(cbtf-messages-converters-thread-static
        -Wl,--no-as-needed
	cbtf-messages-base
	${MRNet_LIBRARIES}
	${CMAKE_DL_LIBS}
    )

    set_target_properties(cbtf-messages-converters-thread-static PROPERTIES VERSION 1.1.0)
    set_target_properties(cbtf-messages-converters-thread-static PROPERTIES OUTPUT_NAME cbtf-messages-converters-thread)
    set_target_properties(cbtf-messages-converters-thread-static PROPERTIES COMPILE_DEFINITIONS "${MRNet_DEFINES}")
    set_target_properties(cbtf-messages-converters-thread-static PROPERTIES POSITION_INDEPENDENT_CODE ON)

    install(TARGETS cbtf-messages-converters-thread-static
	ARCHIVE DESTINATION lib${LIB_SUFFIX}
    )
endif()

add_library(cbtf-messages-thread SHARED
	${CBTF_MESSAGES_THREAD_SOURCES}
)

target_include_directories(cbtf-messages-thread PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../base
	${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(cbtf-messages-thread
        -Wl,--no-as-needed
	cbtf-messages-base
	${CMAKE_DL_LIBS}
)

set_target_properties(cbtf-messages-thread PROPERTIES VERSION 1.1.0)
set_target_properties(cbtf-messages-thread PROPERTIES POSITION_INDEPENDENT_CODE ON)

install(TARGETS cbtf-messages-thread
	LIBRARY DESTINATION lib${LIB_SUFFIX}
)


# Install static version

add_library(cbtf-messages-thread-static STATIC
	${CBTF_MESSAGES_THREAD_SOURCES}
)

target_include_directories(cbtf-messages-thread-static PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../base
	${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(cbtf-messages-thread-static
        -Wl,--no-as-needed
	cbtf-messages-base
	${CMAKE_DL_LIBS}
)

set_target_properties(cbtf-messages-thread-static PROPERTIES VERSION 1.1.0)
set_target_properties(cbtf-messages-thread-static PROPERTIES OUTPUT_NAME cbtf-messages-thread)
set_target_properties(cbtf-messages-thread-static PROPERTIES POSITION_INDEPENDENT_CODE ON)

install(TARGETS cbtf-messages-thread-static
	ARCHIVE DESTINATION lib${LIB_SUFFIX}
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/ThreadControl.h
	DESTINATION include/KrellInstitute/Messages
	PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
)


# rpcgen processing done in CMAKE_CURRENT_BINARY_DIR..
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/ThreadControl.x DESTINATION .)
