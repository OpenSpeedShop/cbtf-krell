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


set(CBTF_MESSAGES_PERFDATA_SOURCES
        ${CMAKE_CURRENT_BINARY_DIR}/DataHeader.h
        ${CMAKE_CURRENT_BINARY_DIR}/DataHeader.c
        ${CMAKE_CURRENT_BINARY_DIR}/PCSamp.h
        ${CMAKE_CURRENT_BINARY_DIR}/PCSamp.c
        ${CMAKE_CURRENT_BINARY_DIR}/PCSamp_data.h
        ${CMAKE_CURRENT_BINARY_DIR}/PCSamp_data.c
        ${CMAKE_CURRENT_BINARY_DIR}/Hwc.h
        ${CMAKE_CURRENT_BINARY_DIR}/Hwc.c
        ${CMAKE_CURRENT_BINARY_DIR}/Hwc_data.h
        ${CMAKE_CURRENT_BINARY_DIR}/Hwc_data.c
        ${CMAKE_CURRENT_BINARY_DIR}/Hwcsamp.h
        ${CMAKE_CURRENT_BINARY_DIR}/Hwcsamp.c
        ${CMAKE_CURRENT_BINARY_DIR}/Hwcsamp_data.h
        ${CMAKE_CURRENT_BINARY_DIR}/Hwcsamp_data.c
        ${CMAKE_CURRENT_BINARY_DIR}/Hwctime.h
        ${CMAKE_CURRENT_BINARY_DIR}/Hwctime.c
        ${CMAKE_CURRENT_BINARY_DIR}/Hwctime_data.h
        ${CMAKE_CURRENT_BINARY_DIR}/Hwctime_data.c
        ${CMAKE_CURRENT_BINARY_DIR}/IO.h
        ${CMAKE_CURRENT_BINARY_DIR}/IO.c
        ${CMAKE_CURRENT_BINARY_DIR}/IO_data.h
        ${CMAKE_CURRENT_BINARY_DIR}/IO_data.c
        ${CMAKE_CURRENT_BINARY_DIR}/Mem.h
        ${CMAKE_CURRENT_BINARY_DIR}/Mem.c
        ${CMAKE_CURRENT_BINARY_DIR}/Mem_data.h
        ${CMAKE_CURRENT_BINARY_DIR}/Mem_data.c
        ${CMAKE_CURRENT_BINARY_DIR}/Mpi.h
        ${CMAKE_CURRENT_BINARY_DIR}/Mpi.c
        ${CMAKE_CURRENT_BINARY_DIR}/Mpi_data.h
        ${CMAKE_CURRENT_BINARY_DIR}/Mpi_data.c
        ${CMAKE_CURRENT_BINARY_DIR}/Pthreads.h
        ${CMAKE_CURRENT_BINARY_DIR}/Pthreads.c
        ${CMAKE_CURRENT_BINARY_DIR}/Pthreads_data.h
        ${CMAKE_CURRENT_BINARY_DIR}/Pthreads_data.c
        ${CMAKE_CURRENT_BINARY_DIR}/Usertime.h
        ${CMAKE_CURRENT_BINARY_DIR}/Usertime.c
        ${CMAKE_CURRENT_BINARY_DIR}/Usertime_data.h
        ${CMAKE_CURRENT_BINARY_DIR}/Usertime_data.c
        ${CMAKE_CURRENT_BINARY_DIR}/Stats.h
        ${CMAKE_CURRENT_BINARY_DIR}/Stats.c
	DataHeader.x
	PCSamp.x
	PCSamp_data.x
	Hwc.x
	Hwc_data.x
	Hwcsamp.x
	Hwcsamp_data.x
	Hwctime.x
	Hwctime_data.x
	IO.x
	IO_data.x
	Mem.x
	Mem_data.x
	Mpi.x
	Mpi_data.x
	Pthreads.x
	Pthreads_data.x
	Usertime.x
	Usertime_data.x
	Stats.x
    )

