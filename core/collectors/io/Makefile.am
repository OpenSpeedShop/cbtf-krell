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

BUILT_FILES = io.xml iop.xml iot.xml

nobase_dist_xml_DATA = \
        io.xml iop.xml iot.xml

pkglib_LTLIBRARIES =
staticpkglib_LTLIBRARIES =

WRAPPER_SOURCES = wrappers.c

if HAVE_LIBMONITOR
pkglib_LTLIBRARIES += io-collector-monitor-fileio.la \
		      iop-collector-monitor-fileio.la \
		      iot-collector-monitor-fileio.la
staticpkglib_LTLIBRARIES += libio-monitor-fileio.la \
			    libiop-monitor-fileio.la \
			    libiot-monitor-fileio.la

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
	-lcbtf-services-unwind \
	-lcbtf-services-timer \
	-lcbtf-services-collector-monitor-fileio \
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
	-lcbtf-services-unwind \
	-lcbtf-services-timer \
	-lcbtf-services-collector-monitor-fileio \
	-lpthread -lrt

libio_monitor_fileio_la_SOURCES = \
	$(OFFLINE_SOURCES) \
	collector.c $(WRAPPER_SOURCES)

iop_collector_monitor_fileio_la_CFLAGS = \
	-D__USE_GNU \
	-DPROFILE \
	-DCBTF_SERVICE_USE_OFFLINE \
	-DCBTF_SERVICE_USE_FILEIO \
	@CBTF_SERVICES_CPPFLAGS@ \
	@LIBMONITOR_CPPFLAGS@ \
	@LIBUNWIND_CPPFLAGS@

iop_collector_monitor_fileio_la_LDFLAGS = \
	@CBTF_SERVICES_LDFLAGS@ \
	@LIBMONITOR_LDFLAGS@ \
	@LIBUNWIND_LDFLAGS@ \
	-no-undefined -module -avoid-version

iop_collector_monitor_fileio_la_LIBADD = \
	@LIBMONITOR_LIBS@ \
	@LIBUNWIND_LIBS@ \
	-lcbtf-services-common \
	-lcbtf-services-monitor \
	-lcbtf-services-offline \
	-lcbtf-services-unwind \
	-lcbtf-services-timer \
	-lcbtf-services-collector-monitor-fileio \
	-lpthread -lrt

iop_collector_monitor_fileio_la_SOURCES = \
	$(OFFLINE_SOURCES) \
	collector.c $(WRAPPER_SOURCES)

libiop_monitor_fileio_la_CFLAGS = \
	-static \
	-D__USE_GNU \
	-DPROFILE \
	-DCBTF_SERVICE_USE_OFFLINE \
	-DCBTF_SERVICE_USE_FILEIO \
	@CBTF_SERVICES_CPPFLAGS@ \
	@LIBMONITOR_CPPFLAGS@ \
	@LIBUNWIND_CPPFLAGS@ \
	@MESSAGES_CPPFLAGS@

libiop_monitor_fileio_la_LDFLAGS = \
	-static \
	@CBTF_SERVICES_LDFLAGS@ \
	@LIBMONITOR_LDFLAGS@ \
	@LIBUNWIND_LDFLAGS@ \
	-no-undefined

libiop_monitor_fileio_la_LIBADD = \
	@LIBMONITOR_LIBS@ \
	@LIBUNWIND_LIBS@ \
	-lcbtf-services-common \
	-lcbtf-services-monitor \
	-lcbtf-services-offline \
	-lcbtf-services-unwind \
	-lcbtf-services-timer \
	-lcbtf-services-collector-monitor-fileio \
	-lpthread -lrt

libiop_monitor_fileio_la_SOURCES = \
	$(OFFLINE_SOURCES) \
	collector.c $(WRAPPER_SOURCES)


iot_collector_monitor_fileio_la_CFLAGS = \
	-D__USE_GNU \
	-DEXTENDEDTRACE \
	-DCBTF_SERVICE_USE_OFFLINE \
	-DCBTF_SERVICE_USE_FILEIO \
	@CBTF_SERVICES_CPPFLAGS@ \
	@LIBMONITOR_CPPFLAGS@ \
	@LIBUNWIND_CPPFLAGS@

iot_collector_monitor_fileio_la_LDFLAGS = \
	@CBTF_SERVICES_LDFLAGS@ \
	@LIBMONITOR_LDFLAGS@ \
	@LIBUNWIND_LDFLAGS@ \
	-no-undefined -module -avoid-version

