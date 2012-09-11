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

CFLAGS = -g -O0

xmldir = $(datadir)/KrellInstitute/xml
plugindir = $(libdir)/KrellInstitute/Components
pkglibdir = $(libdir)/KrellInstitute/Collectors
staticpkglibdir = $(libdir)/KrellInstitute/Collectors/static

BUILT_FILES = hwcsamp.xml

nobase_dist_xml_DATA = \
        hwcsamp.xml


pkglib_LTLIBRARIES =
staticpkglib_LTLIBRARIES =

if !BUILD_TARGETED

if HAVE_LIBMONITOR
pkglib_LTLIBRARIES += hwcsamp-collector-monitor-fileio.la
staticpkglib_LTLIBRARIES += libhwcsamp-monitor-fileio.la

hwcsamp_collector_monitor_fileio_la_CFLAGS = \
	-DCBTF_SERVICE_USE_OFFLINE \
	-DCBTF_SERVICE_USE_FILEIO \
	@CBTF_SERVICES_CPPFLAGS@ \
	@PAPI_CPPFLAGS@ \
	@LIBMONITOR_CPPFLAGS@

hwcsamp_collector_monitor_fileio_la_LDFLAGS = \
	@CBTF_SERVICES_LDFLAGS@ \
	@LIBMONITOR_LDFLAGS@ \
	-no-undefined -module -avoid-version

hwcsamp_collector_monitor_fileio_la_LIBADD = \
	@LIBMONITOR_LIBS@ \
	-lcbtf-services-common \
	-lcbtf-services-monitor \
	-lcbtf-services-offline \
	-lcbtf-services-timer \
	-lcbtf-services-collector-monitor-fileio \
	-lpthread -lrt

hwcsamp_collector_monitor_fileio_la_SOURCES = \
	collector.c

libhwcsamp_monitor_fileio_la_CFLAGS = \
	-static \
	-DCBTF_SERVICE_USE_OFFLINE \
	-DCBTF_SERVICE_USE_FILEIO \
	@CBTF_SERVICES_CPPFLAGS@ \
	@PAPI_CPPFLAGS@ \
	@LIBMONITOR_CPPFLAGS@

libhwcsamp_monitor_fileio_la_LDFLAGS = \
	-static \
	@CBTF_SERVICES_LDFLAGS@ \
	@LIBMONITOR_LDFLAGS@ \
	-no-undefined

libhwcsamp_monitor_fileio_la_LIBADD = \
	@LIBMONITOR_LIBS@ \
	-lcbtf-services-common \
	-lcbtf-services-monitor \
	-lcbtf-services-offline \
	-lcbtf-services-timer \
	-lcbtf-services-collector-monitor-fileio \
	-lpthread -lrt

libhwcsamp_monitor_fileio_la_SOURCES = \
	collector.c

if HAVE_MRNET

pkglib_LTLIBRARIES += hwcsamp-collector-monitor-mrnet.la
staticpkglib_LTLIBRARIES += libhwcsamp-monitor-mrnet.la

hwcsamp_collector_monitor_mrnet_la_CFLAGS = \
	-DCBTF_SERVICE_USE_OFFLINE \
	-DCBTF_SERVICE_USE_MRNET \
	@CBTF_SERVICES_CPPFLAGS@ \
	@LIBMONITOR_CPPFLAGS@ \
	@MESSAGES_CPPFLAGS@ \
	@PAPI_CPPFLAGS@ \
	@MRNET_CPPFLAGS@

hwcsamp_collector_monitor_mrnet_la_LDFLAGS = \
	@CBTF_SERVICES_LDFLAGS@ \
	@LIBMONITOR_LDFLAGS@ \
	@MRNET_LDFLAGS@ \
	-no-undefined -module -avoid-version

hwcsamp_collector_monitor_mrnet_la_LIBADD = \
	@LIBMONITOR_LIBS@ \
	@MRNET_LWR_LIBS@ \
	-lcbtf-services-common \
	-lcbtf-services-monitor \
	-lcbtf-services-offline \
	-lcbtf-services-timer \
	-lcbtf-services-collector-monitor-mrnet \
	-lpthread -lrt

hwcsamp_collector_monitor_mrnet_la_SOURCES = \
	collector.c

