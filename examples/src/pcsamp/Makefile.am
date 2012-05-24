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

BUILT_FILES = pcsampDemo.xml

nobase_dist_demoxml_DATA = \
	pcsampDemo.xml

if !BUILD_TARGETED

demobin_PROGRAMS = pcsampDemo

pcsampDemo_CXXFLAGS = \
	-I$(top_srcdir)/include \
	-DBUILDDIR="\"$(abs_builddir)\"" \
	-DLIBDIR="\"$(libdir)\"" \
	-DTOPDIR="\"$(top_srcdir)\"" \
	-DXMLDIR="\"$(demoxmldir)\"" \
	@BOOST_CPPFLAGS@ \
	@CBTF_CPPFLAGS@ @CBTF_XML_CPPFLAGS@ \
	@LIBXERCES_C_CPPFLAGS@ \
        @MESSAGES_CPPFLAGS@ \
	@BINUTILS_CPPFLAGS@ \
	@MRNET_CPPFLAGS@

pcsampDemo_LDFLAGS = \
	-L$(top_srcdir)/src \
	@BOOST_LDFLAGS@ \
	@CBTF_LDFLAGS@ @CBTF_XML_LDFLAGS@ \
	@BINUTILS_LDFLAGS@ \
	@LIBXERCES_C_LDFLAGS@ \
        @MESSAGES_LDFLAGS@ \
	@MRNET_LDFLAGS@

pcsampDemo_LDADD = \
	-lcbtf-mrnet \
	-lcbtf-core \
	@BOOST_PROGRAM_OPTIONS_LIB@ \
	@BOOST_SYSTEM_LIB@ \
	@BOOST_FILESYSTEM_LIB@ \
	@BOOST_THREAD_LIB@ \
	@CBTF_LIBS@ @CBTF_XML_LIBS@ \
	@BINUTILS_BFD_LIB@ \
	@LIBXERCES_C@ \
        @MESSAGES_BASE_LIBS@ \
        @MESSAGES_EVENTS_LIBS@ \
        @MESSAGES_PERFDATA_LIBS@ \
	@MRNET_LIBS@

pcsampDemo_SOURCES = \
	pcsampDemo.cpp
endif

if HAVE_TARGET_SHARED

demobin_PROGRAMS = pcsampDemo

pcsampDemo_CXXFLAGS = \
	-I$(top_srcdir)/include \
	-DBUILDDIR="\"$(abs_builddir)\"" \
	-DLIBDIR="\"$(libdir)\"" \
	-DTOPDIR="\"$(top_srcdir)\"" \
	-DXMLDIR="\"$(demoxmldir)\"" \
	@TARGET_BOOST_CPPFLAGS@ \
	@TARGET_CBTF_CPPFLAGS@ @TARGET_CBTF_XML_CPPFLAGS@ \
	@TARGET_LIBXERCES_C_CPPFLAGS@ \
        @TARGET_MESSAGES_CPPFLAGS@ \
	@TARGET_BINUTILS_CPPFLAGS@ \
	@TARGET_MRNET_CPPFLAGS@

pcsampDemo_LDFLAGS = \
	-L$(top_srcdir)/src \
	@TARGET_BOOST_LDFLAGS@ \
	@TARGET_CBTF_LDFLAGS@ @TARGET_CBTF_XML_LDFLAGS@ \
	@TARGET_BINUTILS_LDFLAGS@ \
	@TARGET_LIBXERCES_C_LDFLAGS@ \
        @TARGET_MESSAGES_LDFLAGS@ \
	@TARGET_MRNET_LDFLAGS@

pcsampDemo_LDADD = \
	-lcbtf-mrnet \
	-lcbtf-core \
	@TARGET_BOOST_PROGRAM_OPTIONS_LIB@ \
	@TARGET_BOOST_SYSTEM_LIB@ \
	@TARGET_BOOST_THREAD_LIB@ \
	@TARGET_CBTF_LIBS@ @TARGET_CBTF_XML_LIBS@ \
	@TARGET_BINUTILS_BFD_LIB@ \
	@TARGET_LIBXERCES_C@ \
        @TARGET_MESSAGES_BASE_LIBS@ \
        @TARGET_MESSAGES_EVENTS_LIBS@ \
        @TARGET_MESSAGES_PERFDATA_LIBS@ \
	@TARGET_MRNET_LIBS@

