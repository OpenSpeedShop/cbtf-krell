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

set(cbtfinstalldir ${CMAKE_INSTALL_PREFIX})
set(cbtfdir ${CBTF_DIR})
set(cbtfkdir ${CMAKE_INSTALL_PREFIX})
set(cbtflibdir ${CBTF_LIB_DIR})
set(cbtfklibdir ${CMAKE_LIBRARY_PATH})
set(cbtfruncmd ${CMAKE_INSTALL_PREFIX}/bin/cbtfrun)
set(cbtfdefaultmpi ${DEFAULT_MPI_IMPL})

set(libmonitordir ${LibMonitor_DIR})
set(libmonitorlibdir ${LibMonitor_LIB_DIR})
set(libunwinddir ${LibUnwind_DIR})
set(libunwindlibdir ${LibUnwind_LIB_DIR})
set(papidir ${Papi_DIR})
set(libpapidir ${Papi_DIR})
set(papilibdir ${Papi_LIB_DIR})
set(libmrnetdir ${MRNet_DIR})
set(libmrnetlibdir ${MRNet_LIB_DIR})
set(xercescdir ${XercesC_DIR})
set(krootinstalldir ${XercesC_DIR})
set(xercesclibdir ${XercesC_LIB_DIR})

GET_FILENAME_COMPONENT(Boost_Directory_Path ${Boost_INCLUDE_DIR} PATH )
set(boostinstalldir ${Boost_Directory_Path})
set(boostlibdir ${Boost_LIBRARY_DIR})

set(machine_dependent_libdir lib${SUFFIX})
set(machinelibdir lib${SUFFIX})
set(alt_machine_dependent_libdir lib)
set(mpichdir ${MPICH_DIR})
set(mvapichdir ${MVAPICH_DIR})
set(mpich2dir ${MPICH2_DIR})
set(mvapich2dir ${MVAPICH2_DIR})
set(mptdir ${MPT_DIR})
set(openmpidir ${OPENMPI_DIR})
set(openmpilibdir ${OpenMPI_LIB_DIR})
set(target_os ${TARGET_OS})

set(libiompdir ${LibIomp_DIR})
set(libiomplibdir ${LibIomp_LIB_DIR})

configure_file(cbtfrun.in cbtfrun @ONLY)
configure_file(cbtflink.in cbtflink @ONLY)
configure_file(cbtf_mrnet_commnode.in cbtf_mrnet_commnode @ONLY)
configure_file(cbtf_libcbtf_mrnet_backend.in cbtf_libcbtf_mrnet_backend @ONLY)

install(PROGRAMS
	${CMAKE_CURRENT_BINARY_DIR}/cbtfrun
	${CMAKE_CURRENT_BINARY_DIR}/cbtflink
	DESTINATION
	${CMAKE_INSTALL_PREFIX}/bin)

install(PROGRAMS
	${CMAKE_CURRENT_BINARY_DIR}/cbtf_mrnet_commnode
	${CMAKE_CURRENT_BINARY_DIR}/cbtf_libcbtf_mrnet_backend
	DESTINATION
	${CMAKE_INSTALL_PREFIX}/sbin)
