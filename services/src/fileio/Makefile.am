################################################################################
# Copyright (c) 2010-2012 Krell Institute. All Rights Reserved.
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

lib_LTLIBRARIES = libcbtf-services-fileio.la

if BUILD_TARGETED
if HAVE_TARGET_SHARED
libcbtf_services_fileio_la_CFLAGS = \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/src/data \
	@TARGET_MESSAGES_CPPFLAGS@ \
	@LTDLINCL@ 

libcbtf_services_fileio_la_LDFLAGS = \
	-version-info 0:0:0

libcbtf_services_fileio_la_LIBADD = \
	@LIBLTDL@
endif
if HAVE_TARGET_STATIC
libcbtf_services_fileio_la_CFLAGS = \
	-static \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/src/data \
	@TARGET_MESSAGES_CPPFLAGS@ \
	@LTDLINCL@ 

libcbtf_services_fileio_la_LDFLAGS = \
	-static \
	-version-info 0:0:0

libcbtf_services_fileio_la_LIBADD = \
	@LIBLTDL@
endif

# else for not targeted (below) targeted above
else

libcbtf_services_fileio_la_CFLAGS = \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/src/data \
	@MESSAGES_CPPFLAGS@ \
	@LTDLINCL@ 

libcbtf_services_fileio_la_LDFLAGS = \
	-version-info 0:0:0

libcbtf_services_fileio_la_LIBADD = \
	@LIBLTDL@
endif

libcbtf_services_fileio_la_SOURCES = \
	SendToFile.c \
	send.c