iot_collector_monitor_fileio_la_LIBADD = \
	@LIBMONITOR_LIBS@ \
	@LIBUNWIND_LIBS@ \
	-lcbtf-services-common \
	-lcbtf-services-monitor \
	-lcbtf-services-offline \
	-lcbtf-services-unwind \
	-lcbtf-services-timer \
	-lcbtf-services-collector-monitor-fileio \
	-lpthread -lrt

iot_collector_monitor_fileio_la_SOURCES = \
	$(OFFLINE_SOURCES) \
	collector.c $(WRAPPER_SOURCES)

libiot_monitor_fileio_la_CFLAGS = \
	-static \
	-D__USE_GNU \
	-DEXTENDEDTRACE \
	-DCBTF_SERVICE_USE_OFFLINE \
	-DCBTF_SERVICE_USE_FILEIO \
	@CBTF_SERVICES_CPPFLAGS@ \
	@LIBMONITOR_CPPFLAGS@ \
	@LIBUNWIND_CPPFLAGS@ \
	@MESSAGES_CPPFLAGS@

libiot_monitor_fileio_la_LDFLAGS = \
	-static \
	@CBTF_SERVICES_LDFLAGS@ \
	@LIBMONITOR_LDFLAGS@ \
	@LIBUNWIND_LDFLAGS@ \
	-no-undefined

libiot_monitor_fileio_la_LIBADD = \
	@LIBMONITOR_LIBS@ \
	@LIBUNWIND_LIBS@ \
	-lcbtf-services-common \
	-lcbtf-services-monitor \
	-lcbtf-services-offline \
	-lcbtf-services-unwind \
	-lcbtf-services-timer \
	-lcbtf-services-collector-monitor-fileio \
	-lpthread -lrt

libiot_monitor_fileio_la_SOURCES = \
	$(OFFLINE_SOURCES) \
	collector.c $(WRAPPER_SOURCES)

if HAVE_MRNET
pkglib_LTLIBRARIES += io-collector-monitor-mrnet.la \
		      iop-collector-monitor-mrnet.la \
		      iot-collector-monitor-mrnet.la
staticpkglib_LTLIBRARIES += libio-monitor-mrnet.la \
			    libiop-monitor-mrnet.la \
			    libiot-monitor-mrnet.la

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
	@MRNET_LWR_LIBS@ \
	-lcbtf-services-common \
	-lcbtf-services-monitor \
	-lcbtf-services-offline \
	-lcbtf-services-unwind \
	-lcbtf-services-timer \
	-lcbtf-services-collector-monitor-mrnet \
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
	@MRNET_LWR_LIBS@ \
	-lcbtf-services-common \
	-lcbtf-services-monitor \
	-lcbtf-services-offline \
	-lcbtf-services-unwind \
	-lcbtf-services-timer \
	-lcbtf-services-collector-monitor-mrnet \
	-lpthread -lrt

libio_monitor_mrnet_la_SOURCES = \
	$(OFFLINE_SOURCES) \
	collector.c $(WRAPPER_SOURCES)

iop_collector_monitor_mrnet_la_CFLAGS = \
	-D__USE_GNU \
	-DPROFILE \
	-DCBTF_SERVICE_USE_OFFLINE \
	-DCBTF_SERVICE_USE_MRNET \
	@CBTF_SERVICES_CPPFLAGS@ \
	@LIBMONITOR_CPPFLAGS@ \
	@LIBUNWIND_CPPFLAGS@ \
	@MESSAGES_CPPFLAGS@ \
	@MRNET_CPPFLAGS@

iop_collector_monitor_mrnet_la_LDFLAGS = \
	@CBTF_SERVICES_LDFLAGS@ \
	@LIBMONITOR_LDFLAGS@ \
	@LIBUNWIND_LDFLAGS@ \
	@MRNET_LDFLAGS@ \
	-no-undefined -module -avoid-version

iop_collector_monitor_mrnet_la_LIBADD = \
	@LIBMONITOR_LIBS@ \
	@LIBUNWIND_LIBS@ \
	@MRNET_LWR_LIBS@ \
	-lcbtf-services-common \
	-lcbtf-services-monitor \
	-lcbtf-services-offline \
	-lcbtf-services-unwind \
	-lcbtf-services-timer \
	-lcbtf-services-collector-monitor-mrnet \
	-lpthread -lrt

