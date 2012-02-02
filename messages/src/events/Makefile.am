################################################################################
# Copyright (c) 2011-2012 Krell Institute. All Rights Reserved.
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

lib_LTLIBRARIES = libcbtf-messages-events.la
# If build for target only add converters if target is also shared
if BUILD_TARGETED

if HAVE_TARGET_SHARED
lib_LTLIBRARIES += libcbtf-messages-converters-events.la
endif
else
# for non targeted, always build the converters
lib_LTLIBRARIES += libcbtf-messages-converters-events.la
endif

BUILT_SOURCES = \
	EventHeader.h EventHeader.c \
	OfflineEvents.h OfflineEvents.c \
	LinkedObjectEvents.h LinkedObjectEvents.c \
	ThreadEvents.h ThreadEvents.c

libcbtf_messages_events_la_CFLAGS = \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/src/base \
	@LTDLINCL@ 

libcbtf_messages_events_la_LDFLAGS = \
	-version-info 0:0:0

libcbtf_messages_events_la_LIBADD = \
	@LIBLTDL@

libcbtf_messages_events_la_SOURCES = \
	EventHeader.x \
	LinkedObjectEvents.x \
	OfflineEvents.x \
	ThreadEvents.x \
	$(BUILT_SOURCES)

if !BUILD_TARGETED
libcbtf_messages_converters_events_la_CXXFLAGS = \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/src/base \
	@BOOST_CPPFLAGS@ \
	@MRNET_CPPFLAGS@ \
	@CBTF_CPPFLAGS@ @CBTF_MRNET_CPPFLAGS@ \
	@LTDLINCL@ 

libcbtf_messages_converters_events_la_LDFLAGS = \
	@MRNET_LDFLAGS@ \
	-version-info 0:0:0

libcbtf_messages_converters_events_la_LIBADD = \
	@MRNET_LIBS@ \
	@LIBLTDL@

libcbtf_messages_converters_events_la_SOURCES = \
	Converters.cpp
else
if HAVE_TARGET_SHARED
libcbtf_messages_converters_events_la_CXXFLAGS = \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/src/base \
	@TARGET_BOOST_CPPFLAGS@ \
	@TARGET_MRNET_CPPFLAGS@ \
	@TARGET_CBTF_CPPFLAGS@ @TARGET_CBTF_MRNET_CPPFLAGS@ \
	@LTDLINCL@ 

libcbtf_messages_converters_events_la_LDFLAGS = \
	@TARGET_MRNET_LDFLAGS@ \
	-version-info 0:0:0

libcbtf_messages_converters_events_la_LIBADD = \
	@TARGET_MRNET_LIBS@ \
	@LIBLTDL@

libcbtf_messages_converters_events_la_SOURCES = \
	Converters.cpp
endif

if HAVE_TARGET_STATIC
libcbtf_messages_converters_events_la_CXXFLAGS = \
	-static \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/src/base \
	@TARGET_BOOST_CPPFLAGS@ \
	@TARGET_MRNET_CPPFLAGS@ \
	@TARGET_CBTF_CPPFLAGS@ @TARGET_CBTF_MRNET_CPPFLAGS@ \
	@LTDLINCL@ 

libcbtf_messages_converters_events_la_LDFLAGS = \
	-static \
	@TARGET_MRNET_LDFLAGS@ \
	-version-info 0:0:0

libcbtf_messages_converters_events_la_LIBADD = \
	@TARGET_MRNET_LIBS@ \
	@LIBLTDL@

libcbtf_messages_converters_events_la_SOURCES = \
	Converters.cpp
endif
# endif below is for end of build targeted check
endif

xdr_headers = EventHeader.h LinkedObjectEvents.h OfflineEvents.h ThreadEvents.h

SUFFIXES = .x
CLEANFILES = $(BUILT_SOURCES)

EventHeader.h EventHeader.c : EventHeader.x
	rm -f  $(patsubst %.x, %.h, $<) $(patsubst %.x, %.c, $<)
	$(RPCGEN) -h -o $(patsubst %.x, %.h, $<) $<
	$(RPCGEN) -c -o $(patsubst %.x, %.c, $<) $<

LinkedObjectEvents.h LinkedObjectEvents.c : LinkedObjectEvents.x
	rm -f  $(patsubst %.x, %.h, $<) $(patsubst %.x, %.c, $<)
	$(RPCGEN) -h -o $(patsubst %.x, %.h, $<) $<
	$(RPCGEN) -c -o $(patsubst %.x, %.c, $<) $<

OfflineEvents.h OfflineEvents.c : OfflineEvents.x
	rm -f  $(patsubst %.x, %.h, $<) $(patsubst %.x, %.c, $<)
	$(RPCGEN) -h -o $(patsubst %.x, %.h, $<) $<
	$(RPCGEN) -c -o $(patsubst %.x, %.c, $<) $<

ThreadEvents.h ThreadEvents.c : ThreadEvents.x
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
