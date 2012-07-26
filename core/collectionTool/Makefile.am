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

xmldir = $(datadir)/KrellInstitute/xml


if !BUILD_TARGETED

bin_PROGRAMS = collectionTool

collectionTool_CXXFLAGS = \
	-DLIBDIR="\"$(libdir)\"" \
	-DXMLDIR="\"$(xmldir)\"" \
	-I$(top_srcdir)/include \
	@BOOST_CPPFLAGS@ \
	@CBTF_CPPFLAGS@ \
	@CBTF_MRNET_CPPFLAGS@ \
	@CBTF_XML_CPPFLAGS@ \
	@MRNET_CPPFLAGS@

collectionTool_LDFLAGS = \
	-L$(top_srcdir)/src \
	@BOOST_LDFLAGS@ \
	@CBTF_LDFLAGS@ \
	@CBTF_MRNET_LDFLAGS@ \
	@CBTF_XML_LDFLAGS@ \
	@MRNET_LDFLAGS@

collectionTool_LDADD = \
	@BOOST_PROGRAM_OPTIONS_LIB@ \
	@BOOST_SYSTEM_LIB@ \
	@BOOST_THREAD_LIB@ \
	@CBTF_LIBS@ \
	@CBTF_MRNET_LIBS@\
	@CBTF_XML_LIBS@\
	-lcbtf-core \
	@MRNET_LIBS@

collectionTool_SOURCES = \
	collectionTool.cpp
# else for not build targeted
else

if HAVE_TARGET_SHARED

bin_PROGRAMS = collectionTool

collectionTool_CXXFLAGS = \
	-DLIBDIR="\"$(libdir)\"" \
	-DXMLDIR="\"$(xmldir)\"" \
	@TARGET_BOOST_CPPFLAGS@ \
	@TARGET_CBTF_CPPFLAGS@ \
	@TARGET_CBTF_MRNET_CPPFLAGS@ \
	@TARGET_CBTF_XML_CPPFLAGS@ \
	@TARGET_MRNET_CPPFLAGS@

collectionTool_LDFLAGS = \
	@TARGET_BOOST_LDFLAGS@ \
	@TARGET_CBTF_LDFLAGS@ \
	@TARGET_CBTF_MRNET_LDFLAGS@ \
	@TARGET_LIBXERCES_C_LDFLAGS@ \
	@TARGET_CBTF_XML_LDFLAGS@ \
	@TARGET_MRNET_LDFLAGS@

collectionTool_LDADD = \
	@TARGET_MRNET_LIBS@ \
	@TARGET_BOOST_PROGRAM_OPTIONS_LIB@ \
	@TARGET_BOOST_SYSTEM_LIB@ \
	@TARGET_BOOST_THREAD_LIB@ \
	@TARGET_BOOST_FILESYSTEM_LIB@ \
	@TARGET_CBTF_LIBS@ \
	@TARGET_LIBXERCES_C@ \
	@TARGET_CBTF_MRNET_LIBS@\
	-lcbtf-core \
	@TARGET_CBTF_XML_LIBS@

collectionTool_SOURCES = \
	collectionTool.cpp
# endif for targeted shared
endif
# endif for targeted else
endif