iop_collector_monitor_mrnet_la_SOURCES = \
	$(OFFLINE_SOURCES) \
	collector.c $(WRAPPER_SOURCES)

libiop_monitor_mrnet_la_CFLAGS = \
	-static \
	-D__USE_GNU \
	-DPROFILE \
	-DCBTF_SERVICE_USE_OFFLINE \
	-DCBTF_SERVICE_USE_MRNET \
	@CBTF_SERVICES_CPPFLAGS@ \
	@LIBMONITOR_CPPFLAGS@ \
	@LIBUNWIND_CPPFLAGS@ \
	@MESSAGES_CPPFLAGS@ \
	@MRNET_CPPFLAGS@

libiop_monitor_mrnet_la_LDFLAGS = \
	-static \
	@CBTF_SERVICES_LDFLAGS@ \
	@LIBMONITOR_LDFLAGS@ \
	@LIBUNWIND_LDFLAGS@ \
	@MRNET_LDFLAGS@ \
	-no-undefined

libiop_monitor_mrnet_la_LIBADD = \
	@LIBMONITOR_LIBS@ \
	@LIBUNWIND_LIBS@ \
	@MRNET_LWR_LIBS@ \
	-lcbtf-services-common \
	-lcbtf-services-monitor \
	-lcbtf-services-offline \
	-lcbtf-services-unwind \
	-lcbtf-services-timer \
	-lcbtf-services-collector-monitor-mrnet \
	-lpthread -lrt

libiop_monitor_mrnet_la_SOURCES = \
	$(OFFLINE_SOURCES) \
	collector.c $(WRAPPER_SOURCES)


iot_collector_monitor_mrnet_la_CFLAGS = \
	-D__USE_GNU \
	-DEXTENDEDTRACE \
	-DCBTF_SERVICE_USE_OFFLINE \
	-DCBTF_SERVICE_USE_MRNET \
	@CBTF_SERVICES_CPPFLAGS@ \
	@LIBMONITOR_CPPFLAGS@ \
	@LIBUNWIND_CPPFLAGS@ \
	@MESSAGES_CPPFLAGS@ \
	@MRNET_CPPFLAGS@

iot_collector_monitor_mrnet_la_LDFLAGS = \
	@CBTF_SERVICES_LDFLAGS@ \
	@LIBMONITOR_LDFLAGS@ \
	@LIBUNWIND_LDFLAGS@ \
	@MRNET_LDFLAGS@ \
	-no-undefined -module -avoid-version

iot_collector_monitor_mrnet_la_LIBADD = \
	@LIBMONITOR_LIBS@ \
	@LIBUNWIND_LIBS@ \
	@MRNET_LWR_LIBS@ \
	-lcbtf-services-common \
	-lcbtf-services-monitor \
	-lcbtf-services-offline \
	-lcbtf-services-unwind \
	-lcbtf-services-timer \
	-lcbtf-services-collector-monitor-mrnet \
	-lpthread -lrt

iot_collector_monitor_mrnet_la_SOURCES = \
	$(OFFLINE_SOURCES) \
	collector.c $(WRAPPER_SOURCES)

libiot_monitor_mrnet_la_CFLAGS = \
	-static \
	-D__USE_GNU \
	-DEXTENDEDTRACE \
	-DCBTF_SERVICE_USE_OFFLINE \
	-DCBTF_SERVICE_USE_MRNET \
	@CBTF_SERVICES_CPPFLAGS@ \
	@LIBMONITOR_CPPFLAGS@ \
	@LIBUNWIND_CPPFLAGS@ \
	@MESSAGES_CPPFLAGS@ \
	@MRNET_CPPFLAGS@

libiot_monitor_mrnet_la_LDFLAGS = \
	-static \
	@CBTF_SERVICES_LDFLAGS@ \
	@LIBMONITOR_LDFLAGS@ \
	@LIBUNWIND_LDFLAGS@ \
	@MRNET_LDFLAGS@ \
	-no-undefined

libiot_monitor_mrnet_la_LIBADD = \
	@LIBMONITOR_LIBS@ \
	@LIBUNWIND_LIBS@ \
	@MRNET_LWR_LIBS@ \
	-lcbtf-services-common \
	-lcbtf-services-monitor \
	-lcbtf-services-offline \
	-lcbtf-services-unwind \
	-lcbtf-services-timer \
	-lcbtf-services-collector-monitor-mrnet \
	-lpthread -lrt

