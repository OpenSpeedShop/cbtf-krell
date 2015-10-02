################################################################################
# Copyright (c) 2014-2015 Krell Institute. All Rights Reserved.
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

project(cbtf-krell C CXX)

set(CBTF_KRELL_VERSION 1.5.0)
set(CBTF_KRELL_VERSION_MAJOR 1)
set(CBTF_KRELL_VERSION_MINOR 5)
set(CBTF_KRELL_VERSION_PATCH 0)
set(CBTF_KRELL_VERSION_TWEAK 0)
add_definitions(-DVERSION="${CBTF_KRELL_VERSION}")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)

include(CheckIncludeFile)
include(CheckIncludeFileCXX)
include(CheckFunctionExists)
include(CheckLibraryExists)

check_include_file("stdint.h" STDINT_H_FOUND)
if (STDINT_H_FOUND)
    add_definitions(-DHAVE_STDINT_H=1)
endif()

check_include_file("stat.h" STAT_H_FOUND)
if (STAT_H_FOUND)
    add_definitions(-DHAVE_STAT_H=1)
endif()

check_include_file("sys/stat.h" SYS_STAT_H_FOUND)
if (SYS_STAT_H_FOUND)
    add_definitions(-DHAVE_SYS_STAT_H=1)
endif()

check_include_file(dirent.h HAVE_DIRENT_H)
check_include_file(dlfcn.h HAVE_DLFCN_H)
check_include_file(errno.h HAVE_ERRNO_H)
check_include_file(execinfo.h HAVE_EXECINFO_H)
check_include_file(fcntl.h HAVE_FCNTL_H)
check_include_file(inttypes.h HAVE_INTTYPES_H)
check_include_file(limits.h HAVE_LIMITS_H)
check_include_file(link.h HAVE_LINK_H)
check_include_file(malloc.h HAVE_MALLOC_H)
check_include_file(malloc/malloc.h HAVE_MALLOC_MALLOC_H)
check_function_exists(fork HAVE_FORK)
find_library(DL_LIBRARIES NAMES dl)
if(DL_LIBRARIES)
  #set(HAVE_DLOPEN)
  add_definitions(-DHAVE_DLOPEN)
  add_definitions(-DHAVE_TARGET_DLOPEN)
else(DL_LIBRARIES)
  check_function_exists(dlopen HAVE_TARGET_DLOPEN)
  if (HAVE_TARGET_DLOPEN)
    add_definitions(-DHAVE_DLOPEN)
  endif()
  # If dlopen can be found without linking in dl then dlopen is part
  # of libc, so don't need to link extra libs.
  set(DL_LIBRARIES "")
endif(DL_LIBRARIES)

#-----------------------------------------------------------------------------
# This MACRO checks IF the symbol exists in the library and IF it
# does, allow it.
#-----------------------------------------------------------------------------
#MACRO (CHECK_LIBRARY_EXISTS_CONCAT LIBRARY SYMBOL VARIABLE)
#  CHECK_LIBRARY_EXISTS ("${LIBRARY}" ${SYMBOL} "" ${VARIABLE})
#ENDMACRO (CHECK_LIBRARY_EXISTS_CONCAT)

#cmake -DVARNAME:STRING=value

set(TLS_MODEL "implicit" CACHE STRING "cbtf-krell tls model: [implicit,explicit]")
#
#--------------------------------------------------------------------------------
# Handle the TLS options for building the runtimes
#    - explicit means the tool allocates and manages the thread local storage
#    - implicit means the tool uses the systems thread local storage
#--------------------------------------------------------------------------------
#
if (TLS_MODEL MATCHES "explicit")
    # Builder requsted explicit tls to be used in the runtimes and collectors
    set(TLS_DEFINES "USE_EXPLICIT_TLS" )
    message(STATUS "Build runtimes using explicit TLS model.")
elseif (TLS_MODEL MATCHES "implicit")
    # Builder requsted implicit tls to be used in the runtimes and collectors
    set(TLS_DEFINES "USE_IMPLICIT_TLS" )
    message(STATUS "Build runtimes using implicit TLS model.")
else()
    # Default to implicit tls in the runtimes and collectors
    set(TLS_DEFINES "USE_IMPLICIT_TLS" )
    message(STATUS "Build runtimes using implicit TLS model.")
endif()

