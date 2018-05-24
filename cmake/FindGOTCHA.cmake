################################################################################
# Copyright (c) 2018 Krell Institute. All Rights Reserved.
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

SET(CMAKE_FIND_LIBRARY_PREFIXES "lib" "lib64")
SET(CMAKE_FIND_LIBRARY_SUFFIXES ".so")


find_path(Gotcha_INCLUDE_DIR
    NAMES gotcha/gotcha.h
    PATHS /usr /usr/local
    HINTS $ENV{GOTCHA_DIR}
    HINTS ${GOTCHA_DIR}
    PATH_SUFFIXES include
    NO_DEFAULT_PATH
    )

find_library(Gotcha_LIBRARY_SHARED NAMES gotcha
    HINTS $ENV{GOTCHA_DIR}
    HINTS ${GOTCHA_DIR}
    PATHS /usr /usr/local
    PATH_SUFFIXES lib lib64
    NO_DEFAULT_PATH
    )

find_package_handle_standard_args(
    Gotcha DEFAULT_MSG
    Gotcha_LIBRARY_SHARED
    Gotcha_INCLUDE_DIR
    )

set(Gotcha_SHARED_LIBRARIES ${Gotcha_LIBRARY_SHARED})
set(Gotcha_INCLUDE_DIRS ${Gotcha_INCLUDE_DIR})
set(Gotcha_DEFINES "USES_GOTCHA")


GET_FILENAME_COMPONENT(Gotcha_LIB_DIR ${Gotcha_LIBRARY_SHARED} PATH )
GET_FILENAME_COMPONENT(Gotcha_DIR ${Gotcha_INCLUDE_DIR} PATH )

#message(STATUS "Gotcha Gotcha_SHARED_LIBRARIES: " ${Gotcha_SHARED_LIBRARIES})
#message(STATUS "Gotcha Gotcha_STATIC_LIBRARIES: " ${Gotcha_STATIC_LIBRARIES})
#message(STATUS "Gotcha Gotcha_INCLUDE_DIR: " ${Gotcha_INCLUDE_DIR})
#message(STATUS "Gotcha Gotcha_LIB_DIR: " ${Gotcha_LIB_DIR})
message(STATUS "Gotcha found: " ${GOTCHA_FOUND})
message(STATUS "Gotcha location: " ${Gotcha_DIR})


mark_as_advanced(
            Gotcha_LIBRARY_SHARED 
            Gotcha_INCLUDE_DIR
            Gotcha_LIB_DIR
            )