set(CBTF_MESSAGES_CONVERTERS_PERFDATA_SOURCES
        ${CMAKE_CURRENT_BINARY_DIR}/DataHeader.h
        ${CMAKE_CURRENT_BINARY_DIR}/DataHeader.c
        ${CMAKE_CURRENT_BINARY_DIR}/PCSamp.h
        ${CMAKE_CURRENT_BINARY_DIR}/PCSamp.c
        ${CMAKE_CURRENT_BINARY_DIR}/PCSamp_data.h
        ${CMAKE_CURRENT_BINARY_DIR}/PCSamp_data.c
        ${CMAKE_CURRENT_BINARY_DIR}/Hwc.h
        ${CMAKE_CURRENT_BINARY_DIR}/Hwc.c
        ${CMAKE_CURRENT_BINARY_DIR}/Hwc_data.h
        ${CMAKE_CURRENT_BINARY_DIR}/Hwc_data.c
        ${CMAKE_CURRENT_BINARY_DIR}/Hwcsamp.h
        ${CMAKE_CURRENT_BINARY_DIR}/Hwcsamp.c
        ${CMAKE_CURRENT_BINARY_DIR}/Hwcsamp_data.h
        ${CMAKE_CURRENT_BINARY_DIR}/Hwcsamp_data.c
        ${CMAKE_CURRENT_BINARY_DIR}/Hwctime.h
        ${CMAKE_CURRENT_BINARY_DIR}/Hwctime.c
        ${CMAKE_CURRENT_BINARY_DIR}/Hwctime_data.h
        ${CMAKE_CURRENT_BINARY_DIR}/Hwctime_data.c
        ${CMAKE_CURRENT_BINARY_DIR}/IO.h
        ${CMAKE_CURRENT_BINARY_DIR}/IO.c
        ${CMAKE_CURRENT_BINARY_DIR}/IO_data.h
        ${CMAKE_CURRENT_BINARY_DIR}/IO_data.c
        ${CMAKE_CURRENT_BINARY_DIR}/Mem.h
        ${CMAKE_CURRENT_BINARY_DIR}/Mem.c
        ${CMAKE_CURRENT_BINARY_DIR}/Mem_data.h
        ${CMAKE_CURRENT_BINARY_DIR}/Mem_data.c
        ${CMAKE_CURRENT_BINARY_DIR}/Mpi.h
        ${CMAKE_CURRENT_BINARY_DIR}/Mpi.c
        ${CMAKE_CURRENT_BINARY_DIR}/Mpi_data.h
        ${CMAKE_CURRENT_BINARY_DIR}/Mpi_data.c
        ${CMAKE_CURRENT_BINARY_DIR}/Pthreads.h
        ${CMAKE_CURRENT_BINARY_DIR}/Pthreads.c
        ${CMAKE_CURRENT_BINARY_DIR}/Pthreads_data.h
        ${CMAKE_CURRENT_BINARY_DIR}/Pthreads_data.c
        ${CMAKE_CURRENT_BINARY_DIR}/Usertime.h
        ${CMAKE_CURRENT_BINARY_DIR}/Usertime.c
        ${CMAKE_CURRENT_BINARY_DIR}/Usertime_data.h
        ${CMAKE_CURRENT_BINARY_DIR}/Usertime_data.c
        ${CMAKE_CURRENT_BINARY_DIR}/Stats.h
        ${CMAKE_CURRENT_BINARY_DIR}/Stats.c
	Converters.cpp
    )

add_custom_command(
    OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/DataHeader.h
        ${CMAKE_CURRENT_BINARY_DIR}/DataHeader.c
    COMMAND rpcgen -h -o
        ${CMAKE_CURRENT_BINARY_DIR}/DataHeader.h
        ${CMAKE_CURRENT_BINARY_DIR}/DataHeader.x
    COMMAND rpcgen -c -o
        ${CMAKE_CURRENT_BINARY_DIR}/DataHeader.c
        ${CMAKE_CURRENT_BINARY_DIR}/DataHeader.x
    COMMAND cmake -E copy ${CMAKE_CURRENT_BINARY_DIR}/DataHeader.h
                                         ${CMAKE_CURRENT_BINARY_DIR}/KrellInstitute/Messages/DataHeader.h
    DEPENDS
        ${CMAKE_CURRENT_BINARY_DIR}/DataHeader.x
    )

