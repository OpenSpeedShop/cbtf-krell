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

add_definitions(
    -DXMLDIR="${CMAKE_INSTALL_PREFIX}/share/KrellInstitute/xml"
    -DLIBDIR="${CBTF_LIB_DIR}"
)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/base
    ${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/symtab
    ${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/perfdata
    ${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/events
    ${PROJECT_SOURCE_DIR}/messages/include
    ${PROJECT_SOURCE_DIR}/core/include
    ${PROJECT_SOURCE_DIR}/services/include
    ${MRNet_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
    ${XercesC_INCLUDE_DIRS}
    ${CBTF_INCLUDE_DIRS}
)

add_executable(testXDR
	testXDR.cpp
)

target_link_libraries(testXDR
    cbtf-core
    cbtf-core-mrnet
    cbtf-messages-base
    cbtf-messages-converters-base
    ${Boost_LIBRARIES}
    ${CBTF_LIBRARIES}
    ${XercesC_LIBRARIES}
    ${MRNet_LIBRARIES}
    ${CMAKE_DL_LIBS}
)

set_target_properties(testXDR PROPERTIES 
    COMPILE_DEFINITIONS "${MRNet_DEFINES}")

# At this time, do not install testXDR
#install(TARGETS testXDR
#    RUNTIME DESTINATION bin
#)

