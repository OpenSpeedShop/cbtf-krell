################################################################################
# Copyright (c) 2014-2016 Krell Institute. All Rights Reserved.
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

if(Boost_LIBRARY_DIR)
    set(REAL_BOOST_LIBRARY_DIR ${Boost_LIBRARY_DIR})
elseif(Boost_LIBRARY_DIR_RELEASE)
    set(REAL_BOOST_LIBRARY_DIR ${Boost_LIBRARY_DIR_RELEASE})
endif()

# Set cbtftargetos for usage in cbtflink
# This setting gets overridden when we target the cray or other 
# compute node enabled platforms.  See CBTF_FILTERS_RUN_ON MATCHES "native")
# section  below.
#
set(cbtftargetos ${RUNTIME_PLATFORM})

if (BUILD_FE_USING_CN_RUNTIMES)
    # This is the common code section for building the front end using 
    # compute node runtime directories previously built.
    # We are setting up variables for use in runtime scripts used when running
    # CBTF components and tools for a particular compute node platform.
    #
    # The models are different on the Cray and Intel MIC, at this time.
    # The majority of the CBTF infrastructure runs natively on the compute
    # nodes of the Cray, but on the Intel MIC there is a set of x86_64 based
    # host processors that run the CBTF infrastructure.  This is represented
    # below by:  CBTF_FILTERS_RUN_ON MATCHES "native" for the Cray
    # and:       CBTF_FILTERS_RUN_ON MATCHES "host"   for the Intel MIC
    # The following settings are common to both platforms
    #
    # Set cbtfkCNlibdir to the compute node runtime based CBTF-KRELL library
    # directory for usage in cbtfrun, cbtflink, but NOT cbtf_mrnet_commnode
    set(cbtfkCNlibdir ${CBTF_KRELL_CN_RUNTIME_LIBDIR})
    # Set libmonitorlibdir for usage in cbtfrun and cbtflink
    set(libmonitorlibdir ${LIBMONITOR_CN_RUNTIME_LIBDIR})
    # Set libunwindlibdir for usage in cbtfrun and cbtflink
    set(libunwindlibdir ${LIBUNWIND_CN_RUNTIME_LIBDIR})
    # Set libpapilibdir for usage in cbtfrun and cbtflink
    set(libpapilibdir ${PAPI_CN_RUNTIME_LIBDIR})

    # Set libmrnetCNlibdir for usage in cbtfrun, cbtflink
    set(libmrnetCNlibdir ${MRNET_CN_RUNTIME_LIBDIR})

    if(${LTDL_DIR} MATCHES "/usr")
       # Set ltdllibdir for to empty string so we don't introduce /usr/lib or /usr/lib64 to the ld search path
       set(ltdllibdir "")
    else()
       # Set ltdllibdir for usage in cbtf_mrnet_commnode
       set(ltdllibdir ${LTDL_LIB_DIR})
    endif()

    if (CBTF_FILTERS_RUN_ON MATCHES "native")
        # This is currently the Cray case where the compute node versions of the
        # components are used on the Cray compute nodes.  So, we point scripts at
        # the compute node _CN_ versions of the libraries.  There may be
        # duplication in the settings of cbtfklibdir and cbtfkCNlibdir.  Also for
        # libmrnetlibdir and libmrnetCNlibdir.  This is to streamline the case for Intel MIC.
        #

        # Reset cbtftargetos for usage in cbtflink the compute node runtime platform
        set(cbtftargetos ${CN_RUNTIME_PLATFORM})

        # Set cbtfdir to the compute node runtime based CBTF for usage in cbtfrun and cbtflink
        set(cbtfdir ${CBTF_CN_RUNTIME_DIR})
        # Set cbtflibdir to the compute node runtime based CBTF library directory for usage in cbtfrun, cbtflink, and cbtf_mrnet_commnode
        set(cbtflibdir ${CBTF_CN_RUNTIME_LIBDIR})
        # Set cbtfkdir to the compute node runtime based CBTF-KRELL for usage in cbtfrun and cbtflink
        set(cbtfkdir ${CBTF_KRELL_CN_RUNTIME_DIR})
        # Set cbtfklibdir to the compute node runtime based CBTF-KRELL library directory for usage in cbtfrun, cbtflink, and cbtf_mrnet_commnode
        set(cbtfklibdir ${CBTF_KRELL_CN_RUNTIME_LIBDIR})
        # Set cbtfkCNdir to the compute node runtime based CBTF-KRELL for usage in cbtfrun and cbtflink
        set(cbtfkCNdir ${CBTF_KRELL_CN_RUNTIME_DIR})
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
        if((${BOOST_CN_RUNTIME_LIBDIR} MATCHES "/usr/lib") OR (${BOOST_CN_RUNTIME_LIBDIR} MATCHES "/usr/lib64"))
           # Set boostlibdir for to empty string so we don't introduce /usr/lib or /usr/lib64 to the ld search path
           set(boostlibdir "")
        else()
           # Set boostlibdir for usage in cbtf_mrnet_commnode
           set(boostlibdir ${BOOST_CN_RUNTIME_LIBDIR})
        endif()
    elseif (CBTF_FILTERS_RUN_ON MATCHES "host")
        # Set cbtfdir for usage in cbtf_libcbtf_mrnet_backend
        set(cbtfdir ${CBTF_DIR})
        # Set cbtflibdir for usage in cbtf_libcbtf_mrnet_backend and cbtf_mrnet_commnode
        set(cbtflibdir ${CBTF_LIB_DIR})
        # Set cbtfkdir for usage in cbtfrun and cbtflink
        set(cbtfkdir ${CMAKE_INSTALL_PREFIX})
        # Set cbtfklibdir for usage in cbtfrun, cbtflink, and cbtf_mrnet_commnode
        set(cbtfklibdir ${CMAKE_INSTALL_LIBDIR})
        # Set cbtfkCNdir to the compute node runtime based CBTF-KRELL for usage in cbtfrun and cbtflink
        set(cbtfkCNdir ${CBTF_KRELL_CN_RUNTIME_DIR})
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
        if((${REAL_BOOST_LIBRARY_DIR} MATCHES "/usr/lib") OR (${REAL_BOOST_LIBRARY_DIR} MATCHES "/usr/lib64"))
           # Set boostlibdir for to empty string so we don't introduce /usr/lib or /usr/lib64 to the ld search path
           set(boostlibdir "")
        else()
           # Set boostlibdir for usage in cbtf_mrnet_commnode
           set(boostlibdir ${REAL_BOOST_LIBRARY_DIR})
        endif()
    endif()
