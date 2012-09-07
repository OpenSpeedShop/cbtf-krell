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

BUILT_FILES = mpi.xml
nobase_dist_xml_DATA = \
        mpi.xml

WRAPPER_SOURCES = wrappers.c wrappers-fortran.c
COLLECTOR_SOURCES = collector.c

pkglib_LTLIBRARIES =
staticpkglib_LTLIBRARIES =


if HAVE_MPICH
if HAVE_LIBMONITOR
pkglib_LTLIBRARIES += mpi-mpich-collector-monitor-fileio.la
staticpkglib_LTLIBRARIES += libmpi-mpich-monitor-fileio.la
if HAVE_MRNET
pkglib_LTLIBRARIES += mpi-mpich-collector-monitor-mrnet-mpi.la
staticpkglib_LTLIBRARIES += libmpi-mpich-monitor-mrnet-mpi.la
endif
endif
endif

if HAVE_MPICH2
if HAVE_LIBMONITOR
pkglib_LTLIBRARIES += mpi-mpich2-collector-monitor-fileio.la
staticpkglib_LTLIBRARIES += libmpi-mpich2-monitor-fileio.la
if HAVE_MRNET
pkglib_LTLIBRARIES += mpi-mpich2-collector-monitor-mrnet-mpi.la
staticpkglib_LTLIBRARIES += libmpi-mpich2-monitor-mrnet-mpi.la
endif
endif
endif

#if HAVE_TARGET_MPICH2
#if HAVE_TARGET_LIBMONITOR
#if HAVE_TARGET_SHARED
#endif
#endif
#endif

if HAVE_MVAPICH
if HAVE_LIBMONITOR
pkglib_LTLIBRARIES += mpi-mvapich-collector-monitor-fileio.la
staticpkglib_LTLIBRARIES += libmpi-mvapich-monitor-fileio.la
if HAVE_MRNET
pkglib_LTLIBRARIES += mpi-mvapich-collector-monitor-mrnet-mpi.la
staticpkglib_LTLIBRARIES += libmpi-mvapich-monitor-mrnet-mpi.la
endif
endif
endif

if HAVE_MVAPICH2
if HAVE_LIBMONITOR
pkglib_LTLIBRARIES += mpi-mvapich2-collector-monitor-fileio.la
staticpkglib_LTLIBRARIES += libmpi-mvapich2-monitor-fileio.la
if HAVE_MRNET
pkglib_LTLIBRARIES += mpi-mvapich2-collector-monitor-mrnet-mpi.la
staticpkglib_LTLIBRARIES += libmpi-mvapich2-monitor-mrnet-mpi.la
endif
endif
endif

if HAVE_OPENMPI
if HAVE_LIBMONITOR
pkglib_LTLIBRARIES += mpi-openmpi-collector-monitor-fileio.la
staticpkglib_LTLIBRARIES += libmpi-openmpi-monitor-fileio.la
if HAVE_MRNET
pkglib_LTLIBRARIES += mpi-openmpi-collector-monitor-mrnet-mpi.la
staticpkglib_LTLIBRARIES += libmpi-openmpi-monitor-mrnet-mpi.la
endif
endif
endif

if HAVE_MPT
if HAVE_LIBMONITOR
pkglib_LTLIBRARIES += mpi-mpt-collector-monitor-fileio.la
staticpkglib_LTLIBRARIES += libmpi-mpt-monitor-fileio.la
if HAVE_MRNET
pkglib_LTLIBRARIES += mpi-mpt-collector-monitor-mrnet-mpi.la
staticpkglib_LTLIBRARIES += libmpi-mpt-monitor-mrnet-mpi.la
endif
endif
endif



