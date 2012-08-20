################################################################################
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

add_library(CUDA SHARED
    CUDAToIO.cpp
    )

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PROJECT_BINARY_DIR}/messages/cuda
    ${PROJECT_SOURCE_DIR}/messages/src/base
    ${PROJECT_SOURCE_DIR}/messages/src/events
    ${PROJECT_SOURCE_DIR}/messages/src/perfdata
    ${PROJECT_SOURCE_DIR}/messages/src/symtab
    ${CBTF_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
    ${MRNet_INCLUDE_DIRS}
    )

target_link_libraries(CUDA
    ${CBTF_LIBRARIES}
    ${MRNet_LIBRARIES}
    )

set_target_properties(CUDA PROPERTIES PREFIX "")

install(TARGETS CUDA
    RUNTIME DESTINATION bin/KrellInstitute/Components
    LIBRARY DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Components
    ARCHIVE DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Components
    )
