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

set(CBTF_MESSAGES_SYMTAB_SOURCES
        ${CMAKE_CURRENT_BINARY_DIR}/Symbol.h
        ${CMAKE_CURRENT_BINARY_DIR}/Symbol.c
	Symbol.x
)

set(CBTF_MESSAGES_CONVERTERS_SYMTAB_SOURCES
        ${CMAKE_CURRENT_BINARY_DIR}/Symbol.h
        ${CMAKE_CURRENT_BINARY_DIR}/Symbol.c
	Converters.cpp
)

add_custom_command(
    OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/Symbol.h
        ${CMAKE_CURRENT_BINARY_DIR}/Symbol.c
    COMMAND rpcgen -h -o
        ${CMAKE_CURRENT_BINARY_DIR}/Symbol.h
        ${CMAKE_CURRENT_BINARY_DIR}/Symbol.x
    COMMAND rpcgen -c -o
        ${CMAKE_CURRENT_BINARY_DIR}/Symbol.c
        ${CMAKE_CURRENT_BINARY_DIR}/Symbol.x
    COMMAND cmake -E copy ${CMAKE_CURRENT_BINARY_DIR}/Symbol.h ${CMAKE_CURRENT_BINARY_DIR}/KrellInstitute/Messages/Symbol.h
    DEPENDS
        ${CMAKE_CURRENT_BINARY_DIR}/Symbol.x
)


add_library(cbtf-messages-converters-symtab SHARED
        ${CBTF_MESSAGES_CONVERTERS_SYMTAB_SOURCES}
)
 
target_include_directories(cbtf-messages-converters-symtab PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}../base
	${MRNet_INCLUDE_DIRS}
	${Boost_INCLUDE_DIRS}
	${CBTF_INCLUDE_DIRS}
)

target_link_libraries(cbtf-messages-converters-symtab
	cbtf-messages-base
	${MRNet_LIBRARIES}
	pthread
	${CMAKE_DL_LIBS}
)

set_target_properties(cbtf-messages-converters-symtab PROPERTIES VERSION 1.1.0)
set_target_properties(cbtf-messages-converters-symtab PROPERTIES 
        COMPILE_DEFINITIONS "${MRNet_DEFINES}")

install(TARGETS cbtf-messages-converters-symtab
	LIBRARY DESTINATION lib${LIB_SUFFIX}
)

# Install static version

add_library(cbtf-messages-converters-symtab-static STATIC
        ${CBTF_MESSAGES_CONVERTERS_SYMTAB_SOURCES}
)
 
target_include_directories(cbtf-messages-converters-symtab-static PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}../base
	${MRNet_INCLUDE_DIRS}
	${Boost_INCLUDE_DIRS}
	${CBTF_INCLUDE_DIRS}
)

target_link_libraries(cbtf-messages-converters-symtab-static
	cbtf-messages-base
	${MRNet_LIBRARIES}
	pthread
	${CMAKE_DL_LIBS}
)

set_target_properties(cbtf-messages-converters-symtab-static PROPERTIES VERSION 1.1.0)
set_target_properties(cbtf-messages-converters-symtab-static PROPERTIES 
        COMPILE_DEFINITIONS "${MRNet_DEFINES}")

install(TARGETS cbtf-messages-converters-symtab-static
	ARCHIVE DESTINATION lib${LIB_SUFFIX}
)


add_library(cbtf-messages-symtab SHARED
	${CBTF_MESSAGES_SYMTAB_SOURCES}
)

target_include_directories(cbtf-messages-symtab PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../base
	${CMAKE_CURRENT_BINARY_DIR}
	${CBTF_INCLUDE_DIRS}
	${MRNet_INCLUDE_DIRS}
	${Boost_INCLUDE_DIRS}
	${LTDL_INCLUDE_DIR}
)

target_link_libraries(cbtf-messages-symtab
	${MRNet_LIBRARIES}
	pthread
	${LTDL_LIBRARY_SHARED}
	${CMAKE_DL_LIBS}
)

set_target_properties(cbtf-messages-symtab PROPERTIES VERSION 1.1.0)
set_target_properties(cbtf-messages-symtab PROPERTIES 
        COMPILE_DEFINITIONS "${MRNet_DEFINES}")

install(TARGETS cbtf-messages-symtab 
        LIBRARY DESTINATION lib${LIB_SUFFIX}
)

# Install static version

add_library(cbtf-messages-symtab-static STATIC
	${CBTF_MESSAGES_SYMTAB_SOURCES}
)

target_include_directories(cbtf-messages-symtab-static PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../base
	${CMAKE_CURRENT_BINARY_DIR}
	${CBTF_INCLUDE_DIRS}
	${MRNet_INCLUDE_DIRS}
	${Boost_INCLUDE_DIRS}
	${LTDL_INCLUDE_DIR}
)

target_link_libraries(cbtf-messages-symtab-static
	${MRNet_LIBRARIES}
	pthread
	${LTDL_LIBRARY_SHARED}
	${CMAKE_DL_LIBS}
)

set_target_properties(cbtf-messages-symtab-static PROPERTIES VERSION 1.1.0)
set_target_properties(cbtf-messages-symtab-static PROPERTIES 
        COMPILE_DEFINITIONS "${MRNet_DEFINES}")

install(TARGETS cbtf-messages-symtab-static 
        ARCHIVE DESTINATION lib${LIB_SUFFIX}
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/Symbol.h 
        DESTINATION include/KrellInstitute/Messages
        PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)


# rpcgen processing done in CMAKE_CURRENT_BINARY_DIR..
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Symbol.x DESTINATION .)