set(TARGET_OS "" CACHE STRING "cbtf-krell target os: none or [bgq,bgp]")
#
#--------------------------------------------------------------------------------
# Handle the target specific build options for building the runtimes for specific
# platform compute nodes
#--------------------------------------------------------------------------------
#
if (TARGET_OS MATCHES "bgq")
    add_definitions(-DTARGET_OS=bgq)
    message(STATUS "Build runtimes for target bgq.")
elseif (TARGET_OS MATCHES "bgp")
    add_definitions(-DTARGET_OS=bgp)
    message(STATUS "Build runtimes for target bgp.")
elseif (TARGET_OS MATCHES "mic")
    add_definitions(-DTARGET_OS=mic)
    message(STATUS "Build runtimes for target mic.")
elseif (TARGET_OS MATCHES "cray-xk")
    set(TARGET_OS_CRAYXK 1)
    message(STATUS "Set TARGET_OS_CRAYXK for use internal to OpenSpeedShop.")
elseif (TARGET_OS MATCHES "cray-xe")
    set(TARGET_OS_CRAYXE 1)
    message(STATUS "Set TARGET_OS_CRAYXE for use internal to OpenSpeedShop.")
else()
    message(STATUS "Build runtimes for localhost.")
endif()
#
#--------------------------------------------------------------------------------
# Do find package calls for the dependency packages that are commonly needed for 
# cbtf.
#--------------------------------------------------------------------------------
#
find_package(Doxygen)
find_package(OpenMP)

find_package(OpenMPI)
find_package(Mpich)
find_package(Mpich2)
find_package(Mpt)
#find_package(MPI)
find_package(Mvapich2)
find_package(Mvapich)

find_package(PythonInterp)
find_package(PythonLibs)
find_package(LibElf)
find_package(LibDwarf)
find_package(Binutils)
find_package(Threads)
find_package(Sqlite3)
find_package(LTDL)
find_package(SymtabAPI)
find_package(DyninstAPI)

if (DYNINSTAPI_FOUND)
  set(HAVE_DYNINST "true")
endif()

find_package(LibMonitor)
find_package(LibUnwind)
find_package(Papi)

find_package(Boost 1.39.0 REQUIRED
    COMPONENTS date_time filesystem system thread unit_test_framework program_options
    )
find_package(CUDA)
find_package(MRNet 4.0.0)
find_package(XercesC 3.0)

find_package(LibIomp)

message(STATUS "LIBIOMP_FOUND=${LIBIOMP_FOUND}")
if (LIBIOMP_FOUND)
  CHECK_LIBRARY_EXISTS(${LibIomp_LIBRARY_SHARED} ompt_init "" HAVE_OMPT) 
  message(STATUS "HAVE_OMPT=${HAVE_OMPT}")
  if (HAVE_OMPT)
    set(OMPT_DEFINES "HAVE_OMPT=1")
    set(OMPT_FOUND "true")
  else()
    set(OMPT_FOUND "false")
  endif()
endif()
#
#--------------------------------------------------------------------------------
# After looking for the DyninstAPI and SymtabAPI packages, reconsile that with
# whether or not the RESOLVE_SYMBOLS request by the builded can be met.
#--------------------------------------------------------------------------------
#
#On arm/aarch64 systems, dyninst/symtabapi are not ready so default to bfd
set(RESOLVE_SYMBOLS "symtabapi" CACHE STRING "resolve symbols using: [bfd,symtabapi]")

if (RESOLVE_SYMBOLS MATCHES "symtabapi")
    if(DYNINSTAPI_FOUND)
      message(STATUS "Resolve symbols using SymtabAPI.")
    else()
      message(STATUS "Resolve symbols using bfd. No Dyninst package was found.")
    endif()
elseif (RESOLVE_SYMBOLS MATCHES "bfd")
    message(STATUS "Resolve symbols using bfd.")
else()
    # No user specified choice for resolving symbol tables.  By default choose symtabapi, if available
    if(DYNINSTAPI_FOUND)
      message(STATUS "Resolve symbols using SymtabAPI.")
    else()
      message(STATUS "Resolve symbols using bfd.")
    endif()
endif()

#
#--------------------------------------------------------------------------------
# Handle MPI settings here.  Set the appropriate flags for the rest of the tool
# to use when building MPI related libraries (collectors).
#--------------------------------------------------------------------------------
#
set(all_mpi_names "")

if (OPENMPI_FOUND)
  # This is needed by the mpi collectors
  set(HAVE_OPENMPI "true")
  set(HAVE_MPI "true")
  set(DEFAULT_MPI_IMPL_NAME "openmpi")
  set(DEFAULT_MPI_IMPL "OPENMPI")
  set(all_mpi_names ${all_mpi_names} " openmpi ")
