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

xmldir = $(datadir)/KrellInstitute/xml
plugindir = $(libdir)/KrellInstitute/Components
pkglibdir = $(libdir)/KrellInstitute/Collectors
staticpkglibdir = $(libdir)/KrellInstitute/Collectors/static

BUILT_FILES = usertime.xml

nobase_dist_xml_DATA = \
        usertime.xml

pkglib_LTLIBRARIES =
staticpkglib_LTLIBRARIES =


if HAVE_LIBMONITOR
OFFLINE_SOURCES = \
	monitor.c

endif

if HAVE_LIBMONITOR
pkglib_LTLIBRARIES += usertime-collector-monitor-fileio.la
staticpkglib_LTLIBRARIES += libusertime-monitor-fileio.la
usertime_collector_monitor_fileio_la_CFLAGS = \
	-DCBTF_SERVICE_USE_OFFLINE \
	-DCBTF_SERVICE_USE_FILEIO \
	@CBTF_SERVICES_CPPFLAGS@ \
	@LIBMONITOR_CPPFLAGS@ \
	@LIBUNWIND_CPPFLAGS@

usertime_collector_monitor_fileio_la_LDFLAGS = \
	@CBTF_SERVICES_LDFLAGS@ \
	@LIBMONITOR_LDFLAGS@ \
	@LIBUNWIND_LDFLAGS@ \
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
	-DCBTF_SERVICE_USE_OFFLINE \
	-DCBTF_SERVICE_USE_FILEIO \
	@CBTF_SERVICES_CPPFLAGS@ \
	@LIBMONITOR_CPPFLAGS@ \
	@LIBUNWIND_CPPFLAGS@ \
	@MESSAGES_CPPFLAGS@

libusertime_monitor_fileio_la_LDFLAGS = \
	-static \
	@CBTF_SERVICES_LDFLAGS@ \
	@LIBMONITOR_LDFLAGS@ \
	@LIBUNWIND_LDFLAGS@ \
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
pkglib_LTLIBRARIES += usertime-collector-monitor-mrnet.la
staticpkglib_LTLIBRARIES += libusertime-monitor-mrnet.la
usertime_collector_monitor_mrnet_la_CFLAGS = \
	-DCBTF_SERVICE_USE_OFFLINE \
	-DCBTF_SERVICE_USE_MRNET \
	@CBTF_SERVICES_CPPFLAGS@ \
	@LIBMONITOR_CPPFLAGS@ \
	@LIBUNWIND_CPPFLAGS@ \
	@MESSAGES_CPPFLAGS@ \
	@MRNET_CPPFLAGS@

usertime_collector_monitor_mrnet_la_LDFLAGS = \
	@CBTF_SERVICES_LDFLAGS@ \
	@LIBMONITOR_LDFLAGS@ \
	@LIBUNWIND_LDFLAGS@ \
	@MRNET_LDFLAGS@ \
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
	-DCBTF_SERVICE_USE_OFFLINE \
	-DCBTF_SERVICE_USE_MRNET \
	@CBTF_SERVICES_CPPFLAGS@ \
	@LIBMONITOR_CPPFLAGS@ \
	@LIBUNWIND_CPPFLAGS@ \
	@MESSAGES_CPPFLAGS@ \
	@MRNET_CPPFLAGS@

libusertime_monitor_mrnet_la_LDFLAGS = \
	-static \
	@CBTF_SERVICES_LDFLAGS@ \
	@LIBMONITOR_LDFLAGS@ \
	@LIBUNWIND_LDFLAGS@ \
	@MRNET_LDFLAGS@ \
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


pkglib_LTLIBRARIES += usertime-collector-monitor-mrnet-mpi.la
staticpkglib_LTLIBRARIES += libusertime-monitor-mrnet-mpi.la
usertime_collector_monitor_mrnet_mpi_la_CFLAGS = \
	-DCBTF_SERVICE_USE_OFFLINE \
	-DCBTF_SERVICE_USE_MRNET \
	-DCBTF_SERVICE_USE_MRNET_MPI \
	@CBTF_SERVICES_CPPFLAGS@ \
	@LIBMONITOR_CPPFLAGS@ \
	@LIBUNWIND_CPPFLAGS@ \
	@MESSAGES_CPPFLAGS@ \
	@MRNET_CPPFLAGS@

usertime_collector_monitor_mrnet_mpi_la_LDFLAGS = \
	@CBTF_SERVICES_LDFLAGS@ \
	@LIBMONITOR_LDFLAGS@ \
	@LIBUNWIND_LDFLAGS@ \
	@MRNET_LDFLAGS@ \
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
	-DCBTF_SERVICE_USE_OFFLINE \
	-DCBTF_SERVICE_USE_MRNET \
	-DCBTF_SERVICE_USE_MRNET_MPI \
	@CBTF_SERVICES_CPPFLAGS@ \
	@LIBMONITOR_CPPFLAGS@ \
	@LIBUNWIND_CPPFLAGS@ \
	@MESSAGES_CPPFLAGS@ \
	@MRNET_CPPFLAGS@

libusertime_monitor_mrnet_mpi_la_LDFLAGS = \
	-static \
	@CBTF_SERVICES_LDFLAGS@ \
	@LIBMONITOR_LDFLAGS@ \
	@LIBUNWIND_LDFLAGS@ \
	@MRNET_LDFLAGS@ \
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


usertime.xml: usertime.xml.in

	@rm -f usertime.xml
	@sed \
		-e 's,@@component_location@@,$(plugindir),' \
		-e 's,@@cbtflib_location@@,$(libdir),' \
		$(srcdir)/usertime.xml.in > usertime.xml

EXTRA_DIST = usertime.xml.in
