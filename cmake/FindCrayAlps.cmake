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

include(FindPackageHandleStandardArgs)

find_library(CrayAlps_LIBRARY NAMES libalps.so
    HINTS ${CRAYALPS_DIR} $ENV{CRAYALPS_DIR}
    PATH_SUFFIXES lib lib64
    )

find_path(CrayAlps_INCLUDE_DIR alps/alps.h
    HINTS ${CRAYALPS_DIR} $ENV{CRAYALPS_DIR}
    PATH_SUFFIXES include
    )

find_package_handle_standard_args(
    CrayAlps DEFAULT_MSG
    CrayAlps_LIBRARY CrayAlps_INCLUDE_DIR
    )

GET_FILENAME_COMPONENT(CrayAlps_LIB_DIR ${CrayAlps_LIBRARY} PATH )
set(CrayAlps_LIBRARIES ${CrayAlps_LIBRARY} )
set(CrayAlps_INCLUDE_DIRS ${CrayAlps_INCLUDE_DIR})

mark_as_advanced(
    CrayAlps_LIBRARY CrayAlps_INCLUDE_DIR CrayAlps_LIB_DIR
)