else()
    # Set cbtfdir for usage in cbtf_libcbtf_mrnet_backend
    set(cbtfdir ${CBTF_DIR})
    # Set cbtflibdir for usage in cbtf_libcbtf_mrnet_backend and cbtf_mrnet_commnode
    set(cbtflibdir ${CBTF_LIB_DIR})
    # Set cbtfkdir for usage in cbtfrun and cbtflink
    set(cbtfkdir ${CMAKE_INSTALL_PREFIX})
    # Set cbtfklibdir for usage in cbtf_mrnet_commnode
    set(cbtfklibdir ${CMAKE_INSTALL_LIBDIR})
    # Set cbtfkCNdir to the runtime based CBTF-KRELL for usage in cbtfrun and cbtflink
    set(cbtfkCNdir ${CMAKE_INSTALL_PREFIX})
    # Set cbtfkCNlibdir to the compute node runtime based CBTF-KRELL library
    # directory for usage in cbtfrun, cbtflink, but NOT cbtf_mrnet_commnode
    set(cbtfkCNlibdir ${CMAKE_INSTALL_LIBDIR})
    # Set libmonitorlibdir for usage in cbtfrun and cbtflink
    set(libmonitorlibdir ${LibMonitor_LIB_DIR})
    # Set libunwindlibdir for usage in cbtfrun and cbtflink
    set(libunwindlibdir ${LibUnwind_LIB_DIR})
    # Set libpapilibdir for usage in cbtfrun and cbtflink
    set(libpapilibdir ${Papi_LIB_DIR})
    # Set libmrnetdir for usage in cbtf_mrnet_commnode
    set(libmrnetdir ${MRNet_DIR})
    # Set libmrnetlibdir for usage in cbtf_libcbtf_mrnet_backend, and cbtf_mrnet_commnode
    set(libmrnetlibdir ${MRNet_LIB_DIR})
    # Set libmrnetCNlibdir for usage in cbtfrun, cbtflink
    set(libmrnetCNlibdir ${MRNet_LIB_DIR})
    # Set dyninstlibdir for usage in cbtf_mrnet_commnode
    set(dyninstlibdir ${DyninstAPI_LIB_DIR})
    if(${XercesC_DIR} MATCHES "/usr")
       # Set xercesclibdir for to empty string so we don't introduce /usr/lib or /usr/lib64 to the ld search path
       set(xercesclibdir "")
    else()
       # Set xercesclibdir for usage in cbtf_mrnet_commnode
       set(xercesclibdir ${XercesC_LIB_DIR})
    endif()
    if((${REAL_BOOST_LIBRARY_DIR} MATCHES "/usr/lib") OR (${REAL_BOOST_LIBRARY_DIR} MATCHES "/usr/lib64"))

       # Set boostlibdir for to empty string so we don't introduce /usr/lib or /usr/lib64 to the ld search path
       set(boostlibdir "")
    else()
       # Set boostlibdir for usage in cbtf_mrnet_commnode
       set(boostlibdir ${REAL_BOOST_LIBRARY_DIR})
    endif()
    if(${LTDL_DIR} MATCHES "/usr")
       # Set xercesclibdir for to empty string so we don't introduce /usr/lib or /usr/lib64 to the ld search path
       set(ltdllibdir "")
    else()
       # Set xercesclibdir for usage in cbtf_mrnet_commnode
       set(ltdllibdir ${LTDL_LIB_DIR})
    endif()
