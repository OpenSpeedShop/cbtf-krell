################################################################################
# Copyright (c) 2014-2016 Krell Institute. All Rights Reserved.
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

include(CheckIncludeFile)

# The -DCMAKE_BUILD is a hack to inform the CBTF_GetStackTrace unwinder
# which is specific to the x86_64 platform how many signal handler frames
# to skip. For reasons unknown to me the autotools built CBTF_GetStackTrace
# has one less frame of overhead (4) than the cmake built version (5).
# See comments in GetStackTraceFromContext.c.
# TODO: restrict these as needed possibly using CMAKE_SYSTEM_PROCESSOR.
add_definitions(
	-DCMAKE_BUILD
)

if (RUNTIME_PLATFORM MATCHES "arm")
    # The unwind-tables entries where initially intended for arm and aarch64.
    # Only add these definitions when building for arm or aarch64
    add_definitions(
	-fasynchronous-unwind-tables
	-funwind-tables
    )
endif()

set(SERVICES_UNWIND_SOURCES
	GetStackTraceFromContext.c
)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${LTDL_INCLUDE_DIR}
	${PROJECT_SOURCE_DIR}/services/include
	${CMAKE_INSTALL_PREFIX}/include
	${LibUnwind_INCLUDE_DIRS}
)

add_library(cbtf-services-unwind SHARED
	${SERVICES_UNWIND_SOURCES}
)

target_link_libraries(cbtf-services-unwind
        -Wl,--no-as-needed
	${LibUnwind_SHARED_LIBRARIES}
	${LTDL_LIBRARY_SHARED}
	${CMAKE_DL_LIBS}
)

set_target_properties(cbtf-services-unwind PROPERTIES VERSION 1.1.0)
set_target_properties(cbtf-services-unwind PROPERTIES COMPILE_DEFINITIONS "${LibUnwind_DEFINES}")

install(TARGETS cbtf-services-unwind
	LIBRARY DESTINATION lib${LIB_SUFFIX}
)

# Install static version

add_library(cbtf-services-unwind-static STATIC
	${SERVICES_UNWIND_SOURCES}
)

target_link_libraries(cbtf-services-unwind-static
        -Wl,--no-as-needed
	${LibUnwind_STATIC_LIBRARIES}
	${LTDL_LIBRARY_SHARED}
	${CMAKE_DL_LIBS}
)

set_target_properties(cbtf-services-unwind-static PROPERTIES VERSION 1.1.0)
set_target_properties(cbtf-services-unwind-static PROPERTIES OUTPUT_NAME cbtf-services-unwind)
set_target_properties(cbtf-services-unwind-static PROPERTIES COMPILE_DEFINITIONS "${LibUnwind_DEFINES}")

install(TARGETS cbtf-services-unwind-static
	ARCHIVE DESTINATION lib${LIB_SUFFIX}
)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
