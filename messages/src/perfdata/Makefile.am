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

lib_LTLIBRARIES = libcbtf-messages-perfdata.la \
		  libcbtf-messages-converters-perfdata.la

BUILT_SOURCES = \
	DataHeader.h DataHeader.c \
	PCSamp.h PCSamp.c \
	PCSamp_data.h PCSamp_data.c \
	Hwc.h Hwc.c \
	Hwc_data.h Hwc_data.c \
	IO.h IO.c \
	IO_data.h IO_data.c \
	Mem.h Mem.c \
	Mem_data.h Mem_data.c \
	Usertime.h Usertime.c \
	Usertime_data.h Usertime_data.c

libcbtf_messages_perfdata_la_CFLAGS = \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/src/base \
	@LTDLINCL@ 

libcbtf_messages_converters_perfdata_la_CXXFLAGS = \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/src/base \
	@BOOST_CPPFLAGS@ \
	@MRNET_CPPFLAGS@ \
	@CBTF_CPPFLAGS@ @CBTF_MRNET_CPPFLAGS@ \
	@LTDLINCL@ 

libcbtf_messages_perfdata_la_LDFLAGS = \
	-version-info 0:0:0

libcbtf_messages_converters_perfdata_la_LDFLAGS = \
	@MRNET_LDFLAGS@ \
	-version-info 0:0:0

libcbtf_messages_perfdata_la_LIBADD = \
	@LIBLTDL@

libcbtf_messages_converters_perfdata_la_LIBADD = \
	@MRNET_LIBS@ \
	@LIBLTDL@

libcbtf_messages_perfdata_la_SOURCES = \
	DataHeader.x \
	PCSamp.x PCSamp_data.x \
	Hwc.x Hwc_data.x \
	IO.x IO_data.x \
	Mem.x Mem_data.x \
	Usertime.x Usertime_data.x \
	$(BUILT_SOURCES)

libcbtf_messages_converters_perfdata_la_SOURCES = \
	Converters.cpp

xdr_headers = \
	DataHeader.h \
	PCSamp.h PCSamp_data.h \
	IO.h IO_data.h \
	Hwc.h Hwc_data.h \
	Mem.h Mem_data.h \
	Usertime.h Usertime_data.h

SUFFIXES = .x
CLEANFILES = $(BUILT_SOURCES)

DataHeader.h DataHeader.c : DataHeader.x
	rm -f  $(patsubst %.x, %.h, $<) $(patsubst %.x, %.c, $<)
	$(RPCGEN) -h -o $(patsubst %.x, %.h, $<) $<
	$(RPCGEN) -c -o $(patsubst %.x, %.c, $<) $<

PCSamp.h PCSamp.c : PCSamp.x
	rm -f  $(patsubst %.x, %.h, $<) $(patsubst %.x, %.c, $<)
	$(RPCGEN) -h -o $(patsubst %.x, %.h, $<) $<
	$(RPCGEN) -c -o $(patsubst %.x, %.c, $<) $<

PCSamp_data.h PCSamp_data.c : PCSamp_data.x
	rm -f  $(patsubst %.x, %.h, $<) $(patsubst %.x, %.c, $<)
	$(RPCGEN) -h -o $(patsubst %.x, %.h, $<) $<
	$(RPCGEN) -c -o $(patsubst %.x, %.c, $<) $<

Hwc.h Hwc.c : Hwc.x
	rm -f  $(patsubst %.x, %.h, $<) $(patsubst %.x, %.c, $<)
	$(RPCGEN) -h -o $(patsubst %.x, %.h, $<) $<
	$(RPCGEN) -c -o $(patsubst %.x, %.c, $<) $<

Hwc_data.h Hwc_data.c : Hwc_data.x
	rm -f  $(patsubst %.x, %.h, $<) $(patsubst %.x, %.c, $<)
	$(RPCGEN) -h -o $(patsubst %.x, %.h, $<) $<
	$(RPCGEN) -c -o $(patsubst %.x, %.c, $<) $<

IO.h IO.c : IO.x
	rm -f  $(patsubst %.x, %.h, $<) $(patsubst %.x, %.c, $<)
	$(RPCGEN) -h -o $(patsubst %.x, %.h, $<) $<
	$(RPCGEN) -c -o $(patsubst %.x, %.c, $<) $<

IO_data.h IO_data.c : IO_data.x
	rm -f  $(patsubst %.x, %.h, $<) $(patsubst %.x, %.c, $<)
	$(RPCGEN) -h -o $(patsubst %.x, %.h, $<) $<
	$(RPCGEN) -c -o $(patsubst %.x, %.c, $<) $<

Mem.h Mem.c : Mem.x
	rm -f  $(patsubst %.x, %.h, $<) $(patsubst %.x, %.c, $<)
	$(RPCGEN) -h -o $(patsubst %.x, %.h, $<) $<
	$(RPCGEN) -c -o $(patsubst %.x, %.c, $<) $<

Mem_data.h Mem_data.c : Mem_data.x
	rm -f  $(patsubst %.x, %.h, $<) $(patsubst %.x, %.c, $<)
	$(RPCGEN) -h -o $(patsubst %.x, %.h, $<) $<
	$(RPCGEN) -c -o $(patsubst %.x, %.c, $<) $<

Usertime.h Usertime.c : Usertime.x
	rm -f  $(patsubst %.x, %.h, $<) $(patsubst %.x, %.c, $<)
	$(RPCGEN) -h -o $(patsubst %.x, %.h, $<) $<
	$(RPCGEN) -c -o $(patsubst %.x, %.c, $<) $<

Usertime_data.h Usertime_data.c : Usertime_data.x
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

## uninstall the include files installed.
uninstall-hook:
	@for f in $(xdr_headers) ; do \
	d="KrellInstitute/Messages"; \
	    $(RM) $(DESTDIR)$(includedir)/$$d/$$f; \
	done
