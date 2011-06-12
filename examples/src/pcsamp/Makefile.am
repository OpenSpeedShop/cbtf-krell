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

noinst_PROGRAMS = pcsampDemo

pcsampDemo_CXXFLAGS = \
	-I$(top_srcdir)/include \
	-DBUILDDIR="\"$(abs_builddir)\"" \
	-DTOPDIR="\"$(top_srcdir)\"" \
        @MESSAGES_CPPFLAGS@ \
	@BOOST_CPPFLAGS@ \
	@CBTF_CPPFLAGS@ @CBTF_XML_CPPFLAGS@ \
	@LIBXERCES_C_CPPFLAGS@ \
	@MRNET_CPPFLAGS@

pcsampDemo_LDFLAGS = \
	-L$(top_srcdir)/src \
        @MESSAGES_LDFLAGS@ \
	@BOOST_LDFLAGS@ \
	@CBTF_LDFLAGS@ @CBTF_XML_LDFLAGS@ \
	@LIBXERCES_C_LDFLAGS@ \
	@MRNET_LDFLAGS@

pcsampDemo_LDADD = \
	-lcbtf-mrnet \
	-lcbtf-core \
	-lboost_program_options \
        @MESSAGES_BASE_LIBS@ \
        @MESSAGES_EVENTS_LIBS@ \
        @MESSAGES_PERFDATA_LIBS@ \
	@CBTF_LIBS@ @CBTF_XML_LIBS@ \
	@LIBXERCES_C@ \
	@MRNET_LIBS@

pcsampDemo_SOURCES = \
	pcsampDemo.cpp


noinst_LTLIBRARIES = pcsampDemoPlugin.la

pcsampDemoPlugin_la_CXXFLAGS = \
	@BOOST_CPPFLAGS@ \
	@CBTF_CPPFLAGS@ \
        @MESSAGES_CPPFLAGS@ \
	@MRNET_CPPFLAGS@

pcsampDemoPlugin_la_LDFLAGS = \
	-shared -module -avoid-version -rpath /ForceShared \
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
