################################################################################
# Copyright (c) 2014-2019 Krell Institute. All Rights Reserved.
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


include(CheckIncludeFile)

#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu++0x -D__STDC_INT64__")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++0x -D__STDC_INT64__")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D__STDC_INT64__")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__STDC_INT64__")

set(CORE_MRNET_SOURCES
	CBTFTopology.cpp
)

set(CORE_SYMTABAPI_SOURCES
	SymtabAPISymbols.cpp
)

set(CORE_BFD_SOURCES
	BFDSymbols.cpp
)

set(CORE_SOURCES
	AddressBitmap.cpp
	AddressBuffer.cpp
	Blob.cpp
	Exception.cpp
	ExtentGroup.cpp
	Graph.cpp
	LinkedObjectEntry.cpp
	LinkedObject.cpp
	Path.cpp
	PerfData.cpp
	PCData.cpp
	StacktraceData.cpp
	SymbolTable.cpp
	ThreadName.cpp
)

add_library(cbtf-core SHARED
	${CORE_SOURCES}
)

target_include_directories(cbtf-core PUBLIC
        ${Libtirpc_INCLUDE_DIRS}
	${PROJECT_SOURCE_DIR}/core/include
	${PROJECT_SOURCE_DIR}/services/include
	${PROJECT_SOURCE_DIR}/messages/include
	${Boost_INCLUDE_DIRS}
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../../messages/src/base
	${CMAKE_CURRENT_BINARY_DIR}/../../messages/src/events
	${CMAKE_CURRENT_BINARY_DIR}/../../messages/src/symtab
	${CMAKE_CURRENT_BINARY_DIR}/../../messages/src/perfdata
)

# Only link with specific Boost libraries needed rather than Boost_LIBRARIES
# which links all Boost libraries asked for in the cmake config COMPONENTTS
# specification for Boost.
target_link_libraries(cbtf-core
        -Wl,--no-as-needed
        ${Boost_FILESYSTEM_LIBRARY}
        ${Boost_SYSTEM_LIBRARY}
        ${Boost_THREAD_LIBRARY}
	cbtf-messages-perfdata
	cbtf-messages-base
	${CMAKE_DL_LIBS}
)

set_target_properties(cbtf-core PROPERTIES VERSION 1.1.0)
set_target_properties(cbtf-core PROPERTIES POSITION_INDEPENDENT_CODE ON)

install(TARGETS cbtf-core
	LIBRARY DESTINATION lib${LIB_SUFFIX}
)

# Create and install static version

add_library(cbtf-core-static STATIC
	${CORE_SOURCES}
)

target_include_directories(cbtf-core-static PUBLIC
        ${Libtirpc_INCLUDE_DIRS}
	${PROJECT_SOURCE_DIR}/core/include
	${PROJECT_SOURCE_DIR}/services/include
	${PROJECT_SOURCE_DIR}/messages/include
	${Boost_INCLUDE_DIRS}
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../../messages/src/base
	${CMAKE_CURRENT_BINARY_DIR}/../../messages/src/events
	${CMAKE_CURRENT_BINARY_DIR}/../../messages/src/symtab
	${CMAKE_CURRENT_BINARY_DIR}/../../messages/src/perfdata
)

# Only link with specific Boost libraries needed rather than Boost_LIBRARIES
# which links all Boost libraries asked for in the cmake config COMPONENTTS
# specification for Boost.
target_link_libraries(cbtf-core-static
        -Wl,--no-as-needed
        ${Boost_FILESYSTEM_LIBRARY}
        ${Boost_SYSTEM_LIBRARY}
        ${Boost_THREAD_LIBRARY}
	cbtf-messages-perfdata
	cbtf-messages-base
	${CMAKE_DL_LIBS}
)

set_target_properties(cbtf-core-static PROPERTIES VERSION 1.1.0)
set_target_properties(cbtf-core-static PROPERTIES OUTPUT_NAME cbtf-core)
set_target_properties(cbtf-core-static PROPERTIES POSITION_INDEPENDENT_CODE ON)

install(TARGETS cbtf-core-static
	ARCHIVE DESTINATION lib${LIB_SUFFIX}
)


add_library(cbtf-core-bfd SHARED
	${CORE_BFD_SOURCES}
)

