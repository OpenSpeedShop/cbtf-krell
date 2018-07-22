#Usage: wrap.py [-fgd] [-i pmpi_init] [-c mpicc_name] [-o file] wrapper.w [...]
# Python script for creating PMPI wrappers. Roughly follows the syntax of
#   the Argonne PMPI wrapper generator, with some enhancements.
# Options:"
#   -d             Just dump function declarations parsed out of mpi.h
#   -f             Generate fortran wrappers in addition to C wrappers.
#   -g             Generate reentry guards around wrapper functions.
#   -s             Skip writing #includes, #defines, and other front-matter (for non-C output).
#   -c exe         Provide name of MPI compiler (for parsing mpi.h).  Default is \'mpicc\'.
#   -I dir         Provide an extra include directory to use when parsing mpi.h.
#   -i pmpi_init   Specify proper binding for the fortran pmpi_init function.
#                  Default is \'pmpi_init_\'.  Wrappers compiled for PIC will guess the
#                  right binding automatically (use -DPIC when you compile dynamic libs).
#   -o file        Send output to a file instead of stdout.
#   -w             Do not print compiler warnings for deprecated MPI functions.
#                  This option will add macros around {{callfn}} to disable (and
#                  restore) the compilers diagnostic functions, if the compiler
#                  supports this functionality.


set(WRAP ${CMAKE_CURRENT_SOURCE_DIR}/wrap.py)
include(${PROJECT_SOURCE_DIR}/core/collectors/overview/mpi/WrapConfig.cmake)

set(COMMON_INCLUDE_DIRS
        ${PROJECT_SOURCE_DIR}/messages/include
        ${PROJECT_SOURCE_DIR}/services/include
	${PROJECT_SOURCE_DIR}/core/collectors/overview
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}/../../../../messages/src/perfdata
        ${CMAKE_CURRENT_BINARY_DIR}/../../../../messages/src/events
        ${CMAKE_CURRENT_BINARY_DIR}/../../../../messages/src/base
	${LibMonitor_INCLUDE_DIRS}
)

# BUILD MVAPICH2 wrapper libs

if (HAVE_MVAPICH2)
    set(MPI_C_COMPILER ${MVAPICH2_DIR}/bin/mpicc)
    add_wrapped_file(mvapich2-wrappers.c wrappers.w -f)

    set(OVERVIEW_MPIWRAPPER_SOURCES mvapich2-wrappers.c)
    add_library(overview-mvapich2-wrappers MODULE
        ${OVERVIEW_MPIWRAPPER_SOURCES})
    target_include_directories(overview-mvapich2-wrappers PUBLIC
        ${Mvapich2_INCLUDE_DIRS}
        ${COMMON_INCLUDE_DIRS}
    )
    target_link_libraries(overview-mvapich2-wrappers ${Mvapich2_SHARED_LIBRARIES} rt ${CMAKE_DL_LIBS})
    set_target_properties(overview-mvapich2-wrappers PROPERTIES PREFIX "")
    set_target_properties(overview-mvapich2-wrappers PROPERTIES
        COMPILE_DEFINITIONS "${Mvapich2_DEFINES}")

    install(TARGETS overview-mvapich2-wrappers
        LIBRARY DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors
    )
endif()

# BUILD OPENMPI wrapper libs
if (HAVE_OPENMPI)
    set(MPI_C_COMPILER ${OPENMPI_DIR}/bin/mpicc)
    add_wrapped_file(openmpi-wrappers.c wrappers.w -f)

    set(OVERVIEW_MPIWRAPPER_SOURCES openmpi-wrappers.c)
    add_library(overview-openmpi-wrappers MODULE
        ${OVERVIEW_MPIWRAPPER_SOURCES})
    target_include_directories(overview-openmpi-wrappers PUBLIC
        ${OpenMPI_INCLUDE_DIRS}
        ${COMMON_INCLUDE_DIRS}
    )
    target_link_libraries(overview-openmpi-wrappers ${OpenMPI_SHARED_LIBRARIES} rt ${CMAKE_DL_LIBS})
    set_target_properties(overview-openmpi-wrappers PROPERTIES PREFIX "")
    set_target_properties(overview-openmpi-wrappers PROPERTIES
        COMPILE_DEFINITIONS "${OpenMPI_DEFINES}")

    install(TARGETS overview-openmpi-wrappers
        LIBRARY DESTINATION lib${LIB_SUFFIX}/KrellInstitute/Collectors
    )
endif()

# TODO: build other mpi implmentation wrapper libs