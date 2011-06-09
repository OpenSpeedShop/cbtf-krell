################################################################################
# Copyright (c) 2011 Krell Institute. All Rights Reserved.
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

lib_LTLIBRARIES = libcbtf-core.la

libcbtf_core_la_CXXFLAGS = \
	-I$(top_srcdir)/include \
	@MESSAGES_CPPFLAGS@ \
	@LTDLINCL@ 

libcbtf_core_la_LDFLAGS = \
	@MESSAGES_LDFLAGS@ \
	-version-info 0:0:0


libcbtf_core_la_LIBADD = \
	@MESSAGES_PERFDATA_LIBS@ \
	@LIBLTDL@

libcbtf_core_la_SOURCES = \
	AddressBuffer.cpp \
	Blob.cpp \
	Exception.cpp \
	ExtentGroup.cpp \
	Path.cpp \
	PCData.cpp \
	ThreadName.cpp