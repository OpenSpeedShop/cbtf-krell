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

plugin_LTLIBRARIES = CollectionPlugin.la

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

CollectionPlugin_la_SOURCES = \
	AddressAggregatorComponent.cpp \
	AddressBufferComponent.cpp \
	BlobComponent.cpp \
	LinkedObjectComponent.cpp \
	MRNetConverters.cpp \
	ThreadEventComponent.cpp