add_custom_command(
    OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/PCSamp.h
        ${CMAKE_CURRENT_BINARY_DIR}/PCSamp.c
    COMMAND rpcgen -h -o
        ${CMAKE_CURRENT_BINARY_DIR}/PCSamp.h
        ${CMAKE_CURRENT_BINARY_DIR}/PCSamp.x
    COMMAND rpcgen -c -o
        ${CMAKE_CURRENT_BINARY_DIR}/PCSamp.c
        ${CMAKE_CURRENT_BINARY_DIR}/PCSamp.x
    COMMAND cmake -E copy ${CMAKE_CURRENT_BINARY_DIR}/PCSamp.h ${CMAKE_CURRENT_BINARY_DIR}/KrellInstitute/Messages/PCSamp.h
    DEPENDS
        ${CMAKE_CURRENT_BINARY_DIR}/PCSamp.x
    )

add_custom_command(
    OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/PCSamp_data.h
        ${CMAKE_CURRENT_BINARY_DIR}/PCSamp_data.c
    COMMAND rpcgen -h -o
        ${CMAKE_CURRENT_BINARY_DIR}/PCSamp_data.h
        ${CMAKE_CURRENT_BINARY_DIR}/PCSamp_data.x
    COMMAND rpcgen -c -o
        ${CMAKE_CURRENT_BINARY_DIR}/PCSamp_data.c
        ${CMAKE_CURRENT_BINARY_DIR}/PCSamp_data.x
    COMMAND cmake -E copy ${CMAKE_CURRENT_BINARY_DIR}/PCSamp_data.h ${CMAKE_CURRENT_BINARY_DIR}/KrellInstitute/Messages/PCSamp_data.h
    DEPENDS
        ${CMAKE_CURRENT_BINARY_DIR}/PCSamp_data.x
    )

add_custom_command(
    OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/Hwc.h
        ${CMAKE_CURRENT_BINARY_DIR}/Hwc.c
    COMMAND rpcgen -h -o
        ${CMAKE_CURRENT_BINARY_DIR}/Hwc.h
        ${CMAKE_CURRENT_BINARY_DIR}/Hwc.x
    COMMAND rpcgen -c -o
        ${CMAKE_CURRENT_BINARY_DIR}/Hwc.c
        ${CMAKE_CURRENT_BINARY_DIR}/Hwc.x
    COMMAND cmake -E copy ${CMAKE_CURRENT_BINARY_DIR}/Hwc.h ${CMAKE_CURRENT_BINARY_DIR}/KrellInstitute/Messages/Hwc.h
    DEPENDS
        ${CMAKE_CURRENT_BINARY_DIR}/Hwc.x
    )

add_custom_command(
    OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/Hwc_data.h
        ${CMAKE_CURRENT_BINARY_DIR}/Hwc_data.c
    COMMAND rpcgen -h -o
        ${CMAKE_CURRENT_BINARY_DIR}/Hwc_data.h
        ${CMAKE_CURRENT_BINARY_DIR}/Hwc_data.x
    COMMAND rpcgen -c -o
        ${CMAKE_CURRENT_BINARY_DIR}/Hwc_data.c
        ${CMAKE_CURRENT_BINARY_DIR}/Hwc_data.x
    COMMAND cmake -E copy ${CMAKE_CURRENT_BINARY_DIR}/Hwc_data.h ${CMAKE_CURRENT_BINARY_DIR}/KrellInstitute/Messages/Hwc_data.h
    DEPENDS
        ${CMAKE_CURRENT_BINARY_DIR}/Hwc_data.x
    )

