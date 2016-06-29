################################################################################
# Copyright (c) 2015-2016 Krell Institute. All Rights Reserved.
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

if (BUILD_FE_USING_CN_RUNTIMES MATCHES "true")
    add_definitions(
        -DXMLDIR="${CBTF_KRELL_CN_RUNTIME_DIR}/share/KrellInstitute/xml"
        -DLIBDIR="${CBTF_CN_RUNTIME_LIBDIR}"
    )
else()
    add_definitions(
        -DXMLDIR="${CMAKE_INSTALL_PREFIX}/share/KrellInstitute/xml"
        -DLIBDIR="${CBTF_LIB_DIR}"
    )
endif()

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${PROJECT_SOURCE_DIR}/core/include
    ${MRNet_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
    ${CBTF_INCLUDE_DIRS}
)

add_executable(daemonTool
	daemonTool.cpp
)

# Only link with specific Boost libraries needed rather than Boost_LIBRARIES
# which links all Boost libraries asked for in the cmake config COMPONENTS
# specification for Boost.
target_link_libraries(daemonTool
    cbtf-core
    cbtf-core-mrnet
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${Boost_THREAD_LIBRARY}
    ${Boost_PROGRAM_OPTIONS_LIBRARY}
    ${Bfd_LIBRARY_SHARED}
    ${CBTF_LIBRARIES}
    ${XercesC_LIBRARIES}
    ${MRNet_LIBRARIES}
    ${CMAKE_DL_LIBS}
)

set_target_properties(daemonTool PROPERTIES
        COMPILE_DEFINITIONS "${MRNet_DEFINES}")

install(TARGETS daemonTool
    RUNTIME DESTINATION bin
)
