################################################################################
# Copyright (c) 2016-2019 Krell Institute. All Rights Reserved.
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

if (LIBMONITOR_FOUND)

if (${HAVE_OMPT50_TOOL})
set(CALLBACK_SOURCES
	OMPTcallbacks-50.c
)
else()
set(CALLBACK_SOURCES
	OMPTcallbacks.c
)
endif()

add_library(overview-ompt-callbacks MODULE
	${CALLBACK_SOURCES}
)

target_include_directories(overview-ompt-callbacks PUBLIC 
        ${Libtirpc_INCLUDE_DIRS}
	${LibMonitor_INCLUDE_DIRS}
	${LibIomp_INCLUDE_DIRS}
	${PROJECT_SOURCE_DIR}/messages/include
	${PROJECT_SOURCE_DIR}/services/include
	${PROJECT_SOURCE_DIR}/services/collector
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/../
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../../../../messages/src/perfdata
	${CMAKE_CURRENT_BINARY_DIR}/../../../../messages/src/events
	${CMAKE_CURRENT_BINARY_DIR}/../../../../messages/src/base
)

target_link_libraries(overview-ompt-callbacks
        -Wl,--no-as-needed
	${LibMonitor_SHARED_LIBRARIES}
	${LibIomp_SHARED_LIBRARIES}
	pthread
	rt
	${CMAKE_DL_LIBS}
)

set_target_properties(overview-ompt-callbacks PROPERTIES PREFIX "")
#
# These defines apply only for this target module.  
#
set_target_properties(overview-ompt-callbacks PROPERTIES
        COMPILE_DEFINITIONS "${TLS_DEFINES};${LibIomp_DEFINES};CBTF_SERVICE_USE_OFFLINE;__USE_GNU")

install(TARGETS overview-ompt-callbacks
        LIBRARY DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors
)

endif() # end of LIBMONITOR
endif() # end of non BUILD_FE_USING_CN_RUNTIMES section of code
