################################################################################
# Copyright (c) 2011 Krell Institute. All Rights Reserved.
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

lib_LTLIBRARIES = libcbtf-messages-instrumentation.la \
		  libcbtf-messages-converters-instrumentation.la

BUILT_SOURCES = \
	Instrument.h Instrument.c \
	MpiJob.h MpiJob.c \
	Job.h Job.c

libcbtf_messages_instrumentation_la_CFLAGS = \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/src/base \
	-I$(top_srcdir)/src/collector \
	@LTDLINCL@ 

libcbtf_messages_converters_instrumentation_la_CXXFLAGS = \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/src/base \
	-I$(top_srcdir)/src/collector \
	@BOOST_CPPFLAGS@ \
	@MRNET_CPPFLAGS@ \
	@CBTF_CPPFLAGS@ @CBTF_MRNET_CPPFLAGS@ \
	@LTDLINCL@ 

libcbtf_messages_instrumentation_la_LDFLAGS = \
	-version-info 0:0:0

libcbtf_messages_converters_instrumentation_la_LDFLAGS = \
	@MRNET_LDFLAGS@ \
	-version-info 0:0:0

libcbtf_messages_instrumentation_la_LIBADD = \
	@LIBLTDL@

libcbtf_messages_converters_instrumentation_la_LIBADD = \
	@MRNET_LIBS@ \
	@LIBLTDL@

libcbtf_messages_instrumentation_la_SOURCES = \
	Instrument.x Job.x MpiJob.x \
	$(BUILT_SOURCES)

libcbtf_messages_converters_instrumentation_la_SOURCES = \
	Converters.cpp

xdr_headers = \
	Instrument.h Job.h MpiJob.h

SUFFIXES = .x
CLEANFILES = $(BUILT_SOURCES)

Instrument.h Instrument.c : Instrument.x
	rm -f  $(patsubst %.x, %.h, $<) $(patsubst %.x, %.c, $<)
	$(RPCGEN) -h -o $(patsubst %.x, %.h, $<) $<
	$(RPCGEN) -c -o $(patsubst %.x, %.c, $<) $<

Job.h Job.c : Job.x
	rm -f  $(patsubst %.x, %.h, $<) $(patsubst %.x, %.c, $<)
	$(RPCGEN) -h -o $(patsubst %.x, %.h, $<) $<
	$(RPCGEN) -c -o $(patsubst %.x, %.c, $<) $<

MpiJob.h MpiJob.c : MpiJob.x
	rm -f  $(patsubst %.x, %.h, $<) $(patsubst %.x, %.c, $<)
	$(RPCGEN) -h -o $(patsubst %.x, %.h, $<) $<
	$(RPCGEN) -c -o $(patsubst %.x, %.c, $<) $<


## Compute the install directory at runtime.
## Make the install directory.
## Find the header file -- in our case it might be in srcdir or
## it might be in the build directory.  "p" is the variable that
## names the actual file we will install.
## Actually install the file.
install-data-hook:
	@for f in $(xdr_headers) ; do \
	d="KrellInstitute/Messages"; \
          $(mkinstalldirs) $(DESTDIR)$(includedir)/$$d; \
          if test -f $(srcdir)/$$f; then p=$(srcdir)/$$f; else p=$$f; fi; \
          $(INSTALL_DATA) $$p $(DESTDIR)$(includedir)/$$d/$$f; \
        done

uninstall-hook:
	@for f in $(xdr_headers) ; do \
	d="KrellInstitute/Messages"; \
          $(RM) $(DESTDIR)$(includedir)/$$d/$$f; \
        done
