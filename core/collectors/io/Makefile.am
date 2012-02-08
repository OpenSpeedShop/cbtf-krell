################################################################################
# Copyright (c) 2010-2012 Krell Institute All Rights Reserved.
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

xmldir = $(datadir)/KrellInstitute/xml
plugindir = $(libdir)/KrellInstitute/Components
pkglibdir = $(libdir)/KrellInstitute/Collectors
staticpkglibdir = $(libdir)/KrellInstitute/Collectors/static

BUILT_FILES = io.xml

nobase_dist_xml_DATA = \
        io.xml

pkglib_LTLIBRARIES =
staticpkglib_LTLIBRARIES =

WRAPPER_SOURCES = wrappers.c

if !BUILD_TARGETED

if HAVE_LIBMONITOR
OFFLINE_SOURCES = \
	monitor.c

endif

if HAVE_LIBMONITOR
pkglib_LTLIBRARIES += io-collector-monitor-fileio.la
staticpkglib_LTLIBRARIES += libio-monitor-fileio.la
io_collector_monitor_fileio_la_CFLAGS = \
	-D__USE_GNU \
	-DCBTF_SERVICE_USE_OFFLINE \
	-DCBTF_SERVICE_USE_FILEIO \
	@CBTF_SERVICES_CPPFLAGS@ \
	@LIBMONITOR_CPPFLAGS@ \
	@LIBUNWIND_CPPFLAGS@

io_collector_monitor_fileio_la_LDFLAGS = \
	@CBTF_SERVICES_LDFLAGS@ \
	@LIBMONITOR_LDFLAGS@ \
	@LIBUNWIND_LDFLAGS@ \
	-no-undefined -module -avoid-version

io_collector_monitor_fileio_la_LIBADD = \
	@LIBMONITOR_LIBS@ \
	@LIBUNWIND_LIBS@ \
	-lcbtf-services-common \
	-lcbtf-services-monitor \
	-lcbtf-services-offline \
	-lcbtf-services-timer \
	-lcbtf-services-unwind \
	-lpthread -lrt

io_collector_monitor_fileio_la_SOURCES = \
	$(OFFLINE_SOURCES) \
	collector.c $(WRAPPER_SOURCES)

libio_monitor_fileio_la_CFLAGS = \
	-static \
	-D__USE_GNU \
	-DCBTF_SERVICE_USE_OFFLINE \
	-DCBTF_SERVICE_USE_FILEIO \
	@CBTF_SERVICES_CPPFLAGS@ \
	@LIBMONITOR_CPPFLAGS@ \
	@LIBUNWIND_CPPFLAGS@ \
	@MESSAGES_CPPFLAGS@

libio_monitor_fileio_la_LDFLAGS = \
	-static \
	@CBTF_SERVICES_LDFLAGS@ \
	@LIBMONITOR_LDFLAGS@ \
	@LIBUNWIND_LDFLAGS@ \
	-no-undefined

libio_monitor_fileio_la_LIBADD = \
	@LIBMONITOR_LIBS@ \
	@LIBUNWIND_LIBS@ \
	-lcbtf-services-common \
	-lcbtf-services-monitor \
	-lcbtf-services-offline \
	-lcbtf-services-timer \
	-lcbtf-services-unwind \
	-lpthread -lrt

libio_monitor_fileio_la_SOURCES = \
	$(OFFLINE_SOURCES) \
	collector.c $(WRAPPER_SOURCES)

if HAVE_MRNET
pkglib_LTLIBRARIES += io-collector-monitor-mrnet.la
staticpkglib_LTLIBRARIES += libio-monitor-mrnet.la

io_collector_monitor_mrnet_la_CFLAGS = \
	-D__USE_GNU \
	-DCBTF_SERVICE_USE_OFFLINE \
	-DCBTF_SERVICE_USE_MRNET \
	@CBTF_SERVICES_CPPFLAGS@ \
	@LIBMONITOR_CPPFLAGS@ \
	@LIBUNWIND_CPPFLAGS@ \
	@MESSAGES_CPPFLAGS@ \
	@MRNET_CPPFLAGS@

