################################################################################
# Copyright (c) 2014-2015 Krell Institute. All Rights Reserved.
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


#contribxmldir = $(datadir)/KrellInstitute/contrib/xml
#contribbindir = $(datadir)/KrellInstitute/contrib/bin
#contribplugindir = $(datadir)/KrellInstitute/contrib/lib

#add_definitions(
#    -DLIBRARY_DIR="${CMAKE_INSTALL_PREFIX}"
#    -DPLUGIN_DIR="KrellInstitute/Components"
#    -DLIB_DIR_NAME="lib${SUFFIX}"
#)

pstool_SOURCES = pstool.cpp

contribbin_PROGRAMS = pstool
pstool_CXXFLAGS = \
	-DBUILDDIR="\"$(abs_builddir)\"" \
	-DLIBDIR="\"$(libdir)\"" \
	-DTOPDIR="\"$(top_srcdir)\"" \
	-DXMLDIR="\"$(contribxmldir)\"" \
	@BOOST_CPPFLAGS@ \
	@CBTF_CPPFLAGS@ @CBTF_XML_CPPFLAGS@ \
	@LIBXERCES_C_CPPFLAGS@ \
	@MRNET_CPPFLAGS@ 

pstool_LDFLAGS = \
	-L$(top_srcdir) \
	@BOOST_LDFLAGS@ \
	@CBTF_LDFLAGS@ @CBTF_XML_LDFLAGS@ \
	@BINUTILS_LDFLAGS@ \
	@LIBXERCES_C_LDFLAGS@ \
	@MRNET_LDFLAGS@ 

pstool_LDADD = \
	-lcbtf-xml \
	-lcbtf-mrnet \
	-lcbtf-core \
	@BOOST_PROGRAM_OPTIONS_LIB@ \
	@BOOST_FILESYSTEM_LIB@ \
	@BOOST_SYSTEM_LIB@ \
	@BOOST_THREAD_LIB@ \
	@CBTF_LIBS@ @CBTF_XML_LIBS@ \
	@BINUTILS_BFD_LIB@ \
	@LIBXERCES_C@ \
	@MRNET_LIBS@ 



set(OFFLINE_SOURCES
	monitor.c
)

add_library(pcsamp-collector-monitor-fileio MODULE
	collector.c
)

add_definitions(
	-DBUILDDIR="\"$(abs_builddir)\"" \
	-DLIBDIR="\"$(libdir)\"" \
	-DTOPDIR="\"$(top_srcdir)\"" \
	-DXMLDIR="\"$(contribxmldir)\"" \
        KrellInstitute/contrib/xml
)

include_directories(
	${PROJECT_SOURCE_DIR}/messages/include
	${PROJECT_SOURCE_DIR}/services/include
	${PROJECT_SOURCE_DIR}/services/collector
	${XercesC_INCLUDE_DIRS}
	${Boost_INCLUDE_DIRS}
	${MRNet_INCLUDE_DIRS}
	${CBTF_INCLUDE_DIRS}
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${LTDL_INCLUDE_DIR}
)

target_link_libraries(pcsamp-collector-monitor-fileio
	cbtf-services-common
	cbtf-services-monitor
	cbtf-services-offline
	cbtf-services-timer
	cbtf-services-collector-monitor-fileio
	${LibMonitor_SHARED_LIBRARIES}
	${LTDL_LIBRARY_SHARED}
	${CMAKE_DL_LIBS}
)

set_target_properties(pcsamp-collector-monitor-fileio PROPERTIES VERSION 1.1.0)
set_target_properties(pcsamp-collector-monitor-fileio PROPERTIES PREFIX "")


install(TARGETS pcsamp-collector-monitor-fileio
        LIBRARY DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors
)

add_library(pcsamp-collector-monitor-mrnet MODULE
	collector.c
)

add_definitions(
	-DCBTF_SERVICE_USE_OFFLINE
	-DCBTF_SERVICE_USE_MRNET
)

include_directories(
	${PROJECT_SOURCE_DIR}/messages/include
	${PROJECT_SOURCE_DIR}/services/include
	${PROJECT_SOURCE_DIR}/services/collector
	${LibMonitor_INCLUDE_DIRS}
	${MRNet_INCLUDE_DIRS}
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/perfdata
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/events
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/base
	${LTDL_INCLUDE_DIR}
)

