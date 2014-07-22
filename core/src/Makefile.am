################################################################################
# Copyright (c) 2011-2013 Krell Institute. All Rights Reserved.
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

lib_LTLIBRARIES = libcbtf-core.la \
		  libcbtf-core-bfd.la \
		  libcbtf-core-symtabapi.la \
		  libcbtf-core-mrnet.la

libcbtf_core_la_CXXFLAGS = \
	-I$(top_srcdir)/include \
	@BOOST_CPPFLAGS@ \
	@MESSAGES_CPPFLAGS@ \
	@LTDLINCL@ 

libcbtf_core_la_LDFLAGS = \
	@MESSAGES_LDFLAGS@ \
	-export-dynamic -version-info 0:0:0


libcbtf_core_la_LIBADD = \
	@MESSAGES_PERFDATA_LIBS@ \
	@MESSAGES_BASE_LIBS@ \
	@LIBLTDL@

libcbtf_core_bfd_la_CXXFLAGS = \
	-I$(top_srcdir)/include \
	@BINUTILS_CPPFLAGS@ \
	@MESSAGES_CPPFLAGS@ \
	@LTDLINCL@ 

libcbtf_core_bfd_la_LDFLAGS = \
	@BINUTILS_LDFLAGS@ \
	@MESSAGES_LDFLAGS@ \
	-export-dynamic -version-info 0:0:0

libcbtf_core_bfd_la_LIBADD = \
	@BINUTILS_BFD_LIB@ \
	@MESSAGES_PERFDATA_LIBS@ \
	@LIBLTDL@

libcbtf_core_symtabapi_la_CXXFLAGS = \
	-I$(top_srcdir)/include \
	@LIBDWARF_CPPFLAGS@ \
	@LIBELF_CPPFLAGS@ \
	@BOOST_CPPFLAGS@ \
	@DYNINST_CPPFLAGS@ \
	@MESSAGES_CPPFLAGS@ \
	@LTDLINCL@ 

libcbtf_core_symtabapi_la_LDFLAGS = \
	@LIBDWARF_LDFLAGS@ \
	@LIBELF_LDFLAGS@ \
	@DYNINST_LDFLAGS@ \
	-export-dynamic -version-info 0:0:0


libcbtf_core_symtabapi_la_LIBADD = \
	@LIBDWARF_LIBS@ \
	@LIBELF_LIBS@ \
	@DYNINST_SYMTABAPI_LIBS@ \
	@LIBLTDL@

libcbtf_core_mrnet_la_CXXFLAGS = \
	-I$(top_srcdir)/include \
	@BOOST_CPPFLAGS@ \
	@MRNET_CPPFLAGS@ \
	@LTDLINCL@ 

libcbtf_core_mrnet_la_LDFLAGS = \
	@MRNET_LDFLAGS@ \
	-export-dynamic -version-info 0:0:0

libcbtf_core_mrnet_la_LIBADD = \
	@MRNET_LIBS@ \
	@LIBLTDL@

if HAVE_ALPS
libcbtf_core_mrnet_la_CXXFLAGS += @ALPS_CPPFLAGS@ 
libcbtf_core_mrnet_la_LDFLAGS += @ALPS_LDFLAGS@
libcbtf_core_mrnet_la_LIBADD += @ALPS_LIBS@
endif


libcbtf_core_la_SOURCES = \
	AddressBitmap.cpp \
	AddressBuffer.cpp \
	Blob.cpp \
	Exception.cpp \
	ExtentGroup.cpp \
	Graph.cpp \
	LinkedObjectEntry.cpp \
	Path.cpp \
	PCData.cpp \
	StacktraceData.cpp \
	SymbolTable.cpp \
	ThreadName.cpp

libcbtf_core_bfd_la_SOURCES = \
	BFDSymbols.cpp

libcbtf_core_symtabapi_la_SOURCES = \
	SymtabAPISymbols.cpp

libcbtf_core_mrnet_la_SOURCES = \
	CBTFTopology.cpp