io_collector_monitor_mrnet_la_LDFLAGS = \
	@CBTF_SERVICES_LDFLAGS@ \
	@LIBMONITOR_LDFLAGS@ \
	@LIBUNWIND_LDFLAGS@ \
	@MRNET_LDFLAGS@ \
	-no-undefined -module -avoid-version

io_collector_monitor_mrnet_la_LIBADD = \
	@LIBMONITOR_LIBS@ \
	@LIBUNWIND_LIBS@ \
	@MRNET_LW_LIBS@ \
	-lcbtf-services-common \
	-lcbtf-services-monitor \
	-lcbtf-services-offline \
	-lcbtf-services-timer \
	-lcbtf-services-unwind \
	-lpthread -lrt

io_collector_monitor_mrnet_la_SOURCES = \
	$(OFFLINE_SOURCES) \
	collector.c $(WRAPPER_SOURCES)

libio_monitor_mrnet_la_CFLAGS = \
	-static \
	-D__USE_GNU \
	-DCBTF_SERVICE_USE_OFFLINE \
	-DCBTF_SERVICE_USE_MRNET \
	@CBTF_SERVICES_CPPFLAGS@ \
	@LIBMONITOR_CPPFLAGS@ \
	@LIBUNWIND_CPPFLAGS@ \
	@MESSAGES_CPPFLAGS@ \
	@MRNET_CPPFLAGS@

libio_monitor_mrnet_la_LDFLAGS = \
	-static \
	@CBTF_SERVICES_LDFLAGS@ \
	@LIBMONITOR_LDFLAGS@ \
	@LIBUNWIND_LDFLAGS@ \
	@MRNET_LDFLAGS@ \
	-no-undefined

libio_monitor_mrnet_la_LIBADD = \
	@LIBMONITOR_LIBS@ \
	@LIBUNWIND_LIBS@ \
	@MRNET_LW_LIBS@ \
	-lcbtf-services-common \
	-lcbtf-services-monitor \
	-lcbtf-services-offline \
	-lcbtf-services-timer \
	-lcbtf-services-unwind \
	-lpthread -lrt

libio_monitor_mrnet_la_SOURCES = \
	$(OFFLINE_SOURCES) \
	collector.c $(WRAPPER_SOURCES)


pkglib_LTLIBRARIES += io-collector-monitor-mrnet-mpi.la
staticpkglib_LTLIBRARIES += libio-monitor-mrnet-mpi.la

io_collector_monitor_mrnet_mpi_la_CFLAGS = \
	-D__USE_GNU \
	-DCBTF_SERVICE_USE_OFFLINE \
	-DCBTF_SERVICE_USE_MRNET \
	-DCBTF_SERVICE_USE_MRNET_MPI \
	@CBTF_SERVICES_CPPFLAGS@ \
	@LIBMONITOR_CPPFLAGS@ \
	@LIBUNWIND_CPPFLAGS@ \
	@MESSAGES_CPPFLAGS@ \
	@MRNET_CPPFLAGS@

io_collector_monitor_mrnet_mpi_la_LDFLAGS = \
	@CBTF_SERVICES_LDFLAGS@ \
	@LIBMONITOR_LDFLAGS@ \
	@LIBUNWIND_LDFLAGS@ \
	@MRNET_LDFLAGS@ \
	-no-undefined -module -avoid-version

io_collector_monitor_mrnet_mpi_la_LIBADD = \
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

io_collector_monitor_mrnet_mpi_la_SOURCES = \
	$(OFFLINE_SOURCES) \
	collector.c $(WRAPPER_SOURCES)

libio_monitor_mrnet_mpi_la_CFLAGS = \
	-static \
	-D__USE_GNU \
	-DCBTF_SERVICE_USE_OFFLINE \
	-DCBTF_SERVICE_USE_MRNET \
	-DCBTF_SERVICE_USE_MRNET_MPI \
	@CBTF_SERVICES_CPPFLAGS@ \
	@LIBMONITOR_CPPFLAGS@ \
	@LIBUNWIND_CPPFLAGS@ \
	@MESSAGES_CPPFLAGS@ \
	@MRNET_CPPFLAGS@

