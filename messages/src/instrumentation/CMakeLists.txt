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

set(CBTF_MESSAGES_INSTRUMENTATION_SOURCES
    ${CMAKE_CURRENT_BINARY_DIR}/Instrument.h
    ${CMAKE_CURRENT_BINARY_DIR}/Instrument.c
    ${CMAKE_CURRENT_BINARY_DIR}/MpiJob.h
    ${CMAKE_CURRENT_BINARY_DIR}/MpiJob.c
    ${CMAKE_CURRENT_BINARY_DIR}/Job.h
    ${CMAKE_CURRENT_BINARY_DIR}/Job.c
    Instrument.x
    MpiJob.x
    Job.x
)


# Adding the rpcgen .h files to converters sources to ensure
# they are created.
set(CBTF_MESSAGES_CONVERTERS_INSTRUMENTATION_SOURCES
    ${CMAKE_CURRENT_BINARY_DIR}/Instrument.h
    ${CMAKE_CURRENT_BINARY_DIR}/MpiJob.h
    ${CMAKE_CURRENT_BINARY_DIR}/Job.h
    Converters.cpp
)

INCLUDE_DIRECTORIES(SYSTEM ${Libtirpc_INCLUDE_DIRS})

add_custom_command(
    OUTPUT
	${CMAKE_CURRENT_BINARY_DIR}/Instrument.h
	${CMAKE_CURRENT_BINARY_DIR}/Instrument.c
    COMMAND rpcgen -h -o
	${CMAKE_CURRENT_BINARY_DIR}/Instrument.h
	${CMAKE_CURRENT_BINARY_DIR}/Instrument.x
    COMMAND rpcgen -c -o
	${CMAKE_CURRENT_BINARY_DIR}/Instrument.c
	${CMAKE_CURRENT_BINARY_DIR}/Instrument.x
    COMMAND cmake -E copy ${CMAKE_CURRENT_BINARY_DIR}/Instrument.h ${CMAKE_CURRENT_BINARY_DIR}/KrellInstitute/Messages/Instrument.h
    DEPENDS
	${CMAKE_CURRENT_BINARY_DIR}/Instrument.x
)

add_custom_command(
    OUTPUT
	${CMAKE_CURRENT_BINARY_DIR}/MpiJob.h
	${CMAKE_CURRENT_BINARY_DIR}/MpiJob.c
    COMMAND rpcgen -h -o
	${CMAKE_CURRENT_BINARY_DIR}/MpiJob.h
	${CMAKE_CURRENT_BINARY_DIR}/MpiJob.x
    COMMAND rpcgen -c -o
	${CMAKE_CURRENT_BINARY_DIR}/MpiJob.c
	${CMAKE_CURRENT_BINARY_DIR}/MpiJob.x
    COMMAND cmake -E copy ${CMAKE_CURRENT_BINARY_DIR}/MpiJob.h ${CMAKE_CURRENT_BINARY_DIR}/KrellInstitute/Messages/MpiJob.h
    DEPENDS
	${CMAKE_CURRENT_BINARY_DIR}/MpiJob.x
)

add_custom_command(
    OUTPUT
	${CMAKE_CURRENT_BINARY_DIR}/Job.h
	${CMAKE_CURRENT_BINARY_DIR}/Job.c
    COMMAND rpcgen -h -o
	${CMAKE_CURRENT_BINARY_DIR}/Job.h
	${CMAKE_CURRENT_BINARY_DIR}/Job.x
    COMMAND rpcgen -c -o
	${CMAKE_CURRENT_BINARY_DIR}/Job.c
	${CMAKE_CURRENT_BINARY_DIR}/Job.x
    COMMAND cmake -E copy ${CMAKE_CURRENT_BINARY_DIR}/Job.h ${CMAKE_CURRENT_BINARY_DIR}/KrellInstitute/Messages/Job.h
    DEPENDS
	${CMAKE_CURRENT_BINARY_DIR}/Job.x
)

#add_definitions(${MRNet_DEFINES})

