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
	@MRNET_CPPFLAGS@

pcsampDemo_LDFLAGS = \
	-L$(top_srcdir)/src \
	@BOOST_LDFLAGS@ \
	@CBTF_LDFLAGS@ @CBTF_XML_LDFLAGS@ \
	@LIBXERCES_C_LDFLAGS@ \
        @MESSAGES_LDFLAGS@ \
	@MRNET_LDFLAGS@

pcsampDemo_LDADD = \
	-lcbtf-mrnet \
	-lcbtf-core \
	-lboost_program_options \
	@BOOST_SYSTEM_LIB@ \
	@BOOST_THREAD_LIB@ \
	@CBTF_LIBS@ @CBTF_XML_LIBS@ \
	@LIBXERCES_C@ \
        @MESSAGES_BASE_LIBS@ \
        @MESSAGES_EVENTS_LIBS@ \
        @MESSAGES_PERFDATA_LIBS@ \
	@MRNET_LIBS@

pcsampDemo_SOURCES = \
	pcsampDemo.cpp


demoplugin_LTLIBRARIES = pcsampDemoPlugin.la

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
	@CBTF_LIBS@ \
        @MESSAGES_BASE_LIBS@ \
        @MESSAGES_EVENTS_LIBS@ \
        @MESSAGES_PERFDATA_LIBS@ \
	@MRNET_LIBS@

pcsampDemoPlugin_la_SOURCES = pcsampDemoPlugin.cpp

CLEANFILES = $(BUILT_FILES)

pcsampDemo.xml: pcsampDemo.xml.in

	@rm -f pcsampDemo.xml
	@sed \
		-e 's,@@demolib_location@@,$(demoplugindir),' \
		-e 's,@@cbtflib_location@@,$(libdir),' \
		$(srcdir)/pcsampDemo.xml.in > pcsampDemo.xml

EXTRA_DIST = pcsampDemo.xml.in