add_custom_command(
    OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/Hwcsamp.h
        ${CMAKE_CURRENT_BINARY_DIR}/Hwcsamp.c
    COMMAND rpcgen -h -o
        ${CMAKE_CURRENT_BINARY_DIR}/Hwcsamp.h
        ${CMAKE_CURRENT_BINARY_DIR}/Hwcsamp.x
    COMMAND rpcgen -c -o
        ${CMAKE_CURRENT_BINARY_DIR}/Hwcsamp.c
        ${CMAKE_CURRENT_BINARY_DIR}/Hwcsamp.x
    COMMAND cmake -E copy ${CMAKE_CURRENT_BINARY_DIR}/Hwcsamp.h ${CMAKE_CURRENT_BINARY_DIR}/KrellInstitute/Messages/Hwcsamp.h
    DEPENDS
        ${CMAKE_CURRENT_BINARY_DIR}/Hwcsamp.x
    )

add_custom_command(
    OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/Hwcsamp_data.h
        ${CMAKE_CURRENT_BINARY_DIR}/Hwcsamp_data.c
    COMMAND rpcgen -h -o
        ${CMAKE_CURRENT_BINARY_DIR}/Hwcsamp_data.h
        ${CMAKE_CURRENT_BINARY_DIR}/Hwcsamp_data.x
    COMMAND rpcgen -c -o
        ${CMAKE_CURRENT_BINARY_DIR}/Hwcsamp_data.c
        ${CMAKE_CURRENT_BINARY_DIR}/Hwcsamp_data.x
    COMMAND cmake -E copy ${CMAKE_CURRENT_BINARY_DIR}/Hwcsamp_data.h ${CMAKE_CURRENT_BINARY_DIR}/KrellInstitute/Messages/Hwcsamp_data.h
    DEPENDS
        ${CMAKE_CURRENT_BINARY_DIR}/Hwcsamp_data.x
    )

add_custom_command(
    OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/Hwctime.h
        ${CMAKE_CURRENT_BINARY_DIR}/Hwctime.c
    COMMAND rpcgen -h -o
        ${CMAKE_CURRENT_BINARY_DIR}/Hwctime.h
        ${CMAKE_CURRENT_BINARY_DIR}/Hwctime.x
    COMMAND rpcgen -c -o
        ${CMAKE_CURRENT_BINARY_DIR}/Hwctime.c
        ${CMAKE_CURRENT_BINARY_DIR}/Hwctime.x
    COMMAND cmake -E copy ${CMAKE_CURRENT_BINARY_DIR}/Hwctime.h ${CMAKE_CURRENT_BINARY_DIR}/KrellInstitute/Messages/Hwctime.h
    DEPENDS
        ${CMAKE_CURRENT_BINARY_DIR}/Hwctime.x
    )

add_custom_command(
    OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/Hwctime_data.h
        ${CMAKE_CURRENT_BINARY_DIR}/Hwctime_data.c
    COMMAND rpcgen -h -o
        ${CMAKE_CURRENT_BINARY_DIR}/Hwctime_data.h
        ${CMAKE_CURRENT_BINARY_DIR}/Hwctime_data.x
    COMMAND rpcgen -c -o
        ${CMAKE_CURRENT_BINARY_DIR}/Hwctime_data.c
        ${CMAKE_CURRENT_BINARY_DIR}/Hwctime_data.x
    COMMAND cmake -E copy ${CMAKE_CURRENT_BINARY_DIR}/Hwctime_data.h ${CMAKE_CURRENT_BINARY_DIR}/KrellInstitute/Messages/Hwctime_data.h
    DEPENDS
        ${CMAKE_CURRENT_BINARY_DIR}/Hwctime_data.x
    )

add_custom_command(
    OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/IO.h
        ${CMAKE_CURRENT_BINARY_DIR}/IO.c
    COMMAND rpcgen -h -o
        ${CMAKE_CURRENT_BINARY_DIR}/IO.h
        ${CMAKE_CURRENT_BINARY_DIR}/IO.x
    COMMAND rpcgen -c -o
        ${CMAKE_CURRENT_BINARY_DIR}/IO.c
        ${CMAKE_CURRENT_BINARY_DIR}/IO.x
    COMMAND cmake -E copy ${CMAKE_CURRENT_BINARY_DIR}/IO.h ${CMAKE_CURRENT_BINARY_DIR}/KrellInstitute/Messages/IO.h
    DEPENDS
        ${CMAKE_CURRENT_BINARY_DIR}/IO.x
    )