target_include_directories(cbtf-core-bfd PUBLIC
        ${Libtirpc_INCLUDE_DIRS}
	${PROJECT_SOURCE_DIR}/core/include
	${PROJECT_SOURCE_DIR}/services/include
	${PROJECT_SOURCE_DIR}/messages/include
	${Binutils_INCLUDE_DIRS}
	${Binutils_Iberty_INCLUDE_DIRS}
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../../messages/src/base
	${CMAKE_CURRENT_BINARY_DIR}/../../messages/src/events
	${CMAKE_CURRENT_BINARY_DIR}/../../messages/src/symtab
)

target_link_libraries(cbtf-core-bfd
        -Wl,--no-as-needed
	${Bfd_SHARED_LIBRARY}
	cbtf-messages-perfdata
	${CMAKE_DL_LIBS}
)

set_target_properties(cbtf-core-bfd PROPERTIES VERSION 1.1.0)
# Work around problem defined by:
# https://stackoverflow.com/questions/11748035/binutils-bfd-h-wants-config-h-now
set_target_properties(cbtf-core-bfd PROPERTIES COMPILE_DEFINITIONS "PACKAGE=1;PACKAGE_VERSION=1")
set_target_properties(cbtf-core-bfd PROPERTIES POSITION_INDEPENDENT_CODE ON)

install(TARGETS cbtf-core-bfd
	LIBRARY DESTINATION lib${LIB_SUFFIX}
)

# Create and install static version

add_library(cbtf-core-bfd-static STATIC
	${CORE_BFD_SOURCES}
)

target_include_directories(cbtf-core-bfd-static PUBLIC
        ${Libtirpc_INCLUDE_DIRS}
	${PROJECT_SOURCE_DIR}/core/include
	${PROJECT_SOURCE_DIR}/services/include
	${PROJECT_SOURCE_DIR}/messages/include
	${Binutils_INCLUDE_DIRS}
	${Binutils_Iberty_INCLUDE_DIRS}
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../../messages/src/base
	${CMAKE_CURRENT_BINARY_DIR}/../../messages/src/events
	${CMAKE_CURRENT_BINARY_DIR}/../../messages/src/symtab
)

target_link_libraries(cbtf-core-bfd-static
        -Wl,--no-as-needed
	${Bfd_SHARED_LIBRARY}
	cbtf-messages-perfdata
	${CMAKE_DL_LIBS}
)

set_target_properties(cbtf-core-bfd-static PROPERTIES VERSION 1.1.0)
set_target_properties(cbtf-core-bfd-static PROPERTIES OUTPUT_NAME cbtf-core-bfd)
# Work around problem defined by:
# https://stackoverflow.com/questions/11748035/binutils-bfd-h-wants-config-h-now
set_target_properties(cbtf-core-bfd-static PROPERTIES COMPILE_DEFINITIONS "PACKAGE=1;PACKAGE_VERSION=1")
set_target_properties(cbtf-core-bfd-static PROPERTIES POSITION_INDEPENDENT_CODE ON)

install(TARGETS cbtf-core-bfd-static
	ARCHIVE DESTINATION lib${LIB_SUFFIX}
)

#if (CBTF_FILTERS_RUN_ON MATCHES "native")
if (NOT RUNTIME_PLATFORM EQUAL "mic")

    add_library(cbtf-core-symtabapi SHARED
	${CORE_SYMTABAPI_SOURCES}
    )

    target_include_directories(cbtf-core-symtabapi PUBLIC
        ${Libtirpc_INCLUDE_DIRS}
	${PROJECT_SOURCE_DIR}/core/include
	${PROJECT_SOURCE_DIR}/services/include
	${PROJECT_SOURCE_DIR}/messages/include
	${Boost_INCLUDE_DIRS}
	${SymtabAPI_INCLUDE_DIRS}
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../../messages/src/base
	${CMAKE_CURRENT_BINARY_DIR}/../../messages/src/events
	${CMAKE_CURRENT_BINARY_DIR}/../../messages/src/symtab
    )

    target_link_libraries(cbtf-core-symtabapi
        -Wl,--no-as-needed
	${DyninstAPI_common_LIBRARY}
	${DyninstAPI_symtabAPI_LIBRARY}
	${DyninstAPI_instructionAPI_LIBRARY}
	${DyninstAPI_parseAPI_LIBRARY}
	${DyninstAPI_dynElf_LIBRARY}
	${DyninstAPI_dynDwarf_LIBRARY}
	${DyninstAPI_symLite_LIBRARY}
	${CMAKE_DL_LIBS}
        cbtf-core
    )

    set_target_properties(cbtf-core-symtabapi PROPERTIES VERSION 1.1.0)
    set_target_properties(cbtf-core-symtabapi PROPERTIES POSITION_INDEPENDENT_CODE ON)

    install(TARGETS cbtf-core-symtabapi
	LIBRARY DESTINATION lib${LIB_SUFFIX}
    )

