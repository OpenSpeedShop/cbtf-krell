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

set(CBTF_MESSAGES_COLLECTOR_SOURCES
    ${CMAKE_CURRENT_BINARY_DIR}/Collector.h
    ${CMAKE_CURRENT_BINARY_DIR}/Collector.c
    Collector.x
)

# Adding the rpcgen .h files to converters sources to ensure
# they are created.
set(CBTF_MESSAGES_CONVERTERS_COLLECTOR_SOURCES
    ${CMAKE_CURRENT_BINARY_DIR}/Collector.h
    Converters.cpp
)


# FIXME. This like was due to missing .h files in converters sources.
#add_custom_target(runcollectorh ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/KrellInstitute/Messages/Collector.h)

message(STATUS "Libtirpc_INCLUDE_DIRS:" ${Libtirpc_INCLUDE_DIRS})


INCLUDE_DIRECTORIES(SYSTEM ${Libtirpc_INCLUDE_DIRS})

add_custom_command(
    OUTPUT
	${CMAKE_CURRENT_BINARY_DIR}/Collector.h
	${CMAKE_CURRENT_BINARY_DIR}/Collector.c
    COMMAND rpcgen -h -o
	${CMAKE_CURRENT_BINARY_DIR}/Collector.h
	${CMAKE_CURRENT_BINARY_DIR}/Collector.x
    COMMAND rpcgen -c -o
	${CMAKE_CURRENT_BINARY_DIR}/Collector.c
	${CMAKE_CURRENT_BINARY_DIR}/Collector.x
    COMMAND cmake -E copy ${CMAKE_CURRENT_BINARY_DIR}/Collector.h ${CMAKE_CURRENT_BINARY_DIR}/KrellInstitute/Messages/Collector.h
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/Collector.x
)



# Do not build the converters if we are targeting the compute node build for Intel MIC
# which is a platform that runs the CBTF filters on a host processor not the
# compute node processors.
if ((RUNTIME_ONLY AND CBTF_FILTERS_RUN_ON MATCHES "native") OR NOT RUNTIME_ONLY)
    add_library(cbtf-messages-converters-collector SHARED
	${CBTF_MESSAGES_CONVERTERS_COLLECTOR_SOURCES}
    )
  
    target_include_directories(cbtf-messages-converters-collector PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${MRNet_INCLUDE_DIRS}
	${Boost_INCLUDE_DIRS}
	${CBTF_INCLUDE_DIRS}
    )

    target_link_libraries(cbtf-messages-converters-collector
        -Wl,--no-as-needed
	${MRNet_LIBRARIES}
	${CMAKE_DL_LIBS}
    )

    set_target_properties(cbtf-messages-converters-collector PROPERTIES VERSION 1.1.0)
    set_target_properties(cbtf-messages-converters-collector PROPERTIES COMPILE_DEFINITIONS "${MRNet_DEFINES}" )
    set_target_properties(cbtf-messages-converters-collector PROPERTIES POSITION_INDEPENDENT_CODE ON)


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
    )

    target_link_libraries(cbtf-messages-converters-collector-static
        -Wl,--no-as-needed
	${MRNet_LIBRARIES}
	${CMAKE_DL_LIBS}
    )

    set_target_properties(cbtf-messages-converters-collector-static PROPERTIES VERSION 1.1.0)
    set_target_properties(cbtf-messages-converters-collector-static PROPERTIES OUTPUT_NAME cbtf-messages-converters-collector)
    set_target_properties(cbtf-messages-converters-collector-static PROPERTIES COMPILE_DEFINITIONS "${MRNet_DEFINES}")
    set_target_properties(cbtf-messages-converters-collector-static PROPERTIES POSITION_INDEPENDENT_CODE ON)

    install(TARGETS cbtf-messages-converters-collector-static
	ARCHIVE DESTINATION lib${LIB_SUFFIX}
    )
endif()

add_library(cbtf-messages-collector SHARED
	${CBTF_MESSAGES_COLLECTOR_SOURCES}
)

target_include_directories(cbtf-messages-collector PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(cbtf-messages-collector
        -Wl,--no-as-needed
	${CMAKE_DL_LIBS}
)

set_target_properties(cbtf-messages-collector PROPERTIES VERSION 1.1.0)
set_target_properties(cbtf-messages-collector PROPERTIES POSITION_INDEPENDENT_CODE ON)

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
)

target_link_libraries(cbtf-messages-collector-static
        -Wl,--no-as-needed
	${CMAKE_DL_LIBS}
)

set_target_properties(cbtf-messages-collector-static PROPERTIES VERSION 1.1.0)
set_target_properties(cbtf-messages-collector-static PROPERTIES OUTPUT_NAME cbtf-messages-collector)
set_target_properties(cbtf-messages-collector-static PROPERTIES POSITION_INDEPENDENT_CODE ON)


install(TARGETS cbtf-messages-collector-static
	ARCHIVE DESTINATION lib${LIB_SUFFIX}
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/Collector.h
	DESTINATION include/KrellInstitute/Messages
	PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
)

# rpcgen processing done in CMAKE_CURRENT_BINARY_DIR..
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Collector.x DESTINATION .)
