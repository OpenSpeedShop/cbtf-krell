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

dist_bin_SCRIPTS = cbtfrun
BUILT_FILES = cbtfrun

CFLAGS = -g -O0

pkglibdir = $(libdir)/KrellInstitute/Collectors

pkglib_LTLIBRARIES =


if HAVE_LIBMONITOR
OFFLINE_SOURCES = \
	monitor.c

endif

if HAVE_LIBMONITOR
pkglib_LTLIBRARIES += pcsamp-collector-monitor-fileio.la libpcsamp-monitor-fileio.la
pcsamp_collector_monitor_fileio_la_CFLAGS = \
	@LIBMONITOR_CPPFLAGS@ \
	@MESSAGES_CPPFLAGS@ \
	-DCBTF_SERVICE_USE_OFFLINE \
	-DCBTF_SERVICE_USE_FILEIO \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/src/offline \
	-I$(top_srcdir)/src/data

pcsamp_collector_monitor_fileio_la_LDFLAGS = \
	@LIBMONITOR_LDFLAGS@ \
	-L$(top_srcdir)/src/common \
	-L$(top_srcdir)/src/monitor \
	-L$(top_srcdir)/src/offline \
	-L$(top_srcdir)/src/timer \
	-L$(top_srcdir)/src/data \
	-no-undefined -module -avoid-version

pcsamp_collector_monitor_fileio_la_LIBADD = \
	@LIBMONITOR_LIBS@ \
	-lcbtf-services-common \
	-lcbtf-services-monitor \
	-lcbtf-services-offline \
	-lcbtf-services-timer \
	-lpthread -lrt

pcsamp_collector_monitor_fileio_la_SOURCES = \
	$(OFFLINE_SOURCES) \
	collector.c

libpcsamp_monitor_fileio_la_CFLAGS = \
	-static \
	@LIBMONITOR_CPPFLAGS@ \
	@MESSAGES_CPPFLAGS@ \
	-DCBTF_SERVICE_USE_OFFLINE \
	-DCBTF_SERVICE_USE_FILEIO \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/src/offline \
	-I$(top_srcdir)/src/data

libpcsamp_monitor_fileio_la_LDFLAGS = \
	-static \
	@LIBMONITOR_LDFLAGS@ \
	-L$(top_srcdir)/src/common \
	-L$(top_srcdir)/src/monitor \
	-L$(top_srcdir)/src/offline \
	-L$(top_srcdir)/src/timer \
	-L$(top_srcdir)/src/data \
	-no-undefined

libpcsamp_monitor_fileio_la_LIBADD = \
	@LIBMONITOR_LIBS@ \
	-lcbtf-services-common \
	-lcbtf-services-monitor \
	-lcbtf-services-offline \
	-lcbtf-services-timer \
	-lpthread -lrt

libpcsamp_monitor_fileio_la_SOURCES = \
	$(OFFLINE_SOURCES) \
	collector.c

if HAVE_MRNET
pkglib_LTLIBRARIES += pcsamp-collector-monitor-mrnet.la libpcsamp-monitor-mrnet.la
pcsamp_collector_monitor_mrnet_la_CFLAGS = \
	@LIBMONITOR_CPPFLAGS@ \
	@MESSAGES_CPPFLAGS@ \
	@MRNET_CPPFLAGS@ \
	-DCBTF_SERVICE_USE_OFFLINE \
	-DCBTF_SERVICE_USE_MRNET \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/src/offline \
	-I$(top_srcdir)/src/data

pcsamp_collector_monitor_mrnet_la_LDFLAGS = \
	@LIBMONITOR_LDFLAGS@ \
	@MRNET_LDFLAGS@ \
	-L$(top_srcdir)/src/common \
	-L$(top_srcdir)/src/monitor \
	-L$(top_srcdir)/src/offline \
	-L$(top_srcdir)/src/timer \
	-L$(top_srcdir)/src/data \
	-no-undefined -module -avoid-version

pcsamp_collector_monitor_mrnet_la_LIBADD = \
	@LIBMONITOR_LIBS@ \
	@MRNET_LW_LIBS@ \
	-lcbtf-services-common \
	-lcbtf-services-monitor \
	-lcbtf-services-offline \
	-lcbtf-services-timer \
	-lpthread -lrt

pcsamp_collector_monitor_mrnet_la_SOURCES = \
	$(OFFLINE_SOURCES) \
	collector.c

libpcsamp_monitor_mrnet_la_CFLAGS = \
	-static \
	@LIBMONITOR_CPPFLAGS@ \
	@MESSAGES_CPPFLAGS@ \
	@MRNET_CPPFLAGS@ \
	-DCBTF_SERVICE_USE_OFFLINE \
	-DCBTF_SERVICE_USE_MRNET \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/src/offline \
	-I$(top_srcdir)/src/data

libpcsamp_monitor_mrnet_la_LDFLAGS = \
	-static \
	@LIBMONITOR_LDFLAGS@ \
	@MRNET_LDFLAGS@ \
	-L$(top_srcdir)/src/common \
	-L$(top_srcdir)/src/monitor \
	-L$(top_srcdir)/src/offline \
	-L$(top_srcdir)/src/timer \
	-L$(top_srcdir)/src/data \
	-no-undefined

