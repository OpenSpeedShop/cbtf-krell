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

demoxmldir = $(datadir)/KrellInstitute/demos/xml
demobindir = $(datadir)/KrellInstitute/demos/bin
demoplugindir = $(datadir)/KrellInstitute/demos/lib

BUILT_FILES = daemonToolDemo.xml

nobase_dist_demoxml_DATA = \
	daemonToolDemo.xml

demobin_PROGRAMS = daemonToolDemo

if !BUILD_TARGETED
daemonToolDemo_SOURCES = \
	daemonToolDemo.cpp

daemonToolDemo_CXXFLAGS = \
	-I$(top_srcdir)/include \
	-DBUILDDIR="\"$(abs_builddir)\"" \
	-DLIBDIR="\"$(libdir)\"" \
	-DTOPDIR="\"$(top_srcdir)\"" \
	-DXMLDIR="\"$(demoxmldir)\"" \
	@BOOST_CPPFLAGS@ \
	@CBTF_CPPFLAGS@ @CBTF_XML_CPPFLAGS@ \
	@LIBXERCES_C_CPPFLAGS@ \
	@MRNET_CPPFLAGS@

daemonToolDemo_LDFLAGS = \
	-L$(top_srcdir)/src \
	@BOOST_LDFLAGS@ \
	@CBTF_LDFLAGS@ @CBTF_XML_LDFLAGS@ \
	@LIBXERCES_C_LDFLAGS@ \
	@MRNET_LDFLAGS@

daemonToolDemo_LDADD = \
	-lcbtf-mrnet \
	@BOOST_PROGRAM_OPTIONS_LIB@ \
	@BOOST_SYSTEM_LIB@ \
	@BOOST_THREAD_LIB@ \
	@CBTF_LIBS@ @CBTF_XML_LIBS@ \
	@LIBXERCES_C@ \
	@MRNET_LIBS@
endif

if HAVE_TARGET_SHARED
daemonToolDemo_SOURCES = \
	daemonToolDemo.cpp

daemonToolDemo_CXXFLAGS = \
	-I$(top_srcdir)/include \
	-DBUILDDIR="\"$(abs_builddir)\"" \
	-DLIBDIR="\"$(libdir)\"" \
	-DTOPDIR="\"$(top_srcdir)\"" \
	-DXMLDIR="\"$(demoxmldir)\"" \
	@TARGET_BOOST_CPPFLAGS@ \
	@TARGET_CBTF_CPPFLAGS@ @TARGET_CBTF_XML_CPPFLAGS@ \
	@TARGET_LIBXERCES_C_CPPFLAGS@ \
	@TARGET_MRNET_CPPFLAGS@

daemonToolDemo_LDFLAGS = \
	-L$(top_srcdir)/src \
	@TARGET_BOOST_LDFLAGS@ \
	@TARGET_CBTF_LDFLAGS@ @TARGET_CBTF_XML_LDFLAGS@ \
	@TARGET_LIBXERCES_C_LDFLAGS@ \
	@TARGET_MRNET_LDFLAGS@

daemonToolDemo_LDADD = \
	-lcbtf-mrnet \
	@TARGET_BOOST_PROGRAM_OPTIONS_LIB@ \
	@TARGET_BOOST_SYSTEM_LIB@ \
	@TARGET_BOOST_THREAD_LIB@ \
	@TARGET_CBTF_LIBS@ @TARGET_CBTF_XML_LIBS@ \
	@TARGET_LIBXERCES_C@ \
	@TARGET_MRNET_LIBS@
endif


demoplugin_LTLIBRARIES = daemonToolDemoPlugin.la

if !BUILD_TARGETED
daemonToolDemoPlugin_la_SOURCES = daemonToolDemoPlugin.cpp

daemonToolDemoPlugin_la_CXXFLAGS = \
	@BOOST_CPPFLAGS@ \
	@CBTF_CPPFLAGS@ \
	@MRNET_CPPFLAGS@

daemonToolDemoPlugin_la_LDFLAGS = \
	-module -avoid-version \
	@CBTF_LDFLAGS@ \
	@MRNET_LDFLAGS@

daemonToolDemoPlugin_la_LIBADD = \
	@CBTF_LIBS@ \
	@MRNET_LIBS@
endif

if HAVE_TARGET_SHARED
daemonToolDemoPlugin_la_SOURCES = daemonToolDemoPlugin.cpp

daemonToolDemoPlugin_la_CXXFLAGS = \
	@TARGET_BOOST_CPPFLAGS@ \
	@TARGET_CBTF_CPPFLAGS@ \
	@TARGET_MRNET_CPPFLAGS@

daemonToolDemoPlugin_la_LDFLAGS = \
	-module -avoid-version \
	@TARGET_CBTF_LDFLAGS@ \
	@TARGET_MRNET_LDFLAGS@

daemonToolDemoPlugin_la_LIBADD = \
	@TARGET_CBTF_LIBS@ \
	@TARGET_MRNET_LIBS@
endif

CLEANFILES = $(BUILT_FILES)

daemonToolDemo.xml: daemonToolDemo.xml.in

	@rm -f daemonToolDemo.xml
	@sed \
		-e 's,@@demolib_location@@,$(demoplugindir),' \
		-e 's,@@cbtflib_location@@,$(libdir),' \
		$(srcdir)/daemonToolDemo.xml.in > daemonToolDemo.xml

EXTRA_DIST = daemonToolDemo.xml.in
