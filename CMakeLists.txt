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

set(CBTF_KRELL_VERSION 1.6.0)
set(CBTF_KRELL_VERSION_MAJOR 1)
set(CBTF_KRELL_VERSION_MINOR 6)
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
set(RUNTIME_TARGET_OS "" CACHE STRING "cbtf-krell target os: none or [bgq,bgp]")
#
#--------------------------------------------------------------------------------
# Handle the target specific build options for building the runtimes for specific
# platform compute nodes
#--------------------------------------------------------------------------------
#
if (TARGET_OS MATCHES "bgq")
    add_definitions(-DTARGET_OS=bgq)
    message(STATUS "Build runtimes for target bgq platform.")
elseif (TARGET_OS MATCHES "bgp")
    add_definitions(-DTARGET_OS=bgp)
    message(STATUS "Build runtimes for target bgp platform.")
elseif (TARGET_OS MATCHES "mic")
    add_definitions(-DTARGET_OS=mic)
    message(STATUS "Build runtimes for target mic platform.")
elseif (TARGET_OS MATCHES "cray")
    set(TARGET_OS_CRAY 1)
    message(STATUS "Set TARGET_OS_CRAY for use internal to OpenSpeedShop.")
    message(STATUS "Build runtimes for target cray platform.")
elseif (TARGET_OS MATCHES "cray-xk")
    set(TARGET_OS_CRAYXK 1)
    message(STATUS "Set TARGET_OS_CRAYXK for use internal to OpenSpeedShop.")
    message(STATUS "Build runtimes for target cray-xk platform.")
elseif (TARGET_OS MATCHES "cray-xe")
    set(TARGET_OS_CRAYXE 1)
    message(STATUS "Set TARGET_OS_CRAYXE for use internal to OpenSpeedShop.")
    message(STATUS "Build runtimes for target cray-xe platform.")
else()
    message(STATUS "Build runtimes for localhost.")
endif()
#
#--------------------------------------------------------------------------------
# In the case of building the front-end to a targeted compute node version of
# cbtf-krell we need to know what platform the target is, in order to generate
# the correct cbtfAutoTopology file.  In the case of the cray there are special
# considerations to account for.  One of these is the ALPS_XT_NID value.
#--------------------------------------------------------------------------------
#
if (RUNTIME_TARGET_OS MATCHES "bgq")
    add_definitions(-DRUNTIME_TARGET_OS=bgq)
    message(STATUS "Indicate that runtimes are for target bgq platform.")
elseif (RUNTIME_TARGET_OS MATCHES "bgp")
    add_definitions(-DRUNTIME_TARGET_OS=bgp)
    message(STATUS "Indicate that runtimes are for target bgp platform.")
elseif (RUNTIME_TARGET_OS MATCHES "mic")
    add_definitions(-DRUNTIME_TARGET_OS=mic)
    message(STATUS "Indicate that runtimes are for target mic platform.")
elseif (RUNTIME_TARGET_OS MATCHES "cray")
    add_definitions(-DRUNTIME_TARGET_OS_CRAY=1)
    message(STATUS "Indicate that runtimes are for target cray platform.")
elseif (RUNTIME_TARGET_OS MATCHES "cray-xk")
    add_definitions(-DRUNTIME_TARGET_OS_CRAY=1)
    message(STATUS "Indicate that runtimes are for target cray (-xk) platform.")
elseif (RUNTIME_TARGET_OS MATCHES "cray-xe")
    add_definitions(-DRUNTIME_TARGET_OS_CRAY=1)
    message(STATUS "Indicate that runtimes are for target cray (-xe) platform.")
else()
    message(STATUS "Indicate that runtimes are for localhost.")
endif()
#
# Are we only building the runtime portions of CBTF-KRELL, that is not building the client
# portions?  RUNTIME_ONLY indicates that we want to only build the compute node portions
# of cbtf-krell.
#
if (RUNTIME_ONLY MATCHES "true")
    message(STATUS "Build runtime libraries and components only for targetted platform.")
else()
    message(STATUS "Build full cbtf-krell libraries, components, and clients.")
