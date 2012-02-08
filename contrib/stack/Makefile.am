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

contribxmldir = $(datadir)/KrellInstitute/contrib/xml
contribbindir = $(datadir)/KrellInstitute/contrib/bin
contribplugindir = $(datadir)/KrellInstitute/contrib/lib

BUILT_FILES = stack.xml

nobase_dist_contribxml_DATA = \
        stack.xml

ACLOCAL_AMFLAGS = -I m4

contribbin_PROGRAMS = stack

if !BUILD_TARGETED
stack_SOURCES = stack_tool.cpp

stack_CXXFLAGS = \
	-DBUILDDIR="\"$(abs_builddir)\"" \
	-DLIBDIR="\"$(libdir)\"" \
	-DTOPDIR="\"$(top_srcdir)\"" \
	-DXMLDIR="\"$(contribxmldir)\"" \
	@BOOST_CPPFLAGS@ \
	@CBTF_CPPFLAGS@ @CBTF_XML_CPPFLAGS@ \
	@LIBXERCES_C_CPPFLAGS@ \
	@MRNET_CPPFLAGS@ 

stack_LDFLAGS = \
	-L$(top_srcdir) \
	@BOOST_LDFLAGS@ \
	@CBTF_LDFLAGS@ @CBTF_XML_LDFLAGS@ \
	@BINUTILS_LDFLAGS@ \
	@LIBXERCES_C_LDFLAGS@ \
	@MRNET_LDFLAGS@ 

stack_LDADD = \
	-lcbtf-xml \
	-lcbtf-mrnet \
	-lcbtf-core \
        @BOOST_PROGRAM_OPTIONS_LIB@ \
	@BOOST_SYSTEM_LIB@ \
	@BOOST_THREAD_LIB@ \
	@CBTF_LIBS@ @CBTF_XML_LIBS@ \
	@BINUTILS_BFD_LIB@ \
	@LIBXERCES_C@ \
	@MRNET_LIBS@ 
endif
  	
if HAVE_TARGET_SHARED
stack_SOURCES = stack_tool.cpp

stack_CXXFLAGS = \
	-DBUILDDIR="\"$(abs_builddir)\"" \
	-DLIBDIR="\"$(libdir)\"" \
	-DTOPDIR="\"$(top_srcdir)\"" \
	-DXMLDIR="\"$(contribxmldir)\"" \
	@TARGET_BOOST_CPPFLAGS@ \
	@TARGET_CBTF_CPPFLAGS@ @TARGET_CBTF_XML_CPPFLAGS@ \
	@TARGET_LIBXERCES_C_CPPFLAGS@ \
	@TARGET_MRNET_CPPFLAGS@ 

stack_LDFLAGS = \
	-L$(top_srcdir) \
	@TARGET_BOOST_LDFLAGS@ \
	@TARGET_CBTF_LDFLAGS@ @TARGET_CBTF_XML_LDFLAGS@ \
	@TARGET_BINUTILS_LDFLAGS@ \
	@TARGET_LIBXERCES_C_LDFLAGS@ \
	@TARGET_MRNET_LDFLAGS@ 

stack_LDADD = \
	-lcbtf-xml \
	-lcbtf-mrnet \
	-lcbtf-core \
        @TARGET_BOOST_PROGRAM_OPTIONS_LIB@ \
	@TARGET_BOOST_SYSTEM_LIB@ \
	@TARGET_BOOST_THREAD_LIB@ \
	@TARGET_CBTF_LIBS@ @TARGET_CBTF_XML_LIBS@ \
	@TARGET_BINUTILS_BFD_LIB@ \
	@TARGET_LIBXERCES_C@ \
	@TARGET_MRNET_LIBS@ 
  	
endif

contribplugin_LTLIBRARIES = stackPlugin.la mrnetPlugin.la

if !BUILD_TARGETED
stackPlugin_la_SOURCES = stackPlugin.cpp

stackPlugin_la_CXXFLAGS = \
	@BOOST_CPPFLAGS@ \
	@CBTF_CPPFLAGS@

stackPlugin_la_LDFLAGS = \
	-L$(top_srcdir) \
	-module -avoid-version -shared -rpath /ForceShared \
	@CBTF_LDFLAGS@

stackPlugin_la_LIBADD = \
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
endif

if HAVE_TARGET_SHARED
stackPlugin_la_SOURCES = stackPlugin.cpp

stackPlugin_la_CXXFLAGS = \
	@TARGET_BOOST_CPPFLAGS@ \
	@TARGET_CBTF_CPPFLAGS@

stackPlugin_la_LDFLAGS = \
	-L$(top_srcdir) \
	-module -avoid-version -shared -rpath /ForceShared \
	@TARGET_CBTF_LDFLAGS@

stackPlugin_la_LIBADD = \
	-lcbtf-core \
	@TARGET_CBTF_LIBS@

mrnetPlugin_la_SOURCES = mrnetPlugin.cpp

mrnetPlugin_la_CXXFLAGS = \
	@TARGET_BOOST_CPPFLAGS@ \
	@TARGET_CBTF_CPPFLAGS@ \
	@TARGET_MRNET_CPPFLAGS@

mrnetPlugin_la_LDFLAGS = \
        -L$(top_srcdir) \
	-module -avoid-version -shared -rpath /ForceShared \
        @TARGET_CBTF_LDFLAGS@ \
	@TARGET_MRNET_LDFLAGS@

mrnetPlugin_la_LIBADD = \
	-lcbtf-core \
	-lcbtf-mrnet \
	@TARGET_CBTF_LIBS@ \
	@TARGET_MRNET_LIBS@
endif

CLEANFILES = $(BUILT_FILES)

stack.xml: stack.xml.in

	@rm -f stack.xml
	@sed \
                -e 's,@@contriblib_location@@,$(contribplugindir),' \
                -e 's,@@cbtflib_location@@,$(libdir),' \
                $(srcdir)/stack.xml.in > stack.xml

EXTRA_DIST = stack.xml.in
