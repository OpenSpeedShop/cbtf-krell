################################################################################
# Copyright (c) 2014-2019 Krell Institute. All Rights Reserved.
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

if (BUILD_FE_USING_CN_RUNTIMES)

else ()

#	mem_gotcha.c 
set(WRAPPER_SOURCES
	wrappers.c 
)

add_library(overview-memwrappers MODULE
	${WRAPPER_SOURCES}
)

#	${Gotcha_INCLUDE_DIR}
target_include_directories(overview-memwrappers PUBLIC
        ${Libtirpc_INCLUDE_DIRS}
	${PROJECT_SOURCE_DIR}/messages/include
	${PROJECT_SOURCE_DIR}/services/include
	${PROJECT_SOURCE_DIR}/services/collector
	${PROJECT_SOURCE_DIR}/core/collectors/overview
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../../../../messages/src/perfdata
	${CMAKE_CURRENT_BINARY_DIR}/../../../../messages/src/events
	${CMAKE_CURRENT_BINARY_DIR}/../../../../messages/src/base
)

#	${Gotcha_LIBRARY_SHARED}
target_link_libraries(overview-memwrappers
        -Wl,--no-as-needed
	pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(overview-memwrappers PROPERTIES PREFIX "")
set_target_properties(overview-memwrappers PROPERTIES
        COMPILE_DEFINITIONS "CBTF_SERVICE_USE_OFFLINE;__USE_GNU")

install(TARGETS overview-memwrappers
        LIBRARY DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors
)

endif() # end of non BUILD_FE_USING_CN_RUNTIMES section of code