pcsampDemo_SOURCES = \
	pcsampDemo.cpp
endif

demoplugin_LTLIBRARIES = pcsampDemoPlugin.la

if !BUILD_TARGETED
pcsampDemoPlugin_la_CXXFLAGS = \
	@BOOST_CPPFLAGS@ \
	@CBTF_CPPFLAGS@ \
        @MESSAGES_CPPFLAGS@ \
	@MRNET_CPPFLAGS@

pcsampDemoPlugin_la_LDFLAGS = \
	-module -avoid-version \
        @MESSAGES_LDFLAGS@ \
	@CBTF_LDFLAGS@ \
	@MRNET_LDFLAGS@

pcsampDemoPlugin_la_LIBADD = \
	-lcbtf-core \
	-lcbtf-messages-converters-base \
	-lcbtf-messages-converters-events \
	-lcbtf-messages-converters-perfdata \
	@CBTF_LIBS@ \
        @MESSAGES_BASE_LIBS@ \
        @MESSAGES_EVENTS_LIBS@ \
        @MESSAGES_PERFDATA_LIBS@ \
	@MRNET_LIBS@

pcsampDemoPlugin_la_SOURCES = pcsampDemoPlugin.cpp
else

if HAVE_TARGET_SHARED
pcsampDemoPlugin_la_CXXFLAGS = \
	@TARGET_BOOST_CPPFLAGS@ \
	@TARGET_CBTF_CPPFLAGS@ \
        @TARGET_MESSAGES_CPPFLAGS@ \
	@TARGET_MRNET_CPPFLAGS@

pcsampDemoPlugin_la_LDFLAGS = \
	-module -avoid-version \
        @TARGET_MESSAGES_LDFLAGS@ \
	@TARGET_CBTF_LDFLAGS@ \
	@TARGET_MRNET_LDFLAGS@

pcsampDemoPlugin_la_LIBADD = \
	-lcbtf-core \
	-lcbtf-messages-converters-base \
	-lcbtf-messages-converters-events \
	-lcbtf-messages-converters-perfdata \
	@TARGET_CBTF_LIBS@ \
        @TARGET_MESSAGES_BASE_LIBS@ \
        @TARGET_MESSAGES_EVENTS_LIBS@ \
        @TARGET_MESSAGES_PERFDATA_LIBS@ \
	@TARGET_MRNET_LIBS@

pcsampDemoPlugin_la_SOURCES = pcsampDemoPlugin.cpp

else

if HAVE_TARGET_STATIC
pcsampDemoPlugin_la_CXXFLAGS = \
	-static \
	@TARGET_BOOST_CPPFLAGS@ \
	@TARGET_CBTF_CPPFLAGS@ \
        @TARGET_MESSAGES_CPPFLAGS@ \
	@TARGET_MRNET_CPPFLAGS@

pcsampDemoPlugin_la_LDFLAGS = \
	-static \
	-module -avoid-version \
        @TARGET_MESSAGES_LDFLAGS@ \
	@TARGET_CBTF_LDFLAGS@ \
	@TARGET_MRNET_LDFLAGS@

pcsampDemoPlugin_la_LIBADD = \
	-lcbtf-core \
	-lcbtf-messages-converters-base \
	-lcbtf-messages-converters-events \
	-lcbtf-messages-converters-perfdata \
	@TARGET_CBTF_LIBS@ \
        @TARGET_MESSAGES_BASE_LIBS@ \
        @TARGET_MESSAGES_EVENTS_LIBS@ \
        @TARGET_MESSAGES_PERFDATA_LIBS@ \
	@TARGET_MRNET_LIBS@

pcsampDemoPlugin_la_SOURCES = pcsampDemoPlugin.cpp

# endif for target static
endif
# endif for target shared else clause
endif
# endif for not build targeted else clause
endif

CLEANFILES = $(BUILT_FILES)

pcsampDemo.xml: pcsampDemo.xml.in

	@rm -f pcsampDemo.xml
	@sed \
		-e 's,@@demolib_location@@,$(demoplugindir),' \
		-e 's,@@cbtflib_location@@,$(libdir),' \
		$(srcdir)/pcsampDemo.xml.in > pcsampDemo.xml

EXTRA_DIST = pcsampDemo.xml.in
