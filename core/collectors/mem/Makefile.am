################################################################################
# Copyright (c) 2010-2015 Krell Institute All Rights Reserved.
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

BUILT_FILES = mem.xml

nobase_dist_xml_DATA = \
        mem.xml

pkglib_LTLIBRARIES =
staticpkglib_LTLIBRARIES =

WRAPPER_SOURCES = wrappers.c

if HAVE_LIBMONITOR
pkglib_LTLIBRARIES += mem-collector-monitor-fileio.la
staticpkglib_LTLIBRARIES += libmem-monitor-fileio.la

mem_collector_monitor_fileio_la_CFLAGS = \
	-D__USE_GNU \
	-DCBTF_SERVICE_USE_OFFLINE \
	-DCBTF_SERVICE_USE_FILEIO \
	@CBTF_SERVICES_CPPFLAGS@ \
	@LIBMONITOR_CPPFLAGS@ \
	@LIBUNWIND_CPPFLAGS@

mem_collector_monitor_fileio_la_LDFLAGS = \
	@CBTF_SERVICES_LDFLAGS@ \
	@LIBMONITOR_LDFLAGS@ \
	@LIBUNWIND_LDFLAGS@ \
	-no-undefined -module -avoid-version

mem_collector_monitor_fileio_la_LIBADD = \
	@LIBMONITOR_LIBS@ \
	@LIBUNWIND_LIBS@ \
	-lcbtf-services-common \
	-lcbtf-services-monitor \
	-lcbtf-services-offline \
	-lcbtf-services-unwind \
	-lcbtf-services-collector-monitor-fileio \
	-lpthread -lrt

mem_collector_monitor_fileio_la_SOURCES = \
	collector.c $(WRAPPER_SOURCES)

libmem_monitor_fileio_la_CFLAGS = \
	-static \
	-D__USE_GNU \
	-DCBTF_SERVICE_USE_OFFLINE \
	-DCBTF_SERVICE_USE_FILEIO \
	@CBTF_SERVICES_CPPFLAGS@ \
	@LIBMONITOR_CPPFLAGS@ \
	@LIBUNWIND_CPPFLAGS@ \
	@MESSAGES_CPPFLAGS@

libmem_monitor_fileio_la_LDFLAGS = \
	-static \
	@CBTF_SERVICES_LDFLAGS@ \
	@LIBMONITOR_LDFLAGS@ \
	@LIBUNWIND_LDFLAGS@ \
	-no-undefined

libmem_monitor_fileio_la_LIBADD = \
	@LIBMONITOR_LIBS@ \
	@LIBUNWIND_LIBS@ \
	-lcbtf-services-common \
	-lcbtf-services-monitor \
	-lcbtf-services-offline \
	-lcbtf-services-unwind \
	-lcbtf-services-collector-monitor-fileio \
	-lpthread -lrt

libmem_monitor_fileio_la_SOURCES = \
	$(BUILT_SOURCES) \
	collector.c $(WRAPPER_SOURCES)

if HAVE_MRNET
pkglib_LTLIBRARIES += mem-collector-monitor-mrnet.la
staticpkglib_LTLIBRARIES += libmem-monitor-mrnet.la
mem_collector_monitor_mrnet_la_CFLAGS = \
	-D__USE_GNU \
	-DCBTF_SERVICE_USE_OFFLINE \
	-DCBTF_SERVICE_USE_MRNET \
	@CBTF_SERVICES_CPPFLAGS@ \
	@LIBMONITOR_CPPFLAGS@ \
	@LIBUNWIND_CPPFLAGS@ \
	@MESSAGES_CPPFLAGS@ \
	@MRNET_CPPFLAGS@

mem_collector_monitor_mrnet_la_LDFLAGS = \
	@CBTF_SERVICES_LDFLAGS@ \
	@LIBMONITOR_LDFLAGS@ \
	@LIBUNWIND_LDFLAGS@ \
	@MRNET_LDFLAGS@ \
	-no-undefined -module -avoid-version

mem_collector_monitor_mrnet_la_LIBADD = \
	@LIBMONITOR_LIBS@ \
	@LIBUNWIND_LIBS@ \
	@MRNET_LWR_LIBS@ \
	-lcbtf-services-common \
	-lcbtf-services-monitor \
	-lcbtf-services-offline \
	-lcbtf-services-unwind \
	-lcbtf-services-collector-monitor-mrnet \
	-lpthread -lrt

mem_collector_monitor_mrnet_la_SOURCES = \
	collector.c $(WRAPPER_SOURCES)