add_custom_command(
    OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/IO_data.h
        ${CMAKE_CURRENT_BINARY_DIR}/IO_data.c
    COMMAND rpcgen -h -o
        ${CMAKE_CURRENT_BINARY_DIR}/IO_data.h
        ${CMAKE_CURRENT_BINARY_DIR}/IO_data.x
    COMMAND rpcgen -c -o
        ${CMAKE_CURRENT_BINARY_DIR}/IO_data.c
        ${CMAKE_CURRENT_BINARY_DIR}/IO_data.x
    COMMAND cmake -E copy ${CMAKE_CURRENT_BINARY_DIR}/IO_data.h ${CMAKE_CURRENT_BINARY_DIR}/KrellInstitute/Messages/IO_data.h
    DEPENDS
        ${CMAKE_CURRENT_BINARY_DIR}/IO_data.x
    )

add_custom_command(
    OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/Mem.h
        ${CMAKE_CURRENT_BINARY_DIR}/Mem.c
    COMMAND rpcgen -h -o
        ${CMAKE_CURRENT_BINARY_DIR}/Mem.h
        ${CMAKE_CURRENT_BINARY_DIR}/Mem.x
    COMMAND rpcgen -c -o
        ${CMAKE_CURRENT_BINARY_DIR}/Mem.c
        ${CMAKE_CURRENT_BINARY_DIR}/Mem.x
    COMMAND cmake -E copy ${CMAKE_CURRENT_BINARY_DIR}/Mem.h ${CMAKE_CURRENT_BINARY_DIR}/KrellInstitute/Messages/Mem.h
    DEPENDS
        ${CMAKE_CURRENT_BINARY_DIR}/Mem.x
    )

add_custom_command(
    OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/Mem_data.h
        ${CMAKE_CURRENT_BINARY_DIR}/Mem_data.c
    COMMAND rpcgen -h -o
        ${CMAKE_CURRENT_BINARY_DIR}/Mem_data.h
        ${CMAKE_CURRENT_BINARY_DIR}/Mem_data.x
    COMMAND rpcgen -c -o
        ${CMAKE_CURRENT_BINARY_DIR}/Mem_data.c
        ${CMAKE_CURRENT_BINARY_DIR}/Mem_data.x
    COMMAND cmake -E copy ${CMAKE_CURRENT_BINARY_DIR}/Mem_data.h ${CMAKE_CURRENT_BINARY_DIR}/KrellInstitute/Messages/Mem_data.h
    DEPENDS
        ${CMAKE_CURRENT_BINARY_DIR}/Mem_data.x
    )

add_custom_command(
    OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/Mpi.h
        ${CMAKE_CURRENT_BINARY_DIR}/Mpi.c
    COMMAND rpcgen -h -o
        ${CMAKE_CURRENT_BINARY_DIR}/Mpi.h
        ${CMAKE_CURRENT_BINARY_DIR}/Mpi.x
    COMMAND rpcgen -c -o
        ${CMAKE_CURRENT_BINARY_DIR}/Mpi.c
        ${CMAKE_CURRENT_BINARY_DIR}/Mpi.x
    COMMAND cmake -E copy ${CMAKE_CURRENT_BINARY_DIR}/Mpi.h ${CMAKE_CURRENT_BINARY_DIR}/KrellInstitute/Messages/Mpi.h
    DEPENDS
        ${CMAKE_CURRENT_BINARY_DIR}/Mpi.x
    )