libhwcsamp_monitor_mrnet_la_CFLAGS = \
	-static \
	-DCBTF_SERVICE_USE_OFFLINE \
	-DCBTF_SERVICE_USE_MRNET \
	@CBTF_SERVICES_CPPFLAGS@ \
	@LIBMONITOR_CPPFLAGS@ \
	@MESSAGES_CPPFLAGS@ \
	@PAPI_CPPFLAGS@ \
	@MRNET_CPPFLAGS@

libhwcsamp_monitor_mrnet_la_LDFLAGS = \
	-static \
	@CBTF_SERVICES_LDFLAGS@ \
	@LIBMONITOR_LDFLAGS@ \
	@MRNET_LDFLAGS@ \
	-no-undefined

libhwcsamp_monitor_mrnet_la_LIBADD = \
	@LIBMONITOR_LIBS@ \
	@MRNET_LWR_LIBS@ \
	-lcbtf-services-common \
	-lcbtf-services-monitor \
	-lcbtf-services-offline \
	-lcbtf-services-timer \
	-lcbtf-services-collector-monitor-mrnet \
	-lpthread -lrt

libhwcsamp_monitor_mrnet_la_SOURCES = \
	collector.c


pkglib_LTLIBRARIES += hwcsamp-collector-monitor-mrnet-mpi.la
staticpkglib_LTLIBRARIES += libhwcsamp-monitor-mrnet-mpi.la

hwcsamp_collector_monitor_mrnet_mpi_la_CFLAGS = \
	-DCBTF_SERVICE_USE_OFFLINE \
	-DCBTF_SERVICE_USE_MRNET \
	-DCBTF_SERVICE_USE_MRNET_MPI \
	@CBTF_SERVICES_CPPFLAGS@ \
	@LIBMONITOR_CPPFLAGS@ \
	@MESSAGES_CPPFLAGS@ \
	@PAPI_CPPFLAGS@ \
	@MRNET_CPPFLAGS@

hwcsamp_collector_monitor_mrnet_mpi_la_LDFLAGS = \
	@CBTF_SERVICES_LDFLAGS@ \
	@LIBMONITOR_LDFLAGS@ \
	@MRNET_LDFLAGS@ \
	-no-undefined -module -avoid-version

hwcsamp_collector_monitor_mrnet_mpi_la_LIBADD = \
	@LIBMONITOR_LIBS@ \
	@MRNET_LWR_LIBS@ \
	-lcbtf-services-common \
	-lcbtf-services-monitor \
	-lcbtf-services-offline \
	-lcbtf-services-timer \
	-lcbtf-services-data \
	-lcbtf-services-collector-monitor-mrnet-mpi \
	-lpthread -lrt

hwcsamp_collector_monitor_mrnet_mpi_la_SOURCES = \
	collector.c

libhwcsamp_monitor_mrnet_mpi_la_CFLAGS = \
	-static \
	-DCBTF_SERVICE_USE_OFFLINE \
	-DCBTF_SERVICE_USE_MRNET \
	-DCBTF_SERVICE_USE_MRNET_MPI \
	@CBTF_SERVICES_CPPFLAGS@ \
	@LIBMONITOR_CPPFLAGS@ \
	@MESSAGES_CPPFLAGS@ \
	@PAPI_CPPFLAGS@ \
	@MRNET_CPPFLAGS@

libhwcsamp_monitor_mrnet_mpi_la_LDFLAGS = \
	-static \
	@CBTF_SERVICES_LDFLAGS@ \
	@LIBMONITOR_LDFLAGS@ \
	@MRNET_LDFLAGS@ \
	-no-undefined

libhwcsamp_monitor_mrnet_mpi_la_LIBADD = \
	@LIBMONITOR_LIBS@ \
	@MRNET_LWR_LIBS@ \
	-lcbtf-services-common \
	-lcbtf-services-monitor \
	-lcbtf-services-offline \
	-lcbtf-services-timer \
	-lcbtf-services-collector-monitor-mrnet-mpi \
	-lpthread -lrt

libhwcsamp_monitor_mrnet_mpi_la_SOURCES = \
	collector.c

# endif for HAVE_MRNET
endif
# endif for HAVE_LIBMONITOR
endif

# endif for if not build targeted (below)
endif

if HAVE_TARGET_LIBMONITOR
if HAVE_TARGET_SHARED
pkglib_LTLIBRARIES += hwcsamp-collector-monitor-fileio.la
endif
staticpkglib_LTLIBRARIES += libhwcsamp-monitor-fileio.la

