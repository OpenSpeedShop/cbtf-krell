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

set(CBTF_MESSAGES_BASE_SOURCES
    ${CMAKE_CURRENT_BINARY_DIR}/Address.h
    ${CMAKE_CURRENT_BINARY_DIR}/Address.c
    ${CMAKE_CURRENT_BINARY_DIR}/Blob.h
    ${CMAKE_CURRENT_BINARY_DIR}/Blob.c
    ${CMAKE_CURRENT_BINARY_DIR}/Error.h
    ${CMAKE_CURRENT_BINARY_DIR}/Error.c
    ${CMAKE_CURRENT_BINARY_DIR}/File.h
    ${CMAKE_CURRENT_BINARY_DIR}/File.c
    ${CMAKE_CURRENT_BINARY_DIR}/StdIO.h
    ${CMAKE_CURRENT_BINARY_DIR}/StdIO.c
    ${CMAKE_CURRENT_BINARY_DIR}/Time.h
    ${CMAKE_CURRENT_BINARY_DIR}/Time.c
    ${CMAKE_CURRENT_BINARY_DIR}/Thread.h
    ${CMAKE_CURRENT_BINARY_DIR}/Thread.c
    Address.x
    Blob.x
    Error.x
    File.x
    StdIO.x
    Time.x
    Thread.x
)

# Adding the rpcgen .h files to converters sources to ensure
# they are created.
set(CBTF_MESSAGES_CONVERTERS_BASE_SOURCES
    ${CMAKE_CURRENT_BINARY_DIR}/Address.h
    ${CMAKE_CURRENT_BINARY_DIR}/Blob.h
    ${CMAKE_CURRENT_BINARY_DIR}/Error.h
    ${CMAKE_CURRENT_BINARY_DIR}/File.h
    ${CMAKE_CURRENT_BINARY_DIR}/StdIO.h
    ${CMAKE_CURRENT_BINARY_DIR}/Time.h
    ${CMAKE_CURRENT_BINARY_DIR}/Thread.h
    Converters.cpp
)


# FIXME. This like was due to missing .h files in converters sources.
#add_custom_target(runaddressh ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/KrellInstitute/Messages/Address.h)


add_custom_command(
    OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/Address.h
        ${CMAKE_CURRENT_BINARY_DIR}/Address.c
    COMMAND rpcgen -h -o
        ${CMAKE_CURRENT_BINARY_DIR}/Address.h
        ${CMAKE_CURRENT_BINARY_DIR}/Address.x
    COMMAND rpcgen -c -o
        ${CMAKE_CURRENT_BINARY_DIR}/Address.c
        ${CMAKE_CURRENT_BINARY_DIR}/Address.x
    COMMAND cmake -E copy ${CMAKE_CURRENT_BINARY_DIR}/Address.h ${CMAKE_CURRENT_BINARY_DIR}/KrellInstitute/Messages/Address.h
    DEPENDS
        ${CMAKE_CURRENT_BINARY_DIR}/Address.x
)


add_custom_command(
    OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/Blob.h
        ${CMAKE_CURRENT_BINARY_DIR}/Blob.c
    COMMAND rpcgen -h -o
        ${CMAKE_CURRENT_BINARY_DIR}/Blob.h
        ${CMAKE_CURRENT_BINARY_DIR}/Blob.x
    COMMAND rpcgen -c -o
        ${CMAKE_CURRENT_BINARY_DIR}/Blob.c
        ${CMAKE_CURRENT_BINARY_DIR}/Blob.x
    COMMAND cmake -E copy ${CMAKE_CURRENT_BINARY_DIR}/Blob.h ${CMAKE_CURRENT_BINARY_DIR}/KrellInstitute/Messages/Blob.h
    DEPENDS
        ${CMAKE_CURRENT_BINARY_DIR}/Blob.x
)

