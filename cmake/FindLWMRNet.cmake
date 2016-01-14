################################################################################
# Copyright (c) 2011-2016 Krell Institute. All Rights Reserved.
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

include(FindPackageHandleStandardArgs)

find_library(MRNet_MRNET_LW_SHARED_LIBRARY NAMES libmrnet_lightweight.so
    HINTS $ENV{MRNET_DIR}
    HINTS ${MRNET_DIR}
    PATH_SUFFIXES lib lib64
    )

find_library(MRNet_XPLAT_LW_SHARED_LIBRARY NAMES libxplat_lightweight.so
    HINTS $ENV{MRNET_DIR}
    HINTS ${MRNET_DIR}
    PATH_SUFFIXES lib lib64
    )

find_library(MRNet_MRNET_LW_STATIC_LIBRARY NAMES libmrnet_lightweight.a
    HINTS $ENV{MRNET_DIR}
    HINTS ${MRNET_DIR}
    PATH_SUFFIXES lib lib64
    )

find_library(MRNet_XPLAT_LW_STATIC_LIBRARY NAMES libxplat_lightweight.a
    HINTS $ENV{MRNET_DIR}
    HINTS ${MRNET_DIR}
    PATH_SUFFIXES lib lib64
    )

find_library(MRNet_MRNET_LWR_SHARED_LIBRARY NAMES libmrnet_lightweight_r.so
    HINTS $ENV{MRNET_DIR}
    HINTS ${MRNET_DIR}
    PATH_SUFFIXES lib lib64
    )

find_library(MRNet_XPLAT_LWR_SHARED_LIBRARY NAMES libxplat_lightweight_r.so
    HINTS $ENV{MRNET_DIR}
    HINTS ${MRNET_DIR}
    PATH_SUFFIXES lib lib64
    )

find_library(MRNet_MRNET_LWR_STATIC_LIBRARY NAMES libmrnet_lightweight_r.a
    HINTS $ENV{MRNET_DIR}
    HINTS ${MRNET_DIR}
    PATH_SUFFIXES lib lib64
    )

find_library(MRNet_LW_XPLAT_LWR_STATIC_LIBRARY NAMES libxplat_lightweight_r.a
    HINTS $ENV{MRNET_DIR}
    HINTS ${MRNET_DIR}
    PATH_SUFFIXES lib lib64
    )

find_path(MRNet_LW_INCLUDE_DIR mrnet_lightweight/MRNet.h 
    HINTS $ENV{MRNET_DIR}
    HINTS ${MRNET_DIR}
    PATH_SUFFIXES include
    )


set(MRNet_LW_SHARED_LIBRARIES ${MRNet_MRNET_LW_SHARED_LIBRARY} ${MRNet_XPLAT_LW_SHARED_LIBRARY})
set(MRNet_LW_STATIC_LIBRARIES ${MRNet_MRNET_LW_STATIC_LIBRARY} ${MRNet_XPLAT_LW_STATIC_LIBRARY})
set(MRNet_LWR_SHARED_LIBRARIES ${MRNet_MRNET_LWR_SHARED_LIBRARY} ${MRNet_XPLAT_LWR_SHARED_LIBRARY})
set(MRNet_LWR_STATIC_LIBRARIES ${MRNet_MRNET_LWR_STATIC_LIBRARY} ${MRNet_XPLAT_LWR_STATIC_LIBRARY})

