################################################################################
# Copyright (c) 2010 Krell Institute. All Rights Reserved.
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

lib_LTLIBRARIES = libcbtf-services-offline.la

BUILT_SOURCES = \
	offline.h offline.c

libcbtf_services_offline_la_CFLAGS = \
	-I$(top_srcdir)/include \
	@LTDLINCL@ 

libcbtf_services_offline_la_LDFLAGS = \
	-version-info 0:0:0

libcbtf_services_offline_la_LIBADD = \
	@LIBLTDL@

libcbtf_services_offline_la_SOURCES = \
	offline.x $(BUILT_SOURCES) \
	DecodeParameters.c \
	EncodeParameters.c \
	GetDLInfo.c \
	InitializeParameters.c

SUFFIXES = .x
CLEANFILES = $(BUILT_SOURCES)

offline.h offline.c : offline.x
	rm -f  $(patsubst %.x, %.h, $<) $(patsubst %.x, %.c, $<)
	$(RPCGEN) -h -o $(patsubst %.x, %.h, $<) $<
	$(RPCGEN) -c -o $(patsubst %.x, %.c, $<) $<