endif()

if (MPICH2_FOUND)
  # This is needed by the mpi collectors
  set(HAVE_MPICH2 "true")
  set(HAVE_MPI "true")
  set(DEFAULT_MPI_IMPL_NAME "mpich2")
  set(DEFAULT_MPI_IMPL "MPICH2")
  set(all_mpi_names ${all_mpi_names} " mpich2 ")
endif()

if (MPICH_FOUND)
  # This is needed by the mpi collectors
  set(HAVE_MPICH "true")
  set(HAVE_MPI "true")
  set(DEFAULT_MPI_IMPL_NAME "mpich")
  set(DEFAULT_MPI_IMPL "MPICH")
  set(all_mpi_names ${all_mpi_names} " mpich ")
endif()

if (MPT_FOUND)
  # This is needed by the mpi collectors
  set(HAVE_MPT "true")
  set(HAVE_MPI "true")
  set(DEFAULT_MPI_IMPL_NAME "mpt")
  set(DEFAULT_MPI_IMPL "MPT")
  set(all_mpi_names ${all_mpi_names} " mpt ")
endif()

if (MVAPICH_FOUND)
  # This is needed by the mpi collectors
  set(HAVE_MVAPICH "true")
  set(HAVE_MPI "true")
  set(DEFAULT_MPI_IMPL_NAME "mvapich")
  set(DEFAULT_MPI_IMPL "MVAPICH")
  set(all_mpi_names ${all_mpi_names} " mvapich ")
endif()

if (MVAPICH2_FOUND)
  # This is needed by the mpi collectors
  set(HAVE_MVAPICH2 "true")
  set(HAVE_MPI "true")
  set(DEFAULT_MPI_IMPL_NAME "mvapich2")
  set(DEFAULT_MPI_IMPL "MVAPICH2")
  set(all_mpi_names ${all_mpi_names} " mvapich2 ")
endif()

set(ALL_MPI_IMPL_NAMES ${all_mpi_names})
#
#--------------------------------------------------------------------------------
# Try to determine where the libraries created by this build should be stored.
# The choices are lib or lib64.  Set the appropriate variables based on checking
# the system processor type.
#--------------------------------------------------------------------------------
#
if(CMAKE_SYSTEM_PROCESSOR MATCHES "ppc64*")
    set(CMAKE_LIBRARY_PATH ${CMAKE_INSTALL_PREFIX}/lib64)
    set(LIB_SUFFIX 64)
    set(SUFFIX 64)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "ppc*")
    set(CMAKE_LIBRARY_PATH ${CMAKE_INSTALL_PREFIX}/lib)
elseif(EXISTS /usr/lib64)
    set(LIB_SUFFIX 64)
    set(SUFFIX 64)
    set(CMAKE_LIBRARY_PATH ${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX})
endif()
#
#--------------------------------------------------------------------------------
# Look for the base CBTF package which is required in order for this build to be
# successful.
#--------------------------------------------------------------------------------
#
find_package(CBTF)

#
#--------------------------------------------------------------------------------
# Build the major components for cbtf-krell if CBTF is found
# Required external packages are checked for in the sub-directories' cmake files
#--------------------------------------------------------------------------------
#

if(CBTF_FOUND)
  add_subdirectory(messages)
  add_subdirectory(services)
  add_subdirectory(core)
  add_subdirectory(examples)
  add_subdirectory(test)
endif()

#
#--------------------------------------------------------------------------------
# Create the package configuration file for the CBTF-Krell project so that other
# external builds/builders can find the libraries and binaries created by this build.
#--------------------------------------------------------------------------------
#
set(INCLUDE_INSTALL_DIR include )
set(LIBRARY_INSTALL_DIR lib${SUFFIX} )

include(CMakePackageConfigHelpers)
configure_package_config_file(CBTF-KrellConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/CBTF-KrellConfig.cmake
    INSTALL_DESTINATION ${LIB_INSTALL_DIR}/CBTF-Krell/cmake
    PATH_VARS INCLUDE_INSTALL_DIR LIBRARY_INSTALL_DIR)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/CBTF-KrellConfigVersion.cmake
    VERSION ${CBTF_KRELL_VERSION}
    COMPATIBILITY SameMajorVersion )
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/CBTF-KrellConfig.cmake
              ${CMAKE_CURRENT_BINARY_DIR}/CBTF-KrellConfigVersion.cmake
    DESTINATION share/CBTF-Krell/cmake )


