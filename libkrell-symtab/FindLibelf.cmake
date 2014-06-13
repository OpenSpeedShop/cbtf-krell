################################################################################
# Copyright (c) 2013-2014 Krell Institute. All Rights Reserved.
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

include(FindPackageHandleStandardArgs)
INCLUDE (CheckSymbolExists)
INCLUDE (CheckFunctionExists)

SET(CMAKE_FIND_LIBRARY_PREFIXES "lib")
SET(CMAKE_FIND_LIBRARY_SUFFIXES ".so" ".a")


find_path(Libelf_INCLUDE_DIR
    NAMES libelf.h elf.h 
    PATHS /usr /usr/local
    HINTS $ENV{LIBELF_ROOT} ${LIBELF_ROOT}
    PATH_SUFFIXES include include/libelf
    NO_DEFAULT_PATH
    )

find_library(Libelf_LIBRARY_SHARED NAMES elf
    HINTS $ENV{LIBELF_ROOT} ${LIBELF_ROOT}
    PATHS /usr /usr/local
    PATH_SUFFIXES lib lib64
    NO_DEFAULT_PATH
    )

find_library(Libelf_LIBRARY_STATIC NAMES libelf.a
    HINTS $ENV{LIBELF_ROOT} ${LIBELF_ROOT}
    PATHS /usr /usr/local
    PATH_SUFFIXES lib lib64
    NO_DEFAULT_PATH
    )



find_package_handle_standard_args(
    Libelf DEFAULT_MSG
    Libelf_LIBRARY_SHARED Libelf_LIBRARY_STATIC
    Libelf_INCLUDE_DIR
    )

set(Libelf_SHARED_LIBRARIES ${Libelf_LIBRARY_SHARED})
set(Libelf_STATIC_LIBRARIES ${Libelf_LIBRARY_STATIC})
set(Libelf_INCLUDE_DIRS ${Libelf_INCLUDE_DIR})

message(STATUS "Libelf Libelf_SHARED_LIBRARIES: " ${Libelf_SHARED_LIBRARIES})
message(STATUS "Libelf Libelf_STATIC_LIBRARIES: " ${Libelf_STATIC_LIBRARIES})
message(STATUS "Libelf Libelf_INCLUDE_DIR: " ${Libelf_INCLUDE_DIR})
message(STATUS "Libelf Libelf_FOUND: " ${LIBELF_FOUND})

GET_FILENAME_COMPONENT(Libelf_LIB_DIR ${Libelf_LIBRARY_SHARED} PATH )

message(STATUS "Libelf Libelf_LIB_DIR: " ${Libelf_LIB_DIR})


mark_as_advanced(
            Libelf_LIBRARY_SHARED 
            Libelf_LIBRARY_STATIC
            Libelf_INCLUDE_DIR
            Libelf_LIB_DIR
            )
