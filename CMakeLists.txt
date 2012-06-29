################################################################################
# Copyright (c) 2012 Argo Navis Technologies. All Rights Reserved.
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

project(CBTF-Tools)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR})
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/../framework)

find_package(CBTF REQUIRED)
find_package(CUPTI 2)
find_package(MRNet 4.0.0)
find_package(PAPI 3.0.0)

find_path(CBTF_TOOLS_INCLUDE_DIR KrellInstitute/Core/Assert.hpp
    HINTS $ENV{CBTF_ROOT} $ENV{CBTF_PREFIX}
    PATH_SUFFIXES include
    )

message(STATUS 
    "CBTF non-CMake tools include directory: " ${CBTF_TOOLS_INCLUDE_DIR}
    )

find_path(CBTF_TOOLS_LIBRARY_DIR libcbtf-core.so
    HINTS $ENV{CBTF_ROOT} $ENV{CBTF_PREFIX}
    PATH_SUFFIXES lib lib64
    )

message(STATUS 
    "CBTF non-CMake tools library directory: " ${CBTF_TOOLS_LIBRARY_DIR}
    )

add_subdirectory(core)
add_subdirectory(services)