libio_monitor_mrnet_mpi_la_LDFLAGS = \
	-static \
	@CBTF_SERVICES_LDFLAGS@ \
	@LIBMONITOR_LDFLAGS@ \
	@LIBUNWIND_LDFLAGS@ \
	@MRNET_LDFLAGS@ \
	-no-undefined

libio_monitor_mrnet_mpi_la_LIBADD = \
	@LIBMONITOR_LIBS@ \
	@LIBUNWIND_LIBS@ \
	@MRNET_LW_LIBS@ \
	-lcbtf-services-common \
	-lcbtf-services-monitor \
	-lcbtf-services-offline \
	-lcbtf-services-timer \
	-lcbtf-services-unwind \
	-lpthread -lrt

libio_monitor_mrnet_mpi_la_SOURCES = \
	$(OFFLINE_SOURCES) \
	collector.c $(WRAPPER_SOURCES)

# endif for HAVE_MRNET
endif
# endif for HAVE_LIBMONITOR
endif

# endif for if not build targeted (below)
endif

if HAVE_TARGET_LIBMONITOR
OFFLINE_SOURCES = \
	monitor.c
endif

if HAVE_TARGET_LIBMONITOR
pkglib_LTLIBRARIES += io-collector-monitor-fileio.la
staticpkglib_LTLIBRARIES += libio-monitor-fileio.la

if HAVE_TARGET_SHARED
io_collector_monitor_fileio_la_CFLAGS = \
	-D__USE_GNU \
	-DCBTF_SERVICE_USE_OFFLINE \
	-DCBTF_SERVICE_USE_FILEIO \
	@TARGET_CBTF_SERVICES_CPPFLAGS@ \
	@TARGET_LIBMONITOR_CPPFLAGS@ \
	@TARGET_LIBUNWIND_CPPFLAGS@

io_collector_monitor_fileio_la_LDFLAGS = \
	@TARGET_CBTF_SERVICES_LDFLAGS@ \
	@TARGET_LIBMONITOR_LDFLAGS@ \
	@TARGET_LIBUNWIND_LDFLAGS@ \
	-no-undefined -module -avoid-version

io_collector_monitor_fileio_la_LIBADD = \
	@TARGET_LIBMONITOR_LIBS@ \
	@TARGET_LIBUNWIND_LIBS@ \
	-lcbtf-services-common \
	-lcbtf-services-monitor \
	-lcbtf-services-offline \
	-lcbtf-services-timer \
	-lcbtf-services-unwind \
	-lpthread -lrt

io_collector_monitor_fileio_la_SOURCES = \
	$(OFFLINE_SOURCES) \
	collector.c $(WRAPPER_SOURCES)

libio_monitor_fileio_la_CFLAGS = \
	-static \
	-D__USE_GNU \
	-DCBTF_SERVICE_USE_OFFLINE \
	-DCBTF_SERVICE_USE_FILEIO \
	@TARGET_CBTF_SERVICES_CPPFLAGS@ \
	@TARGET_LIBMONITOR_CPPFLAGS@ \
	@TARGET_LIBUNWIND_CPPFLAGS@ \
	@TARGET_MESSAGES_CPPFLAGS@

libio_monitor_fileio_la_LDFLAGS = \
	-static \
	@TARGET_CBTF_SERVICES_LDFLAGS@ \
	@TARGET_LIBMONITOR_LDFLAGS@ \
	@TARGET_LIBUNWIND_LDFLAGS@ \
	-no-undefined

libio_monitor_fileio_la_LIBADD = \
	@TARGET_LIBMONITOR_LIBS@ \
	@TARGET_LIBUNWIND_LIBS@ \
	-lcbtf-services-common \
	-lcbtf-services-monitor \
	-lcbtf-services-offline \
	-lcbtf-services-timer \
	-lcbtf-services-unwind \
	-lpthread -lrt

libio_monitor_fileio_la_SOURCES = \
	$(OFFLINE_SOURCES) \
	collector.c $(WRAPPER_SOURCES)