# Common definitions for all MPI implementations
#
# We have to build a separate version of the plugin's runtime library
# for each MPI implementation on the system.  Various compilation
# flags for the different implementations are provided by the
# configuration process, in the autoconf variables *_CPPFLAGS,
# *_LDFLAGS, and *_LIBS, where * comes from the following set:
#
#     LAMPI, MPICH, MPT, OPENMPI
#
# The *_generate definitions below are GNU make functions which are
# used to produce values for the automake library variables for each
# plugin runtime library, given the implementation-specific flags as
# arguments.  However, these functions are not invoked until make
# time, so their final results are not seen by automake.  Because of
# this, the "-module" flag, despite being common to all the libraries,
# is moved out into each invocation of rt_LDFLAGS_generate, so that
# automake will see it and not issue warnings about the libraries'
# names not beginning with "lib" (GNU automake, 7.3.6).  Also, the
# definitions below for each specific implementation give empty
# *_DEPENDENCIES definitions, since automake will not see the final
# values of *_LIBADD and automatically set *_DEPENDENCIES correctly
# (GNU automake, 7.4, entry for _DEPENDENCIES).
#
# The generation of the wrapper sources is based on reading mpi.h in
# its entirety, and therefore it is possible for the results to be
# different if mpi.h differs among the implementations.  In practice,
# the only wrapper source differences are in the parameter lists for
# the MPI_Test{all,any,some} functions: parameter types of
# MPI_Request* v.  MPI_Request[], for example.  These are equivalent
# as parameter types, and the compilers do not currently complain, so
# the same wrapper sources can be used for all implementations.


if HAVE_LIBMONITOR
monitor_fileio_CFLAGS_generate = \
	-DCBTF_SERVICE_USE_OFFLINE \
	-DCBTF_SERVICE_USE_FILEIO \
	@CBTF_SERVICES_CPPFLAGS@ \
	@LIBMONITOR_CPPFLAGS@ \
	@LIBUNWIND_CPPFLAGS@ \
	@MESSAGES_CPPFLAGS@ \
	$(1)

monitor_fileio_LDFLAGS_generate = \
	@CBTF_SERVICES_LDFLAGS@ \
	@LIBMONITOR_LDFLAGS@ \
	@LIBUNWIND_LDFLAGS@ \
	$(1) \
	-no-undefined -avoid-version

monitor_fileio_LIBADD_generate = \
	@LIBMONITOR_LIBS@ \
	@LIBUNWIND_LIBS@ \
	-lcbtf-services-common \
	-lcbtf-services-monitor \
	-lcbtf-services-offline \
	-lcbtf-services-unwind \
	-lcbtf-services-timer \
	-lcbtf-services-collector-monitor-fileio \
	$(1) \
	-lrt -lpthread

monitor_fileio_sources = \
	$(WRAPPER_SOURCES) \
	$(COLLECTOR_SOURCES)

lib_monitor_fileio_CFLAGS_generate = \
	-static \
	-DCBTF_SERVICE_USE_OFFLINE \
	-DCBTF_SERVICE_USE_FILEIO \
	-DCBTF_STATIC \
	@CBTF_SERVICES_CPPFLAGS@ \
	@LIBMONITOR_CPPFLAGS@ \
	@LIBUNWIND_CPPFLAGS@ \
	@MESSAGES_CPPFLAGS@ \
	$(1)

lib_monitor_fileio_LDFLAGS_generate = \
	-static \
	@CBTF_SERVICES_LDFLAGS@ \
	@LIBMONITOR_LDFLAGS@ \
	@LIBUNWIND_LDFLAGS@ \
	$(1) \
	-no-undefined

lib_monitor_fileio_LIBADD_generate = \
	@LIBMONITOR_LIBS@ \
	@LIBUNWIND_LIBS@ \
	-lcbtf-services-common \
	-lcbtf-services-monitor \
	-lcbtf-services-offline \
	-lcbtf-services-unwind \
	-lcbtf-services-timer \
	-lcbtf-services-collector-monitor-fileio \
	$(1) \
	-lrt -lpthread

lib_monitor_fileio_sources = \
	$(WRAPPER_SOURCES) \
	$(COLLECTOR_SOURCES)

if HAVE_MRNET
monitor_mrnet_mpi_CFLAGS_generate = \
	-DCBTF_SERVICE_USE_OFFLINE \
	-DCBTF_SERVICE_USE_MRNET \
	-DCBTF_SERVICE_USE_MRNET_MPI \
	@CBTF_SERVICES_CPPFLAGS@ \
	@LIBMONITOR_CPPFLAGS@ \
	@LIBUNWIND_CPPFLAGS@ \
	@MRNET_CPPFLAGS@ \
	@MESSAGES_CPPFLAGS@ \
	$(1)