# Create and install static version

    add_library(cbtf-core-symtabapi-static STATIC
	${CORE_SYMTABAPI_SOURCES}
    )

    target_include_directories(cbtf-core-symtabapi-static PUBLIC
        ${Libtirpc_INCLUDE_DIRS}
	${PROJECT_SOURCE_DIR}/core/include
	${PROJECT_SOURCE_DIR}/services/include
	${PROJECT_SOURCE_DIR}/messages/include
	${Boost_INCLUDE_DIRS}
	${SymtabAPI_INCLUDE_DIRS}
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../../messages/src/base
	${CMAKE_CURRENT_BINARY_DIR}/../../messages/src/events
	${CMAKE_CURRENT_BINARY_DIR}/../../messages/src/symtab
    )

    target_link_libraries(cbtf-core-symtabapi-static
        -Wl,--no-as-needed
	${DyninstAPI_common_LIBRARY}
	${DyninstAPI_symtabAPI_LIBRARY}
	${DyninstAPI_instructionAPI_LIBRARY}
	${DyninstAPI_parseAPI_LIBRARY}
	${DyninstAPI_dynElf_LIBRARY}
	${DyninstAPI_dynDwarf_LIBRARY}
	${DyninstAPI_symLite_LIBRARY}
	${CMAKE_DL_LIBS}
    )

    set_target_properties(cbtf-core-symtabapi-static PROPERTIES VERSION 1.1.0)
    set_target_properties(cbtf-core-symtabapi-static PROPERTIES OUTPUT_NAME cbtf-core-symtabapi)
    set_target_properties(cbtf-core-symtabapi-static PROPERTIES POSITION_INDEPENDENT_CODE ON)

    install(TARGETS cbtf-core-symtabapi-static
	ARCHIVE DESTINATION lib${LIB_SUFFIX}
    )

# Have to handle this when working on cray port to cmake
#if HAVE_ALPS
#libcbtf_core_mrnet_la_CXXFLAGS += @ALPS_CPPFLAGS@ 
#libcbtf_core_mrnet_la_LDFLAGS += @ALPS_LDFLAGS@
#libcbtf_core_mrnet_la_LIBADD += @ALPS_LIBS@
#endif

endif()