endif()

# Set cbtfdefaultmpi for usage in cbtfrun
set(cbtfdefaultmpi ${DEFAULT_MPI_IMPL})

# Set these for usage in cbtfrun
set(machine_dependent_libdir lib${SUFFIX})
set(alt_machine_dependent_libdir lib)
set(cbtf_targetos ${RUNTIME_PLATFORM})

# Set these MPI directories for usage in cbtfrun
set(mpichdir ${MPICH_DIR})
set(mvapichdir ${MVAPICH_DIR})
set(mpich2dir ${MPICH2_DIR})
set(mvapich2dir ${MVAPICH2_DIR})
set(mptdir ${MPT_DIR})
set(openmpidir ${OPENMPI_DIR})


# Set libiompdir OpenMP directory for usage in cbtfrun
set(libiompdir ${LibIomp_DIR})

set(cbtfpythonmodules ${CMAKE_INSTALL_LIBDIR})

configure_file(cbtfrun.in cbtfrun @ONLY)
configure_file(cbtflink.in cbtflink @ONLY)
configure_file(cbtf_mrnet_commnode.in cbtf_mrnet_commnode @ONLY)
configure_file(cbtf_libcbtf_mrnet_backend.in cbtf_libcbtf_mrnet_backend @ONLY)
configure_file(cbtfdriver.in cbtfdriver @ONLY)
configure_file(cbtfprocesscsv.in cbtfprocesscsv @ONLY)
configure_file(cbtfcsvtool.py cbtfcsvtool.py @ONLY)
configure_file(cbtfpapi.py cbtfpapi.py @ONLY)
configure_file(cbtfextract.in cbtfextract @ONLY)

install(PROGRAMS
	${CMAKE_CURRENT_BINARY_DIR}/cbtfrun
	${CMAKE_CURRENT_BINARY_DIR}/cbtflink
	${CMAKE_CURRENT_BINARY_DIR}/cbtfdriver
	${CMAKE_CURRENT_BINARY_DIR}/cbtfprocesscsv
	${CMAKE_CURRENT_BINARY_DIR}/cbtfextract
	DESTINATION
	${CMAKE_INSTALL_PREFIX}/bin)

install(PROGRAMS
	${CMAKE_CURRENT_BINARY_DIR}/cbtfcsvtool.py
	${CMAKE_CURRENT_BINARY_DIR}/cbtfpapi.py
	DESTINATION
	${CMAKE_INSTALL_LIBDIR})

if(CMAKE_HOST_UNIX)
        # create driver links
        install(CODE " EXECUTE_PROCESS(COMMAND ln -sf cbtfdriver cbtfpcsamp
            WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/bin) ")
        install(CODE " EXECUTE_PROCESS(COMMAND ln -sf cbtfdriver cbtfusertime
            WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/bin) ")
        install(CODE " EXECUTE_PROCESS(COMMAND ln -sf cbtfdriver cbtfhwc
            WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/bin) ")
        install(CODE " EXECUTE_PROCESS(COMMAND ln -sf cbtfdriver cbtfhwcsamp
            WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/bin) ")
        install(CODE " EXECUTE_PROCESS(COMMAND ln -sf cbtfdriver cbtfhwctime
            WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/bin) ")
        install(CODE " EXECUTE_PROCESS(COMMAND ln -sf cbtfdriver cbtfio
            WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/bin) ")
        install(CODE " EXECUTE_PROCESS(COMMAND ln -sf cbtfdriver cbtfiot
            WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/bin) ")
        install(CODE " EXECUTE_PROCESS(COMMAND ln -sf cbtfdriver cbtfiop
            WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/bin) ")
        install(CODE " EXECUTE_PROCESS(COMMAND ln -sf cbtfdriver cbtfmpi
            WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/bin) ")
        install(CODE " EXECUTE_PROCESS(COMMAND ln -sf cbtfdriver cbtfmpit
            WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/bin) ")
        install(CODE " EXECUTE_PROCESS(COMMAND ln -sf cbtfdriver cbtfmpip
            WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/bin) ")
        install(CODE " EXECUTE_PROCESS(COMMAND ln -sf cbtfdriver cbtfomptp
            WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/bin) ")
        install(CODE " EXECUTE_PROCESS(COMMAND ln -sf cbtfdriver cbtfsummary
            WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/bin) ")

endif()

install(PROGRAMS
	${CMAKE_CURRENT_BINARY_DIR}/cbtf_mrnet_commnode
	${CMAKE_CURRENT_BINARY_DIR}/cbtf_libcbtf_mrnet_backend
	DESTINATION
	${CMAKE_INSTALL_PREFIX}/sbin)
