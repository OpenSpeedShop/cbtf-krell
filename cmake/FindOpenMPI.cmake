################################################################################
# Copyright (c) 2014-2018 Krell Institute. All Rights Reserved.
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

find_path(OpenMPI_INCLUDE_DIR
    NAMES mpi.h
    HINTS $ENV{OPENMPI_DIR}
    HINTS ${OPENMPI_DIR}
    PATHS /usr /usr/local 
    PATH_SUFFIXES include include/openmpi include/openmpi3-ppc64le include/openmpi-ppc64le include/openmpi-x86_64
    NO_DEFAULT_PATH
    )

find_path(OpenMPI_BIN_DIR
    NAMES mpicc
    HINTS $ENV{OPENMPI_DIR}
    HINTS ${OPENMPI_DIR}
    PATHS /usr /usr/local /usr/lib64/openmpi /usr/lib64/openmpi3
    PATH_SUFFIXES bin 
    NO_DEFAULT_PATH
    )

find_library(OpenMPI_LIBRARY_SHARED NAMES mpi mpi_ibm
    HINTS $ENV{OPENMPI_DIR}
    HINTS ${OPENMPI_DIR}
    PATHS /usr /usr/local /usr/lib64/openmpi /usr/lib64/openmpi3
    PATH_SUFFIXES lib lib64
    NO_DEFAULT_PATH
    )

find_package_handle_standard_args(
    OpenMPI DEFAULT_MSG
    OpenMPI_LIBRARY_SHARED
    OpenMPI_INCLUDE_DIR
    )

set(OpenMPI_SHARED_LIBRARIES ${OpenMPI_LIBRARY_SHARED})
set(OpenMPI_INCLUDE_DIRS ${OpenMPI_INCLUDE_DIR})
set(OpenMPI_DEFINES "")

GET_FILENAME_COMPONENT(OpenMPI_LIB_DIR ${OpenMPI_LIBRARY_SHARED} PATH )
GET_FILENAME_COMPONENT(OpenMPI_DIR ${OpenMPI_INCLUDE_DIR} PATH )
#message(STATUS "OpenMPI OpenMPI_SHARED_LIBRARIES: " ${OpenMPI_SHARED_LIBRARIES})
#message(STATUS "OpenMPI OpenMPI_INCLUDE_DIR: " ${OpenMPI_INCLUDE_DIR})
#message(STATUS "OpenMPI OpenMPI_LIB_DIR: " ${OpenMPI_LIB_DIR})
#message(STATUS "OpenMPI OpenMPI_BIN_DIR: " ${OpenMPI_BIN_DIR})
message(STATUS "OpenMPI found: " ${OPENMPI_FOUND})
message(STATUS "OpenMPI location: " ${OpenMPI_LIB_DIR})

set(OpenMPI_DEFINES "HAVE_OPENMPI=${OPENMPI_FOUND}")


mark_as_advanced(
            OpenMPI_LIBRARY_SHARED 
            OpenMPI_INCLUDE_DIR
            OpenMPI_LIB_DIR
            OpenMPI_BIN_DIR
            )