endif()
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
# Handle the case where we are building a FRONT-END version of cbtf-krell that
# interfaces with a compute node targeted backend version of cbtf-krell.
#
# We determine if that is the case if we find a target cbtf-krell define specified
# for the cmake build.   If not, we proceed to build the vanilla cluster like version
# of cbtf-krell.  If there is a target cbtf-krell define specified then we proceed 
# with looking for the other compute node libraries that need to be specified in
# core/scripts when building cbtfrun, cbtflink, cbtf_libcbtf_mrnet_commnode, and
# cbtf_mrnet_commnode.
#
set(BUILD_TARGETED_FE "false" CACHE STRING "Is this a special front-end build where the targeted components are passed to the build in addition to the FE related components." )
#
#--------------------------------------------------------------------------------
# If CBTF_KRELL_CN_RUNTIME_DIR exists, then we have built a separate version of the portion of
# CBTF-KRELL that runs with the application: runtime libraries and data collectors.
# This is done/needed when the processor type of the compute node (where the application
# executes) is different from the front-end node (where the CBTF-KRELL client 
# components run).  We call that building targeted runtimes, and this is accomplished
# by using the "-DRUNTIME_ONLY" cmake argument in a separate build.
#--------------------------------------------------------------------------------
message(STATUS "BEGIN TARGETED COMPONENT CHECKS, BUILD_TARGETED_FE=${BUILD_TARGETED_FE}.")

if (NOT "${CBTF_KRELL_CN_RUNTIME_DIR}" STREQUAL "")
    if(EXISTS "${CBTF_KRELL_CN_RUNTIME_DIR}" AND IS_DIRECTORY "${CBTF_KRELL_CN_RUNTIME_DIR}")
        # This setting of BUILD_TARGETED_FE will persist if all the compute node versions of the component are found
        # One component failure will set the BUILD_TARGETED_FE flag false and the build will fail.
        set (BUILD_TARGETED_FE "true")
        if( EXISTS "${CBTF_KRELL_CN_RUNTIME_DIR}/lib${LIB_SUFFIX}" AND IS_DIRECTORY "${CBTF_KRELL_CN_RUNTIME_DIR}/lib${LIB_SUFFIX}" AND EXISTS "${CBTF_KRELL_CN_RUNTIME_DIR}/lib${LIB_SUFFIX}/libcbtf-core.so" )
            set (CBTF_KRELL_CN_RUNTIME_LIBDIR  ${CBTF_KRELL_CN_RUNTIME_DIR}/lib${LIB_SUFFIX})
        elseif ( EXISTS "${CBTF_KRELL_CN_RUNTIME_DIR}/lib" AND IS_DIRECTORY "${CBTF_KRELL_CN_RUNTIME_DIR}/lib" AND EXISTS "${CBTF_KRELL_CN_RUNTIME_DIR}/lib/libcbtf-core.so" )
            set (CBTF_KRELL_CN_RUNTIME_LIBDIR  ${CBTF_KRELL_CN_RUNTIME_DIR}/lib)
        else()
            # Override the settings we had up until now.  We need to find all the targeted libraries to succeed.
            set (BUILD_TARGETED_FE "false")
        endif()
    else()
        set (BUILD_TARGETED_FE "false")
    endif()
else()
    set (BUILD_TARGETED_FE "false")
endif()


#
# Check to see if we have a frontend build that references a targeted cbtf-krell build
# by verifying the necessary compute node packages required to create the proper 
# references in the cbtfrun, cbtflink, cbtf_libcbtf_mrnet_backend and cbtf_mrnet_commnode.
#