monitor_mrnet_mpi_LDFLAGS_generate = \
	@CBTF_SERVICES_LDFLAGS@ \
	@LIBMONITOR_LDFLAGS@ \
	@LIBUNWIND_LDFLAGS@ \
	@MRNET_LDFLAGS@ \
	$(1) \
	-no-undefined  -avoid-version

monitor_mrnet_mpi_LIBADD_generate = \
	@LIBMONITOR_LIBS@ \
	@LIBUNWIND_LIBS@ \
	@MRNET_LWR_LIBS@ \
	-lcbtf-services-common \
	-lcbtf-services-monitor \
	-lcbtf-services-offline \
	-lcbtf-services-unwind \
	-lcbtf-services-timer \
	-lcbtf-services-collector-monitor-mrnet-mpi \
	$(1) \
	-lrt -lpthread

monitor_mrnet_mpi_sources = \
	$(WRAPPER_SOURCES) \
	$(COLLECTOR_SOURCES)


lib_monitor_mrnet_mpi_CFLAGS_generate = \
	-static \
	-DCBTF_SERVICE_USE_OFFLINE \
	-DCBTF_SERVICE_USE_MRNET \
	-DCBTF_SERVICE_USE_MRNET_MPI \
	-DCBTF_STATIC \
	@CBTF_SERVICES_CPPFLAGS@ \
	@LIBMONITOR_CPPFLAGS@ \
	@LIBUNWIND_CPPFLAGS@ \
	@MRNET_CPPFLAGS@ \
	@MESSAGES_CPPFLAGS@ \
	$(1)

lib_monitor_mrnet_mpi_LDFLAGS_generate = \
	-static \
	@CBTF_SERVICES_LDFLAGS@ \
	@LIBMONITOR_LDFLAGS@ \
	@LIBUNWIND_LDFLAGS@ \
	@MRNET_LDFLAGS@ \
	$(1) \
	-no-undefined

lib_monitor_mrnet_mpi_LIBADD_generate = \
	@LIBMONITOR_LIBS@ \
	@LIBUNWIND_LIBS@ \
	@MRNET_LWR_LIBS@ \
	-lcbtf-services-common \
	-lcbtf-services-monitor \
	-lcbtf-services-offline \
	-lcbtf-services-unwind \
	-lcbtf-services-timer \
	-lcbtf-services-collector-monitor-mrnet-mpi \
	$(1) \
	-lrt -lpthread

lib_monitor_mrnet_mpi_sources = \
	$(WRAPPER_SOURCES) \
	$(COLLECTOR_SOURCES)

endif
endif

if HAVE_TARGET_LIBMONITOR
if HAVE_TARGET_SHARED
monitor_fileio_CFLAGS_generate = \
	-DCBTF_SERVICE_USE_OFFLINE \
	-DCBTF_SERVICE_USE_FILEIO \
	@TARGET_CBTF_SERVICES_CPPFLAGS@ \
	@TARGET_LIBUNWIND_CPPFLAGS@ \
	@TARGET_LIBMONITOR_CPPFLAGS@ \
	$(1)

monitor_fileio_LDFLAGS_generate = \
	@TARGET_CBTF_SERVICES_LDFLAGS@ \
	@TARGET_LIBMONITOR_LDFLAGS@ \
	@TARGET_LIBUNWIND_LDFLAGS@ \
	$(1) \
	-no-undefined -avoid-version

monitor_fileio_LIBADD_generate = \
	@TARGET_LIBMONITOR_LIBS@ \
	@TARGET_LIBUNWIND_LIBS@ \
	-lcbtf-services-common \
	-lcbtf-services-monitor \
	-lcbtf-services-offline \
	-lcbtf-services-unwind \
	-lcbtf-services-timer \
	$(1) \
	-lrt -lpthread

monitor_fileio_sources = \
	$(WRAPPER_SOURCES) \
	$(COLLECTOR_SOURCES)

endif