else

if HAVE_TARGET_STATIC
libio_monitor_fileio_la_CFLAGS = \
	-static \
	-D__USE_GNU \
	-DCBTF_SERVICE_USE_OFFLINE \
	-DCBTF_SERVICE_USE_FILEIO \
	@TARGET_CBTF_SERVICES_CPPFLAGS@ \
	@TARGET_LIBMONITOR_CPPFLAGS@ \
	@TARGET_LIBUNWIND_CPPFLAGS@ \
	@TARGET_MESSAGES_CPPFLAGS@

libio_monitor_fileio_la_LDFLAGS = \
	-static \
	@TARGET_CBTF_SERVICES_LDFLAGS@ \
	@TARGET_LIBMONITOR_LDFLAGS@ \
	@TARGET_LIBUNWIND_LDFLAGS@ \
	-no-undefined

libio_monitor_fileio_la_LIBADD = \
	@TARGET_LIBMONITOR_LIBS@ \
	@TARGET_LIBUNWIND_LIBS@ \
	-lcbtf-services-common \
	-lcbtf-services-monitor \
	-lcbtf-services-offline \
	-lcbtf-services-timer \
	-lcbtf-services-unwind \
	-lpthread -lrt

libio_monitor_fileio_la_SOURCES = \
	$(OFFLINE_SOURCES) \
	collector.c $(WRAPPER_SOURCES)
# endif TARGET_STATIC
endif

# endif for else for not TARGET_SHARED
endif

if HAVE_TARGET_MRNET
pkglib_LTLIBRARIES += io-collector-monitor-mrnet.la
staticpkglib_LTLIBRARIES += libio-monitor-mrnet.la

if HAVE_TARGET_SHARED
io_collector_monitor_mrnet_la_CFLAGS = \
	-D__USE_GNU \
	-DCBTF_SERVICE_USE_OFFLINE \
	-DCBTF_SERVICE_USE_MRNET \
	@TARGET_CBTF_SERVICES_CPPFLAGS@ \
	@TARGET_LIBMONITOR_CPPFLAGS@ \
	@TARGET_LIBUNWIND_CPPFLAGS@ \
	@TARGET_MESSAGES_CPPFLAGS@ \
	@TARGET_MRNET_CPPFLAGS@

io_collector_monitor_mrnet_la_LDFLAGS = \
	@TARGET_CBTF_SERVICES_LDFLAGS@ \
	@TARGET_LIBMONITOR_LDFLAGS@ \
	@TARGET_LIBUNWIND_LDFLAGS@ \
	@TARGET_MRNET_LDFLAGS@ \
	-no-undefined -module -avoid-version

io_collector_monitor_mrnet_la_LIBADD = \
	@TARGET_LIBMONITOR_LIBS@ \
	@TARGET_LIBUNWIND_LIBS@ \
	@TARGET_MRNET_LW_LIBS@ \
	-lcbtf-services-common \
	-lcbtf-services-monitor \
	-lcbtf-services-offline \
	-lcbtf-services-timer \
	-lcbtf-services-unwind \
	-lpthread -lrt

io_collector_monitor_mrnet_la_SOURCES = \
	$(OFFLINE_SOURCES) \
	collector.c $(WRAPPER_SOURCES)

libio_monitor_mrnet_la_CFLAGS = \
	-static \
	-D__USE_GNU \
	-DCBTF_SERVICE_USE_OFFLINE \
	-DCBTF_SERVICE_USE_MRNET \
	@TARGET_CBTF_SERVICES_CPPFLAGS@ \
	@TARGET_LIBMONITOR_CPPFLAGS@ \
	@TARGET_LIBUNWIND_CPPFLAGS@ \
	@TARGET_MESSAGES_CPPFLAGS@ \
	@TARGET_MRNET_CPPFLAGS@

libio_monitor_mrnet_la_LDFLAGS = \
	-static \
	@TARGET_CBTF_SERVICES_LDFLAGS@ \
	@TARGET_LIBMONITOR_LDFLAGS@ \
	@TARGET_LIBUNWIND_LDFLAGS@ \
	@TARGET_MRNET_LDFLAGS@ \
	-no-undefined

