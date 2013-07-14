################################################################################
# Copyright (c) 2010-2013 Krell Institute All Rights Reserved.
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

CFLAGS = -g -O0

nobase_include_HEADERS = \
	KrellInstitute/Services/Collector.h


lib_LTLIBRARIES =

if HAVE_LIBMONITOR
OFFLINE_SOURCES = \
	monitor.c
endif

if HAVE_LIBMONITOR
lib_LTLIBRARIES += libcbtf-services-collector-monitor-fileio.la

libcbtf_services_collector_monitor_fileio_la_CFLAGS = \
	-DCBTF_SERVICE_USE_OFFLINE \
	-DCBTF_SERVICE_USE_FILEIO \
	@MESSAGES_CPPFLAGS@ \
	@LIBMONITOR_CPPFLAGS@

libcbtf_services_collector_monitor_fileio_la_LDFLAGS = \
	@LIBMONITOR_LDFLAGS@ \
	-no-undefined -version-info 0:0:0

libcbtf_services_collector_monitor_fileio_la_LIBADD = \
	@LIBMONITOR_LIBS@ \
	-lpthread -lrt

libcbtf_services_collector_monitor_fileio_la_SOURCES = \
	$(OFFLINE_SOURCES) \
	collector.c

if HAVE_MRNET

lib_LTLIBRARIES += libcbtf-services-collector-monitor-mrnet.la

libcbtf_services_collector_monitor_mrnet_la_CFLAGS = \
	-DCBTF_SERVICE_USE_OFFLINE \
	-DCBTF_SERVICE_USE_MRNET \
	@MESSAGES_CPPFLAGS@ \
	@LIBMONITOR_CPPFLAGS@ \
	@MESSAGES_CPPFLAGS@ \
	@MRNET_CPPFLAGS@

libcbtf_services_collector_monitor_mrnet_la_LDFLAGS = \
	@MESSAGES_LDFLAGS@ \
	@LIBMONITOR_LDFLAGS@ \
	@MRNET_LDFLAGS@ \
	-no-undefined -version-info 0:0:0

libcbtf_services_collector_monitor_mrnet_la_LIBADD = \
	@LIBMONITOR_LIBS@ \
	@MRNET_LWR_LIBS@ \
	-lpthread -lrt

libcbtf_services_collector_monitor_mrnet_la_SOURCES = \
	$(OFFLINE_SOURCES) \
	collector.c

lib_LTLIBRARIES += libcbtf-services-collector-monitor-mrnet-mpi.la

libcbtf_services_collector_monitor_mrnet_mpi_la_CFLAGS = \
	-DCBTF_SERVICE_USE_OFFLINE \
	-DCBTF_SERVICE_USE_MRNET \
	-DCBTF_SERVICE_USE_MRNET_MPI \
	@MESSAGES_CPPFLAGS@ \
	@LIBMONITOR_CPPFLAGS@ \
	@MRNET_CPPFLAGS@

libcbtf_services_collector_monitor_mrnet_mpi_la_LDFLAGS = \
	@MESSAGES_LDFLAGS@ \
	@LIBMONITOR_LDFLAGS@ \
	@MRNET_LDFLAGS@ \
	-no-undefined -version-info 0:0:0

libcbtf_services_collector_monitor_mrnet_mpi_la_LIBADD = \
	@LIBMONITOR_LIBS@ \
	@MRNET_LWR_LIBS@ \
	-lpthread -lrt

libcbtf_services_collector_monitor_mrnet_mpi_la_SOURCES = \
	$(OFFLINE_SOURCES) \
	collector.c

# endif for HAVE_MRNET
endif
# endif for HAVE_LIBMONITOR
endif