lib_monitor_fileio_CFLAGS_generate = \
	-static \
	-DCBTF_SERVICE_USE_OFFLINE \
	-DCBTF_SERVICE_USE_FILEIO \
	-DCBTF_STATIC \
	@TARGET_CBTF_SERVICES_CPPFLAGS@ \
	@TARGET_LIBMONITOR_CPPFLAGS@ \
	@TARGET_LIBUNWIND_CPPFLAGS@ \
	@TARGET_MESSAGES_CPPFLAGS@ \
	$(1)

lib_monitor_fileio_LDFLAGS_generate = \
	-static \
	@TARGET_CBTF_SERVICES_LDFLAGS@ \
	@TARGET_LIBMONITOR_LDFLAGS@ \
	@TARGET_LIBUNWIND_LDFLAGS@ \
	$(1) \
	-no-undefined

lib_monitor_fileio_LIBADD_generate = \
	@TARGET_LIBMONITOR_LIBS@ \
	@TARGET_LIBUNWIND_LIBS@ \
	-lcbtf-services-common \
	-lcbtf-services-monitor \
	-lcbtf-services-offline \
	-lcbtf-services-unwind \
	-lcbtf-services-timer \
	$(1) \
	-lrt -lpthread

lib_monitor_fileio_sources = \
	$(WRAPPER_SOURCES) \
	$(COLLECTOR_SOURCES)

endif


if HAVE_MPICH
if HAVE_LIBMONITOR
mpi_mpich_collector_monitor_fileio_la_CFLAGS = $(call monitor_fileio_CFLAGS_generate,@MPICH_CPPFLAGS@)
mpi_mpich_collector_monitor_fileio_la_LDFLAGS = -module $(call monitor_fileio_LDFLAGS_generate,@MPICH_LDFLAGS@)
mpi_mpich_collector_monitor_fileio_la_LIBADD = $(call monitor_fileio_LIBADD_generate,@MPICH_LIBS@)
mpi_mpich_collector_monitor_fileio_la_DEPENDENCIES =
mpi_mpich_collector_monitor_fileio_la_SOURCES = $(monitor_fileio_sources)

libmpi_mpich_monitor_fileio_la_CFLAGS = -static $(call lib_monitor_fileio_CFLAGS_generate,@MPICH_CPPFLAGS@)
libmpi_mpich_monitor_fileio_la_LDFLAGS = $(call lib_monitor_fileio_LDFLAGS_generate,@MPICH_LDFLAGS@)
libmpi_mpich_monitor_fileio_la_LIBADD = $(call lib_monitor_fileio_LIBADD_generate,@MPICH_LIBS@)
libmpi_mpich_monitor_fileio_la_DEPENDENCIES =
libmpi_mpich_monitor_fileio_la_SOURCES = $(WRAPPER_SOURCES)  $(COLLECTOR_SOURCES)
if HAVE_MRNET
mpi_mpich_collector_monitor_mrnet_mpi_la_CFLAGS = $(call monitor_mrnet_mpi_CFLAGS_generate,@MPICH_CPPFLAGS@)
mpi_mpich_collector_monitor_mrnet_mpi_la_LDFLAGS = -module $(call monitor_mrnet_mpi_LDFLAGS_generate,@MPICH_LDFLAGS@)
mpi_mpich_collector_monitor_mrnet_mpi_la_LIBADD = $(call monitor_mrnet_mpi_LIBADD_generate,@MPICH_LIBS@)
mpi_mpich_collector_monitor_mrnet_mpi_la_DEPENDENCIES =
mpi_mpich_collector_monitor_mrnet_mpi_la_SOURCES = $(WRAPPER_SOURCES)  $(COLLECTOR_SOURCES)

libmpi_mpich_monitor_mrnet_mpi_la_CFLAGS = -static $(call lib_monitor_mrnet_mpi_CFLAGS_generate,@MPICH_CPPFLAGS@)
libmpi_mpich_monitor_mrnet_mpi_la_LDFLAGS = $(call lib_monitor_mrnet_mpi_LDFLAGS_generate,@MPICH_LDFLAGS@)
libmpi_mpich_monitor_mrnet_mpi_la_LIBADD = $(call lib_monitor_mrnet_mpi_LIBADD_generate,@MPICH_LIBS@)
libmpi_mpich_monitor_mrnet_mpi_la_DEPENDENCIES =
libmpi_mpich_monitor_mrnet_mpi_la_SOURCES = $(WRAPPER_SOURCES)  $(COLLECTOR_SOURCES)
endif
endif
endif