libiot_monitor_mrnet_la_SOURCES = \
	$(OFFLINE_SOURCES) \
	collector.c $(WRAPPER_SOURCES)


pkglib_LTLIBRARIES += io-collector-monitor-mrnet-mpi.la \
		      iop-collector-monitor-mrnet-mpi.la \
		      iot-collector-monitor-mrnet-mpi.la
staticpkglib_LTLIBRARIES += libio-monitor-mrnet-mpi.la \
			    libiop-monitor-mrnet-mpi.la \
			    libiot-monitor-mrnet-mpi.la

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
	@MRNET_LWR_LIBS@ \
	-lcbtf-services-common \
	-lcbtf-services-monitor \
	-lcbtf-services-offline \
	-lcbtf-services-unwind \
	-lcbtf-services-timer \
	-lcbtf-services-data \
	-lcbtf-services-collector-monitor-mrnet-mpi \
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
	@MRNET_LWR_LIBS@ \
	-lcbtf-services-common \
	-lcbtf-services-monitor \
	-lcbtf-services-offline \
	-lcbtf-services-unwind \
	-lcbtf-services-timer \
	-lcbtf-services-collector-monitor-mrnet-mpi \
	-lpthread -lrt

libio_monitor_mrnet_mpi_la_SOURCES = \
	$(OFFLINE_SOURCES) \
	collector.c $(WRAPPER_SOURCES)

iop_collector_monitor_mrnet_mpi_la_CFLAGS = \
	-D__USE_GNU \
	-DPROFILE \
	-DCBTF_SERVICE_USE_OFFLINE \
	-DCBTF_SERVICE_USE_MRNET \
	-DCBTF_SERVICE_USE_MRNET_MPI \
	@CBTF_SERVICES_CPPFLAGS@ \
	@LIBMONITOR_CPPFLAGS@ \
	@LIBUNWIND_CPPFLAGS@ \
	@MESSAGES_CPPFLAGS@ \
	@MRNET_CPPFLAGS@

iop_collector_monitor_mrnet_mpi_la_LDFLAGS = \
	@CBTF_SERVICES_LDFLAGS@ \
	@LIBMONITOR_LDFLAGS@ \
	@LIBUNWIND_LDFLAGS@ \
	@MRNET_LDFLAGS@ \
	-no-undefined -module -avoid-version

iop_collector_monitor_mrnet_mpi_la_LIBADD = \
	@LIBMONITOR_LIBS@ \
	@LIBUNWIND_LIBS@ \
	@MRNET_LWR_LIBS@ \
	-lcbtf-services-common \
	-lcbtf-services-monitor \
	-lcbtf-services-offline \
	-lcbtf-services-unwind \
	-lcbtf-services-timer \
	-lcbtf-services-data \
	-lcbtf-services-collector-monitor-mrnet-mpi \
	-lpthread -lrt

iop_collector_monitor_mrnet_mpi_la_SOURCES = \
	$(OFFLINE_SOURCES) \
	collector.c $(WRAPPER_SOURCES)

libiop_monitor_mrnet_mpi_la_CFLAGS = \
	-static \
	-D__USE_GNU \
	-DPROFILE \
	-DCBTF_SERVICE_USE_OFFLINE \
	-DCBTF_SERVICE_USE_MRNET \
	-DCBTF_SERVICE_USE_MRNET_MPI \
	@CBTF_SERVICES_CPPFLAGS@ \
	@LIBMONITOR_CPPFLAGS@ \
	@LIBUNWIND_CPPFLAGS@ \
	@MESSAGES_CPPFLAGS@ \
	@MRNET_CPPFLAGS@

libiop_monitor_mrnet_mpi_la_LDFLAGS = \
	-static \
	@CBTF_SERVICES_LDFLAGS@ \
	@LIBMONITOR_LDFLAGS@ \
	@LIBUNWIND_LDFLAGS@ \
	@MRNET_LDFLAGS@ \
	-no-undefined

libiop_monitor_mrnet_mpi_la_LIBADD = \
	@LIBMONITOR_LIBS@ \
	@LIBUNWIND_LIBS@ \
	@MRNET_LWR_LIBS@ \
	-lcbtf-services-common \
	-lcbtf-services-monitor \
	-lcbtf-services-offline \
	-lcbtf-services-unwind \
	-lcbtf-services-timer \
	-lcbtf-services-collector-monitor-mrnet-mpi \
	-lpthread -lrt

