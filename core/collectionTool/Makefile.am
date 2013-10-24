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

xmldir = $(datadir)/KrellInstitute/xml

bin_PROGRAMS = collectionTool

collectionTool_CXXFLAGS = \
	-DLIBDIR="\"$(libdir)\"" \
	-DXMLDIR="\"$(xmldir)\"" \
	-I$(top_srcdir)/include \
	@BOOST_CPPFLAGS@ \
	@CBTF_CPPFLAGS@ \
	@CBTF_MRNET_CPPFLAGS@ \
	@CBTF_XML_CPPFLAGS@ \
	@DYNINST_CPPFLAGS@ \
        @LIBXERCES_C_CPPFLAGS@ \
	@LIBDWARF_CPPFLAGS@ \
	@MRNET_CPPFLAGS@

# Any lib directory search path that may override a library
# that is potentially in the default (/usr) should come last.
# examples of this are binutils, dwarf, boost, xerces-c...
collectionTool_LDFLAGS = \
	-L$(top_srcdir)/src \
	@BOOST_LDFLAGS@ \
	@CBTF_LDFLAGS@ \
	@CBTF_MRNET_LDFLAGS@ \
	@CBTF_XML_LDFLAGS@ \
	@DYNINST_LDFLAGS@ \
	@LIBELF_LDFLAGS@ \
	@LIBDWARF_LDFLAGS@ \
	@LIBXERCES_C_LDFLAGS@ \
	@MRNET_LDFLAGS@

collectionTool_LDADD = \
	@BOOST_PROGRAM_OPTIONS_LIB@ \
	@BOOST_FILESYSTEM_LIB@ \
	@BOOST_SYSTEM_LIB@ \
	@BOOST_THREAD_LIB@ \
	@CBTF_LIBS@ \
	@CBTF_MRNET_LIBS@\
	@CBTF_XML_LIBS@ \
	@DYNINST_SYMTABAPI_LIBS@ \
	@LIBELF_LIBS@ \
	@LIBDWARF_LIBS@ \
        @LIBXERCES_C@ \
	@MRNET_LIBS@ \
	-lcbtf-core-symtabapi -lcbtf-core-mrnet -lcbtf-core

collectionTool_SOURCES = \
	collectionTool.cpp