if (NOT RUNTIME_ONLY)

    add_library(cbtf-core-mrnet SHARED
	${CORE_MRNET_SOURCES}
    )

    if (HAVE_CRAYALPS MATCHES "true")
        target_include_directories(cbtf-core-mrnet PUBLIC
            ${Libtirpc_INCLUDE_DIRS}
    	    ${PROJECT_SOURCE_DIR}/core/include
	    ${PROJECT_SOURCE_DIR}/services/include
	    ${PROJECT_SOURCE_DIR}/messages/include
	    ${Boost_INCLUDE_DIRS}
	    ${MRNet_INCLUDE_DIRS}
	    ${CMAKE_CURRENT_SOURCE_DIR}
	    ${CMAKE_CURRENT_BINARY_DIR}
	    ${CMAKE_CURRENT_BINARY_DIR}/../../messages/src/base
	    ${CMAKE_CURRENT_BINARY_DIR}/../../messages/src/events
	    ${CMAKE_CURRENT_BINARY_DIR}/../../messages/src/symtab
	    ${CrayAlps_INCLUDE_DIRS}
        )
    else()
        target_include_directories(cbtf-core-mrnet PUBLIC
            ${Libtirpc_INCLUDE_DIRS}
	    ${PROJECT_SOURCE_DIR}/core/include
	    ${PROJECT_SOURCE_DIR}/services/include
	    ${PROJECT_SOURCE_DIR}/messages/include
	    ${Boost_INCLUDE_DIRS}
	    ${MRNet_INCLUDE_DIRS}
	    ${CMAKE_CURRENT_SOURCE_DIR}
	    ${CMAKE_CURRENT_BINARY_DIR}
	    ${CMAKE_CURRENT_BINARY_DIR}/../../messages/src/base
	    ${CMAKE_CURRENT_BINARY_DIR}/../../messages/src/events
	    ${CMAKE_CURRENT_BINARY_DIR}/../../messages/src/symtab
        )
    endif()

    target_link_libraries(cbtf-core-mrnet
        -Wl,--no-as-needed
	${MRNet_LIBRARIES}
	pthread
	${CMAKE_DL_LIBS}
    )

    set_target_properties(cbtf-core-mrnet PROPERTIES VERSION 1.1.0)
    set_target_properties(cbtf-core-mrnet PROPERTIES POSITION_INDEPENDENT_CODE ON)
    if (HAVE_CRAYALPS MATCHES "true")
        set_target_properties(cbtf-core-mrnet PROPERTIES 
            COMPILE_DEFINITIONS "${MRNet_DEFINES};${CRAYALPS_DEFINES}")
    else()
        set_target_properties(cbtf-core-mrnet PROPERTIES 
            COMPILE_DEFINITIONS "${MRNet_DEFINES}")
    endif()

    install(TARGETS cbtf-core-mrnet
	LIBRARY DESTINATION lib${LIB_SUFFIX}
    )

    # Create and install static version

    add_library(cbtf-core-mrnet-static STATIC
	${CORE_MRNET_SOURCES}
    )

    if (HAVE_CRAYALPS MATCHES "true")
        target_include_directories(cbtf-core-mrnet-static PUBLIC
            ${Libtirpc_INCLUDE_DIRS}
    	    ${PROJECT_SOURCE_DIR}/core/include
	    ${PROJECT_SOURCE_DIR}/services/include
	    ${PROJECT_SOURCE_DIR}/messages/include
	    ${Boost_INCLUDE_DIRS}
	    ${MRNet_INCLUDE_DIRS}
	    ${CMAKE_CURRENT_SOURCE_DIR}
	    ${CMAKE_CURRENT_BINARY_DIR}
	    ${CMAKE_CURRENT_BINARY_DIR}/../../messages/src/base
	    ${CMAKE_CURRENT_BINARY_DIR}/../../messages/src/events
	    ${CMAKE_CURRENT_BINARY_DIR}/../../messages/src/symtab
	    ${CrayAlps_INCLUDE_DIRS}
        )
    else()
        target_include_directories(cbtf-core-mrnet-static PUBLIC
            ${Libtirpc_INCLUDE_DIRS}
	    ${PROJECT_SOURCE_DIR}/core/include
	    ${PROJECT_SOURCE_DIR}/services/include
	    ${PROJECT_SOURCE_DIR}/messages/include
	    ${Boost_INCLUDE_DIRS}
	    ${MRNet_INCLUDE_DIRS}
	    ${CMAKE_CURRENT_SOURCE_DIR}
	    ${CMAKE_CURRENT_BINARY_DIR}
	    ${CMAKE_CURRENT_BINARY_DIR}/../../messages/src/base
	    ${CMAKE_CURRENT_BINARY_DIR}/../../messages/src/events
	    ${CMAKE_CURRENT_BINARY_DIR}/../../messages/src/symtab
        )
    endif()
    target_link_libraries(cbtf-core-mrnet-static
        -Wl,--no-as-needed
	${MRNet_LIBRARIES}
	pthread
	${CMAKE_DL_LIBS}
    )

    set_target_properties(cbtf-core-mrnet-static PROPERTIES VERSION 1.1.0)
    set_target_properties(cbtf-core-mrnet-static PROPERTIES OUTPUT_NAME cbtf-core-mrnet)
    set_target_properties(cbtf-core-mrnet-static PROPERTIES POSITION_INDEPENDENT_CODE ON)
    if (HAVE_CRAYALPS MATCHES "true")
        set_target_properties(cbtf-core-mrnet-static PROPERTIES 
            COMPILE_DEFINITIONS "${MRNet_DEFINES};${CRAYALPS_DEFINES}")
    else()
        set_target_properties(cbtf-core-mrnet-static PROPERTIES 
            COMPILE_DEFINITIONS "${MRNet_DEFINES}")
    endif()

    install(TARGETS cbtf-core-mrnet-static
	ARCHIVE DESTINATION lib${LIB_SUFFIX}
    )

endif()

install (FILES ${PROJECT_SOURCE_DIR}/services/collector/KrellInstitute/Services/Collector.h
        DESTINATION include/KrellInstitute/Services
)
