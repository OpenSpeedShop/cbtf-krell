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

set(SERVICES_BINUTILS_SOURCES
	GetInstrLength.c
)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${Binutils_INCLUDE_DIRS}
	${LTDL_INCLUDE_DIR}
	${PROJECT_SOURCE_DIR}/services/include
)

add_library(cbtf-services-binutils SHARED
	${SERVICES_BINUTILS_SOURCES}
)

target_link_libraries(cbtf-services-binutils
        -Wl,--no-as-needed
	${Bfd_SHARED_LIBRARY}
	${LTDL_LIBRARY_SHARED}
	${CMAKE_DL_LIBS}
)

set_target_properties(cbtf-services-binutils PROPERTIES VERSION 1.1.0)
# Work around problem defined by:
# https://stackoverflow.com/questions/11748035/binutils-bfd-h-wants-config-h-now
set_target_properties(cbtf-services-binutils PROPERTIES COMPILE_DEFINITIONS "PACKAGE=1;PACKAGE_VERSION=1")

install(TARGETS cbtf-services-binutils
	LIBRARY DESTINATION lib${LIB_SUFFIX}
)

# Install static version

add_library(cbtf-services-binutils-static STATIC
	${SERVICES_BINUTILS_SOURCES}
)

target_link_libraries(cbtf-services-binutils-static
        -Wl,--no-as-needed
	${Bfd_SHARED_LIBRARY}
	${LTDL_LIBRARY_SHARED}
	${CMAKE_DL_LIBS}
)

set_target_properties(cbtf-services-binutils-static PROPERTIES VERSION 1.1.0)
set_target_properties(cbtf-services-binutils-static PROPERTIES OUTPUT_NAME cbtf-services-binutils)

# Work around problem defined by:
# https://stackoverflow.com/questions/11748035/binutils-bfd-h-wants-config-h-now
set_target_properties(cbtf-services-binutils-static PROPERTIES COMPILE_DEFINITIONS "PACKAGE=1;PACKAGE_VERSION=1")

install(TARGETS cbtf-services-binutils-static
	ARCHIVE DESTINATION lib${LIB_SUFFIX}
)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")