#
# Look for cbtf target libraries
#
if (NOT "${CBTF_CN_RUNTIME_DIR}" STREQUAL "")
    if(EXISTS ${CBTF_CN_RUNTIME_DIR} AND IS_DIRECTORY "${CBTF_CN_RUNTIME_DIR}")
        if( EXISTS ${CBTF_CN_RUNTIME_DIR}/lib${LIB_SUFFIX} AND IS_DIRECTORY "${CBTF_CN_RUNTIME_DIR}/lib${LIB_SUFFIX}" AND EXISTS "${CBTF_CN_RUNTIME_DIR}/lib${LIB_SUFFIX}/libcbtf.so")
           # CBTF_CN_RUNTIME_DIR/lib64 was recognized.")
           set (CBTF_CN_RUNTIME_LIBDIR ${CBTF_CN_RUNTIME_DIR}/lib${LIB_SUFFIX} CACHE PATH "Location of compute node cbtf libraries.")
           message(STATUS "CBTF_CN_RUNTIME_DIR/lib64 was recognized.")
        elseif ( EXISTS ${CBTF_CN_RUNTIME_DIR}/lib AND IS_DIRECTORY "${CBTF_CN_RUNTIME_DIR}/lib" AND EXISTS "${CBTF_CN_RUNTIME_DIR}/lib/libcbtf.so")
           # CBTF_CN_RUNTIME_DIR/lib was recognized.")
           set (CBTF_CN_RUNTIME_LIBDIR ${CBTF_CN_RUNTIME_DIR}/lib CACHE PATH "Location of compute node cbtf libraries.")
           message(STATUS "CBTF_CN_RUNTIME_DIR/lib was recognized.")
        else()
           # Override the settings we had up until now.  We need to find all the targeted libraries to succeed.
           set (BUILD_TARGETED_FE "false")
        endif()
    else()
        set (BUILD_TARGETED_FE "false")
    endif()
else()
    set (BUILD_TARGETED_FE "false")
endif()

#
# Look for libmonitor libraries
#
if (NOT "${LIBMONITOR_CN_RUNTIME_DIR}" STREQUAL "")
    if(EXISTS ${LIBMONITOR_CN_RUNTIME_DIR} AND IS_DIRECTORY "${LIBMONITOR_CN_RUNTIME_DIR}")
        if( EXISTS ${LIBMONITOR_CN_RUNTIME_DIR}/lib${LIB_SUFFIX} AND IS_DIRECTORY "${LIBMONITOR_CN_RUNTIME_DIR}/lib${LIB_SUFFIX}" AND EXISTS "${LIBMONITOR_CN_RUNTIME_DIR}/lib${LIB_SUFFIX}/libmonitor.so")
           # LIBMONITOR_CN_RUNTIME_DIR/lib64 was recognized.
           set (LIBMONITOR_CN_RUNTIME_LIBDIR ${LIBMONITOR_CN_RUNTIME_DIR}/lib${LIB_SUFFIX} CACHE PATH "Location of compute node libmonitor libraries.")
           message(STATUS "LIBMONITOR_CN_RUNTIME_DIR/lib64 was recognized.")
        elseif ( EXISTS ${LIBMONITOR_CN_RUNTIME_DIR}/lib AND IS_DIRECTORY "${LIBMONITOR_CN_RUNTIME_DIR}/lib" AND EXISTS "${LIBMONITOR_CN_RUNTIME_DIR}/lib/libmonitor.so")
           # LIBMONITOR_CN_RUNTIME_DIR/lib was recognized.
           set (LIBMONITOR_CN_RUNTIME_LIBDIR ${LIBMONITOR_CN_RUNTIME_DIR}/lib CACHE PATH "Location of compute node libmonitor libraries.")
           message(STATUS "LIBMONITOR_CN_RUNTIME_DIR/lib was recognized.")
        else()
           # Override the settings we had up until now.  We need to find all the targeted libraries to succeed.
           set (BUILD_TARGETED_FE "false")
        endif()
    else()
        set (BUILD_TARGETED_FE "false")
    endif()
else()
    set (BUILD_TARGETED_FE "false")
endif()

