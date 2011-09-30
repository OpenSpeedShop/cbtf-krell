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
	@BINUTILS_CPPFLAGS@ \
	@LIBDWARF_CPPFLAGS@ \
	@DYNINST_CPPFLAGS@ \
	@MESSAGES_CPPFLAGS@ \
	@LTDLINCL@ 

libcbtf_core_la_LDFLAGS = \
	@BINUTILS_LDFLAGS@ \
	@LIBDWARF_LDFLAGS@ \
	@DYNINST_LDFLAGS@ \
	@MESSAGES_LDFLAGS@ \
	-version-info 0:0:0


libcbtf_core_la_LIBADD = \
	@BINUTILS_BFD_LIB@ \
	@LIBDWARF_LIBS@ \
	@DYNINST_LIBS@ \
	@MESSAGES_PERFDATA_LIBS@ \
	@LIBLTDL@

libcbtf_core_la_SOURCES = \
	AddressBitmap.cpp \
	AddressBuffer.cpp \
	BFDSymbols.cpp \
	Blob.cpp \
	Exception.cpp \
	ExtentGroup.cpp \
	LinkedObjectEntry.cpp \
	Path.cpp \
	PCData.cpp \
	StacktraceData.cpp \
	SymbolTable.cpp \
	SymtabAPISymbols.cpp \
	ThreadName.cpp
