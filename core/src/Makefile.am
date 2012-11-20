################################################################################
# Copyright (c) 2011-2012 Krell Institute. All Rights Reserved.
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

if BUILD_TARGETED
if HAVE_TARGET_SHARED
libcbtf_core_la_CXXFLAGS = \
	-I$(top_srcdir)/include \
	@TARGET_MESSAGES_CPPFLAGS@ \
	@LTDLINCL@ 

libcbtf_core_la_LDFLAGS = \
	@TARGET_MESSAGES_LDFLAGS@ \
	-export-dynamic -version-info 0:0:0


libcbtf_core_la_LIBADD = \
	@TARGET_MESSAGES_PERFDATA_LIBS@ \
	@LIBLTDL@

libcbtf_core_bfd_la_CXXFLAGS = \
	-I$(top_srcdir)/include \
	@TARGET_BINUTILS_CPPFLAGS@ \
	@TARGET_MESSAGES_CPPFLAGS@ \
	@LTDLINCL@ 

libcbtf_core_bfd_la_LDFLAGS = \
	@TARGET_BINUTILS_LDFLAGS@ \
	@TARGET_MESSAGES_LDFLAGS@ \
	-export-dynamic -version-info 0:0:0

libcbtf_core_bfd_la_LIBADD = \
	@TARGET_BINUTILS_BFD_LIB@ \
	@TARGET_MESSAGES_PERFDATA_LIBS@ \
	@LIBLTDL@

libcbtf_core_symtabapi_la_CXXFLAGS = \
	-I$(top_srcdir)/include \
	@TARGET_LIBDWARF_CPPFLAGS@ \
	@TARGET_DYNINST_CPPFLAGS@ \
	@TARGET_MESSAGES_CPPFLAGS@ \
	@LTDLINCL@ 

libcbtf_core_symtabapi_la_LDFLAGS = \
	@TARGET_LIBDWARF_LDFLAGS@ \
	@TARGET_DYNINST_LDFLAGS@ \
	@TARGET_MESSAGES_LDFLAGS@ \
	-export-dynamic -version-info 0:0:0


libcbtf_core_symtabapi_la_LIBADD = \
	@TARGET_LIBDWARF_LIBS@ \
	@TARGET_DYNINST_LIBS@ \
	@TARGET_MESSAGES_PERFDATA_LIBS@ \
	@LIBLTDL@

libcbtf_core_mrnet_la_CXXFLAGS = \
	-I$(top_srcdir)/include \
	@TARGET_MESSAGES_CPPFLAGS@ \
	@TARGET_MRNET_CPPFLAGS@ \
	@LTDLINCL@ 

libcbtf_core_mrnet_la_LDFLAGS = \
	@TARGET_MESSAGES_LDFLAGS@ \
	@TARGET_MRNET_LDFLAGS@ \
	-export-dynamic -version-info 0:0:0

libcbtf_core_mrnet_la_LIBADD = \
	@TARGET_MESSAGES_PERFDATA_LIBS@ \
	@TARGET_MRNET_LIBS@ \
	@LIBLTDL@

else
if HAVE_TARGET_STATIC
libcbtf_core_la_CXXFLAGS = \
	-I$(top_srcdir)/include \
	@TARGET_MESSAGES_CPPFLAGS@ \
	@LTDLINCL@ 

libcbtf_core_la_LDFLAGS = \
	@TARGET_MESSAGES_LDFLAGS@ \
	-export-dynamic -version-info 0:0:0


libcbtf_core_la_LIBADD = \
	@TARGET_MESSAGES_PERFDATA_LIBS@ \
	@LIBLTDL@

libcbtf_core_bfd_la_CXXFLAGS = \
	-I$(top_srcdir)/include \
	@TARGET_BINUTILS_CPPFLAGS@ \
	@TARGET_MESSAGES_CPPFLAGS@ \
	@LTDLINCL@ 

libcbtf_core_bfd_la_LDFLAGS = \
	@TARGET_BINUTILS_LDFLAGS@ \
	@TARGET_MESSAGES_LDFLAGS@ \
	-export-dynamic -version-info 0:0:0

libcbtf_core_bfd_la_LIBADD = \
	@TARGET_BINUTILS_BFD_LIB@ \
	@TARGET_MESSAGES_PERFDATA_LIBS@ \
	@LIBLTDL@

libcbtf_core_symtabapi_la_CXXFLAGS = \
	-I$(top_srcdir)/include \
	@TARGET_LIBDWARF_CPPFLAGS@ \
	@TARGET_DYNINST_CPPFLAGS@ \
	@TARGET_MESSAGES_CPPFLAGS@ \
	@LTDLINCL@ 

libcbtf_core_symtabapi_la_LDFLAGS = \
	@TARGET_LIBDWARF_LDFLAGS@ \
	@TARGET_DYNINST_LDFLAGS@ \
	@TARGET_MESSAGES_LDFLAGS@ \
	-export-dynamic -version-info 0:0:0


libcbtf_core_symtabapi_la_LIBADD = \
	@TARGET_LIBDWARF_LIBS@ \
	@TARGET_DYNINST_LIBS@ \
	@TARGET_MESSAGES_PERFDATA_LIBS@ \
	@LIBLTDL@

libcbtf_core_mrnet_la_CXXFLAGS = \
	-I$(top_srcdir)/include \
	@TARGET_MESSAGES_CPPFLAGS@ \
	@TARGET_MRNET_CPPFLAGS@ \
	@LTDLINCL@ 

libcbtf_core_mrnet_la_LDFLAGS = \
	@TARGET_MESSAGES_LDFLAGS@ \
	@TARGET_MRNET_LDFLAGS@ \
	-export-dynamic -version-info 0:0:0

libcbtf_core_mrnet_la_LIBADD = \
	@TARGET_MESSAGES_PERFDATA_LIBS@ \
	@TARGET_MRNET_LIBS@ \
	@LIBLTDL@

#endif for HAVE_TARGET_STATIC
endif
#endif for HAVE_TARGET_SHARED
endif
# else for BUILD_TARGETED
else
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
	@DYNINST_CPPFLAGS@ \
	@MESSAGES_CPPFLAGS@ \
	@LTDLINCL@ 

libcbtf_core_symtabapi_la_LDFLAGS = \
	@LIBDWARF_LDFLAGS@ \
	@DYNINST_LDFLAGS@ \
	@MESSAGES_LDFLAGS@ \
	-export-dynamic -version-info 0:0:0


libcbtf_core_symtabapi_la_LIBADD = \
	@LIBDWARF_LIBS@ \
	@DYNINST_LIBS@ \
	@MESSAGES_PERFDATA_LIBS@ \
	@LIBLTDL@

libcbtf_core_mrnet_la_CXXFLAGS = \
	-I$(top_srcdir)/include \
	@MESSAGES_CPPFLAGS@ \
	@MRNET_CPPFLAGS@ \
	@LTDLINCL@ 

libcbtf_core_mrnet_la_LDFLAGS = \
	@MESSAGES_LDFLAGS@ \
	@MRNET_LDFLAGS@ \
	-export-dynamic -version-info 0:0:0

libcbtf_core_mrnet_la_LIBADD = \
	@MESSAGES_PERFDATA_LIBS@ \
	@MRNET_LIBS@ \
	@LIBLTDL@

endif

libcbtf_core_la_SOURCES = \
	AddressBitmap.cpp \
	AddressBuffer.cpp \
	Blob.cpp \
	Exception.cpp \
	ExtentGroup.cpp \
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
