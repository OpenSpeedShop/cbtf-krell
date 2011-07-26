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

CFLAGS = -g -O0

pkglibdir = $(libdir)/KrellInstitute/Collectors

pkglib_LTLIBRARIES =


if HAVE_LIBMONITOR
OFFLINE_SOURCES = \
	monitor.c

endif

if HAVE_LIBMONITOR
pkglib_LTLIBRARIES += usertime-collector-monitor-fileio.la libusertime-monitor-fileio.la
usertime_collector_monitor_fileio_la_CFLAGS = \
	@LIBMONITOR_CPPFLAGS@ \
	@LIBUNWIND_CPPFLAGS@ \
	@MESSAGES_CPPFLAGS@ \
	-DCBTF_SERVICE_USE_OFFLINE \
	-DCBTF_SERVICE_USE_FILEIO \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/src/offline \
	-I$(top_srcdir)/src/unwind \
	-I$(top_srcdir)/src/data

usertime_collector_monitor_fileio_la_LDFLAGS = \
	@LIBMONITOR_LDFLAGS@ \
	@LIBUNWIND_LDFLAGS@ \
	-L$(top_srcdir)/src/common \
	-L$(top_srcdir)/src/monitor \
	-L$(top_srcdir)/src/offline \
	-L$(top_srcdir)/src/timer \
	-L$(top_srcdir)/src/unwind \
	-L$(top_srcdir)/src/data \
	-no-undefined -module -avoid-version

usertime_collector_monitor_fileio_la_LIBADD = \
	@LIBMONITOR_LIBS@ \
	@LIBUNWIND_LIBS@ \
	-lcbtf-services-common \
	-lcbtf-services-monitor \
	-lcbtf-services-offline \
	-lcbtf-services-timer \
	-lcbtf-services-unwind \
	-lpthread -lrt

usertime_collector_monitor_fileio_la_SOURCES = \
	$(OFFLINE_SOURCES) \
	collector.c

libusertime_monitor_fileio_la_CFLAGS = \
	-static \
	@LIBMONITOR_CPPFLAGS@ \
	@LIBUNWIND_CPPFLAGS@ \
	@MESSAGES_CPPFLAGS@ \
	-DCBTF_SERVICE_USE_OFFLINE \
	-DCBTF_SERVICE_USE_FILEIO \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/src/offline \
	-I$(top_srcdir)/src/unwind \
	-I$(top_srcdir)/src/data

libusertime_monitor_fileio_la_LDFLAGS = \
	-static \
	@LIBMONITOR_LDFLAGS@ \
	@LIBUNWIND_LDFLAGS@ \
	-L$(top_srcdir)/src/common \
	-L$(top_srcdir)/src/monitor \
	-L$(top_srcdir)/src/offline \
	-L$(top_srcdir)/src/timer \
	-L$(top_srcdir)/src/unwind \
	-L$(top_srcdir)/src/data \
	-no-undefined

libusertime_monitor_fileio_la_LIBADD = \
	@LIBMONITOR_LIBS@ \
	@LIBUNWIND_LIBS@ \
	-lcbtf-services-common \
	-lcbtf-services-monitor \
	-lcbtf-services-offline \
	-lcbtf-services-timer \
	-lcbtf-services-unwind \
	-lpthread -lrt

libusertime_monitor_fileio_la_SOURCES = \
	$(OFFLINE_SOURCES) \
	collector.c

if HAVE_MRNET
pkglib_LTLIBRARIES += usertime-collector-monitor-mrnet.la libusertime-monitor-mrnet.la
usertime_collector_monitor_mrnet_la_CFLAGS = \
	@LIBMONITOR_CPPFLAGS@ \
	@LIBUNWIND_CPPFLAGS@ \
	@MESSAGES_CPPFLAGS@ \
	@MRNET_CPPFLAGS@ \
	-DCBTF_SERVICE_USE_OFFLINE \
	-DCBTF_SERVICE_USE_MRNET \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/src/offline \
	-I$(top_srcdir)/src/unwind \
	-I$(top_srcdir)/src/data

usertime_collector_monitor_mrnet_la_LDFLAGS = \
	@LIBMONITOR_LDFLAGS@ \
	@LIBUNWIND_LDFLAGS@ \
	@MRNET_LDFLAGS@ \
	-L$(top_srcdir)/src/common \
	-L$(top_srcdir)/src/monitor \
	-L$(top_srcdir)/src/offline \
	-L$(top_srcdir)/src/timer \
	-L$(top_srcdir)/src/unwind \
	-L$(top_srcdir)/src/data \
	-no-undefined -module -avoid-version

usertime_collector_monitor_mrnet_la_LIBADD = \
	@LIBMONITOR_LIBS@ \
	@LIBUNWIND_LIBS@ \
	@MRNET_LW_LIBS@ \
	-lcbtf-services-common \
	-lcbtf-services-monitor \
	-lcbtf-services-offline \
	-lcbtf-services-timer \
	-lcbtf-services-unwind \
	-lpthread -lrt

usertime_collector_monitor_mrnet_la_SOURCES = \
	$(OFFLINE_SOURCES) \
	collector.c