#
# MPICH2 SETUP
#
if HAVE_MPICH2
if HAVE_LIBMONITOR
mpi_mpich2_collector_monitor_fileio_la_CFLAGS = $(call monitor_fileio_CFLAGS_generate,@MPICH2_CPPFLAGS@)
mpi_mpich2_collector_monitor_fileio_la_LDFLAGS = -module $(call monitor_fileio_LDFLAGS_generate,@MPICH2_LDFLAGS@)
mpi_mpich2_collector_monitor_fileio_la_LIBADD = $(call monitor_fileio_LIBADD_generate,@MPICH2_LIBS@)
mpi_mpich2_collector_monitor_fileio_la_DEPENDENCIES =
mpi_mpich2_collector_monitor_fileio_la_SOURCES = $(monitor_fileio_sources)
libmpi_mpich2_monitor_fileio_la_CFLAGS = -static $(call lib_monitor_fileio_CFLAGS_generate,@MPICH2_CPPFLAGS@)
libmpi_mpich2_monitor_fileio_la_LDFLAGS = $(call lib_monitor_fileio_LDFLAGS_generate,@MPICH2_LDFLAGS@)
libmpi_mpich2_monitor_fileio_la_LIBADD = $(call lib_monitor_fileio_LIBADD_generate,@MPICH2_LIBS@)
libmpi_mpich2_monitor_fileio_la_DEPENDENCIES =
libmpi_mpich2_monitor_fileio_la_SOURCES = $(lib_monitor_fileio_sources)
if HAVE_MRNET
mpi_mpich2_collector_monitor_mrnet_mpi_la_CFLAGS = $(call monitor_mrnet_mpi_CFLAGS_generate,@MPICH2_CPPFLAGS@)
mpi_mpich2_collector_monitor_mrnet_mpi_la_LDFLAGS = -module $(call monitor_mrnet_mpi_LDFLAGS_generate,@MPICH2_LDFLAGS@)
mpi_mpich2_collector_monitor_mrnet_mpi_la_LIBADD = $(call monitor_mrnet_mpi_LIBADD_generate,@MPICH2_LIBS@)
mpi_mpich2_collector_monitor_mrnet_mpi_la_DEPENDENCIES =
mpi_mpich2_collector_monitor_mrnet_mpi_la_SOURCES = $(monitor_mrnet_mpi_sources)

libmpi_mpich2_monitor_mrnet_mpi_la_CFLAGS = -static $(call lib_monitor_mrnet_mpi_CFLAGS_generate,@MPICH2_CPPFLAGS@)
libmpi_mpich2_monitor_mrnet_mpi_la_LDFLAGS = $(call lib_monitor_mrnet_mpi_LDFLAGS_generate,@MPICH2_LDFLAGS@)
libmpi_mpich2_monitor_mrnet_mpi_la_LIBADD = $(call lib_monitor_mrnet_mpi_LIBADD_generate,@MPICH2_LIBS@)
libmpi_mpich2_monitor_mrnet_mpi_la_DEPENDENCIES =
libmpi_mpich2_monitor_mrnet_mpi_la_SOURCES = $(lib_monitor_mrnet_mpi_sources)
endif
endif
endif


if HAVE_MVAPICH
if HAVE_LIBMONITOR
mpi_mvapich_collector_monitor_fileio_la_CFLAGS = $(call monitor_fileio_CFLAGS_generate,@MVAPICH_CPPFLAGS@)
mpi_mvapich_collector_monitor_fileio_la_LDFLAGS = -module $(call monitor_fileio_LDFLAGS_generate,@MVAPICH_LDFLAGS@)
mpi_mvapich_collector_monitor_fileio_la_LIBADD = $(call monitor_fileio_LIBADD_generate,@MVAPICH_LIBS@)
mpi_mvapich_collector_monitor_fileio_la_DEPENDENCIES =
mpi_mvapich_collector_monitor_fileio_la_SOURCES = $(monitor_fileio_sources)