set(MRNet_DEFINES "os_linux")

        
if(DEFINED MRNet_LW_INCLUDE_DIR)

    file(READ ${MRNet_LW_INCLUDE_DIR}/mrnet_lightweight/Types.h MRNet_LW_VERSION_FILE)
  
    string(REGEX REPLACE
        ".*#[ ]*define MRNET_VERSION_MAJOR[ ]+([0-9]+)\n.*" "\\1"
        MRNet_LW_VERSION_MAJOR ${MRNet_LW_VERSION_FILE}
        )
      
    string(REGEX REPLACE
        ".*#[ ]*define MRNET_VERSION_MINOR[ ]+([0-9]+)\n.*" "\\1"
        MRNet_LW_VERSION_MINOR ${MRNet_LW_VERSION_FILE}
        )
  
    string(REGEX REPLACE
        ".*#[ ]*define MRNET_VERSION_REV[ ]+([0-9]+)\n.*" "\\1"
        MRNet_LW_VERSION_PATCH ${MRNet_LW_VERSION_FILE}
        )
  
    set(MRNet_LW_VERSION_STRING 
        ${MRNet_LW_VERSION_MAJOR}.${MRNet_LW_VERSION_MINOR}.${MRNet_LW_VERSION_PATCH}
        )
  
    #message(STATUS "LWMRNet version: " ${MRNet_LW_VERSION_STRING})
    #message(STATUS "MRNet_FIND_VERSION version: " ${MRNet_FIND_VERSION})
    #message(STATUS "MRNet_LW_VERSION_STRING version: " ${MRNet_LW_VERSION_STRING})

    if(DEFINED MRNet_FIND_VERSION)
        if(${MRNet_LW_VERSION_STRING} VERSION_LESS ${MRNet_FIND_VERSION})

            set(LWMRNET_FOUND FALSE)

            if(DEFINED MRNet_LW_FIND_REQUIRED)
                message(FATAL_ERROR
                    "Could NOT find lightweight MRNet  (version < "
                    ${MRNet_FIND_VERSION} ")"
                    )
            else()
                message(STATUS
                    "Could NOT find lightweight MRNet  (version < " 
                    ${MRNet_FIND_VERSION} ")"
                    )
            endif()
 
        endif()
    endif()
  
    if(MRNet_LW_VERSION_STRING VERSION_LESS "4.0.0")

        # There was no lightweight support in these versions
        set(LWMRNET_FOUND FALSE)

    else()
      
        #
        # Find the MRNet 4 (and up) configuration header files. These are found
        # in the lib[64] subdirectory rather than the include subdirectory where
        # one would expect to find them...
        #
        GET_FILENAME_COMPONENT(MRNet_LW_LIB_DIR ${MRNet_MRNET_LWR_SHARED_LIBRARY} PATH )
        GET_FILENAME_COMPONENT(MRNet_LW_DIR ${MRNet_LW_INCLUDE_DIR} PATH )

        find_package_handle_standard_args(
            LWMRNet DEFAULT_MSG
            MRNet_MRNET_LW_SHARED_LIBRARY MRNet_XPLAT_LW_SHARED_LIBRARY
            MRNet_MRNET_LW_STATIC_LIBRARY MRNet_XPLAT_LW_STATIC_LIBRARY
            MRNet_MRNET_LWR_SHARED_LIBRARY MRNet_XPLAT_LWR_SHARED_LIBRARY
            MRNet_LW_INCLUDE_DIR
        )
            #MRNet_LW_XPLAT_CONFIG_INCLUDE_DIR
            #MRNet_LW_MRNET_CONFIG_INCLUDE_DIR

        find_path(
            MRNet_LW_MRNET_CONFIG_INCLUDE_DIR mrnet_config.h
            HINTS $ENV{MRNET_DIR}
            HINTS ${MRNET_DIR}
            PATH_SUFFIXES 
                lib/mrnet-${MRNet_LW_VERSION_STRING}/include
                lib64/mrnet-${MRNet_LW_VERSION_STRING}/include
            )

        if(NOT MRNet_LW_MRNET_CONFIG_INCLUDE_DIR)
            set(LWMRNET_FOUND FALSE)
            message(STATUS
                "Could NOT find the lightweight MRNet " ${MRNet_LW_VERSION_STRING}
                " mrnet configuration header file"
                )
        endif()

        find_path(
            MRNet_LW_XPLAT_CONFIG_INCLUDE_DIR xplat_config.h
            HINTS $ENV{MRNET_DIR}
            HINTS ${MRNET_DIR}
            PATH_SUFFIXES 
                lib/xplat-${MRNet_LW_VERSION_STRING}/include
                lib64/xplat-${MRNet_LW_VERSION_STRING}/include
            )

        if(NOT MRNet_LW_XPLAT_CONFIG_INCLUDE_DIR)
            set(LWMRNET_FOUND FALSE)
            message(STATUS
                "Could NOT find the lightweight MRNet " ${MRNet_LW_VERSION_STRING}
                " xplat configuration header files"
                )
        endif()

        message(STATUS "LWMRNet found: " ${LWMRNET_FOUND})

        set(MRNet_LW_INCLUDE_DIRS 
            ${MRNet_LW_INCLUDE_DIR} 
            ${MRNet_LW_XPLAT_CONFIG_INCLUDE_DIR} 
            ${MRNet_LW_MRNET_CONFIG_INCLUDE_DIR} )

        
        mark_as_advanced(
            MRNet_MRNET_LW_SHARED_LIBRARY
            MRNet_XPLAT_LW_SHARED_LIBRARY
            MRNet_MRNET_LW_STATIC_LIBRARY
            MRNet_XPLAT_LW_STATIC_LIBRARY
            MRNet_MRNET_LWR_SHARED_LIBRARY
            MRNet_XPLAT_LWR_SHARED_LIBRARY
            MRNet_MRNET_LWR_STATIC_LIBRARY
            MRNet_XPLAT_LWR_STATIC_LIBRARY
            MRNet_LWR_SHARED_LIBRARIES
            MRNet_LWR_STATIC_LIBRARIES
            MRNet_LW_SHARED_LIBRARIES
            MRNet_LW_STATIC_LIBRARIES
            MRNet_LW_INCLUDE_DIR
            MRNet_LW_DIR
            MRNet_LW_LIB_DIR
            MRNet_LW_XPLAT_CONFIG_INCLUDE_DIR
            MRNet_LW_MRNET_CONFIG_INCLUDE_DIR
    )
        
    endif()
      
endif()