libusertime_monitor_mrnet_la_CFLAGS = \
	-static \
	@LIBMONITOR_CPPFLAGS@ \
	@LIBUNWIND_CPPFLAGS@ \
	@MESSAGES_CPPFLAGS@ \
	@MRNET_CPPFLAGS@ \
	-DCBTF_SERVICE_USE_OFFLINE \
	-DCBTF_SERVICE_USE_MRNET \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/src/offline \
	-I$(top_srcdir)/src/unwind \
	-I$(top_srcdir)/src/data

libusertime_monitor_mrnet_la_LDFLAGS = \
	-static \
	@LIBMONITOR_LDFLAGS@ \
	@LIBUNWIND_LDFLAGS@ \
	@MRNET_LDFLAGS@ \
	-L$(top_srcdir)/src/common \
	-L$(top_srcdir)/src/monitor \
	-L$(top_srcdir)/src/offline \
	-L$(top_srcdir)/src/timer \
	-L$(top_srcdir)/src/unwind \
	-L$(top_srcdir)/src/data \
	-no-undefined

libusertime_monitor_mrnet_la_LIBADD = \
	@LIBMONITOR_LIBS@ \
	@LIBUNWIND_LIBS@ \
	@MRNET_LW_LIBS@ \
	-lcbtf-services-common \
	-lcbtf-services-monitor \
	-lcbtf-services-offline \
	-lcbtf-services-timer \
	-lcbtf-services-unwind \
	-lpthread -lrt

libusertime_monitor_mrnet_la_SOURCES = \
	$(OFFLINE_SOURCES) \
	collector.c


pkglib_LTLIBRARIES += usertime-collector-monitor-mrnet-mpi.la libusertime-monitor-mrnet-mpi.la
usertime_collector_monitor_mrnet_mpi_la_CFLAGS = \
	@LIBMONITOR_CPPFLAGS@ \
	@LIBUNWIND_CPPFLAGS@ \
	@MESSAGES_CPPFLAGS@ \
	@MRNET_CPPFLAGS@ \
	-DCBTF_SERVICE_USE_OFFLINE \
	-DCBTF_SERVICE_USE_MRNET \
	-DCBTF_SERVICE_USE_MRNET_MPI \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/src/offline \
	-I$(top_srcdir)/src/unwind \
	-I$(top_srcdir)/src/data

usertime_collector_monitor_mrnet_mpi_la_LDFLAGS = \
	@LIBMONITOR_LDFLAGS@ \
	@LIBUNWIND_LDFLAGS@ \
	@MRNET_LDFLAGS@ \
	-L$(top_srcdir)/src/common \
	-L$(top_srcdir)/src/monitor \
	-L$(top_srcdir)/src/offline \
	-L$(top_srcdir)/src/timer \
	-L$(top_srcdir)/src/unwind \
	-L$(top_srcdir)/src/data \
	-no-undefined -module -avoid-version

usertime_collector_monitor_mrnet_mpi_la_LIBADD = \
	@LIBMONITOR_LIBS@ \
	@LIBUNWIND_LIBS@ \
	@MRNET_LW_LIBS@ \
	-lcbtf-services-common \
	-lcbtf-services-monitor \
	-lcbtf-services-offline \
	-lcbtf-services-timer \
	-lcbtf-services-unwind \
	-lcbtf-services-data \
	-lpthread -lrt

usertime_collector_monitor_mrnet_mpi_la_SOURCES = \
	$(OFFLINE_SOURCES) \
	collector.c

libusertime_monitor_mrnet_mpi_la_CFLAGS = \
	-static \
	@LIBMONITOR_CPPFLAGS@ \
	@LIBUNWIND_CPPFLAGS@ \
	@MESSAGES_CPPFLAGS@ \
	@MRNET_CPPFLAGS@ \
	-DCBTF_SERVICE_USE_OFFLINE \
	-DCBTF_SERVICE_USE_MRNET \
	-DCBTF_SERVICE_USE_MRNET_MPI \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/src/offline \
	-I$(top_srcdir)/src/unwind \
	-I$(top_srcdir)/src/data

libusertime_monitor_mrnet_mpi_la_LDFLAGS = \
	-static \
	@LIBMONITOR_LDFLAGS@ \
	@LIBUNWIND_LDFLAGS@ \
	@MRNET_LDFLAGS@ \
	-L$(top_srcdir)/src/common \
	-L$(top_srcdir)/src/monitor \
	-L$(top_srcdir)/src/offline \
	-L$(top_srcdir)/src/timer \
	-L$(top_srcdir)/src/unwind \
	-L$(top_srcdir)/src/data \
	-no-undefined

libusertime_monitor_mrnet_mpi_la_LIBADD = \
	@LIBMONITOR_LIBS@ \
	@LIBUNWIND_LIBS@ \
	@MRNET_LW_LIBS@ \
	-lcbtf-services-common \
	-lcbtf-services-monitor \
	-lcbtf-services-offline \
	-lcbtf-services-timer \
	-lcbtf-services-unwind \
	-lpthread -lrt

libusertime_monitor_mrnet_mpi_la_SOURCES = \
	$(OFFLINE_SOURCES) \
	collector.c

endif
endif

CLEANFILES = $(BUILT_FILES)