# Do not build the converters if we are targeting the compute node build for Intel MIC
# which is a platform that runs the CBTF filters on a host processor not the
# compute node processors.
if ((RUNTIME_ONLY AND CBTF_FILTERS_RUN_ON MATCHES "native") OR NOT RUNTIME_ONLY)

    add_library(cbtf-messages-converters-instrumentation SHARED
        ${CBTF_MESSAGES_CONVERTERS_INSTRUMENTATION_SOURCES}
    )
 
    target_include_directories(cbtf-messages-converters-instrumentation PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../base
	${CMAKE_CURRENT_BINARY_DIR}/../collector
	${MRNet_INCLUDE_DIRS}
	${Boost_INCLUDE_DIRS}
	${CBTF_INCLUDE_DIRS}
    )

    target_link_libraries(cbtf-messages-converters-instrumentation
        -Wl,--no-as-needed
	cbtf-messages-base
	cbtf-messages-collector
	${MRNet_LIBRARIES}
	${CMAKE_DL_LIBS}
    )

    set_target_properties(cbtf-messages-converters-instrumentation PROPERTIES VERSION 1.1.0)
    set_target_properties(cbtf-messages-converters-instrumentation PROPERTIES COMPILE_DEFINITIONS "${MRNet_DEFINES}")
    set_target_properties(cbtf-messages-converters-instrumentation PROPERTIES POSITION_INDEPENDENT_CODE ON)

    install(TARGETS cbtf-messages-converters-instrumentation
	LIBRARY DESTINATION lib${LIB_SUFFIX}
    )

# Install static version

    add_library(cbtf-messages-converters-instrumentation-static STATIC
        ${CBTF_MESSAGES_CONVERTERS_INSTRUMENTATION_SOURCES}
    )
 
    target_include_directories(cbtf-messages-converters-instrumentation-static PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../base
	${CMAKE_CURRENT_BINARY_DIR}/../collector
	${MRNet_INCLUDE_DIRS}
	${Boost_INCLUDE_DIRS}
	${CBTF_INCLUDE_DIRS}
    )

    target_link_libraries(cbtf-messages-converters-instrumentation-static
        -Wl,--no-as-needed
	cbtf-messages-base
	cbtf-messages-collector
	${MRNet_LIBRARIES}
	${CMAKE_DL_LIBS}
    )

    set_target_properties(cbtf-messages-converters-instrumentation-static PROPERTIES VERSION 1.1.0)
    set_target_properties(cbtf-messages-converters-instrumentation-static PROPERTIES OUTPUT_NAME cbtf-messages-converters-instrumentation)
    set_target_properties(cbtf-messages-converters-instrumentation-static PROPERTIES COMPILE_DEFINITIONS "${MRNet_DEFINES}")
    set_target_properties(cbtf-messages-converters-instrumentation-static PROPERTIES POSITION_INDEPENDENT_CODE ON)

    install(TARGETS cbtf-messages-converters-instrumentation-static
	ARCHIVE DESTINATION lib${LIB_SUFFIX}
    )
endif()

add_library(cbtf-messages-instrumentation SHARED
	${CBTF_MESSAGES_INSTRUMENTATION_SOURCES}
)

target_include_directories(cbtf-messages-instrumentation PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../base
	${CMAKE_CURRENT_BINARY_DIR}/../collector
	${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(cbtf-messages-instrumentation
        -Wl,--no-as-needed
	cbtf-messages-base
	cbtf-messages-collector
	${CMAKE_DL_LIBS}
)

set_target_properties(cbtf-messages-instrumentation PROPERTIES VERSION 1.1.0)
set_target_properties(cbtf-messages-instrumentation PROPERTIES POSITION_INDEPENDENT_CODE ON)

install(TARGETS cbtf-messages-instrumentation 
        LIBRARY DESTINATION lib${LIB_SUFFIX}
)

add_library(cbtf-messages-instrumentation-static STATIC
	${CBTF_MESSAGES_INSTRUMENTATION_SOURCES}
)

target_include_directories(cbtf-messages-instrumentation-static PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../base
	${CMAKE_CURRENT_BINARY_DIR}/../collector
	${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(cbtf-messages-instrumentation-static
        -Wl,--no-as-needed
	cbtf-messages-base
	cbtf-messages-collector
	${CMAKE_DL_LIBS}
)

set_target_properties(cbtf-messages-instrumentation-static PROPERTIES VERSION 1.1.0)
set_target_properties(cbtf-messages-instrumentation-static PROPERTIES OUTPUT_NAME cbtf-messages-instrumentation)
set_target_properties(cbtf-messages-instrumentation-static PROPERTIES POSITION_INDEPENDENT_CODE ON)

install(TARGETS cbtf-messages-instrumentation-static
        ARCHIVE DESTINATION lib${LIB_SUFFIX}
)

install(FILES
 	${CMAKE_CURRENT_BINARY_DIR}/Instrument.h 
      	${CMAKE_CURRENT_BINARY_DIR}/MpiJob.h 
      	${CMAKE_CURRENT_BINARY_DIR}/Job.h 
        DESTINATION include/KrellInstitute/Messages
        PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
)

# rpcgen processing done in CMAKE_CURRENT_BINARY_DIR..
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Instrument.x DESTINATION .)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/MpiJob.x DESTINATION .)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Job.x DESTINATION .)
