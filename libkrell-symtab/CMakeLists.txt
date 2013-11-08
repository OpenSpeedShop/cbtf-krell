################################################################################
# Copyright (c) 2013 Krell Institute. All Rights Reserved.
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

cmake_minimum_required(VERSION 2.8.0)

project(Krell-SymbolTable)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR})

find_package(Boost 1.39.0 REQUIRED 
    COMPONENTS filesystem system unit_test_framework
    )
find_package(CBTF-Messages REQUIRED)
find_package(Krell-Base REQUIRED)
find_package(SymtabAPI)

if(CMAKE_SYSTEM_PROCESSOR MATCHES "ppc64*")
    set(CMAKE_LIBRARY_PATH ${CMAKE_INSTALL_PREFIX}/lib64)
    set(LIB_SUFFIX 64)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "ppc*")
    set(CMAKE_LIBRARY_PATH ${CMAKE_INSTALL_PREFIX}/lib)
elseif(EXISTS /usr/lib64)
    set(LIB_SUFFIX 64)
    set(CMAKE_LIBRARY_PATH ${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX})
endif()

if(SYMTABAPI_FOUND)
    set(SYMTABAPI_SOURCES SymtabAPIResolver.hpp SymtabAPIResolver.cpp)
endif()

add_library(krell-symtab SHARED
    AddressBitmap.hpp AddressBitmap.cpp
    KrellInstitute/SymbolTable/AddressSpaces.hpp AddressSpaces.cpp
    KrellInstitute/SymbolTable/Function.hpp Function.cpp
    KrellInstitute/SymbolTable/FunctionVisitor.hpp
    KrellInstitute/SymbolTable/LinkedObject.hpp LinkedObject.cpp
    KrellInstitute/SymbolTable/LinkedObjectVisitor.hpp
    KrellInstitute/SymbolTable/MappingVisitor.hpp
    KrellInstitute/SymbolTable/Resolver.hpp Resolver.cpp
    KrellInstitute/SymbolTable/Statement.hpp Statement.cpp
    KrellInstitute/SymbolTable/StatementVisitor.hpp
    KrellInstitute/SymbolTable/ThreadVisitor.hpp
    SymbolTable.hpp SymbolTable.cpp
    ${SYMTABAPI_SOURCES}
    )

target_link_libraries(krell-symtab
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${CBTF_MESSAGE_LIBRARIES}
    ${KRELL_BASE_LIBRARIES}
    ${SymtabAPI_LIBRARIES}
    )

set_target_properties(krell-symtab PROPERTIES VERSION 1.1.0)

add_executable(test test.cpp)

target_link_libraries(test
    krell-symtab
    ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
    )

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${Boost_INCLUDE_DIRS}
    ${CBTF_MESSAGES_INCLUDE_DIRS}
    ${KRELL_BASE_INCLUDE_DIRS}
    )

if(SYMTABAPI_FOUND)
    include_directories(${SymtabAPI_INCLUDE_DIRS})
    add_definitions(-DSYMTABAPI_FOUND)
endif()

install(DIRECTORY KrellInstitute DESTINATION include)

install(TARGETS krell-symtab
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib${LIB_SUFFIX}
    ARCHIVE DESTINATION lib${LIB_SUFFIX}
    )

install(
    FILES FindKrell-Symtab.cmake FindSymtabAPI.cmake
    DESTINATION share/KrellInstitute/cmake
    )
