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

lib_LTLIBRARIES = libcbtf-services-binutils.la

libcbtf_services_binutils_la_CFLAGS = \
	-I$(top_srcdir)/include \
	@BINUTILS_CPPFLAGS@ \
	@LTDLINCL@ 

libcbtf_services_binutils_la_LDFLAGS = \
	@BINUTILS_LDFLAGS@ \
	-version-info 0:0:0

libcbtf_services_binutils_la_LIBADD = \
	@BINUTILS_LIBS@ \
	@LIBLTDL@

libcbtf_services_binutils_la_SOURCES = \
	GetInstrLength.c