add_custom_command(
    OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/Error.h
        ${CMAKE_CURRENT_BINARY_DIR}/Error.c
    COMMAND rpcgen -h -o
        ${CMAKE_CURRENT_BINARY_DIR}/Error.h
        ${CMAKE_CURRENT_BINARY_DIR}/Error.x
    COMMAND rpcgen -c -o
        ${CMAKE_CURRENT_BINARY_DIR}/Error.c
        ${CMAKE_CURRENT_BINARY_DIR}/Error.x
    COMMAND cmake -E copy ${CMAKE_CURRENT_BINARY_DIR}/Error.h ${CMAKE_CURRENT_BINARY_DIR}/KrellInstitute/Messages/Error.h
    DEPENDS
        ${CMAKE_CURRENT_BINARY_DIR}/Error.x
)

add_custom_command(
    OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/File.h
        ${CMAKE_CURRENT_BINARY_DIR}/File.c
    COMMAND rpcgen -h -o
        ${CMAKE_CURRENT_BINARY_DIR}/File.h
        ${CMAKE_CURRENT_BINARY_DIR}/File.x
    COMMAND rpcgen -c -o
        ${CMAKE_CURRENT_BINARY_DIR}/File.c
        ${CMAKE_CURRENT_BINARY_DIR}/File.x
    COMMAND cmake -E copy ${CMAKE_CURRENT_BINARY_DIR}/File.h ${CMAKE_CURRENT_BINARY_DIR}/KrellInstitute/Messages/File.h
    DEPENDS
        ${CMAKE_CURRENT_BINARY_DIR}/File.x
)

add_custom_command(
    OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/StdIO.h
        ${CMAKE_CURRENT_BINARY_DIR}/StdIO.c
    COMMAND rpcgen -h -o
        ${CMAKE_CURRENT_BINARY_DIR}/StdIO.h
        ${CMAKE_CURRENT_BINARY_DIR}/StdIO.x
    COMMAND rpcgen -c -o
        ${CMAKE_CURRENT_BINARY_DIR}/StdIO.c
        ${CMAKE_CURRENT_BINARY_DIR}/StdIO.x
    COMMAND cmake -E copy ${CMAKE_CURRENT_BINARY_DIR}/StdIO.h ${CMAKE_CURRENT_BINARY_DIR}/KrellInstitute/Messages/StdIO.h
    DEPENDS
        ${CMAKE_CURRENT_BINARY_DIR}/StdIO.x
)

add_custom_command(
    OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/Time.h
        ${CMAKE_CURRENT_BINARY_DIR}/Time.c
    COMMAND rpcgen -h -o
        ${CMAKE_CURRENT_BINARY_DIR}/Time.h
        ${CMAKE_CURRENT_BINARY_DIR}/Time.x
    COMMAND rpcgen -c -o
        ${CMAKE_CURRENT_BINARY_DIR}/Time.c
        ${CMAKE_CURRENT_BINARY_DIR}/Time.x
    COMMAND cmake -E copy ${CMAKE_CURRENT_BINARY_DIR}/Time.h ${CMAKE_CURRENT_BINARY_DIR}/KrellInstitute/Messages/Time.h
    DEPENDS
        ${CMAKE_CURRENT_BINARY_DIR}/Time.x
)

add_custom_command(
    OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/Thread.h
        ${CMAKE_CURRENT_BINARY_DIR}/Thread.c
    COMMAND rpcgen -h -o
        ${CMAKE_CURRENT_BINARY_DIR}/Thread.h
        ${CMAKE_CURRENT_BINARY_DIR}/Thread.x
    COMMAND rpcgen -c -o
        ${CMAKE_CURRENT_BINARY_DIR}/Thread.c
        ${CMAKE_CURRENT_BINARY_DIR}/Thread.x
    COMMAND cmake -E copy ${CMAKE_CURRENT_BINARY_DIR}/Thread.h ${CMAKE_CURRENT_BINARY_DIR}/KrellInstitute/Messages/Thread.h
    DEPENDS
        ${CMAKE_CURRENT_BINARY_DIR}/Thread.x
)

