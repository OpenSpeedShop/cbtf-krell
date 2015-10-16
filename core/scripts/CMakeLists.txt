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

if (BUILD_TARGETED_FE)
    # Set cbtfdir to the compute node runtime based CBTF for usage in cbtfrun and cbtflink
    set(cbtfdir ${CBTF_CN_RUNTIME_DIR})
    # Set cbtflibdir to the compute node runtime based CBTF library directory for usage in cbtfrun, cbtflink, and cbtf_mrnet_commnode
    set(cbtflibdir ${CBTF_CN_RUNTIME_LIBDIR})
    # Set cbtfkdir to the compute node runtime based CBTF-KRELL for usage in cbtfrun and cbtflink
    set(cbtfkdir ${CBTF_KRELL_CN_RUNTIME_DIR})
    # Set cbtfklibdir to the compute node runtime based CBTF-KRELL library directory for usage in cbtfrun, cbtflink, and cbtf_mrnet_commnode
    set(cbtfklibdir ${CBTF_KRELL_CN_RUNTIME_LIBDIR})
    # Set libmonitorlibdir for usage in cbtfrun and cbtflink
    set(libmonitorlibdir ${LIBMONITOR_CN_RUNTIME_LIBDIR})
    # Set libunwindlibdir for usage in cbtfrun and cbtflink
    set(libunwindlibdir ${LIBUNWIND_CN_RUNTIME_LIBDIR})
    # Set libpapilibdir for usage in cbtfrun and cbtflink
    set(libpapilibdir ${PAPI_CN_RUNTIME_LIBDIR})
    # Set libmrnetdir for usage in cbtf_mrnet_commnode
    set(libmrnetdir ${MRNET_CN_RUNTIME_DIR})
    # Set libmrnetlibdir for usage in cbtfrun, cbtflink, cbtf_libcbtf_mrnet_backend, and cbtf_mrnet_commnode
    set(libmrnetlibdir ${MRNET_CN_RUNTIME_LIBDIR})
    # Set dyninstlibdir for usage in cbtf_mrnet_commnode
    set(dyninstlibdir ${DYNINST_CN_RUNTIME_LIBDIR})
    if(${XercesC_DIR} MATCHES "/usr")
       # Set xercesclibdir for to empty string so we don't introduce /usr/lib or /usr/lib64 to the ld search path
       set(xercesclibdir "")
    else()
       # Set xercesclibdir for usage in cbtf_mrnet_commnode
       set(xercesclibdir ${XERCESC_CN_RUNTIME_LIBDIR})
    endif()
    GET_FILENAME_COMPONENT(Boost_Directory_Path ${Boost_INCLUDE_DIR} PATH )
    if(Boost_Directory_Path MATCHES "/usr")
       # Set boostlibdir for to empty string so we don't introduce /usr/lib or /usr/lib64 to the ld search path
       set(boostlibdir "")
    else()
       # Set boostlibdir for usage in cbtf_mrnet_commnode
       set(boostlibdir ${BOOST_CN_RUNTIME_LIBDIR})
    endif()
else()
    # Set cbtfdir for usage in cbtf_libcbtf_mrnet_backend
    set(cbtfdir ${CBTF_DIR})
    # Set cbtflibdir for usage in cbtf_libcbtf_mrnet_backend and cbtf_mrnet_commnode
    set(cbtflibdir ${CBTF_LIB_DIR})
    # Set cbtfkdir for usage in cbtfrun and cbtflink
    set(cbtfkdir ${CMAKE_INSTALL_PREFIX})
    # Set cbtfklibdir for usage in cbtfrun, cbtflink, and cbtf_mrnet_commnode
    set(cbtfklibdir ${CMAKE_LIBRARY_PATH})
    # Set libmonitorlibdir for usage in cbtfrun and cbtflink
    set(libmonitorlibdir ${LibMonitor_LIB_DIR})
    # Set libunwindlibdir for usage in cbtfrun and cbtflink
    set(libunwindlibdir ${LibUnwind_LIB_DIR})
    # Set libpapilibdir for usage in cbtfrun and cbtflink
    set(libpapilibdir ${Papi_LIB_DIR})
    # Set libmrnetdir for usage in cbtf_mrnet_commnode
    set(libmrnetdir ${MRNet_DIR})
    # Set libmrnetlibdir for usage in cbtfrun, cbtflink, cbtf_libcbtf_mrnet_backend, and cbtf_mrnet_commnode
    set(libmrnetlibdir ${MRNet_LIB_DIR})
    # Set dyninstlibdir for usage in cbtf_mrnet_commnode
    set(dyninstlibdir ${DyninstAPI_LIB_DIR})
    if(${XercesC_DIR} MATCHES "/usr")
       # Set xercesclibdir for to empty string so we don't introduce /usr/lib or /usr/lib64 to the ld search path
       set(xercesclibdir "")
    else()
       # Set xercesclibdir for usage in cbtf_mrnet_commnode
       set(xercesclibdir ${XercesC_LIB_DIR})
    endif()
    GET_FILENAME_COMPONENT(Boost_Directory_Path ${Boost_INCLUDE_DIR} PATH )
    if(Boost_Directory_Path MATCHES "/usr")
       # Set boostlibdir for to empty string so we don't introduce /usr/lib or /usr/lib64 to the ld search path
       set(boostlibdir "")
    else()
       # Set boostlibdir for usage in cbtf_mrnet_commnode
       set(boostlibdir ${Boost_LIBRARY_DIR})
    endif()
endif()



# Set cbtfdefaultmpi for usage in cbtfrun
set(cbtfdefaultmpi ${DEFAULT_MPI_IMPL})

# Set these for usage in cbtfrun
set(machine_dependent_libdir lib${SUFFIX})
set(alt_machine_dependent_libdir lib)

# Set these MPI directories for usage in cbtfrun
set(mpichdir ${MPICH_DIR})
set(mvapichdir ${MVAPICH_DIR})
set(mpich2dir ${MPICH2_DIR})
set(mvapich2dir ${MVAPICH2_DIR})
set(mptdir ${MPT_DIR})
set(openmpidir ${OPENMPI_DIR})

# Set cbtftargetos for usage in cbtflink
set(cbtftargetos ${TARGET_OS})


# Set libiompdir OpenMP directory for usage in cbtfrun
set(libiompdir ${LibIomp_DIR})

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
