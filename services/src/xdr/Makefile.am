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

lib_LTLIBRARIES = libcbtf-services-send.la

if BUILD_TARGETED
if HAVE_TARGET_SHARED
libcbtf_services_send_la_CFLAGS = \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/src/data \
	@TARGET_MESSAGES_CPPFLAGS@  \
	@LTDLINCL@ 

libcbtf_services_send_la_LDFLAGS = \
	@TARGET_MESSAGES_LDFLAGS@  \
	-version-info 0:0:0

libcbtf_services_send_la_LIBADD = \
	@TARGET_MESSAGES_PERFDATA_LIBS@  \
	@LIBLTDL@
else
if HAVE_TARGET_STATIC
libcbtf_services_send_la_CFLAGS = \
	-static \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/src/data \
	@TARGET_MESSAGES_CPPFLAGS@  \
	@LTDLINCL@ 

libcbtf_services_send_la_LDFLAGS = \
	-static \
	@TARGET_MESSAGES_LDFLAGS@  \
	-version-info 0:0:0

libcbtf_services_send_la_LIBADD = \
	@TARGET_MESSAGES_PERFDATA_LIBS@  \
	@LIBLTDL@
endif
endif

# else for not targeted (below) targeted above
else

libcbtf_services_send_la_CFLAGS = \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/src/data \
	@MESSAGES_CPPFLAGS@  \
	@LTDLINCL@ 

libcbtf_services_send_la_LDFLAGS = \
	@MESSAGES_LDFLAGS@  \
	-version-info 0:0:0

libcbtf_services_send_la_LIBADD = \
	@MESSAGES_PERFDATA_LIBS@  \
	@LIBLTDL@
endif

libcbtf_services_send_la_SOURCES = \
	Send.c
