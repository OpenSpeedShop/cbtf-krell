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

project(Krell-Base)

find_package(Boost 1.39.0 REQUIRED 
    COMPONENTS filesystem system unit_test_framework
    )
find_package(CBTF-Messages REQUIRED)

if(CMAKE_SYSTEM_PROCESSOR MATCHES "ppc64*")
    set(CMAKE_LIBRARY_PATH ${CMAKE_INSTALL_PREFIX}/lib64)
    set(LIB_SUFFIX 64)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "ppc*")
    set(CMAKE_LIBRARY_PATH ${CMAKE_INSTALL_PREFIX}/lib)
elseif(EXISTS /usr/lib64)
    set(LIB_SUFFIX 64)
    set(CMAKE_LIBRARY_PATH ${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX})
endif()

add_library(krell-base SHARED
    KrellInstitute/Base/Address.hpp
    KrellInstitute/Base/AddressRange.hpp
    KrellInstitute/Base/FileName.hpp FileName.cpp
    KrellInstitute/Base/Interval.hpp
    KrellInstitute/Base/ThreadName.hpp ThreadName.cpp
    KrellInstitute/Base/Time.hpp
    KrellInstitute/Base/TimeInterval.hpp
    )

target_link_libraries(krell-base
    -lrt
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${CBTF_MESSAGE_LIBRARIES}
    )

set_target_properties(krell-base PROPERTIES VERSION 0.0.0)

add_executable(test test.cpp)

target_link_libraries(test
    krell-base
    ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
    )

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${Boost_INCLUDE_DIRS}
    ${CBTF_MESSAGES_INCLUDE_DIRS}
    )

install(DIRECTORY KrellInstitute DESTINATION include)

install(TARGETS krell-base
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib${LIB_SUFFIX}
    ARCHIVE DESTINATION lib${LIB_SUFFIX}
    )

install(FILES FindKrell-Base.cmake DESTINATION share/KrellInstitute/cmake)
