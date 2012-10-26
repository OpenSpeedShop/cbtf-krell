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

project(CBTF_CUDA_Collector)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/../../../framework
    )

find_package(Boost 1.39.0 REQUIRED)
find_package(CBTF REQUIRED)
find_package(CBTF-Core REQUIRED)
find_package(CBTF-Messages REQUIRED)
find_package(CBTF-Services REQUIRED)
find_package(CUPTI 2)
find_package(MRNet 4.0.0)
find_package(PAPI 3.0.0)
find_package(Threads)

add_subdirectory(collector)
add_subdirectory(components)
add_subdirectory(messages)
#add_subdirectory(symtab)
