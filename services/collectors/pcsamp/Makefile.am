################################################################################
# Copyright (c) 2010 Krell Institute All Rights Reserved.
#
# This library is free software; you can redistribute it and/or modify it under
# the terms of the GNU Lesser General Public License as published by the Free
# Software Foundation; either version 2.1 of the License, or (at your option)
# any later version.
#
# This library is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
# details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this library; if not, write to the Free Software Foundation, Inc.,
# 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
################################################################################

plugindir = $(libdir)/KrellInstitute/Collectors

plugin_LTLIBRARIES =


if HAVE_LIBMONITOR
OFFLINE_SOURCES = \
	monitor.c

endif

plugin_LTLIBRARIES += pcsamp-collector.la
pcsamp_collector_la_CFLAGS = \
	@MESSAGES_CPPFLAGS@ \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/src/data

pcsamp_collector_la_LDFLAGS = \
	@MESSAGES_LDFLAGS@ \
	-L$(top_srcdir)/src/common \
	-L$(top_srcdir)/src/timer \
	-L$(top_srcdir)/src/data \
	-no-undefined -module -avoid-version

pcsamp_collector_la_LIBADD = \
	@MESSAGES_BASE_LIBS@ \
	@MESSAGES_EVENTS_LIBS@ \
	@MESSAGES_PERFDATA_LIBS@ \
	-lcbtf-services-common \
	-lcbtf-services-timer \
	-lpthread -lrt

pcsamp_collector_la_SOURCES = \
	collector.c

if HAVE_LIBMONITOR
plugin_LTLIBRARIES += pcsamp-collector-monitor.la libpcsamp-monitor.la
pcsamp_collector_monitor_la_CFLAGS = \
	$(LIBMONITOR_CPPFLAGS) \
	@MESSAGES_CPPFLAGS@ \
	-DCBTF_SERVICE_USE_OFFLINE \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/src/offline \
	-I$(top_srcdir)/src/data

pcsamp_collector_monitor_la_LDFLAGS = \
	$(LIBMONITOR_LDFLAGS) \
	-L$(top_srcdir)/src/common \
	-L$(top_srcdir)/src/monitor \
	-L$(top_srcdir)/src/offline \
	-L$(top_srcdir)/src/timer \
	-L$(top_srcdir)/src/data \
	-no-undefined -module -avoid-version

pcsamp_collector_monitor_la_LIBADD = \
	$(LIBMONITOR_LIBS) \
	-lcbtf-services-common \
	-lcbtf-services-monitor \
	-lcbtf-services-offline \
	-lcbtf-services-timer \
	-lpthread -lrt

pcsamp_collector_monitor_la_SOURCES = \
	$(OFFLINE_SOURCES) \
	collector.c

libpcsamp_monitor_la_CFLAGS = \
	-static \
	$(LIBMONITOR_CPPFLAGS) \
	@MESSAGES_CPPFLAGS@ \
	-DCBTF_SERVICE_USE_OFFLINE \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/src/offline \
	-I$(top_srcdir)/src/data

libpcsamp_monitor_la_LDFLAGS = \
	-static \
	$(LIBMONITOR_LDFLAGS) \
	-L$(top_srcdir)/src/common \
	-L$(top_srcdir)/src/monitor \
	-L$(top_srcdir)/src/offline \
	-L$(top_srcdir)/src/timer \
	-L$(top_srcdir)/src/data \
	-no-undefined

libpcsamp_monitor_la_LIBADD = \
	$(LIBMONITOR_LIBS) \
	-lcbtf-services-common \
	-lcbtf-services-monitor \
	-lcbtf-services-offline \
	-lcbtf-services-timer \
	-lpthread -lrt

libpcsamp_monitor_la_SOURCES = \
	$(OFFLINE_SOURCES) \
	collector.c
endif
