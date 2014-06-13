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


find_path(Libdwarf_INCLUDE_DIR NAMES dwarf.h 
    HINTS $ENV{LIBDWARF_ROOT} ${LIBDWARF_ROOT}
    PATH_SUFFIXES include
    NO_DEFAULT_PATH
    )

find_library(Libdwarf_LIBRARY_SHARED NAMES libdwarf.so
    HINTS $ENV{LIBDWARF_ROOT} ${LIBDWARF_ROOT}
    PATH_SUFFIXES lib lib64
    NO_DEFAULT_PATH
    )

find_library(Libdwarf_LIBRARY_STATIC NAMES libdwarf.a
    HINTS $ENV{LIBDWARF_ROOT}${LIBDWARF_ROOT}
    PATH_SUFFIXES lib lib64
    NO_DEFAULT_PATH
    )

find_package_handle_standard_args(
    Libdwarf DEFAULT_MSG
    Libdwarf_LIBRARY_SHARED Libdwarf_LIBRARY_STATIC
    Libdwarf_INCLUDE_DIR
    )

set(Libdwarf_SHARED_LIBRARIES ${Libdwarf_LIBRARY_SHARED})
set(Libdwarf_STATIC_LIBRARIES ${Libdwarf_LIBRARY_STATIC})
set(Libdwarf_INCLUDE_DIRS ${Libdwarf_INCLUDE_DIR})

set(Libdwarf_DEFINES "")

if(LIBDWARF_FOUND)
#  CMAKE_REQUIRED_FLAGS = string of compile command line flags
#  CMAKE_REQUIRED_DEFINITIONS = list of macros to define (-DFOO=bar)
#  CMAKE_REQUIRED_INCLUDES = list of include directories
#  CMAKE_REQUIRED_LIBRARIES = list of libraries to link
    SET(CMAKE_REQUIRED_LIBRARIES ${CMAKE_REQUIRED_LIBRARIES} -lelf ${Libdwarf_LIBRARY_SHARED})
    SET(CMAKE_REQUIRED_INCLUDES ${CMAKE_REQUIRED_INCLUDES} ${Libdwarf_INCLUDE_DIRS})
    CHECK_SYMBOL_EXISTS(dwarf_next_cu_header_c "libdwarf.h" HAVE_DWARF)
    #CHECK_FUNCTION_EXISTS(dwarf_next_cu_header_c HAVE_DWARF)
    message(STATUS "Libdwarf HAVE_dwarf_next_cu_header_c : " ${HAVE_DWARF})
endif()

if (NOT ${HAVE_DWARF})
    message(FATAL "Libdwarf does not contain the needed dwarf_next_cu_header_c symbol")
endif()

message(STATUS "Libdwarf dwarf_next_cu_header_c symbol : " ${HAVE_DWARF})
message(STATUS "Libdwarf Libdwarf_SHARED_LIBRARIES: " ${Libdwarf_SHARED_LIBRARIES})
message(STATUS "Libdwarf Libdwarf_STATIC_LIBRARIES: " ${Libdwarf_STATIC_LIBRARIES})
message(STATUS "Libdwarf Libdwarf_INCLUDE_DIR: " ${Libdwarf_INCLUDE_DIR})
message(STATUS "Libdwarf Libdwarf_FOUND: " ${LIBDWARF_FOUND})

GET_FILENAME_COMPONENT(Libdwarf_LIB_DIR ${Libdwarf_LIBRARY_SHARED} PATH )

message(STATUS "Libdwarf Libdwarf_LIB_DIR: " ${Libdwarf_LIB_DIR})


mark_as_advanced(
            Libdwarf_LIBRARY_SHARED 
            Libdwarf_LIBRARY_STATIC
            Libdwarf_INCLUDE_DIR
            Libdwarf_LIB_DIR
            )