add_custom_command(
    OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/Mpi_data.h
        ${CMAKE_CURRENT_BINARY_DIR}/Mpi_data.c
    COMMAND rpcgen -h -o
        ${CMAKE_CURRENT_BINARY_DIR}/Mpi_data.h
        ${CMAKE_CURRENT_BINARY_DIR}/Mpi_data.x
    COMMAND rpcgen -c -o
        ${CMAKE_CURRENT_BINARY_DIR}/Mpi_data.c
        ${CMAKE_CURRENT_BINARY_DIR}/Mpi_data.x
    COMMAND cmake -E copy ${CMAKE_CURRENT_BINARY_DIR}/Mpi_data.h ${CMAKE_CURRENT_BINARY_DIR}/KrellInstitute/Messages/Mpi_data.h
    DEPENDS
        ${CMAKE_CURRENT_BINARY_DIR}/Mpi_data.x
    )

add_custom_command(
    OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/Pthreads.h
        ${CMAKE_CURRENT_BINARY_DIR}/Pthreads.c
    COMMAND rpcgen -h -o
        ${CMAKE_CURRENT_BINARY_DIR}/Pthreads.h
        ${CMAKE_CURRENT_BINARY_DIR}/Pthreads.x
    COMMAND rpcgen -c -o
        ${CMAKE_CURRENT_BINARY_DIR}/Pthreads.c
        ${CMAKE_CURRENT_BINARY_DIR}/Pthreads.x
    COMMAND cmake -E copy ${CMAKE_CURRENT_BINARY_DIR}/Pthreads.h ${CMAKE_CURRENT_BINARY_DIR}/KrellInstitute/Messages/Pthreads.h
    DEPENDS
        ${CMAKE_CURRENT_BINARY_DIR}/Pthreads.x
    )

add_custom_command(
    OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/Pthreads_data.h
        ${CMAKE_CURRENT_BINARY_DIR}/Pthreads_data.c
    COMMAND rpcgen -h -o
        ${CMAKE_CURRENT_BINARY_DIR}/Pthreads_data.h
        ${CMAKE_CURRENT_BINARY_DIR}/Pthreads_data.x
    COMMAND rpcgen -c -o
        ${CMAKE_CURRENT_BINARY_DIR}/Pthreads_data.c
        ${CMAKE_CURRENT_BINARY_DIR}/Pthreads_data.x
    COMMAND cmake -E copy ${CMAKE_CURRENT_BINARY_DIR}/Pthreads_data.h ${CMAKE_CURRENT_BINARY_DIR}/KrellInstitute/Messages/Pthreads_data.h
    DEPENDS
        ${CMAKE_CURRENT_BINARY_DIR}/Pthreads_data.x
    )

add_custom_command(
    OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/Usertime.h
        ${CMAKE_CURRENT_BINARY_DIR}/Usertime.c
    COMMAND rpcgen -h -o
        ${CMAKE_CURRENT_BINARY_DIR}/Usertime.h
        ${CMAKE_CURRENT_BINARY_DIR}/Usertime.x
    COMMAND rpcgen -c -o
        ${CMAKE_CURRENT_BINARY_DIR}/Usertime.c
        ${CMAKE_CURRENT_BINARY_DIR}/Usertime.x
    COMMAND cmake -E copy ${CMAKE_CURRENT_BINARY_DIR}/Usertime.h ${CMAKE_CURRENT_BINARY_DIR}/KrellInstitute/Messages/Usertime.h
    DEPENDS
        ${CMAKE_CURRENT_BINARY_DIR}/Usertime.x
    )

add_custom_command(
    OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/Usertime_data.h
        ${CMAKE_CURRENT_BINARY_DIR}/Usertime_data.c
    COMMAND rpcgen -h -o
        ${CMAKE_CURRENT_BINARY_DIR}/Usertime_data.h
        ${CMAKE_CURRENT_BINARY_DIR}/Usertime_data.x
    COMMAND rpcgen -c -o
        ${CMAKE_CURRENT_BINARY_DIR}/Usertime_data.c
        ${CMAKE_CURRENT_BINARY_DIR}/Usertime_data.x
    COMMAND cmake -E copy ${CMAKE_CURRENT_BINARY_DIR}/Usertime_data.h ${CMAKE_CURRENT_BINARY_DIR}/KrellInstitute/Messages/Usertime_data.h
    DEPENDS
        ${CMAKE_CURRENT_BINARY_DIR}/Usertime_data.x
    )

