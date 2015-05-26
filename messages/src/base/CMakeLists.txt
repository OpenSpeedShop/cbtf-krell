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
set(CBTF_MESSAGES_CONVERTERS_BASE_SOURCES
	Converters.cpp
    )

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
    DEPENDS
        ${CMAKE_CURRENT_BINARY_DIR}/Thread.x
    )

add_library(cbtf-messages-converters-base SHARED
        ${CBTF_MESSAGES_CONVERTERS_BASE_SOURCES}
    )

target_include_directories(cbtf-messages-converters-base PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${MRNet_INCLUDE_DIRS}
	${Boost_INCLUDE_DIRS}
	${CBTF_INCLUDE_DIRS}
	${LTDL_INCLUDE_DIR}
    )

target_link_libraries(cbtf-messages-converters-base
	${MRNet_LIBRARIES}
	pthread
	${LTDL_LIBRARY_SHARED}
	${CMAKE_DL_LIBS}
    )

set_target_properties(cbtf-messages-converters-base PROPERTIES VERSION 1.1.0)
set_target_properties(cbtf-messages-converters-base PROPERTIES
	COMPILE_DEFINITIONS "${MRNet_DEFINES}")

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
	${LTDL_INCLUDE_DIR}
    )

target_link_libraries(cbtf-messages-converters-base-static
	${MRNet_LIBRARIES}
	pthread
	${LTDL_LIBRARY_SHARED}
	${CMAKE_DL_LIBS}
    )

set_target_properties(cbtf-messages-converters-base-static PROPERTIES VERSION 1.1.0)
set_target_properties(cbtf-messages-converters-base-static PROPERTIES
	COMPILE_DEFINITIONS "${MRNet_DEFINES}")


install(TARGETS cbtf-messages-converters-base-static
	ARCHIVE DESTINATION lib${LIB_SUFFIX}
    )

add_library(cbtf-messages-base SHARED
	${CBTF_MESSAGES_BASE_SOURCES}
)

target_include_directories(cbtf-messages-base PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${LTDL_INCLUDE_DIR}
)

target_link_libraries(cbtf-messages-base
	${LTDL_LIBRARY_SHARED}
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
	${LTDL_INCLUDE_DIR}
)

target_link_libraries(cbtf-messages-base-static
	${LTDL_LIBRARY_SHARED}
	${CMAKE_DL_LIBS}
)

set_target_properties(cbtf-messages-base-static PROPERTIES VERSION 1.1.0)

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

# copy this header file so that the services/src/mrnet build can find it in the KrellInstitute/Messages form that it needs
# messages used to be installed prior to the services build but with cmake that is not the case
add_custom_target(copy_messages_blobheader ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/Blob.h)
add_custom_command(TARGET copy_messages_blobheader POST_BUILD
                   COMMAND cmake -E copy ${CMAKE_CURRENT_BINARY_DIR}/Blob.h ${CMAKE_CURRENT_BINARY_DIR}/KrellInstitute/Messages/Blob.h)
add_custom_target(copy_messages_fileheader ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/File.h)
add_custom_command(TARGET copy_messages_fileheader POST_BUILD
                   COMMAND cmake -E copy ${CMAKE_CURRENT_BINARY_DIR}/File.h ${CMAKE_CURRENT_BINARY_DIR}/KrellInstitute/Messages/File.h)
add_custom_target(copy_messages_threadheader ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/Thread.h)
add_custom_command(TARGET copy_messages_threadheader POST_BUILD
                   COMMAND cmake -E copy ${CMAKE_CURRENT_BINARY_DIR}/Thread.h ${CMAKE_CURRENT_BINARY_DIR}/KrellInstitute/Messages/Thread.h)
add_custom_target(copy_messages_addressheader ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/Address.h)
add_custom_command(TARGET copy_messages_addressheader POST_BUILD
                   COMMAND cmake -E copy ${CMAKE_CURRENT_BINARY_DIR}/Address.h ${CMAKE_CURRENT_BINARY_DIR}/KrellInstitute/Messages/Address.h)

# copy these files into the build area.  add_custom_command seems to need this
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Address.x DESTINATION .)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Blob.x DESTINATION .)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Error.x DESTINATION .)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/File.x DESTINATION .)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/StdIO.x DESTINATION .)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Time.x DESTINATION .)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Thread.x DESTINATION .)

