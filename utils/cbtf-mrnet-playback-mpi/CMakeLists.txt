################################################################################
# Copyright (c) 2017 Argo Navis Technologies. All Rights Reserved.
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

if (OPENMPI_FOUND)
  set(MPI_INCLUDE_DIRS ${OpenMPI_INCLUDE_DIRS})
  set(MPI_SHARED_LIBRARIES ${OpenMPI_SHARED_LIBRARIES})
endif()

if (MPICH2_FOUND)
  set(MPI_INCLUDE_DIRS ${Mpich2_INCLUDE_DIRS})
  set(MPI_SHARED_LIBRARIES ${Mpich2_SHARED_LIBRARIES})  
endif()

if (MPICH_FOUND)
  set(MPI_INCLUDE_DIRS ${Mpich_INCLUDE_DIRS})
  set(MPI_SHARED_LIBRARIES ${Mpich_SHARED_LIBRARIES})  
endif()

if (MPT_FOUND)
  set(MPI_INCLUDE_DIRS ${Mpt_INCLUDE_DIRS})
  set(MPI_SHARED_LIBRARIES ${Mpt_SHARED_LIBRARIES})  
endif()

if (MVAPICH_FOUND)
  set(MPI_INCLUDE_DIRS ${Mvapich_INCLUDE_DIRS})
  set(MPI_SHARED_LIBRARIES ${Mvapich_SHARED_LIBRARIES})  
endif()

if (MVAPICH2_FOUND)
  set(MPI_INCLUDE_DIRS ${Mvapich2_INCLUDE_DIRS})
  set(MPI_SHARED_LIBRARIES ${Mvapich2_SHARED_LIBRARIES})  
endif()

include_directories(${MPI_INCLUDE_DIRS})
add_executable(cbtf-mrnet-playback-mpi main.c)
target_link_libraries(cbtf-mrnet-playback-mpi ${MPI_SHARED_LIBRARIES})
install(TARGETS cbtf-mrnet-playback-mpi RUNTIME DESTINATION bin)
