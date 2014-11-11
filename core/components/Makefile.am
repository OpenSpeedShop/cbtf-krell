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

plugindir = $(libdir)/KrellInstitute/Components

plugin_LTLIBRARIES = MRNetConvertorsPlugin.la \
		     AggregationPlugin.la \
		     ThreadPlugin.la \
		     LinkedObjectPlugin.la \
		     SymbolPlugin.la

MRNetConvertorsPlugin_la_CXXFLAGS = \
	-I$(top_srcdir)/include \
	@BOOST_CPPFLAGS@ \
	@CBTF_CPPFLAGS@ \
        @MESSAGES_CPPFLAGS@ \
	@MRNET_CPPFLAGS@

MRNetConvertorsPlugin_la_LDFLAGS = \
	-module -avoid-version \
	-L$(top_srcdir)/src \
        @MESSAGES_LDFLAGS@ \
	@CBTF_LDFLAGS@ \
	@MRNET_LDFLAGS@

MRNetConvertorsPlugin_la_LIBADD = \
	-lcbtf-core \
	@CBTF_LIBS@ \
	@MRNET_LIBS@

MRNetConvertorsPlugin_la_SOURCES = \
	MRNetConverters.cpp

AggregationPlugin_la_CXXFLAGS = \
	-I$(top_srcdir)/include \
	@BOOST_CPPFLAGS@ \
	@CBTF_CPPFLAGS@ \
        @MESSAGES_CPPFLAGS@ \
	@MRNET_CPPFLAGS@

AggregationPlugin_la_LDFLAGS = \
	-module -avoid-version \
	-L$(top_srcdir)/src \
        @MESSAGES_LDFLAGS@ \
	@CBTF_LDFLAGS@ \
	@MRNET_LDFLAGS@

AggregationPlugin_la_LIBADD = \
	-lcbtf-core \
	-lcbtf-messages-base \
	-lcbtf-messages-converters-base \
	-lcbtf-messages-collector \
	-lcbtf-messages-converters-collector \
	-lcbtf-messages-events \
	-lcbtf-messages-converters-events \
	-lcbtf-messages-perfdata \
	-lcbtf-messages-converters-perfdata \
	@CBTF_LIBS@ \
        @MESSAGES_BASE_LIBS@ \
        @MESSAGES_EVENTS_LIBS@ \
        @MESSAGES_PERFDATA_LIBS@ \
	@MRNET_LIBS@

AggregationPlugin_la_SOURCES = \
	AddressAggregatorComponent.cpp \
	AddressBufferComponent.cpp

SymbolPlugin_la_CXXFLAGS = \
	-I$(top_srcdir)/include \
	@BOOST_CPPFLAGS@ \
	@CBTF_CPPFLAGS@ \
        @MESSAGES_CPPFLAGS@ \
	@MRNET_CPPFLAGS@

SymbolPlugin_la_LDFLAGS = \
	-module -avoid-version \
	-L$(top_srcdir)/src \
        @MESSAGES_LDFLAGS@ \
	@CBTF_LDFLAGS@ \
	@MRNET_LDFLAGS@

SymbolPlugin_la_LIBADD = \
	-lcbtf-core \
	-lcbtf-core-symtabapi \
	-lcbtf-messages-base \
	-lcbtf-messages-converters-base \
	-lcbtf-messages-events \
	-lcbtf-messages-converters-events \
	-lcbtf-messages-symtab \
	-lcbtf-messages-converters-symtab \
	@CBTF_LIBS@ \
        @MESSAGES_BASE_LIBS@ \
        @MESSAGES_EVENTS_LIBS@ \
	@MRNET_LIBS@

SymbolPlugin_la_SOURCES = \
	SymbolComponent.cpp

ThreadPlugin_la_CXXFLAGS = \
	-I$(top_srcdir)/include \
	@BOOST_CPPFLAGS@ \
	@CBTF_CPPFLAGS@ \
        @MESSAGES_CPPFLAGS@ \
	@MRNET_CPPFLAGS@

ThreadPlugin_la_LDFLAGS = \
	-module -avoid-version \
	-L$(top_srcdir)/src \
        @MESSAGES_LDFLAGS@ \
	@CBTF_LDFLAGS@ \
	@MRNET_LDFLAGS@

ThreadPlugin_la_LIBADD = \
	-lcbtf-core \
	-lcbtf-messages-base \
	-lcbtf-messages-converters-base \
	-lcbtf-messages-events \
	-lcbtf-messages-converters-events \
	-lcbtf-messages-thread \
	-lcbtf-messages-converters-thread \
	@CBTF_LIBS@ \
        @MESSAGES_BASE_LIBS@ \
        @MESSAGES_EVENTS_LIBS@ \
	@MRNET_LIBS@

ThreadPlugin_la_SOURCES = \
	ThreadEventComponent.cpp

LinkedObjectPlugin_la_CXXFLAGS = \
	-I$(top_srcdir)/include \
	@BOOST_CPPFLAGS@ \
	@CBTF_CPPFLAGS@ \
        @MESSAGES_CPPFLAGS@ \
	@MRNET_CPPFLAGS@

LinkedObjectPlugin_la_LDFLAGS = \
	-module -avoid-version \
	-L$(top_srcdir)/src \
        @MESSAGES_LDFLAGS@ \
	@CBTF_LDFLAGS@ \
	@MRNET_LDFLAGS@

LinkedObjectPlugin_la_LIBADD = \
	-lcbtf-core \
	-lcbtf-messages-base \
	-lcbtf-messages-converters-base \
	-lcbtf-messages-events \
	-lcbtf-messages-converters-events \
	@CBTF_LIBS@ \
        @MESSAGES_BASE_LIBS@ \
        @MESSAGES_EVENTS_LIBS@ \
	@MRNET_LIBS@

LinkedObjectPlugin_la_SOURCES = \
	LinkedObjectComponent.cpp