libmpi_mvapich_monitor_fileio_la_CFLAGS = -static $(call lib_monitor_fileio_CFLAGS_generate,@MVAPICH_CPPFLAGS@)
libmpi_mvapich_monitor_fileio_la_LDFLAGS = $(call lib_monitor_fileio_LDFLAGS_generate,@MVAPICH_LDFLAGS@)
libmpi_mvapich_monitor_fileio_la_LIBADD = $(call lib_monitor_fileio_LIBADD_generate,@MVAPICH_LIBS@)
libmpi_mvapich_monitor_fileio_la_DEPENDENCIES =
libmpi_mvapich_monitor_fileio_la_SOURCES = $(lib_monitor_fileio_sources)
if HAVE_MRNET
mpi_mvapich_collector_monitor_mrnet_mpi_la_CFLAGS = $(call monitor_mrnet_mpi_CFLAGS_generate,@MVAPICH_CPPFLAGS@)
mpi_mvapich_collector_monitor_mrnet_mpi_la_LDFLAGS = -module $(call monitor_mrnet_mpi_LDFLAGS_generate,@MVAPICH_LDFLAGS@)
mpi_mvapich_collector_monitor_mrnet_mpi_la_LIBADD = $(call monitor_mrnet_mpi_LIBADD_generate,@MVAPICH_LIBS@)
mpi_mvapich_collector_monitor_mrnet_mpi_la_DEPENDENCIES =
mpi_mvapich_collector_monitor_mrnet_mpi_la_SOURCES = $(monitor_mrnet_mpi_sources)

libmpi_mvapich_monitor_mrnet_mpi_la_CFLAGS = -static $(call lib_monitor_mrnet_mpi_CFLAGS_generate,@MVAPICH_CPPFLAGS@)
libmpi_mvapich_monitor_mrnet_mpi_la_LDFLAGS = $(call lib_monitor_mrnet_mpi_LDFLAGS_generate,@MVAPICH_LDFLAGS@)
libmpi_mvapich_monitor_mrnet_mpi_la_LIBADD = $(call lib_monitor_mrnet_mpi_LIBADD_generate,@MVAPICH_LIBS@)
libmpi_mvapich_monitor_mrnet_mpi_la_DEPENDENCIES =
libmpi_mvapich_monitor_mrnet_mpi_la_SOURCES = $(lib_monitor_mrnet_mpi_sources)
endif
endif
endif

if HAVE_MVAPICH2
if HAVE_LIBMONITOR
mpi_mvapich2_collector_monitor_fileio_la_CFLAGS = $(call monitor_fileio_CFLAGS_generate,@MVAPICH2_CPPFLAGS@)
mpi_mvapich2_collector_monitor_fileio_la_LDFLAGS = -module $(call monitor_fileio_LDFLAGS_generate,@MVAPICH2_LDFLAGS@)
mpi_mvapich2_collector_monitor_fileio_la_LIBADD = $(call monitor_fileio_LIBADD_generate,@MVAPICH2_LIBS@)
mpi_mvapich2_collector_monitor_fileio_la_DEPENDENCIES =
mpi_mvapich2_collector_monitor_fileio_la_SOURCES = $(monitor_fileio_sources)

libmpi_mvapich2_monitor_fileio_la_CFLAGS = -static $(call lib_monitor_fileio_CFLAGS_generate,@MVAPICH2_CPPFLAGS@)
libmpi_mvapich2_monitor_fileio_la_LDFLAGS = $(call lib_monitor_fileio_LDFLAGS_generate,@MVAPICH2_LDFLAGS@)
libmpi_mvapich2_monitor_fileio_la_LIBADD = $(call lib_monitor_fileio_LIBADD_generate,@MVAPICH2_LIBS@)
libmpi_mvapich2_monitor_fileio_la_DEPENDENCIES =
libmpi_mvapich2_monitor_fileio_la_SOURCES = $(lib_monitor_fileio_sources)
if HAVE_MRNET
mpi_mvapich2_collector_monitor_mrnet_mpi_la_CFLAGS = $(call monitor_mrnet_mpi_CFLAGS_generate,@MVAPICH2_CPPFLAGS@)
mpi_mvapich2_collector_monitor_mrnet_mpi_la_LDFLAGS = -module $(call monitor_mrnet_mpi_LDFLAGS_generate,@MVAPICH2_LDFLAGS@)
mpi_mvapich2_collector_monitor_mrnet_mpi_la_LIBADD = $(call monitor_mrnet_mpi_LIBADD_generate,@MVAPICH2_LIBS@)
mpi_mvapich2_collector_monitor_mrnet_mpi_la_DEPENDENCIES =
mpi_mvapich2_collector_monitor_mrnet_mpi_la_SOURCES = $(monitor_mrnet_mpi_sources)