libiop_monitor_mrnet_mpi_la_SOURCES = \
	$(OFFLINE_SOURCES) \
	collector.c $(WRAPPER_SOURCES)

iot_collector_monitor_mrnet_mpi_la_CFLAGS = \
	-D__USE_GNU \
	-DEXTENDEDTRACE \
	-DCBTF_SERVICE_USE_OFFLINE \
	-DCBTF_SERVICE_USE_MRNET \
	-DCBTF_SERVICE_USE_MRNET_MPI \
	@CBTF_SERVICES_CPPFLAGS@ \
	@LIBMONITOR_CPPFLAGS@ \
	@LIBUNWIND_CPPFLAGS@ \
	@MESSAGES_CPPFLAGS@ \
	@MRNET_CPPFLAGS@

iot_collector_monitor_mrnet_mpi_la_LDFLAGS = \
	@CBTF_SERVICES_LDFLAGS@ \
	@LIBMONITOR_LDFLAGS@ \
	@LIBUNWIND_LDFLAGS@ \
	@MRNET_LDFLAGS@ \
	-no-undefined -module -avoid-version

iot_collector_monitor_mrnet_mpi_la_LIBADD = \
	@LIBMONITOR_LIBS@ \
	@LIBUNWIND_LIBS@ \
	@MRNET_LWR_LIBS@ \
	-lcbtf-services-common \
	-lcbtf-services-monitor \
	-lcbtf-services-offline \
	-lcbtf-services-unwind \
	-lcbtf-services-timer \
	-lcbtf-services-data \
	-lcbtf-services-collector-monitor-mrnet-mpi \
	-lpthread -lrt

iot_collector_monitor_mrnet_mpi_la_SOURCES = \
	$(OFFLINE_SOURCES) \
	collector.c $(WRAPPER_SOURCES)

libiot_monitor_mrnet_mpi_la_CFLAGS = \
	-static \
	-D__USE_GNU \
	-DEXTENDEDTRACE \
	-DCBTF_SERVICE_USE_OFFLINE \
	-DCBTF_SERVICE_USE_MRNET \
	-DCBTF_SERVICE_USE_MRNET_MPI \
	@CBTF_SERVICES_CPPFLAGS@ \
	@LIBMONITOR_CPPFLAGS@ \
	@LIBUNWIND_CPPFLAGS@ \
	@MESSAGES_CPPFLAGS@ \
	@MRNET_CPPFLAGS@

libiot_monitor_mrnet_mpi_la_LDFLAGS = \
	-static \
	@CBTF_SERVICES_LDFLAGS@ \
	@LIBMONITOR_LDFLAGS@ \
	@LIBUNWIND_LDFLAGS@ \
	@MRNET_LDFLAGS@ \
	-no-undefined

libiot_monitor_mrnet_mpi_la_LIBADD = \
	@LIBMONITOR_LIBS@ \
	@LIBUNWIND_LIBS@ \
	@MRNET_LWR_LIBS@ \
	-lcbtf-services-common \
	-lcbtf-services-monitor \
	-lcbtf-services-offline \
	-lcbtf-services-unwind \
	-lcbtf-services-timer \
	-lcbtf-services-collector-monitor-mrnet-mpi \
	-lpthread -lrt

libiot_monitor_mrnet_mpi_la_SOURCES = \
	$(OFFLINE_SOURCES) \
	collector.c $(WRAPPER_SOURCES)

# endif for HAVE_MRNET
endif
# endif for HAVE_LIBMONITOR
endif

CLEANFILES = $(BUILT_FILES)

io.xml: ../template.xml.in

	@rm -f io.xml
	@sed \
		-e 's,@component_location@,$(plugindir),' \
		-e 's,@cbtflib_location@,$(libdir),' \
		-e 's,@collector_name@,io,' \
		../template.xml.in > io.xml

iop.xml: ../template.xml.in

	@rm -f iop.xml
	@sed \
		-e 's,@component_location@,$(plugindir),' \
		-e 's,@cbtflib_location@,$(libdir),' \
		-e 's,@collector_name@,iop,' \
		../template.xml.in > iop.xml

iot.xml: iot.xml.in

	@rm -f iot.xml
	@sed \
		-e 's,@component_location@,$(plugindir),' \
		-e 's,@cbtflib_location@,$(libdir),' \
		-e 's,@collector_name@,iot,' \
		../template.xml.in > iot.xml

#EXTRA_DIST = io.xml.in iot.xml.in