if HAVE_TARGET_SHARED
hwcsamp_collector_monitor_fileio_la_CFLAGS = \
	-DCBTF_SERVICE_USE_OFFLINE \
	-DCBTF_SERVICE_USE_FILEIO \
	@TARGET_MESSAGES_CPPFLAGS@ \
	@TARGET_CBTF_SERVICES_CPPFLAGS@ \
	@TARGET_LIBMONITOR_CPPFLAGS@

hwcsamp_collector_monitor_fileio_la_LDFLAGS = \
	@TARGET_CBTF_SERVICES_LDFLAGS@ \
	@TARGET_LIBMONITOR_LDFLAGS@ \
	-no-undefined -module -avoid-version

hwcsamp_collector_monitor_fileio_la_LIBADD = \
	@TARGET_LIBMONITOR_LIBS@ \
	-lcbtf-services-common \
	-lcbtf-services-monitor \
	-lcbtf-services-offline \
	-lcbtf-services-timer \
	-lcbtf-services-collector-monitor-fileio \
	-lpthread -lrt

hwcsamp_collector_monitor_fileio_la_SOURCES = \
	collector.c

libhwcsamp_monitor_fileio_la_CFLAGS = \
	-static \
	-DCBTF_SERVICE_USE_OFFLINE \
	-DCBTF_SERVICE_USE_FILEIO \
	@TARGET_MESSAGES_CPPFLAGS@ \
	@TARGET_CBTF_SERVICES_CPPFLAGS@ \
	@TARGET_LIBMONITOR_CPPFLAGS@

libhwcsamp_monitor_fileio_la_LDFLAGS = \
	-static \
	@TARGET_CBTF_SERVICES_LDFLAGS@ \
	@TARGET_LIBMONITOR_LDFLAGS@ \
	-no-undefined

libhwcsamp_monitor_fileio_la_LIBADD = \
	@TARGET_LIBMONITOR_LIBS@ \
	-lcbtf-services-common \
	-lcbtf-services-monitor \
	-lcbtf-services-offline \
	-lcbtf-services-timer \
	-lcbtf-services-collector-monitor-fileio \
	-lpthread -lrt

libhwcsamp_monitor_fileio_la_SOURCES = \
	collector.c
else

if HAVE_TARGET_STATIC
libhwcsamp_monitor_fileio_la_CFLAGS = \
	-static \
	-DCBTF_SERVICE_USE_OFFLINE \
	-DCBTF_SERVICE_USE_FILEIO \
	@TARGET_MESSAGES_CPPFLAGS@ \
	@TARGET_CBTF_SERVICES_CPPFLAGS@ \
	@TARGET_LIBMONITOR_CPPFLAGS@

libhwcsamp_monitor_fileio_la_LDFLAGS = \
	-static \
	@TARGET_CBTF_SERVICES_LDFLAGS@ \
	@TARGET_LIBMONITOR_LDFLAGS@ \
	-no-undefined

libhwcsamp_monitor_fileio_la_LIBADD = \
	@TARGET_LIBMONITOR_LIBS@ \
	-lcbtf-services-common \
	-lcbtf-services-monitor \
	-lcbtf-services-offline \
	-lcbtf-services-timer \
	-lcbtf-services-collector-monitor-fileio \
	-lpthread -lrt

libhwcsamp_monitor_fileio_la_SOURCES = \
	collector.c
# endif TARGET_STATIC
endif

# endif for else for not TARGET_SHARED
endif

if HAVE_TARGET_MRNET
if HAVE_TARGET_SHARED
pkglib_LTLIBRARIES += hwcsamp-collector-monitor-mrnet.la
endif
staticpkglib_LTLIBRARIES += libhwcsamp-monitor-mrnet.la

if HAVE_TARGET_SHARED
hwcsamp_collector_monitor_mrnet_la_CFLAGS = \
	-DCBTF_SERVICE_USE_OFFLINE \
	-DCBTF_SERVICE_USE_MRNET \
	@TARGET_CBTF_SERVICES_CPPFLAGS@ \
	@TARGET_LIBMONITOR_CPPFLAGS@ \
	@TARGET_MESSAGES_CPPFLAGS@ \
	@TARGET_PAPI_CPPFLAGS@ \
	@TARGET_MRNET_CPPFLAGS@

hwcsamp_collector_monitor_mrnet_la_LDFLAGS = \
	@TARGET_CBTF_SERVICES_LDFLAGS@ \
	@TARGET_LIBMONITOR_LDFLAGS@ \
	@TARGET_MRNET_LDFLAGS@ \
	-no-undefined -module -avoid-version

