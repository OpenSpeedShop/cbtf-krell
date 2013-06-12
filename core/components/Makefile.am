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

plugindir = $(libdir)/KrellInstitute/Components

plugin_LTLIBRARIES = CollectionPlugin.la

if BUILD_TARGETED
if HAVE_TARGET_SHARED
CollectionPlugin_la_CXXFLAGS = \
	-I$(top_srcdir)/include \
	@TARGET_BOOST_CPPFLAGS@ \
	@TARGET_CBTF_CPPFLAGS@ \
        @TARGET_MESSAGES_CPPFLAGS@ \
	@TARGET_MRNET_CPPFLAGS@

CollectionPlugin_la_LDFLAGS = \
	-module -avoid-version \
	-L$(top_srcdir)/src \
        @TARGET_MESSAGES_LDFLAGS@ \
	@TARGET_CBTF_LDFLAGS@ \
	@TARGET_MRNET_LDFLAGS@

CollectionPlugin_la_LIBADD = \
	-lcbtf-core \
	-lcbtf-messages-base \
	-lcbtf-messages-converters-base \
	-lcbtf-messages-collector \
	-lcbtf-messages-converters-collector \
	-lcbtf-messages-events \
	-lcbtf-messages-converters-events \
	-lcbtf-messages-perfdata \
	-lcbtf-messages-converters-perfdata \
	-lcbtf-messages-symtab \
	-lcbtf-messages-converters-symtab \
	-lcbtf-messages-thread \
	-lcbtf-messages-converters-thread \
	@TARGET_CBTF_LIBS@ \
        @TARGET_MESSAGES_BASE_LIBS@ \
        @TARGET_MESSAGES_EVENTS_LIBS@ \
        @TARGET_MESSAGES_PERFDATA_LIBS@ \
	@TARGET_MRNET_LIBS@
else
if HAVE_TARGET_STATIC
CollectionPlugin_la_CXXFLAGS = \
	-static \
	-I$(top_srcdir)/include \
	@TARGET_BOOST_CPPFLAGS@ \
	@TARGET_CBTF_CPPFLAGS@ \
        @TARGET_MESSAGES_CPPFLAGS@ \
	@TARGET_MRNET_CPPFLAGS@

CollectionPlugin_la_LDFLAGS = \
	-static \
	-module -avoid-version \
	-L$(top_srcdir)/src \
        @MESSAGES_LDFLAGS@ \
	@TARGET_CBTF_LDFLAGS@ \
	@TARGET_MRNET_LDFLAGS@

CollectionPlugin_la_LIBADD = \
	-lcbtf-core \
	-lcbtf-messages-base \
	-lcbtf-messages-converters-base \
	-lcbtf-messages-collector \
	-lcbtf-messages-converters-collector \
	-lcbtf-messages-events \
	-lcbtf-messages-converters-events \
	-lcbtf-messages-perfdata \
	-lcbtf-messages-converters-perfdata \
	-lcbtf-messages-symtab \
	-lcbtf-messages-converters-symtab \
	-lcbtf-messages-thread \
	-lcbtf-messages-converters-thread \
	@TARGET_CBTF_LIBS@ \
        @TARGET_MESSAGES_BASE_LIBS@ \
        @TARGET_MESSAGES_EVENTS_LIBS@ \
        @TARGET_MESSAGES_PERFDATA_LIBS@ \
	@TARGET_MRNET_LIBS@
endif
endif
# else non-targeted
else
CollectionPlugin_la_CXXFLAGS = \
	-I$(top_srcdir)/include \
	@BOOST_CPPFLAGS@ \
	@CBTF_CPPFLAGS@ \
        @MESSAGES_CPPFLAGS@ \
	@MRNET_CPPFLAGS@

CollectionPlugin_la_LDFLAGS = \
	-module -avoid-version \
	-L$(top_srcdir)/src \
        @MESSAGES_LDFLAGS@ \
	@CBTF_LDFLAGS@ \
	@MRNET_LDFLAGS@

CollectionPlugin_la_LIBADD = \
	-lcbtf-core \
	-lcbtf-messages-base \
	-lcbtf-messages-converters-base \
	-lcbtf-messages-collector \
	-lcbtf-messages-converters-collector \
	-lcbtf-messages-events \
	-lcbtf-messages-converters-events \
	-lcbtf-messages-perfdata \
	-lcbtf-messages-converters-perfdata \
	-lcbtf-messages-symtab \
	-lcbtf-messages-converters-symtab \
	-lcbtf-messages-thread \
	-lcbtf-messages-converters-thread \
	@CBTF_LIBS@ \
        @MESSAGES_BASE_LIBS@ \
        @MESSAGES_EVENTS_LIBS@ \
        @MESSAGES_PERFDATA_LIBS@ \
	@MRNET_LIBS@
endif

CollectionPlugin_la_SOURCES = \
	AddressAggregatorComponent.cpp \
	AddressBufferComponent.cpp \
	BlobComponent.cpp \
	LinkedObjectComponent.cpp \
	MRNetConverters.cpp \
	ThreadEventComponent.cpp

