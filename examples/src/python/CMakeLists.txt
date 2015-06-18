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

GET_FILENAME_COMPONENT(PYTHON_LIB ${PYTHON_LIBRARY} PATH )

add_definitions(
    -DXMLDIR="${CMAKE_INSTALL_PREFIX}/share/KrellInstitute/xml"
    -DLIBDIR="${CBTF_LIB_DIR}"
    -DLIBPYTHON="${PYTHON_LIB}"
)


include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${PROJECT_SOURCE_DIR}/core/include
    ${MRNet_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
    ${PYTHON_INCLUDE_DIR}
    ${CBTF_INCLUDE_DIRS}
)

add_library(pyExample MODULE
    pyExample.cpp
    mrnetConverters.cpp
)

target_link_libraries(pyExample
    cbtf-core
    cbtf-core-mrnet
    ${Boost_LIBRARIES}
    boost_python
    ${Bfd_LIBRARY_SHARED}
    ${CBTF_LIBRARIES}
    ${XercesC_LIBRARIES}
    ${MRNet_LIBRARIES}
    ${PYTHON_LIBRARY}
    ${CMAKE_DL_LIBS}
)

set_target_properties(pyExample PROPERTIES PREFIX "")
set_target_properties(pyExample PROPERTIES COMPILE_DEFINITIONS "${MRNet_DEFINES}")


set(cbtfkrelldir ${CMAKE_INSTALL_PREFIX})
set(cbtfkrellxmldir ${CMAKE_INSTALL_PREFIX}/share/KrellInstitute/contrib/xml)
set(contriblib_location ${CMAKE_INSTALL_PREFIX}/share/KrellInstitute/contrib/Components)

install(TARGETS pyExample
    LIBRARY DESTINATION ${contriblib_location}
)

configure_file(pyexample.xml.in pyexample.xml @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/pyexample.xml
        DESTINATION ${cbtfkrellxmldir} )