#
# Look for libunwind target libraries
#
if (NOT "${LIBUNWIND_CN_RUNTIME_DIR}" STREQUAL "")
    if(EXISTS ${LIBUNWIND_CN_RUNTIME_DIR} AND IS_DIRECTORY "${LIBUNWIND_CN_RUNTIME_DIR}")
        if( EXISTS ${LIBUNWIND_CN_RUNTIME_DIR}/lib${LIB_SUFFIX} AND IS_DIRECTORY "${LIBUNWIND_CN_RUNTIME_DIR}/lib${LIB_SUFFIX}" AND EXISTS "${LIBUNWIND_CN_RUNTIME_DIR}/lib${LIB_SUFFIX}/libunwind.so")
           # LIBUNWIND_CN_RUNTIME_DIR/lib64 was recognized
           set (LIBUNWIND_CN_RUNTIME_LIBDIR ${LIBUNWIND_CN_RUNTIME_DIR}/lib${LIB_SUFFIX} CACHE PATH "Location of compute node libunwind libraries.")
           message(STATUS "LIBUNWIND_CN_RUNTIME_DIR/lib64 was recognized.")
        elseif ( EXISTS ${LIBUNWIND_CN_RUNTIME_DIR}/lib AND IS_DIRECTORY "${LIBUNWIND_CN_RUNTIME_DIR}/lib" AND EXISTS "${LIBUNWIND_CN_RUNTIME_DIR}/lib/libunwind.so")
           # LIBUNWIND_CN_RUNTIME_DIR/lib was recognized
           set (LIBUNWIND_CN_RUNTIME_LIBDIR ${LIBUNWIND_CN_RUNTIME_DIR}/lib CACHE PATH "Location of compute node libunwind libraries.")
           message(STATUS "LIBUNWIND_CN_RUNTIME_DIR/lib was recognized.")
        else()
           # Override the settings we had up until now.  We need to find all the targeted libraries to succeed.
           set (BUILD_TARGETED_FE "false")
        endif()
    else()
        set (BUILD_TARGETED_FE "false")
    endif()
else()
    set (BUILD_TARGETED_FE "false")
endif()

#
# Look for papi target libraries
#
if (NOT "${PAPI_CN_RUNTIME_DIR}" STREQUAL "")
    if(EXISTS ${PAPI_CN_RUNTIME_DIR} AND IS_DIRECTORY "${PAPI_CN_RUNTIME_DIR}")
        if( EXISTS ${PAPI_CN_RUNTIME_DIR}/lib${LIB_SUFFIX} AND IS_DIRECTORY "${PAPI_CN_RUNTIME_DIR}/lib${LIB_SUFFIX}" AND EXISTS "${PAPI_CN_RUNTIME_DIR}/lib${LIB_SUFFIX}/libpapi.so")
           # PAPI_CN_RUNTIME_DIR/lib64 was recognized
           set (PAPI_CN_RUNTIME_LIBDIR ${PAPI_CN_RUNTIME_DIR}/lib${LIB_SUFFIX} CACHE PATH "Location of compute node papi libraries.")
           message(STATUS "PAPI_CN_RUNTIME_DIR/lib64 was recognized.")
        elseif ( EXISTS ${PAPI_CN_RUNTIME_DIR}/lib AND IS_DIRECTORY "${PAPI_CN_RUNTIME_DIR}/lib" AND EXISTS "${PAPI_CN_RUNTIME_DIR}/lib/libpapi.so")
           # PAPI_CN_RUNTIME_DIR/lib was recognized
           set (PAPI_CN_RUNTIME_LIBDIR ${PAPI_CN_RUNTIME_DIR}/lib CACHE PATH "Location of compute node papi libraries.")
           message(STATUS "PAPI_CN_RUNTIME_DIR/lib was recognized.")
        else()
           # Override the settings we had up until now.  We need to find all the targeted libraries to succeed.
           set (BUILD_TARGETED_FE "false")
        endif()
    else()
        set (BUILD_TARGETED_FE "false")
    endif()
else()
    set (BUILD_TARGETED_FE "false")
endif()