# Do not build the converters if we are targeting the compute node build for Intel MIC
# which is a platform that runs the CBTF filters on a host processor not the
# compute node processors.
if ((RUNTIME_ONLY AND CBTF_FILTERS_RUN_ON MATCHES "native") OR NOT RUNTIME_ONLY)
    add_library(cbtf-messages-converters-base SHARED
	${CBTF_MESSAGES_CONVERTERS_BASE_SOURCES}
    )

    target_include_directories(cbtf-messages-converters-base PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${MRNet_INCLUDE_DIRS}
	${Boost_INCLUDE_DIRS}
	${CBTF_INCLUDE_DIRS}
    )

    target_link_libraries(cbtf-messages-converters-base
        -Wl,--no-as-needed
	${MRNet_LIBRARIES}
	${CMAKE_DL_LIBS}
    )

    set_target_properties(cbtf-messages-converters-base PROPERTIES VERSION 1.1.0)
    set_target_properties(cbtf-messages-converters-base PROPERTIES COMPILE_DEFINITIONS "${MRNet_DEFINES}")

    install(TARGETS cbtf-messages-converters-base
	LIBRARY DESTINATION lib${LIB_SUFFIX}
    )

# Install static version

    add_library(cbtf-messages-converters-base-static STATIC
	${CBTF_MESSAGES_CONVERTERS_BASE_SOURCES}
    )

    target_include_directories(cbtf-messages-converters-base-static PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${MRNet_INCLUDE_DIRS}
	${Boost_INCLUDE_DIRS}
	${CBTF_INCLUDE_DIRS}
    )

    target_link_libraries(cbtf-messages-converters-base-static
        -Wl,--no-as-needed
	${MRNet_LIBRARIES}
	${CMAKE_DL_LIBS}
    )

    set_target_properties(cbtf-messages-converters-base-static PROPERTIES VERSION 1.1.0)
    set_target_properties(cbtf-messages-converters-base-static PROPERTIES OUTPUT_NAME cbtf-messages-converters-base)
    set_target_properties(cbtf-messages-converters-base-static PROPERTIES COMPILE_DEFINITIONS "${MRNet_DEFINES}")

    install(TARGETS cbtf-messages-converters-base-static
	ARCHIVE DESTINATION lib${LIB_SUFFIX}
    )
endif()

add_library(cbtf-messages-base SHARED
	${CBTF_MESSAGES_BASE_SOURCES}
)

target_include_directories(cbtf-messages-base PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(cbtf-messages-base
        -Wl,--no-as-needed
	${CMAKE_DL_LIBS}
)

set_target_properties(cbtf-messages-base PROPERTIES VERSION 1.1.0)

install(TARGETS cbtf-messages-base 
        LIBRARY DESTINATION lib${LIB_SUFFIX}
)

# Install static version

add_library(cbtf-messages-base-static STATIC
	${CBTF_MESSAGES_BASE_SOURCES}
)

target_include_directories(cbtf-messages-base-static PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(cbtf-messages-base-static
        -Wl,--no-as-needed
	${CMAKE_DL_LIBS}
)

set_target_properties(cbtf-messages-base-static PROPERTIES VERSION 1.1.0)
set_target_properties(cbtf-messages-base-static PROPERTIES OUTPUT_NAME cbtf-messages-base)

install(TARGETS cbtf-messages-base-static 
        ARCHIVE DESTINATION lib${LIB_SUFFIX}
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/Address.h 
              ${CMAKE_CURRENT_BINARY_DIR}/Blob.h 
              ${CMAKE_CURRENT_BINARY_DIR}/Error.h 
              ${CMAKE_CURRENT_BINARY_DIR}/File.h 
              ${CMAKE_CURRENT_BINARY_DIR}/StdIO.h 
              ${CMAKE_CURRENT_BINARY_DIR}/Time.h 
              ${CMAKE_CURRENT_BINARY_DIR}/Thread.h
        DESTINATION include/KrellInstitute/Messages
        PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)


# rpcgen processing done in CMAKE_CURRENT_BINARY_DIR..
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Address.x DESTINATION .)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Blob.x DESTINATION .)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Error.x DESTINATION .)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/File.x DESTINATION .)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/StdIO.x DESTINATION .)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Time.x DESTINATION .)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Thread.x DESTINATION .)

