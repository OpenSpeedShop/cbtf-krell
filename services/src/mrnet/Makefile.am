################################################################################
# Copyright (c) 2010-2013 Krell Institute. All Rights Reserved.
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

lib_LTLIBRARIES = libcbtf-services-mrnet.la

libcbtf_services_mrnet_la_CFLAGS = \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/src/data \
	@MESSAGES_CPPFLAGS@ \
	@MRNET_CPPFLAGS@ \
	@LTDLINCL@ 

libcbtf_services_mrnet_la_LDFLAGS = \
	@MESSAGES_LDFLAGS@ \
	@MRNET_LDFLAGS@ \
	-version-info 0:0:0

libcbtf_services_mrnet_la_LIBADD = \
	@MESSAGES_BASE_LIBS@ \
	@MESSAGES_EVENTS_LIBS@ \
	@MRNET_LWR_LIBS@ \
	@LIBLTDL@

libcbtf_services_mrnet_la_SOURCES = \
	MRNet_Send.c \
	MRNet_Senders.c