add_custom_command(
    OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/Stats.h
        ${CMAKE_CURRENT_BINARY_DIR}/Stats.c
    COMMAND rpcgen -h -o
        ${CMAKE_CURRENT_BINARY_DIR}/Stats.h
        ${CMAKE_CURRENT_BINARY_DIR}/Stats.x
    COMMAND rpcgen -c -o
        ${CMAKE_CURRENT_BINARY_DIR}/Stats.c
        ${CMAKE_CURRENT_BINARY_DIR}/Stats.x
    COMMAND cmake -E copy ${CMAKE_CURRENT_BINARY_DIR}/Stats.h ${CMAKE_CURRENT_BINARY_DIR}/KrellInstitute/Messages/Stats.h
    DEPENDS
        ${CMAKE_CURRENT_BINARY_DIR}/Stats.x
    )


add_library(cbtf-messages-converters-perfdata SHARED
        ${CBTF_MESSAGES_CONVERTERS_PERFDATA_SOURCES}
)
  
target_include_directories(cbtf-messages-converters-perfdata PUBLIC
	${PROJECT_SOURCE_DIR}/messages/include
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../base
	${MRNet_INCLUDE_DIRS}
	${Boost_INCLUDE_DIRS}
	${CBTF_INCLUDE_DIRS}
	${LTDL_INCLUDE_DIR}
    )

target_link_libraries(cbtf-messages-converters-perfdata
	${MRNet_LIBRARIES}
	pthread
	${LTDL_LIBRARY_SHARED}
	${CMAKE_DL_LIBS}
    )

set_target_properties(cbtf-messages-converters-perfdata PROPERTIES VERSION 1.1.0)
set_target_properties(cbtf-messages-converters-perfdata PROPERTIES 
        COMPILE_DEFINITIONS "${MRNet_DEFINES}")

install(TARGETS cbtf-messages-converters-perfdata
	LIBRARY DESTINATION lib${LIB_SUFFIX}
    )

# Install static versions

add_library(cbtf-messages-converters-perfdata-static STATIC
        ${CBTF_MESSAGES_CONVERTERS_PERFDATA_SOURCES}
    )
  
target_include_directories(cbtf-messages-converters-perfdata-static PUBLIC
	${PROJECT_SOURCE_DIR}/messages/include
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../base
	${MRNet_INCLUDE_DIRS}
	${Boost_INCLUDE_DIRS}
	${CBTF_INCLUDE_DIRS}
	${LTDL_INCLUDE_DIR}
)

target_link_libraries(cbtf-messages-converters-perfdata-static
	${MRNet_LIBRARIES}
	pthread
	${LTDL_LIBRARY_SHARED}
	${CMAKE_DL_LIBS}
    )

set_target_properties(cbtf-messages-converters-perfdata-static PROPERTIES VERSION 1.1.0)
set_target_properties(cbtf-messages-converters-perfdata-static PROPERTIES 
        COMPILE_DEFINITIONS "${MRNet_DEFINES}")

install(TARGETS cbtf-messages-converters-perfdata-static
	ARCHIVE DESTINATION lib${LIB_SUFFIX}
    )

add_library(cbtf-messages-perfdata SHARED
	${CBTF_MESSAGES_PERFDATA_SOURCES}
)

target_include_directories(cbtf-messages-perfdata PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../base
	${CMAKE_CURRENT_BINARY_DIR}
	${CBTF_INCLUDE_DIRS}
	${MRNet_INCLUDE_DIRS}
	${Boost_INCLUDE_DIRS}
	${LTDL_INCLUDE_DIR}
)

target_link_libraries(cbtf-messages-perfdata
	cbtf-messages-base
	${LTDL_LIBRARY_SHARED}
	${CMAKE_DL_LIBS}
)