libio_monitor_mrnet_la_LIBADD = \
	@TARGET_LIBMONITOR_LIBS@ \
	@TARGET_LIBUNWIND_LIBS@ \
	@TARGET_MRNET_LW_LIBS@ \
	-lcbtf-services-common \
	-lcbtf-services-monitor \
	-lcbtf-services-offline \
	-lcbtf-services-timer \
	-lcbtf-services-unwind \
	-lpthread -lrt

libio_monitor_mrnet_la_SOURCES = \
	$(OFFLINE_SOURCES) \
	collector.c $(WRAPPER_SOURCES)
else

if HAVE_TARGET_STATIC

libio_monitor_mrnet_la_CFLAGS = \
	-static \
	-D__USE_GNU \
	-DCBTF_SERVICE_USE_OFFLINE \
	-DCBTF_SERVICE_USE_MRNET \
	@TARGET_CBTF_SERVICES_CPPFLAGS@ \
	@TARGET_LIBMONITOR_CPPFLAGS@ \
	@TARGET_LIBUNWIND_CPPFLAGS@ \
	@TARGET_MESSAGES_CPPFLAGS@ \
	@TARGET_MRNET_CPPFLAGS@

libio_monitor_mrnet_la_LDFLAGS = \
	-static \
	@TARGET_CBTF_SERVICES_LDFLAGS@ \
	@TARGET_LIBMONITOR_LDFLAGS@ \
	@TARGET_LIBUNWIND_LDFLAGS@ \
	@TARGET_MRNET_LDFLAGS@ \
	-no-undefined

libio_monitor_mrnet_la_LIBADD = \
	@TARGET_LIBMONITOR_LIBS@ \
	@TARGET_LIBUNWIND_LIBS@ \
	@TARGET_MRNET_LW_LIBS@ \
	-lcbtf-services-common \
	-lcbtf-services-monitor \
	-lcbtf-services-offline \
	-lcbtf-services-timer \
	-lcbtf-services-unwind \
	-lpthread -lrt

libio_monitor_mrnet_la_SOURCES = \
	$(OFFLINE_SOURCES) \
	collector.c $(WRAPPER_SOURCES)
# endif TARGET_STATIC
endif
# endif for else for not TARGET_SHARED
endif


pkglib_LTLIBRARIES += io-collector-monitor-mrnet-mpi.la
staticpkglib_LTLIBRARIES += libio-monitor-mrnet-mpi.la

if HAVE_TARGET_SHARED
io_collector_monitor_mrnet_mpi_la_CFLAGS = \
	-D__USE_GNU \
	-DCBTF_SERVICE_USE_OFFLINE \
	-DCBTF_SERVICE_USE_MRNET \
	-DCBTF_SERVICE_USE_MRNET_MPI \
	@TARGET_CBTF_SERVICES_CPPFLAGS@ \
	@TARGET_LIBMONITOR_CPPFLAGS@ \
	@TARGET_LIBUNWIND_CPPFLAGS@ \
	@TARGET_MESSAGES_CPPFLAGS@ \
	@TARGET_MRNET_CPPFLAGS@

io_collector_monitor_mrnet_mpi_la_LDFLAGS = \
	@TARGET_CBTF_SERVICES_LDFLAGS@ \
	@TARGET_LIBMONITOR_LDFLAGS@ \
	@TARGET_LIBUNWIND_LDFLAGS@ \
	@TARGET_MRNET_LDFLAGS@ \
	-no-undefined -module -avoid-version

io_collector_monitor_mrnet_mpi_la_LIBADD = \
	@TARGET_LIBMONITOR_LIBS@ \
	@TARGET_LIBUNWIND_LIBS@ \
	@TARGET_MRNET_LW_LIBS@ \
	-lcbtf-services-common \
	-lcbtf-services-monitor \
	-lcbtf-services-offline \
	-lcbtf-services-timer \
	-lcbtf-services-unwind \
	-lcbtf-services-data \
	-lpthread -lrt