libmpi_mvapich2_monitor_mrnet_mpi_la_CFLAGS = -static $(call lib_monitor_mrnet_mpi_CFLAGS_generate,@MVAPICH2_CPPFLAGS@)
libmpi_mvapich2_monitor_mrnet_mpi_la_LDFLAGS = $(call lib_monitor_mrnet_mpi_LDFLAGS_generate,@MVAPICH2_LDFLAGS@)
libmpi_mvapich2_monitor_mrnet_mpi_la_LIBADD = $(call lib_monitor_mrnet_mpi_LIBADD_generate,@MVAPICH2_LIBS@)
libmpi_mvapich2_monitor_mrnet_mpi_la_DEPENDENCIES =
libmpi_mvapich2_monitor_mrnet_mpi_la_SOURCES = $(lib_monitor_mrnet_mpi_sources)
endif
endif
endif

if HAVE_OPENMPI
if HAVE_LIBMONITOR
mpi_openmpi_collector_monitor_fileio_la_CFLAGS = $(call monitor_fileio_CFLAGS_generate,@OPENMPI_CPPFLAGS@)
mpi_openmpi_collector_monitor_fileio_la_LDFLAGS = -module $(call monitor_fileio_LDFLAGS_generate,@OPENMPI_LDFLAGS@)
mpi_openmpi_collector_monitor_fileio_la_LIBADD = $(call monitor_fileio_LIBADD_generate,@OPENMPI_LIBS@)
mpi_openmpi_collector_monitor_fileio_la_DEPENDENCIES =
mpi_openmpi_collector_monitor_fileio_la_SOURCES = $(monitor_fileio_sources)

libmpi_openmpi_monitor_fileio_la_CFLAGS = -static $(call lib_monitor_fileio_CFLAGS_generate,@OPENMPI_CPPFLAGS@)
libmpi_openmpi_monitor_fileio_la_LDFLAGS = $(call lib_monitor_fileio_LDFLAGS_generate,@OPENMPI_LDFLAGS@)
libmpi_openmpi_monitor_fileio_la_LIBADD = $(call lib_monitor_fileio_LIBADD_generate,@OPENMPI_LIBS@)
libmpi_openmpi_monitor_fileio_la_DEPENDENCIES =
libmpi_openmpi_monitor_fileio_la_SOURCES = $(lib_monitor_fileio_sources)
if HAVE_MRNET
mpi_openmpi_collector_monitor_mrnet_mpi_la_CFLAGS = $(call monitor_mrnet_mpi_CFLAGS_generate,@OPENMPI_CPPFLAGS@)
mpi_openmpi_collector_monitor_mrnet_mpi_la_LDFLAGS = -module $(call monitor_mrnet_mpi_LDFLAGS_generate,@OPENMPI_LDFLAGS@)
mpi_openmpi_collector_monitor_mrnet_mpi_la_LIBADD = $(call monitor_mrnet_mpi_LIBADD_generate,@OPENMPI_LIBS@)
mpi_openmpi_collector_monitor_mrnet_mpi_la_DEPENDENCIES =
mpi_openmpi_collector_monitor_mrnet_mpi_la_SOURCES = $(monitor_mrnet_mpi_sources)

libmpi_openmpi_monitor_mrnet_mpi_la_CFLAGS = -static $(call lib_monitor_mrnet_mpi_CFLAGS_generate,@OPENMPI_CPPFLAGS@)
libmpi_openmpi_monitor_mrnet_mpi_la_LDFLAGS = $(call lib_monitor_mrnet_mpi_LDFLAGS_generate,@OPENMPI_LDFLAGS@)
libmpi_openmpi_monitor_mrnet_mpi_la_LIBADD = $(call lib_monitor_mrnet_mpi_LIBADD_generate,@OPENMPI_LIBS@)
libmpi_openmpi_monitor_mrnet_mpi_la_DEPENDENCIES =
libmpi_openmpi_monitor_mrnet_mpi_la_SOURCES = $(lib_monitor_mrnet_mpi_sources)
endif
endif
endif

