################################################################################
# Copyright (c) 2010-2013 Krell Institute. All Rights Reserved.
#
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation; either version 2 of the License, or (at your option) any later
# version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# this program; if not, write to the Free Software Foundation, Inc., 59 Temple
# Place, Suite 330, Boston, MA  02111-1307  USA
################################################################################

pkglibdir = ${libdir}/KrellInstitute/Collectors
lib_LTLIBRARIES = libcbtf-services-monitor.la

libcbtf_services_monitor_la_CFLAGS = \
	-I$(top_srcdir)/include \
	@MESSAGES_CPPFLAGS@ \
	@LIBMONITOR_CPPFLAGS@ \
	@LTDLINCL@ 

libcbtf_services_monitor_la_LDFLAGS = \
	@MESSAGES_LDFLAGS@ \
	 @LIBMONITOR_LDFLAGS@ \
	-version-info 0:0:0

libcbtf_services_monitor_la_LIBADD = \
	@LIBMONITOR_LIBS@ \
	@LIBLTDL@

libcbtf_services_monitor_la_SOURCES = \
	Monitor.c


install-exec-hook:
	test -d "${pkglibdir}" || mkdir -p "${pkglibdir}"
	ar x .libs/libcbtf-services-monitor.a libcbtf_services_monitor_la-Monitor.o
	cp  libcbtf_services_monitor_la-Monitor.o ${pkglibdir}/cbtf_monitor.o
uninstall-hook:
	rm -f ${pkglibdir}/cbtf_monitor.o