io_collector_monitor_mrnet_mpi_la_SOURCES = \
	$(OFFLINE_SOURCES) \
	collector.c $(WRAPPER_SOURCES)

libio_monitor_mrnet_mpi_la_CFLAGS = \
	-static \
	-D__USE_GNU \
	-DCBTF_SERVICE_USE_OFFLINE \
	-DCBTF_SERVICE_USE_MRNET \
	-DCBTF_SERVICE_USE_MRNET_MPI \
	@TARGET_CBTF_SERVICES_CPPFLAGS@ \
	@TARGET_LIBMONITOR_CPPFLAGS@ \
	@TARGET_LIBUNWIND_CPPFLAGS@ \
	@TARGET_MESSAGES_CPPFLAGS@ \
	@TARGET_MRNET_CPPFLAGS@

libio_monitor_mrnet_mpi_la_LDFLAGS = \
	-static \
	@TARGET_CBTF_SERVICES_LDFLAGS@ \
	@TARGET_LIBMONITOR_LDFLAGS@ \
	@TARGET_LIBUNWIND_LDFLAGS@ \
	@TARGET_MRNET_LDFLAGS@ \
	-no-undefined

libio_monitor_mrnet_mpi_la_LIBADD = \
	@TARGET_LIBMONITOR_LIBS@ \
	@TARGET_LIBUNWIND_LIBS@ \
	@TARGET_MRNET_LW_LIBS@ \
	-lcbtf-services-common \
	-lcbtf-services-monitor \
	-lcbtf-services-offline \
	-lcbtf-services-timer \
	-lcbtf-services-unwind \
	-lpthread -lrt

libio_monitor_mrnet_mpi_la_SOURCES = \
	$(OFFLINE_SOURCES) \
	collector.c $(WRAPPER_SOURCES)
else

if HAVE_TARGET_STATIC
libio_monitor_mrnet_mpi_la_CFLAGS = \
	-static \
	-D__USE_GNU \
	-DCBTF_SERVICE_USE_OFFLINE \
	-DCBTF_SERVICE_USE_MRNET \
	-DCBTF_SERVICE_USE_MRNET_MPI \
	@TARGET_CBTF_SERVICES_CPPFLAGS@ \
	@TARGET_LIBMONITOR_CPPFLAGS@ \
	@TARGET_LIBUNWIND_CPPFLAGS@ \
	@TARGET_MESSAGES_CPPFLAGS@ \
	@TARGET_MRNET_CPPFLAGS@

libio_monitor_mrnet_mpi_la_LDFLAGS = \
	-static \
	@TARGET_CBTF_SERVICES_LDFLAGS@ \
	@TARGET_LIBMONITOR_LDFLAGS@ \
	@TARGET_LIBUNWIND_LDFLAGS@ \
	@TARGET_MRNET_LDFLAGS@ \
	-no-undefined

libio_monitor_mrnet_mpi_la_LIBADD = \
	@TARGET_LIBMONITOR_LIBS@ \
	@TARGET_LIBUNWIND_LIBS@ \
	@TARGET_MRNET_LW_LIBS@ \
	-lcbtf-services-common \
	-lcbtf-services-monitor \
	-lcbtf-services-offline \
	-lcbtf-services-timer \
	-lcbtf-services-unwind \
	-lpthread -lrt

libio_monitor_mrnet_mpi_la_SOURCES = \
	$(OFFLINE_SOURCES) \
	collector.c $(WRAPPER_SOURCES)
# endif TARGET_STATIC
endif
# endif for else for not TARGET_SHARED
endif

# endif for else for not TARGET_MRNET
endif
# endif for else for not TARGET_LIBMONITOR
endif


CLEANFILES = $(BUILT_FILES)

io.xml: io.xml.in

	@rm -f io.xml
	@sed \
		-e 's,@@component_location@@,$(plugindir),' \
		-e 's,@@cbtflib_location@@,$(libdir),' \
		$(srcdir)/io.xml.in > io.xml

EXTRA_DIST = io.xml.in