if HAVE_MPT
if HAVE_LIBMONITOR
mpi_mpt_collector_monitor_fileio_la_CFLAGS = -DSGI_MPT $(call monitor_fileio_CFLAGS_generate,@MPT_CPPFLAGS@)
mpi_mpt_collector_monitor_fileio_la_LDFLAGS = -module $(call monitor_fileio_LDFLAGS_generate,@MPT_LDFLAGS@)
mpi_mpt_collector_monitor_fileio_la_LIBADD = $(call monitor_fileio_LIBADD_generate,@MPT_LIBS@)
mpi_mpt_collector_monitor_fileio_la_DEPENDENCIES =
mpi_mpt_collector_monitor_fileio_la_SOURCES = $(monitor_fileio_sources)

libmpi_mpt_monitor_fileio_la_CFLAGS = -static -DSGI_MPT $(call lib_monitor_fileio_CFLAGS_generate,@MPT_CPPFLAGS@)
libmpi_mpt_monitor_fileio_la_LDFLAGS = $(call lib_monitor_fileio_LDFLAGS_generate,@MPT_LDFLAGS@)
libmpi_mpt_monitor_fileio_la_LIBADD = $(call lib_monitor_fileio_LIBADD_generate,@MPT_LIBS@)
libmpi_mpt_monitor_fileio_la_DEPENDENCIES =
libmpi_mpt_monitor_fileio_la_SOURCES = $(lib_monitor_fileio_sources)
if HAVE_MRNET
mpi_mpt_collector_monitor_mrnet_mpi_la_CFLAGS = $(call monitor_mrnet_mpi_CFLAGS_generate,@MPT_CPPFLAGS@)
mpi_mpt_collector_monitor_mrnet_mpi_la_LDFLAGS = -module $(call monitor_mrnet_mpi_LDFLAGS_generate,@MPT_LDFLAGS@)
mpi_mpt_collector_monitor_mrnet_mpi_la_LIBADD = $(call monitor_mrnet_mpi_LIBADD_generate,@MPT_LIBS@)
mpi_mpt_collector_monitor_mrnet_mpi_la_DEPENDENCIES =
mpi_mpt_collector_monitor_mrnet_mpi_la_SOURCES = $(monitor_mrnet_mpi_sources)

libmpi_mpt_monitor_mrnet_mpi_la_CFLAGS = -static $(call lib_monitor_mrnet_mpi_CFLAGS_generate,@MPT_CPPFLAGS@)
libmpi_mpt_monitor_mrnet_mpi_la_LDFLAGS = $(call lib_monitor_mrnet_mpi_LDFLAGS_generate,@MPT_LDFLAGS@)
libmpi_mpt_monitor_mrnet_mpi_la_LIBADD = $(call lib_monitor_mrnet_mpi_LIBADD_generate,@MPT_LIBS@)
libmpi_mpt_monitor_mrnet_mpi_la_DEPENDENCIES =
libmpi_mpt_monitor_mrnet_mpi_la_SOURCES = $(lib_monitor_mrnet_mpi_sources)
endif
endif
endif

#
# If we have targetted MPI then use those variables.  Can not build both from same configure.
#
if HAVE_TARGET_MPI
ANY_MPI_IMPL = @DEFAULT_TARGET_MPI_IMPL@
ANY_MPI_HEADER = $(TARGET_$(ANY_MPI_IMPL)_HEADER)
else
ANY_MPI_IMPL = @DEFAULT_MPI_IMPL@
ANY_MPI_HEADER = $($(ANY_MPI_IMPL)_HEADER)
endif

CLEANFILES = $(BUILT_FILES)

mpi.xml: mpi.xml.in

	@rm -f mpi.xml
	@sed \
		-e 's,@@component_location@@,$(plugindir),' \
		-e 's,@@cbtflib_location@@,$(libdir),' \
		$(srcdir)/mpi.xml.in > mpi.xml

EXTRA_DIST = mpi.xml.in
