################################################################################
# Copyright (c) 2015 Krell Institute. All Rights Reserved.
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

add_definitions(${MRNet_DEFINES})

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}/../src/base
    ${CMAKE_CURRENT_BINARY_DIR}/../src/perfdata
    ${PROJECT_SOURCE_DIR}/messages/include
    ${PROJECT_SOURCE_DIR}/core/include
    ${MRNet_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
    ${CBTF_INCLUDE_DIRS}
    ${LTDL_INCLUDE_DIR}
    )

add_executable(testXDR
	testXDR.cpp
    )
target_link_libraries(testXDR
    ${LTDL_LIBRARY_SHARED}
    ${Boost_LIBRARIES}
    ${CBTF_LIBRARIES}
    cbtf-messages-base
    cbtf-messages-converters-base
    ${MRNet_LIBRARIES}
    ${CMAKE_DL_LIBS}
    )
install(TARGETS testXDR
    RUNTIME DESTINATION .
    )
