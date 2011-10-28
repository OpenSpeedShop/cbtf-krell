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

ACLOCAL_AMFLAGS = -I m4

noinst_PROGRAMS = tbonFS

tbonFS_SOURCES = tbonFS_tool.cpp 

tbonFS_CXXFLAGS = \
	-DBUILDDIR="\"$(abs_builddir)\"" \
	-DLIBDIR="\"$(libdir)\"" \
	-DTOPDIR="\"$(top_srcdir)\"" \
	@BOOST_CPPFLAGS@ \
	@CBTF_CPPFLAGS@ @CBTF_XML_CPPFLAGS@ \
	@LIBXERCES_C_CPPFLAGS@ \
	@MRNET_CPPFLAGS@ 

tbonFS_LDFLAGS = \
	-L$(top_srcdir) \
	@BOOST_LDFLAGS@ \
	@CBTF_LDFLAGS@ @CBTF_XML_LDFLAGS@ \
	@LIBXERCES_C_LDFLAGS@ \
	@MRNET_LDFLAGS@ 

tbonFS_LDADD = \
	-lcbtf-xml \
	-lcbtf-mrnet \
	-lcbtf-core \
	-lboost_program_options \
	@CBTF_LIBS@ @CBTF_XML_LIBS@ \
	@LIBXERCES_C@ \
	@MRNET_LIBS@ 
  	

noinst_LTLIBRARIES = tbonFSPlugin.la tbonFSFilter.la mrnetPlugin.la

tbonFSPlugin_la_SOURCES = tbonFSPlugin.cpp

tbonFSPlugin_la_CXXFLAGS = \
	@BOOST_CPPFLAGS@ \
	@CBTF_CPPFLAGS@

tbonFSPlugin_la_LDFLAGS = \
	-L$(top_srcdir) \
	-module -avoid-version -shared -rpath /ForceShared \
	@CBTF_LDFLAGS@

tbonFSPlugin_la_LIBADD = \
	-lcbtf-core \
	@CBTF_LIBS@

tbonFSFilter_la_SOURCES = tbonFSFilter.cpp

tbonFSFilter_la_CXXFLAGS = \
	@BOOST_CPPFLAGS@ \
	@CBTF_CPPFLAGS@

tbonFSFilter_la_LDFLAGS = \
	-L$(top_srcdir) \
	-module -avoid-version -shared -rpath /ForceShared \
	@CBTF_LDFLAGS@

tbonFSFilter_la_LIBADD = \
	-lcbtf-core \
	@CBTF_LIBS@

mrnetPlugin_la_SOURCES = mrnetPlugin.cpp

mrnetPlugin_la_CXXFLAGS = \
	@BOOST_CPPFLAGS@ \
	@CBTF_CPPFLAGS@ \
	@MRNET_CPPFLAGS@

mrnetPlugin_la_LDFLAGS = \
        -L$(top_srcdir) \
	-module -avoid-version -shared -rpath /ForceShared \
        @CBTF_LDFLAGS@ \
	@MRNET_LDFLAGS@

mrnetPlugin_la_LIBADD = \
	-lcbtf-core \
	-lcbtf-mrnet \
	@CBTF_LIBS@ \
	@MRNET_LIBS@