target_link_libraries(pcsamp-collector-monitor-mrnet
	cbtf-services-common
	cbtf-services-monitor
	cbtf-services-offline
	cbtf-services-timer
	cbtf-services-collector-monitor-mrnet
	${LibMonitor_SHARED_LIBRARIES}
	${MRNet_LW_SHARED_LIBRARIES}
	${LTDL_LIBRARY_SHARED}
	${CMAKE_DL_LIBS}
)

set_target_properties(pcsamp-collector-monitor-mrnet PROPERTIES VERSION 1.1.0)
set_target_properties(pcsamp-collector-monitor-mrnet PROPERTIES PREFIX "")

install(TARGETS pcsamp-collector-monitor-mrnet
    LIBRARY DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors
)

add_library(pcsamp-collector-monitor-mrnet-mpi MODULE
	collector.c
)

add_definitions(
	-DCBTF_SERVICE_USE_OFFLINE
	-DCBTF_SERVICE_USE_MRNET
	-DCBTF_SERVICE_USE_MRNET_MPI
)

include_directories(
	${PROJECT_SOURCE_DIR}/messages/include
	${PROJECT_SOURCE_DIR}/services/include
	${PROJECT_SOURCE_DIR}/services/collector
	${LibMonitor_INCLUDE_DIRS}
	${MRNet_INCLUDE_DIRS}
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/perfdata
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/events
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/base
	${LTDL_INCLUDE_DIR}
)

target_link_libraries(pcsamp-collector-monitor-mrnet-mpi
	cbtf-services-common
	cbtf-services-monitor
	cbtf-services-offline
	cbtf-services-timer
	cbtf-services-data
	cbtf-services-collector-monitor-mrnet-mpi
	${LibMonitor_SHARED_LIBRARIES}
	${MRNet_LW_SHARED_LIBRARIES}
	${LTDL_LIBRARY_SHARED}
	${CMAKE_DL_LIBS}
)

set_target_properties(pcsamp-collector-monitor-mrnet-mpi PROPERTIES VERSION 1.1.0)
set_target_properties(pcsamp-collector-monitor-mrnet-mpi PROPERTIES PREFIX "")

install(TARGETS pcsamp-collector-monitor-mrnet-mpi
    LIBRARY DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors
)

install(TARGETS pcsamp-collector-monitor-mrnet
    LIBRARY DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors
)

add_library(pcsamp-monitor-mrnet-mpi STATIC
	collector.c
)

add_definitions(
	-DCBTF_SERVICE_USE_OFFLINE
	-DCBTF_SERVICE_USE_MRNET
	-DCBTF_SERVICE_USE_MRNET_MPI
)

include_directories(
	${PROJECT_SOURCE_DIR}/messages/include
	${PROJECT_SOURCE_DIR}/services/include
	${PROJECT_SOURCE_DIR}/services/collector
	${LibMonitor_INCLUDE_DIRS}
	${MRNet_INCLUDE_DIRS}
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/perfdata
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/events
	${CMAKE_CURRENT_BINARY_DIR}/../../../messages/src/base
	${LTDL_INCLUDE_DIR}
)

target_link_libraries(pcsamp-monitor-mrnet-mpi
	cbtf-services-common
	cbtf-services-monitor
	cbtf-services-offline
	cbtf-services-timer
	cbtf-services-data
	cbtf-services-collector-monitor-mrnet-mpi
	${LibMonitor_SHARED_LIBRARIES}
	${MRNet_LW_SHARED_LIBRARIES}
	${LTDL_LIBRARY_SHARED}
	${CMAKE_DL_LIBS}
)

set_target_properties(pcsamp-monitor-mrnet-mpi PROPERTIES VERSION 1.1.0)

install(TARGETS pcsamp-monitor-mrnet-mpi
    ARCHIVE DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors
)


set(contriblib_location ${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}/KrellInstitute/contrib/lib)
set(cbtflib_location ${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX})
configure_file(./ps.xml.in ps.xml @ONLY)



install(FILES ${CMAKE_CURRENT_BINARY_DIR}/pcsamp.xml
        DESTINATION share/KrellInstitute/xml)
