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

set(CBTF_MESSAGES_SYMTAB_SOURCES
    ${CMAKE_CURRENT_BINARY_DIR}/Symbol.h
    ${CMAKE_CURRENT_BINARY_DIR}/Symbol.c
    Symbol.x
)

# Adding the rpcgen .h files to converters sources to ensure
# they are created.
set(CBTF_MESSAGES_CONVERTERS_SYMTAB_SOURCES
    ${CMAKE_CURRENT_BINARY_DIR}/Symbol.h
    Converters.cpp
)

INCLUDE_DIRECTORIES(SYSTEM ${Libtirpc_INCLUDE_DIRS})

add_custom_command(
    OUTPUT
	${CMAKE_CURRENT_BINARY_DIR}/Symbol.h
	${CMAKE_CURRENT_BINARY_DIR}/Symbol.c
    COMMAND rpcgen -h -o
	${CMAKE_CURRENT_BINARY_DIR}/Symbol.h
	${CMAKE_CURRENT_BINARY_DIR}/Symbol.x
    COMMAND rpcgen -c -i 0 -M -o
	${CMAKE_CURRENT_BINARY_DIR}/Symbol.c
	${CMAKE_CURRENT_BINARY_DIR}/Symbol.x
    COMMAND cmake -E copy ${CMAKE_CURRENT_BINARY_DIR}/Symbol.h ${CMAKE_CURRENT_BINARY_DIR}/KrellInstitute/Messages/Symbol.h
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/Symbol.x
)




# Do not build the converters if we are targeting the compute node build for Intel MIC
# which is a platform that runs the CBTF filters on a host processor not the
# compute node processors.
if ((RUNTIME_ONLY AND CBTF_FILTERS_RUN_ON MATCHES "native") OR NOT RUNTIME_ONLY)
    add_library(cbtf-messages-converters-symtab SHARED
	${CBTF_MESSAGES_CONVERTERS_SYMTAB_SOURCES}
    )

    target_include_directories(cbtf-messages-converters-symtab PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../base
	${MRNet_INCLUDE_DIRS}
	${Boost_INCLUDE_DIRS}
	${CBTF_INCLUDE_DIRS}
    )

    target_link_libraries(cbtf-messages-converters-symtab
	-Wl,--no-as-needed
	cbtf-messages-base
	${MRNet_LIBRARIES}
	${CMAKE_DL_LIBS}
    )

    set_target_properties(cbtf-messages-converters-symtab PROPERTIES VERSION 1.1.0)
    set_target_properties(cbtf-messages-converters-symtab PROPERTIES COMPILE_DEFINITIONS "${MRNet_DEFINES}")
    set_target_properties(cbtf-messages-converters-symtab PROPERTIES POSITION_INDEPENDENT_CODE ON)

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
	${CMAKE_CURRENT_BINARY_DIR}/../base
	${MRNet_INCLUDE_DIRS}
	${Boost_INCLUDE_DIRS}
	${CBTF_INCLUDE_DIRS}
    )

    target_link_libraries(cbtf-messages-converters-symtab-static
	-Wl,--no-as-needed
	cbtf-messages-base
	${MRNet_LIBRARIES}
	${CMAKE_DL_LIBS}
    )

    set_target_properties(cbtf-messages-converters-symtab-static PROPERTIES VERSION 1.1.0)
    set_target_properties(cbtf-messages-converters-symtab-static PROPERTIES OUTPUT_NAME cbtf-messages-converters-symtab)
    set_target_properties(cbtf-messages-converters-symtab-static PROPERTIES COMPILE_DEFINITIONS "${MRNet_DEFINES}")
    set_target_properties(cbtf-messages-converters-symtab-static PROPERTIES POSITION_INDEPENDENT_CODE ON)

    install(TARGETS cbtf-messages-converters-symtab-static
	ARCHIVE DESTINATION lib${LIB_SUFFIX}
    )
endif()

add_library(cbtf-messages-symtab SHARED
	${CBTF_MESSAGES_SYMTAB_SOURCES}
)

target_include_directories(cbtf-messages-symtab PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../base
	${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(cbtf-messages-symtab
	-Wl,--no-as-needed
	cbtf-messages-base
	${CMAKE_DL_LIBS}
)

set_target_properties(cbtf-messages-symtab PROPERTIES VERSION 1.1.0)
set_target_properties(cbtf-messages-symtab PROPERTIES POSITION_INDEPENDENT_CODE ON)

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
)

target_link_libraries(cbtf-messages-symtab-static
	-Wl,--no-as-needed
	cbtf-messages-base
	${CMAKE_DL_LIBS}
)

set_target_properties(cbtf-messages-symtab-static PROPERTIES VERSION 1.1.0)
set_target_properties(cbtf-messages-symtab-static PROPERTIES OUTPUT_NAME cbtf-messages-symtab)
set_target_properties(cbtf-messages-symtab-static PROPERTIES POSITION_INDEPENDENT_CODE ON)

install(TARGETS cbtf-messages-symtab-static 
	ARCHIVE DESTINATION lib${LIB_SUFFIX}
)

install(FILES
	${CMAKE_CURRENT_BINARY_DIR}/Symbol.h 
	DESTINATION include/KrellInstitute/Messages
	PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
)


# rpcgen processing done in CMAKE_CURRENT_BINARY_DIR..
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Symbol.x DESTINATION .)