libpcsamp_monitor_mrnet_la_LIBADD = \
	@LIBMONITOR_LIBS@ \
	@MRNET_LW_LIBS@ \
	-lcbtf-services-common \
	-lcbtf-services-monitor \
	-lcbtf-services-offline \
	-lcbtf-services-timer \
	-lpthread -lrt

libpcsamp_monitor_mrnet_la_SOURCES = \
	$(OFFLINE_SOURCES) \
	collector.c


pkglib_LTLIBRARIES += pcsamp-collector-monitor-mrnet-mpi.la libpcsamp-monitor-mrnet-mpi.la
pcsamp_collector_monitor_mrnet_mpi_la_CFLAGS = \
	@LIBMONITOR_CPPFLAGS@ \
	@MESSAGES_CPPFLAGS@ \
	@MRNET_CPPFLAGS@ \
	-DCBTF_SERVICE_USE_OFFLINE \
	-DCBTF_SERVICE_USE_MRNET \
	-DCBTF_SERVICE_USE_MRNET_MPI \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/src/offline \
	-I$(top_srcdir)/src/data

pcsamp_collector_monitor_mrnet_mpi_la_LDFLAGS = \
	@LIBMONITOR_LDFLAGS@ \
	@MRNET_LDFLAGS@ \
	-L$(top_srcdir)/src/common \
	-L$(top_srcdir)/src/monitor \
	-L$(top_srcdir)/src/offline \
	-L$(top_srcdir)/src/timer \
	-L$(top_srcdir)/src/data \
	-no-undefined -module -avoid-version

pcsamp_collector_monitor_mrnet_mpi_la_LIBADD = \
	@LIBMONITOR_LIBS@ \
	@MRNET_LW_LIBS@ \
	-lcbtf-services-common \
	-lcbtf-services-monitor \
	-lcbtf-services-offline \
	-lcbtf-services-timer \
	-lcbtf-services-data \
	-lpthread -lrt

pcsamp_collector_monitor_mrnet_mpi_la_SOURCES = \
	$(OFFLINE_SOURCES) \
	collector.c

libpcsamp_monitor_mrnet_mpi_la_CFLAGS = \
	-static \
	@LIBMONITOR_CPPFLAGS@ \
	@MESSAGES_CPPFLAGS@ \
	@MRNET_CPPFLAGS@ \
	-DCBTF_SERVICE_USE_OFFLINE \
	-DCBTF_SERVICE_USE_MRNET \
	-DCBTF_SERVICE_USE_MRNET_MPI \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/src/offline \
	-I$(top_srcdir)/src/data

libpcsamp_monitor_mrnet_mpi_la_LDFLAGS = \
	-static \
	@LIBMONITOR_LDFLAGS@ \
	@MRNET_LDFLAGS@ \
	-L$(top_srcdir)/src/common \
	-L$(top_srcdir)/src/monitor \
	-L$(top_srcdir)/src/offline \
	-L$(top_srcdir)/src/timer \
	-L$(top_srcdir)/src/data \
	-no-undefined

libpcsamp_monitor_mrnet_mpi_la_LIBADD = \
	@LIBMONITOR_LIBS@ \
	@MRNET_LW_LIBS@ \
	-lcbtf-services-common \
	-lcbtf-services-monitor \
	-lcbtf-services-offline \
	-lcbtf-services-timer \
	-lpthread -lrt

libpcsamp_monitor_mrnet_mpi_la_SOURCES = \
	$(OFFLINE_SOURCES) \
	collector.c

endif
endif

CLEANFILES = $(BUILT_FILES)


monitor_dir=$(LIBMONITOR_DIR)
unwind_dir=$(LIBUNWIND_DIR)
papi_dir=$(PAPI_DIR)
mrnet_dir=$(MRNET_DIR)

cbtfrun: cbtfrun.in

	@rm -f cbtfrun
	@sed \
		-e 's,@@machine_dependent_libdir@@,$(abi_libdir_name),' \
		-e 's,@@alt_machine_dependent_libdir@@,$(alt_abi_libdir_name),' \
		-e 's,@@cbtfinstalldir@@,$(prefix),' \
		-e 's,@@libmonitordir@@,$(monitor_dir),' \
		-e 's,@@libunwinddir@@,$(unwind_dir),' \
		-e 's,@@libmrnetdir@@,$(mrnet_dir),' \
		-e 's,@@libpapidir@@,$(papi_dir),' \
		-e 's,@@lamdir@@,$(LAM_DIR),' \
		-e 's,@@lampidir@@,$(LAMPI_DIR),' \
		-e 's,@@mpichdir@@,$(MPICH_DIR),' \
		-e 's,@@mpich2dir@@,$(mpich2_dir),' \
		-e 's,@@mvapichdir@@,$(MVAPICH_DIR),' \
		-e 's,@@mvapich2dir@@,$(MVAPICH2_DIR),' \
		-e 's,@@mptdir@@,$(MPT_DIR),' \
		-e 's,@@openmpidir@@,$(OPENMPI_DIR),' \
		-e 's,@@cbtfdefaultmpi@@,$(DEFAULT_MPI_IMPL),' \
			$(srcdir)/cbtfrun.in > cbtfrun
	@chmod 755 cbtfrun
EXTRA_DIST = cbtfrun.in