#
# Look for mrnet target libraries
#
if (NOT "${MRNET_CN_RUNTIME_DIR}" STREQUAL "")
    if(EXISTS ${MRNET_CN_RUNTIME_DIR} AND IS_DIRECTORY "${MRNET_CN_RUNTIME_DIR}")
        if( EXISTS ${MRNET_CN_RUNTIME_DIR}/lib${LIB_SUFFIX} AND IS_DIRECTORY "${MRNET_CN_RUNTIME_DIR}/lib${LIB_SUFFIX}" AND EXISTS "${MRNET_CN_RUNTIME_DIR}/lib${LIB_SUFFIX}/libmrnet.so")
           # MRNET_CN_RUNTIME_DIR/lib64 was recognized
           set (MRNET_CN_RUNTIME_LIBDIR ${MRNET_CN_RUNTIME_DIR}/lib${LIB_SUFFIX} CACHE PATH "Location of compute node mrnet libraries.")
           message(STATUS "MRNET_CN_RUNTIME_DIR/lib64 was recognized.")
        elseif ( EXISTS ${MRNET_CN_RUNTIME_DIR}/lib AND IS_DIRECTORY "${MRNET_CN_RUNTIME_DIR}/lib" AND EXISTS "${MRNET_CN_RUNTIME_DIR}/lib/libmrnet.so")
           # MRNET_CN_RUNTIME_DIR/lib was recognized
           set (MRNET_CN_RUNTIME_LIBDIR ${MRNET_CN_RUNTIME_DIR}/lib CACHE PATH "Location of compute node mrnet libraries.")
           message(STATUS "MRNET_CN_RUNTIME_DIR/lib was recognized.")
        else()
           # Override the settings we had up until now.  We need to find all the targeted libraries to succeed.
           set (BUILD_TARGETED_FE "false")
        endif()
    else()
        set (BUILD_TARGETED_FE "false")
    endif()
else()
    set (BUILD_TARGETED_FE "false")
endif()

#
# Look for boost target libraries
#
if (NOT "${BOOST_CN_RUNTIME_DIR}" STREQUAL "")
    if(EXISTS ${BOOST_CN_RUNTIME_DIR} AND IS_DIRECTORY "${BOOST_CN_RUNTIME_DIR}")
        if( EXISTS ${BOOST_CN_RUNTIME_DIR}/lib${LIB_SUFFIX} AND IS_DIRECTORY "${BOOST_CN_RUNTIME_DIR}/lib${LIB_SUFFIX}" AND EXISTS "${BOOST_CN_RUNTIME_DIR}/lib${LIB_SUFFIX}/libboost_system.so")
           # BOOST_CN_RUNTIME_DIR/lib64 was recognized
           set (BOOST_CN_RUNTIME_LIBDIR ${BOOST_CN_RUNTIME_DIR}/lib${LIB_SUFFIX} CACHE PATH "Location of compute node boost libraries.")
           message(STATUS "BOOST_CN_RUNTIME_DIR/lib64 was recognized.")
        elseif( EXISTS ${BOOST_CN_RUNTIME_DIR}/lib AND IS_DIRECTORY "${BOOST_CN_RUNTIME_DIR}/lib" AND EXISTS "${BOOST_CN_RUNTIME_DIR}/lib/libboost_system.so")
           # BOOST_CN_RUNTIME_DIR/lib was recognized
           set (BOOST_CN_RUNTIME_LIBDIR ${BOOST_CN_RUNTIME_DIR}/lib CACHE PATH "Location of compute node boost libraries.")
           message(STATUS "BOOST_CN_RUNTIME_DIR/lib was recognized.")
        else()
           # Override the settings we had up until now.  We need to find all the targeted libraries to succeed.
           set (BUILD_TARGETED_FE "false")
        endif()
    else()
        set (BUILD_TARGETED_FE "false")
    endif()
else()
    set (BUILD_TARGETED_FE "false")
endif()

