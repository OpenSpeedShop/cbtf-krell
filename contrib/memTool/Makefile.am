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

BUILT_FILES = mem.xml

nobase_dist_contribxml_DATA = \
        mem.xml

ACLOCAL_AMFLAGS = -I m4



if !BUILD_TARGETED
contribbin_PROGRAMS = memtool
memtool_SOURCES = mem_tool.cpp

memtool_CXXFLAGS = \
	-DBUILDDIR="\"$(abs_builddir)\"" \
	-DLIBDIR="\"$(libdir)\"" \
	-DTOPDIR="\"$(top_srcdir)\"" \
	-DXMLDIR="\"$(contribxmldir)\"" \
	@BOOST_CPPFLAGS@ \
	@CBTF_CPPFLAGS@ @CBTF_XML_CPPFLAGS@ \
	@LIBXERCES_C_CPPFLAGS@ \
	@MRNET_CPPFLAGS@ 

memtool_LDFLAGS = \
	-L$(top_srcdir) \
	@BOOST_LDFLAGS@ \
	@CBTF_LDFLAGS@ @CBTF_XML_LDFLAGS@ \
	@BINUTILS_LDFLAGS@ \
	@LIBXERCES_C_LDFLAGS@ \
	@MRNET_LDFLAGS@ 

memtool_LDADD = \
	-lcbtf-xml \
	-lcbtf-mrnet \
	-lcbtf-core \
	@BOOST_PROGRAM_OPTIONS_LIB@ \
	@BOOST_SYSTEM_LIB@ \
	@BOOST_THREAD_LIB@ \
	@BOOST_FILESYSTEM_LIB@ \
	@CBTF_LIBS@ @CBTF_XML_LIBS@ \
	@BINUTILS_BFD_LIB@ \
	@LIBXERCES_C@ \
	@MRNET_LIBS@ 
endif
  	
if HAVE_TARGET_SHARED
contribbin_PROGRAMS = memtool
memtool_SOURCES = mem_tool.cpp

memtool_CXXFLAGS = \
	-DBUILDDIR="\"$(abs_builddir)\"" \
	-DLIBDIR="\"$(libdir)\"" \
	-DTOPDIR="\"$(top_srcdir)\"" \
	-DXMLDIR="\"$(contribxmldir)\"" \
	@TARGET_BOOST_CPPFLAGS@ \
	@TARGET_CBTF_CPPFLAGS@ @TARGET_CBTF_XML_CPPFLAGS@ \
	@TARGET_LIBXERCES_C_CPPFLAGS@ \
	@TARGET_MRNET_CPPFLAGS@ 

memtool_LDFLAGS = \
	-L$(top_srcdir) \
	@TARGET_BOOST_LDFLAGS@ \
	@TARGET_CBTF_LDFLAGS@ @TARGET_CBTF_XML_LDFLAGS@ \
	@TARGET_BINUTILS_LDFLAGS@ \
	@TARGET_LIBXERCES_C_LDFLAGS@ \
	@TARGET_MRNET_LDFLAGS@ 

memtool_LDADD = \
	-lcbtf-xml \
	-lcbtf-mrnet \
	-lcbtf-core \
	@TARGET_BOOST_PROGRAM_OPTIONS_LIB@ \
	@TARGET_BOOST_SYSTEM_LIB@ \
	@TARGET_BOOST_THREAD_LIB@ \
	@TARGET_BOOST_FILESYSTEM_LIB@ \
	@TARGET_CBTF_LIBS@ @TARGET_CBTF_XML_LIBS@ \
	@TARGET_BINUTILS_BFD_LIB@ \
	@TARGET_LIBXERCES_C@ \
	@TARGET_MRNET_LIBS@ 
endif
  	

contribplugin_LTLIBRARIES = memPlugin.la mrnetPlugin.la


if !BUILD_TARGETED
memPlugin_la_SOURCES = memPlugin.cpp

memPlugin_la_CXXFLAGS = \
	@BOOST_CPPFLAGS@ \
	@CBTF_CPPFLAGS@

memPlugin_la_LDFLAGS = \
	-L$(top_srcdir) \
	-module -avoid-version -shared -rpath /ForceShared \
	@CBTF_LDFLAGS@

memPlugin_la_LIBADD = \
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
else

if HAVE_TARGET_SHARED
memPlugin_la_SOURCES = memPlugin.cpp

memPlugin_la_CXXFLAGS = \
	@TARGET_BOOST_CPPFLAGS@ \
	@TARGET_CBTF_CPPFLAGS@

memPlugin_la_LDFLAGS = \
	-L$(top_srcdir) \
	-module -avoid-version -shared -rpath /ForceShared \
	@TARGET_CBTF_LDFLAGS@

memPlugin_la_LIBADD = \
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
else

if HAVE_TARGET_STATIC
memPlugin_la_SOURCES = memPlugin.cpp

memPlugin_la_CXXFLAGS = \
	-static \
	@TARGET_BOOST_CPPFLAGS@ \
	@TARGET_CBTF_CPPFLAGS@

memPlugin_la_LDFLAGS = \
	-static \
	-L$(top_srcdir) \
	-module -avoid-version -shared -rpath /ForceShared \
	@TARGET_CBTF_LDFLAGS@

memPlugin_la_LIBADD = \
	-lcbtf-core \
	@TARGET_CBTF_LIBS@

mrnetPlugin_la_SOURCES = mrnetPlugin.cpp

mrnetPlugin_la_CXXFLAGS = \
	-static \
	@TARGET_BOOST_CPPFLAGS@ \
	@TARGET_CBTF_CPPFLAGS@ \
	@TARGET_MRNET_CPPFLAGS@

mrnetPlugin_la_LDFLAGS = \
	-static \
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
# endif for target shared
endif
# endif for not build targeted
endif


CLEANFILES = $(BUILT_FILES)

mem.xml: mem.xml.in

	@rm -f mem.xml
	@sed \
                -e 's,@@contriblib_location@@,$(contribplugindir),' \
                -e 's,@@cbtflib_location@@,$(libdir),' \
                $(srcdir)/mem.xml.in > mem.xml

EXTRA_DIST = mem.xml.in