hwcsamp_collector_monitor_mrnet_la_LIBADD = \
	@TARGET_LIBMONITOR_LIBS@ \
	@TARGET_MRNET_LWR_LIBS@ \
	-lcbtf-services-common \
	-lcbtf-services-monitor \
	-lcbtf-services-offline \
	-lcbtf-services-timer \
	-lcbtf-services-collector-monitor-mrnet \
	-lpthread -lrt

hwcsamp_collector_monitor_mrnet_la_SOURCES = \
	collector.c

libhwcsamp_monitor_mrnet_la_CFLAGS = \
	-static \
	-DCBTF_SERVICE_USE_OFFLINE \
	-DCBTF_SERVICE_USE_MRNET \
	@TARGET_CBTF_SERVICES_CPPFLAGS@ \
	@TARGET_LIBMONITOR_CPPFLAGS@ \
	@TARGET_MESSAGES_CPPFLAGS@ \
	@TARGET_PAPI_CPPFLAGS@ \
	@TARGET_MRNET_CPPFLAGS@

libhwcsamp_monitor_mrnet_la_LDFLAGS = \
	-static \
	@TARGET_CBTF_SERVICES_LDFLAGS@ \
	@TARGET_LIBMONITOR_LDFLAGS@ \
	@TARGET_MRNET_LDFLAGS@ \
	-no-undefined

libhwcsamp_monitor_mrnet_la_LIBADD = \
	@TARGET_LIBMONITOR_LIBS@ \
	@TARGET_MRNET_LWR_LIBS@ \
	-lcbtf-services-common \
	-lcbtf-services-monitor \
	-lcbtf-services-offline \
	-lcbtf-services-timer \
	-lcbtf-services-collector-monitor-mrnet \
	-lpthread -lrt

libhwcsamp_monitor_mrnet_la_SOURCES = \
	collector.c
else

if HAVE_TARGET_STATIC
libhwcsamp_monitor_mrnet_la_CFLAGS = \
	-static \
	-DCBTF_SERVICE_USE_OFFLINE \
	-DCBTF_SERVICE_USE_MRNET \
	@TARGET_CBTF_SERVICES_CPPFLAGS@ \
	@TARGET_LIBMONITOR_CPPFLAGS@ \
	@TARGET_MESSAGES_CPPFLAGS@ \
	@TARGET_PAPI_CPPFLAGS@ \
	@TARGET_MRNET_CPPFLAGS@

libhwcsamp_monitor_mrnet_la_LDFLAGS = \
	-static \
	@TARGET_CBTF_SERVICES_LDFLAGS@ \
	@TARGET_LIBMONITOR_LDFLAGS@ \
	@TARGET_MRNET_LDFLAGS@ \
	-no-undefined

libhwcsamp_monitor_mrnet_la_LIBADD = \
	@TARGET_LIBMONITOR_LIBS@ \
	@TARGET_MRNET_LWR_LIBS@ \
	-lcbtf-services-common \
	-lcbtf-services-monitor \
	-lcbtf-services-offline \
	-lcbtf-services-timer \
	-lcbtf-services-collector-monitor-mrnet \
	-lpthread -lrt

libhwcsamp_monitor_mrnet_la_SOURCES = \
	collector.c
# endif TARGET_STATIC
endif
# endif for else for not TARGET_SHARED
endif


if HAVE_TARGET_SHARED
pkglib_LTLIBRARIES += hwcsamp-collector-monitor-mrnet-mpi.la
endif
staticpkglib_LTLIBRARIES += libhwcsamp-monitor-mrnet-mpi.la

if HAVE_TARGET_SHARED
hwcsamp_collector_monitor_mrnet_mpi_la_CFLAGS = \
	-DCBTF_SERVICE_USE_OFFLINE \
	-DCBTF_SERVICE_USE_MRNET \
	-DCBTF_SERVICE_USE_MRNET_MPI \
	@TARGET_CBTF_SERVICES_CPPFLAGS@ \
	@TARGET_LIBMONITOR_CPPFLAGS@ \
	@TARGET_MESSAGES_CPPFLAGS@ \
	@TARGET_PAPI_CPPFLAGS@ \
	@TARGET_MRNET_CPPFLAGS@

