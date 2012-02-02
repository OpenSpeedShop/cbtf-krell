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

lib_LTLIBRARIES = libcbtf-messages-base.la 
# If build for target only add converters if target is also shared
if BUILD_TARGETED

if HAVE_TARGET_SHARED
lib_LTLIBRARIES += libcbtf-messages-converters-base.la 
endif
else
# for non targeted, always build the converters
lib_LTLIBRARIES += libcbtf-messages-converters-base.la 
endif

BUILT_SOURCES = \
	Address.h Address.c \
	Blob.h Blob.c \
	Error.h Error.c \
	File.h File.c \
	StdIO.h StdIO.c \
	Thread.h Thread.c \
	Time.h Time.c

if !BUILD_TARGETED
libcbtf_messages_converters_base_la_CXXFLAGS = \
	-I$(top_srcdir)/include \
	@BOOST_CPPFLAGS@ \
	@MRNET_CPPFLAGS@ \
	@CBTF_CPPFLAGS@ @CBTF_MRNET_CPPFLAGS@ \
	@LTDLINCL@ 

libcbtf_messages_converters_base_la_LDFLAGS = \
	@MRNET_LDFLAGS@ \
	-version-info 0:0:0

libcbtf_messages_converters_base_la_LIBADD = \
	@MRNET_LIBS@ \
	@LIBLTDL@

libcbtf_messages_converters_base_la_SOURCES = \
	Converters.cpp
else
if HAVE_TARGET_SHARED
libcbtf_messages_converters_base_la_CXXFLAGS = \
	-I$(top_srcdir)/include \
	@TARGET_BOOST_CPPFLAGS@ \
	@TARGET_MRNET_CPPFLAGS@ \
	@TARGET_CBTF_CPPFLAGS@ @TARGET_CBTF_MRNET_CPPFLAGS@ \
	@LTDLINCL@ 

libcbtf_messages_converters_base_la_LDFLAGS = \
	@TARGET_MRNET_LDFLAGS@ \
	-version-info 0:0:0

libcbtf_messages_converters_base_la_LIBADD = \
	@TARGET_MRNET_LIBS@ \
	@LIBLTDL@

libcbtf_messages_converters_base_la_SOURCES = \
	Converters.cpp
endif

if HAVE_TARGET_STATIC
libcbtf_messages_converters_base_la_CXXFLAGS = \
	-static \
	-I$(top_srcdir)/include \
	@TARGET_BOOST_CPPFLAGS@ \
	@TARGET_MRNET_CPPFLAGS@ \
	@TARGET_CBTF_CPPFLAGS@ @TARGET_CBTF_MRNET_CPPFLAGS@ \
	@LTDLINCL@ 

libcbtf_messages_converters_base_la_LDFLAGS = \
	-static \
	@TARGET_MRNET_LDFLAGS@ \
	-version-info 0:0:0

libcbtf_messages_converters_base_la_LIBADD = \
	@TARGET_MRNET_LIBS@ \
	@LIBLTDL@

libcbtf_messages_converters_base_la_SOURCES = \
	Converters.cpp
endif
# endif below is for build targeted
endif

libcbtf_messages_base_la_CFLAGS = \
	-I$(top_srcdir)/include \
	@LTDLINCL@ 

libcbtf_messages_base_la_LDFLAGS = \
	-version-info 0:0:0

libcbtf_messages_base_la_LIBADD = \
	@LIBLTDL@

libcbtf_messages_base_la_SOURCES = \
	Address.x \
	Blob.x \
	Error.x \
	File.x \
	StdIO.x \
	Time.x \
	Thread.x \
	$(BUILT_SOURCES)

xdr_headers = \
	Address.h \
	Blob.h \
	Error.h \
	File.h \
	StdIO.h \
	Time.h \
	Thread.h

SUFFIXES = .x
CLEANFILES = $(BUILT_SOURCES)

Address.h Address.c : Address.x
	rm -f  $(patsubst %.x, %.h, $<) $(patsubst %.x, %.c, $<)
	$(RPCGEN) -h -o $(patsubst %.x, %.h, $<) $<
	$(RPCGEN) -c -o $(patsubst %.x, %.c, $<) $<

Blob.h Blob.c : Blob.x
	rm -f  $(patsubst %.x, %.h, $<) $(patsubst %.x, %.c, $<)
	$(RPCGEN) -h -o $(patsubst %.x, %.h, $<) $<
	$(RPCGEN) -c -o $(patsubst %.x, %.c, $<) $<

Error.h Error.c : Error.x
	rm -f  $(patsubst %.x, %.h, $<) $(patsubst %.x, %.c, $<)
	$(RPCGEN) -h -o $(patsubst %.x, %.h, $<) $<
	$(RPCGEN) -c -o $(patsubst %.x, %.c, $<) $<

File.h File.c : File.x
	rm -f  $(patsubst %.x, %.h, $<) $(patsubst %.x, %.c, $<)
	$(RPCGEN) -h -o $(patsubst %.x, %.h, $<) $<
	$(RPCGEN) -c -o $(patsubst %.x, %.c, $<) $<

StdIO.h StdIO.c : StdIO.x
	rm -f  $(patsubst %.x, %.h, $<) $(patsubst %.x, %.c, $<)
	$(RPCGEN) -h -o $(patsubst %.x, %.h, $<) $<
	$(RPCGEN) -c -o $(patsubst %.x, %.c, $<) $<

Time.h Time.c : Time.x
	rm -f  $(patsubst %.x, %.h, $<) $(patsubst %.x, %.c, $<)
	$(RPCGEN) -h -o $(patsubst %.x, %.h, $<) $<
	$(RPCGEN) -c -o $(patsubst %.x, %.c, $<) $<

Thread.h Thread.c : Thread.x
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