message(STATUS "BUILD_TARGETED_FE is TRUE, look for XERCESC compute node components.")
#
# Look for xercesc target libraries
#
if (NOT "${XERCESC_CN_RUNTIME_DIR}" STREQUAL "")
    if(EXISTS ${XERCESC_CN_RUNTIME_DIR} AND IS_DIRECTORY "${XERCESC_CN_RUNTIME_DIR}")
        if( EXISTS ${XERCESC_CN_RUNTIME_DIR}/lib${LIB_SUFFIX} AND IS_DIRECTORY "${XERCESC_CN_RUNTIME_DIR}/lib${LIB_SUFFIX}" AND EXISTS "${XERCESC_CN_RUNTIME_DIR}/lib${LIB_SUFFIX}/libxerces-c.so")
           # XERCESC_CN_RUNTIME_DIR/lib64 was recognized
           set (XERCESC_CN_RUNTIME_LIBDIR ${XERCESC_CN_RUNTIME_DIR}/lib${LIB_SUFFIX} CACHE PATH "Location of compute node xercesc libraries.")
           message(STATUS "XERCESC_CN_RUNTIME_DIR/lib64 was recognized.")
        elseif ( EXISTS ${XERCESC_CN_RUNTIME_DIR}/lib AND IS_DIRECTORY "${XERCESC_CN_RUNTIME_DIR}/lib" AND EXISTS "${XERCESC_CN_RUNTIME_DIR}/lib${LIB_SUFFIX}/libxerces-c.so")
           # XERCESC_CN_RUNTIME_DIR/lib was recognized
           set (XERCESC_CN_RUNTIME_LIBDIR ${XERCESC_CN_RUNTIME_DIR}/lib CACHE PATH "Location of compute node xercesc libraries.")
           message(STATUS "XERCESC_CN_RUNTIME_DIR/lib was recognized.")
        else()
           # Override the settings we had up until now.  We need to find all the targeted libraries to succeed.
           set (BUILD_TARGETED_FE "false")
        endif()
    else()
        set (BUILD_TARGETED_FE "false")
    endif()
else()
    set (BUILD_TARGETED_FE "false")
endif()

#
# Look for dyninst target libraries
#
if (NOT "${DYNINST_CN_RUNTIME_DIR}" STREQUAL "")
    if(EXISTS ${DYNINST_CN_RUNTIME_DIR} AND IS_DIRECTORY "${DYNINST_CN_RUNTIME_DIR}")
        if( EXISTS ${DYNINST_CN_RUNTIME_DIR}/lib${LIB_SUFFIX} AND IS_DIRECTORY "${DYNINST_CN_RUNTIME_DIR}/lib${LIB_SUFFIX}" AND EXISTS "${DYNINST_CN_RUNTIME_DIR}/lib${LIB_SUFFIX}/libdyninstAPI.so")
           # DYNINST_CN_RUNTIME_DIR/lib64 was recognized
           set (DYNINST_CN_RUNTIME_LIBDIR ${DYNINST_CN_RUNTIME_DIR}/lib${LIB_SUFFIX} CACHE PATH "Location of compute node boost libraries.")
           message(STATUS "DYNINST_CN_RUNTIME_DIR/lib64 was recognized.")
        elseif ( EXISTS ${DYNINST_CN_RUNTIME_DIR}/lib AND IS_DIRECTORY "${DYNINST_CN_RUNTIME_DIR}/lib" AND EXISTS "${DYNINST_CN_RUNTIME_DIR}/lib${LIB_SUFFIX}/libdyninstAPI.so")
           # DYNINST_CN_RUNTIME_DIR/lib was recognized
           set (DYNINST_CN_RUNTIME_LIBDIR ${DYNINST_CN_RUNTIME_DIR}/lib CACHE PATH "Location of compute node boost libraries.")
           message(STATUS "DYNINST_CN_RUNTIME_DIR/lib was recognized.")
        else()
           # Override the settings we had up until now.  We need to find all the targeted libraries to succeed.
           set (BUILD_TARGETED_FE "false")
        endif()
    else()
        set (BUILD_TARGETED_FE "false")
    endif()
else()
    set (BUILD_TARGETED_FE "false")
endif()

message(STATUS "*** END of TARGETED COMPONENT CHECKS, BUILD_TARGETED_FE=${BUILD_TARGETED_FE}.")

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
find_package(CrayAlps)

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
    if (RUNTIME_ONLY MATCHES "true")
        add_subdirectory(messages)
        add_subdirectory(services)
        add_subdirectory(core)
    else ()
        add_subdirectory(messages)
        if (BUILD_TARGETED_FE MATCHES "false")
            add_subdirectory(services)
        endif()
        add_subdirectory(core)
        add_subdirectory(test)
    endif ()
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