hwcsamp_collector_monitor_mrnet_mpi_la_LDFLAGS = \
	@TARGET_CBTF_SERVICES_LDFLAGS@ \
	@TARGET_LIBMONITOR_LDFLAGS@ \
	@TARGET_MRNET_LDFLAGS@ \
	-no-undefined -module -avoid-version

hwcsamp_collector_monitor_mrnet_mpi_la_LIBADD = \
	@TARGET_LIBMONITOR_LIBS@ \
	@TARGET_MRNET_LWR_LIBS@ \
	-lcbtf-services-common \
	-lcbtf-services-monitor \
	-lcbtf-services-offline \
	-lcbtf-services-timer \
	-lcbtf-services-data \
	-lcbtf-services-collector-monitor-mrnet-mpi \
	-lpthread -lrt

hwcsamp_collector_monitor_mrnet_mpi_la_SOURCES = \
	collector.c

libhwcsamp_monitor_mrnet_mpi_la_CFLAGS = \
	-static \
	-DCBTF_SERVICE_USE_OFFLINE \
	-DCBTF_SERVICE_USE_MRNET \
	-DCBTF_SERVICE_USE_MRNET_MPI \
	@TARGET_CBTF_SERVICES_CPPFLAGS@ \
	@TARGET_LIBMONITOR_CPPFLAGS@ \
	@TARGET_MESSAGES_CPPFLAGS@ \
	@TARGET_PAPI_CPPFLAGS@ \
	@TARGET_MRNET_CPPFLAGS@

libhwcsamp_monitor_mrnet_mpi_la_LDFLAGS = \
	-static \
	@TARGET_CBTF_SERVICES_LDFLAGS@ \
	@TARGET_LIBMONITOR_LDFLAGS@ \
	@TARGET_MRNET_LDFLAGS@ \
	-no-undefined

libhwcsamp_monitor_mrnet_mpi_la_LIBADD = \
	@TARGET_LIBMONITOR_LIBS@ \
	@TARGET_MRNET_LWR_LIBS@ \
	-lcbtf-services-common \
	-lcbtf-services-monitor \
	-lcbtf-services-offline \
	-lcbtf-services-timer \
	-lcbtf-services-collector-monitor-mrnet-mpi \
	-lpthread -lrt

libhwcsamp_monitor_mrnet_mpi_la_SOURCES = \
	collector.c
else

if HAVE_TARGET_STATIC
libhwcsamp_monitor_mrnet_mpi_la_CFLAGS = \
	-static \
	-DCBTF_SERVICE_USE_OFFLINE \
	-DCBTF_SERVICE_USE_MRNET \
	-DCBTF_SERVICE_USE_MRNET_MPI \
	@TARGET_CBTF_SERVICES_CPPFLAGS@ \
	@TARGET_LIBMONITOR_CPPFLAGS@ \
	@TARGET_MESSAGES_CPPFLAGS@ \
	@TARGET_PAPI_CPPFLAGS@ \
	@TARGET_MRNET_CPPFLAGS@

libhwcsamp_monitor_mrnet_mpi_la_LDFLAGS = \
	-static \
	@TARGET_CBTF_SERVICES_LDFLAGS@ \
	@TARGET_LIBMONITOR_LDFLAGS@ \
	@TARGET_MRNET_LDFLAGS@ \
	-no-undefined

libhwcsamp_monitor_mrnet_mpi_la_LIBADD = \
	@TARGET_LIBMONITOR_LIBS@ \
	@TARGET_MRNET_LWR_LIBS@ \
	-lcbtf-services-common \
	-lcbtf-services-monitor \
	-lcbtf-services-offline \
	-lcbtf-services-timer \
	-lcbtf-services-collector-monitor-mrnet-mpi \
	-lpthread -lrt

libhwcsamp_monitor_mrnet_mpi_la_SOURCES = \
	collector.c
# endif TARGET_STATIC
endif
# endif for else for not TARGET_SHARED
endif

# endif for else for not TARGET_MRNET
endif
# endif for else for not TARGET_LIBMONITOR
endif


CLEANFILES = $(BUILT_FILES)

hwcsamp.xml: hwcsamp.xml.in

	@rm -f hwcsamp.xml
	@sed \
		-e 's,@@component_location@@,$(plugindir),' \
		-e 's,@@cbtflib_location@@,$(libdir),' \
		$(srcdir)/hwcsamp.xml.in > hwcsamp.xml

EXTRA_DIST = hwcsamp.xml.in