set_target_properties(cbtf-messages-perfdata PROPERTIES VERSION 1.1.0)
set_target_properties(cbtf-messages-perfdata PROPERTIES 
        COMPILE_DEFINITIONS "${MRNet_DEFINES}")

install(TARGETS cbtf-messages-perfdata 
        LIBRARY DESTINATION lib${LIB_SUFFIX}
)

# Install static versions

add_library(cbtf-messages-perfdata-static STATIC
	${CBTF_MESSAGES_PERFDATA_SOURCES}
)

target_include_directories(cbtf-messages-perfdata-static PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../base
	${CMAKE_CURRENT_BINARY_DIR}
	${CBTF_INCLUDE_DIRS}
	${MRNet_INCLUDE_DIRS}
	${Boost_INCLUDE_DIRS}
	${LTDL_INCLUDE_DIR}
)

target_link_libraries(cbtf-messages-perfdata-static
	cbtf-messages-base
	${LTDL_LIBRARY_SHARED}
	${CMAKE_DL_LIBS}
)

set_target_properties(cbtf-messages-perfdata-static PROPERTIES VERSION 1.1.0)
set_target_properties(cbtf-messages-perfdata-static PROPERTIES 
        COMPILE_DEFINITIONS "${MRNet_DEFINES}")

install(TARGETS cbtf-messages-perfdata-static 
        ARCHIVE DESTINATION lib${LIB_SUFFIX}
)


install(FILES ${CMAKE_CURRENT_BINARY_DIR}/DataHeader.h 
              ${CMAKE_CURRENT_BINARY_DIR}/PCSamp.h 
              ${CMAKE_CURRENT_BINARY_DIR}/PCSamp_data.h 
              ${CMAKE_CURRENT_BINARY_DIR}/Hwc.h 
              ${CMAKE_CURRENT_BINARY_DIR}/Hwc_data.h 
              ${CMAKE_CURRENT_BINARY_DIR}/Hwcsamp.h 
              ${CMAKE_CURRENT_BINARY_DIR}/Hwcsamp_data.h 
              ${CMAKE_CURRENT_BINARY_DIR}/Hwctime.h 
              ${CMAKE_CURRENT_BINARY_DIR}/Hwctime_data.h 
              ${CMAKE_CURRENT_BINARY_DIR}/IO.h 
              ${CMAKE_CURRENT_BINARY_DIR}/IO_data.h 
              ${CMAKE_CURRENT_BINARY_DIR}/Mem.h 
              ${CMAKE_CURRENT_BINARY_DIR}/Mem_data.h 
              ${CMAKE_CURRENT_BINARY_DIR}/Mpi.h 
              ${CMAKE_CURRENT_BINARY_DIR}/Mpi_data.h 
              ${CMAKE_CURRENT_BINARY_DIR}/Pthreads.h 
              ${CMAKE_CURRENT_BINARY_DIR}/Pthreads_data.h 
              ${CMAKE_CURRENT_BINARY_DIR}/Usertime.h 
              ${CMAKE_CURRENT_BINARY_DIR}/Usertime_data.h 
              ${CMAKE_CURRENT_BINARY_DIR}/Stats.h 
        DESTINATION include/KrellInstitute/Messages
        PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)

# rpcgen processing done in CMAKE_CURRENT_BINARY_DIR..
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/DataHeader.x DESTINATION .)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/PCSamp.x DESTINATION .)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/PCSamp_data.x DESTINATION .)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Hwc.x DESTINATION .)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Hwc_data.x DESTINATION .)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Hwcsamp.x DESTINATION .)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Hwcsamp_data.x DESTINATION .)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Hwctime.x DESTINATION .)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Hwctime_data.x DESTINATION .)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/IO.x DESTINATION .)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/IO_data.x DESTINATION .)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Mem.x DESTINATION .)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Mem_data.x DESTINATION .)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Mpi.x DESTINATION .)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Mpi_data.x DESTINATION .)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Pthreads.x DESTINATION .)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Pthreads_data.x DESTINATION .)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Usertime.x DESTINATION .)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Usertime_data.x DESTINATION .)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Stats.x DESTINATION .)