libmem_monitor_mrnet_la_CFLAGS = \
	-static \
	-D__USE_GNU \
	-DCBTF_SERVICE_USE_OFFLINE \
	-DCBTF_SERVICE_USE_MRNET \
	@CBTF_SERVICES_CPPFLAGS@ \
	@LIBMONITOR_CPPFLAGS@ \
	@LIBUNWIND_CPPFLAGS@ \
	@MESSAGES_CPPFLAGS@ \
	@MRNET_CPPFLAGS@

libmem_monitor_mrnet_la_LDFLAGS = \
	-static \
	@CBTF_SERVICES_LDFLAGS@ \
	@LIBMONITOR_LDFLAGS@ \
	@LIBUNWIND_LDFLAGS@ \
	@MRNET_LDFLAGS@ \
	-no-undefined

libmem_monitor_mrnet_la_LIBADD = \
	@LIBMONITOR_LIBS@ \
	@LIBUNWIND_LIBS@ \
	@MRNET_LWR_LIBS@ \
	-lcbtf-services-common \
	-lcbtf-services-monitor \
	-lcbtf-services-offline \
	-lcbtf-services-unwind \
	-lpthread -lrt

libmem_monitor_mrnet_la_SOURCES = \
	$(BUILT_SOURCES) \
	collector.c $(WRAPPER_SOURCES)

pkglib_LTLIBRARIES += mem-collector-monitor-mrnet-mpi.la
staticpkglib_LTLIBRARIES += libmem-monitor-mrnet-mpi.la
mem_collector_monitor_mrnet_mpi_la_CFLAGS = \
	-D__USE_GNU \
	-DCBTF_SERVICE_USE_OFFLINE \
	-DCBTF_SERVICE_USE_MRNET \
	-DCBTF_SERVICE_USE_MRNET_MPI \
	@CBTF_SERVICES_CPPFLAGS@ \
	@LIBMONITOR_CPPFLAGS@ \
	@LIBUNWIND_CPPFLAGS@ \
	@MESSAGES_CPPFLAGS@ \
	@MRNET_CPPFLAGS@

mem_collector_monitor_mrnet_mpi_la_LDFLAGS = \
	@CBTF_SERVICES_LDFLAGS@ \
	@LIBMONITOR_LDFLAGS@ \
	@LIBUNWIND_LDFLAGS@ \
	@MRNET_LDFLAGS@ \
	-no-undefined -module -avoid-version

mem_collector_monitor_mrnet_mpi_la_LIBADD = \
	@LIBMONITOR_LIBS@ \
	@LIBUNWIND_LIBS@ \
	@MRNET_LWR_LIBS@ \
	-lcbtf-services-common \
	-lcbtf-services-monitor \
	-lcbtf-services-offline \
	-lcbtf-services-unwind \
	-lcbtf-services-data \
	-lcbtf-services-collector-monitor-mrnet-mpi \
	-lpthread -lrt

mem_collector_monitor_mrnet_mpi_la_SOURCES = \
	$(BUILT_SOURCES) \
	collector.c $(WRAPPER_SOURCES)

libmem_monitor_mrnet_mpi_la_CFLAGS = \
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

libmem_monitor_mrnet_mpi_la_LDFLAGS = \
	-static \
	@CBTF_SERVICES_LDFLAGS@ \
	@LIBMONITOR_LDFLAGS@ \
	@LIBUNWIND_LDFLAGS@ \
	@MRNET_LDFLAGS@ \
	-no-undefined

libmem_monitor_mrnet_mpi_la_LIBADD = \
	@LIBMONITOR_LIBS@ \
	@LIBUNWIND_LIBS@ \
	@MRNET_LWR_LIBS@ \
	-lcbtf-services-common \
	-lcbtf-services-monitor \
	-lcbtf-services-offline \
	-lcbtf-services-unwind \
	-lcbtf-services-collector-monitor-mrnet-mpi \
	-lpthread -lrt

libmem_monitor_mrnet_mpi_la_SOURCES = \
	$(BUILT_SOURCES) \
	collector.c $(WRAPPER_SOURCES)

# endif for else for not HAVE_MRNET
endif
# endif for else for not HAVE_LIBMONITOR
endif

CLEANFILES = $(BUILT_SOURCES)

mem.xml: ../template.xml.in

	@rm -f mem.xml
	@sed \
		-e 's,@component_location@,$(plugindir),' \
		-e 's,@cbtflib_location@,$(libdir),' \
		-e 's,@collector_name@,mem,' \
		../template.xml.in > mem.xml

#EXTRA_DIST = mem.xml.in
