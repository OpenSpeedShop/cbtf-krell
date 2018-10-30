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
#
#Usage: wrap.py [-fgd] [-i pmpi_init] [-c mpicc_name] [-o file] wrapper.w [...]
# Python script for creating PMPI wrappers. Roughly follows the syntax of
#   the Argonne PMPI wrapper generator, with some enhancements.
# Options:"
#   -d             Just dump function declarations parsed out of mpi.h
#   -f             Generate fortran wrappers in addition to C wrappers.
#   -g             Generate reentry guards around wrapper functions.
#   -s             Skip writing #includes, #defines, and other front-matter (for non-C output).
#   -c exe         Provide name of MPI compiler (for parsing mpi.h).  Default is \'mpicc\'.
#   -I dir         Provide an extra include directory to use when parsing mpi.h.
#   -i pmpi_init   Specify proper binding for the fortran pmpi_init function.
#                  Default is \'pmpi_init_\'.  Wrappers compiled for PIC will guess the
#                  right binding automatically (use -DPIC when you compile dynamic libs).
#   -o file        Send output to a file instead of stdout.
#   -w             Do not print compiler warnings for deprecated MPI functions.
#                  This option will add macros around {{callfn}} to disable (and
#                  restore) the compilers diagnostic functions, if the compiler
#                  supports this functionality.

set(WRAP ${CMAKE_CURRENT_SOURCE_DIR}/wrap.py)
include(${PROJECT_SOURCE_DIR}/core/collectors/overview/mpi/WrapConfig.cmake)

set(COMMON_INCLUDE_DIRS
	${PROJECT_SOURCE_DIR}/messages/include
	${PROJECT_SOURCE_DIR}/services/include
	${PROJECT_SOURCE_DIR}/core/collectors/overview
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../../../../messages/src/perfdata
	${CMAKE_CURRENT_BINARY_DIR}/../../../../messages/src/events
	${CMAKE_CURRENT_BINARY_DIR}/../../../../messages/src/base
	${LibMonitor_INCLUDE_DIRS}
)

# BUILD MVAPICH2 wrapper libs

if (HAVE_MVAPICH2)
    set(MPI_C_COMPILER ${MVAPICH2_DIR}/bin/mpicc)
    add_wrapped_file(mvapich2-wrappers.c wrappers.w -f)

    set(OVERVIEW_MPIWRAPPER_SOURCES mvapich2-wrappers.c)
    add_library(overview-mvapich2-wrappers MODULE
	${OVERVIEW_MPIWRAPPER_SOURCES})
    target_include_directories(overview-mvapich2-wrappers PUBLIC
	${Mvapich2_INCLUDE_DIRS}
	${COMMON_INCLUDE_DIRS}
    )
    target_link_libraries(overview-mvapich2-wrappers ${Mvapich2_SHARED_LIBRARIES} rt ${CMAKE_DL_LIBS})
    set_target_properties(overview-mvapich2-wrappers PROPERTIES PREFIX "")
    set_target_properties(overview-mvapich2-wrappers PROPERTIES
	COMPILE_DEFINITIONS "${Mvapich2_DEFINES}")

    install(TARGETS overview-mvapich2-wrappers
	LIBRARY DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors
    )
endif()

# BUILD OPENMPI wrapper libs
if (HAVE_OPENMPI)
    if (EXISTS ${OpenMPI_BIN_DIR}/mpicc )
       set(MPI_C_COMPILER ${OpenMPI_BIN_DIR}/mpicc)
    else()
       set(MPI_C_COMPILER ${OPENMPI_DIR}/bin/mpicc)
    endif()

    add_wrapped_file(openmpi-wrappers.c wrappers.w -f)

    set(OVERVIEW_MPIWRAPPER_SOURCES openmpi-wrappers.c)
    add_library(overview-openmpi-wrappers MODULE
	${OVERVIEW_MPIWRAPPER_SOURCES})
    target_include_directories(overview-openmpi-wrappers PUBLIC
	${OpenMPI_INCLUDE_DIRS}
	${COMMON_INCLUDE_DIRS}
    )
    target_link_libraries(overview-openmpi-wrappers ${OpenMPI_SHARED_LIBRARIES} rt ${CMAKE_DL_LIBS})
    set_target_properties(overview-openmpi-wrappers PROPERTIES PREFIX "")
    set_target_properties(overview-openmpi-wrappers PROPERTIES
	COMPILE_DEFINITIONS "${OpenMPI_DEFINES}")

    install(TARGETS overview-openmpi-wrappers
	LIBRARY DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors
    )
endif()


# BUILD MPT wrapper libs
if (HAVE_MPT)
    set(MPI_C_COMPILER ${MPT_DIR}/bin/mpicc)
    add_wrapped_file(mpt-wrappers.c wrappers.w -f)

    set(OVERVIEW_MPIWRAPPER_SOURCES mpt-wrappers.c)
    add_library(overview-mpt-wrappers MODULE
	${OVERVIEW_MPIWRAPPER_SOURCES})
    target_include_directories(overview-mpt-wrappers PUBLIC
	${Mpt_INCLUDE_DIRS}
	${COMMON_INCLUDE_DIRS}
    )
    target_link_libraries(overview-mpt-wrappers ${Mpt_SHARED_LIBRARIES} rt ${CMAKE_DL_LIBS})
    set_target_properties(overview-mpt-wrappers PROPERTIES PREFIX "")
    set_target_properties(overview-mpt-wrappers PROPERTIES
	COMPILE_DEFINITIONS "${Mpt_DEFINES}")

    install(TARGETS overview-mpt-wrappers
	LIBRARY DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors
    )
endif()


# BUILD MPICH wrapper libs

if (HAVE_MPICH AND EXISTS(${Mpich_BIN_DIR}/mpicc))
    # Here we use Mpich_BIN_DIR instead of MPICH_DIR/bin/mpicc 
    # because mpich is installed in various different ways
    # where mpicc is not always in the simple/standard install
    # paths that over mpi implementations are.
    set(MPI_C_COMPILER ${Mpich_BIN_DIR}/mpicc)
    add_wrapped_file(mpich-wrappers.c wrappers.w -f)

    set(OVERVIEW_MPIWRAPPER_SOURCES mpich-wrappers.c)
    add_library(overview-mpich-wrappers MODULE
	${OVERVIEW_MPIWRAPPER_SOURCES})
    target_include_directories(overview-mpich-wrappers PUBLIC
	${Mpich_INCLUDE_DIRS}
	${COMMON_INCLUDE_DIRS}
    )
    target_link_libraries(overview-mpich-wrappers ${Mpich_SHARED_LIBRARIES} rt ${CMAKE_DL_LIBS})
    set_target_properties(overview-mpich-wrappers PROPERTIES PREFIX "")
    set_target_properties(overview-mpich-wrappers PROPERTIES
	COMPILE_DEFINITIONS "${Mpich_DEFINES}")

    install(TARGETS overview-mpich-wrappers
	LIBRARY DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors
    )
endif()

# TODO: build other mpi implmentation wrapper libs
