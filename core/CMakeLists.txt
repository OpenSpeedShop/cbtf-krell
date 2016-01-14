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

#
# Skip building the clients like collectionTool and daemonTool when building
# for the targetted, compute node runtime only version of the core libraries and components.
#
if (RUNTIME_ONLY MATCHES "true")
    add_subdirectory(collectors)
    if (CBTF_FILTERS_RUN_ON MATCHES "native")
        add_subdirectory(src)
        add_subdirectory(components)
    endif()
else()
    add_subdirectory(src)
    add_subdirectory(collectors)
    add_subdirectory(components)
    add_subdirectory(scripts)
    if (DYNINSTAPI_FOUND)
        add_subdirectory(collectionTool)
    endif()
    add_subdirectory(daemonTool)
endif()

install( FILES CBTF-CoreConfig.cmake
         DESTINATION share/CBTF-Core/cmake)


install(DIRECTORY include/KrellInstitute
        DESTINATION include
        FILES_MATCHING
        PATTERN "*.hpp"
        PATTERN "Makefile.*" EXCLUDE
        PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)


