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


if !BUILD_TARGETED
noinst_PROGRAMS = testXDR
testXDR_CXXFLAGS = \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/../../tools/messages/include \
	-I$(top_srcdir)/../../tools/messages/src/perfdata \
	-I$(top_srcdir)/../../tools/services/include \
	-I$(top_srcdir)/../../tools/core/include \
	-I$(top_srcdir)/../../framework/libcbtf-xml/include \
	-DBUILDDIR="\"$(abs_builddir)\"" \
	-DTOPDIR="\"$(top_srcdir)\"" \
	@BOOST_CPPFLAGS@ \
	@CBTF_CPPFLAGS@ \
	@BINUTILS_CPPFLAGS@ \
	@MRNET_CPPFLAGS@

testXDR_LDFLAGS = \
	-L$(top_srcdir)/src \
	-L$(top_srcdir)/../../tools/messages/src/base/.libs \
	-L$(top_srcdir)/../../tools/core/src/.libs \
	-L$(top_srcdir)/../../framework/build/libcbtf-xml \
	@BOOST_LDFLAGS@ \
	@LIBXERCES_C_LDFLAGS@ \
	@CBTF_LDFLAGS@ \
	@BINUTILS_LDFLAGS@ \
	@MRNET_LDFLAGS@


testXDR_LDADD = \
	-lcbtf-mrnet \
	-lcbtf-xml \
        -lcbtf-messages-base -lcbtf-messages-converters-base \
	-lcbtf-core \
	@BOOST_UNIT_TEST_FRAMEWORK_LIB@ \
	@BOOST_SYSTEM_LIB@ \
	@BOOST_THREAD_LIB@ \
	@BOOST_FILESYSTEM_LIB@ \
	@LIBXERCES_C@ \
	@CBTF_LIBS@ \
	@BINUTILS_BFD_LIB@ \
	@MRNET_LIBS@

testXDR_SOURCES = \
	testXDR.cpp
endif

if HAVE_TARGET_SHARED
noinst_PROGRAMS = testXDR
testXDR_CXXFLAGS = \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/../../tools/messages/include \
	-I$(top_srcdir)/../../tools/messages/src/perfdata \
	-I$(top_srcdir)/../../tools/services/include \
	-I$(top_srcdir)/../../tools/core/include \
	-I$(top_srcdir)/../../framework/libcbtf-xml/include \
	-DBUILDDIR="\"$(abs_builddir)\"" \
	-DTOPDIR="\"$(top_srcdir)\"" \
	@TARGET_BOOST_CPPFLAGS@ \
	@TARGET_CBTF_CPPFLAGS@ \
	@TARGET_BINUTILS_CPPFLAGS@ \
	@TARGET_MRNET_CPPFLAGS@

testXDR_LDFLAGS = \
	-L$(top_srcdir)/src \
	-L$(top_srcdir)/../../tools/messages/src/base/.libs \
	-L$(top_srcdir)/../../tools/core/src/.libs \
	-L$(top_srcdir)/../../framework/build/libcbtf-xml \
	@TARGET_BOOST_LDFLAGS@ \
	@TARGET_CBTF_LDFLAGS@ \
	@TARGET_BINUTILS_LDFLAGS@ \
	@TARGET_MRNET_LDFLAGS@


testXDR_LDADD = \
	-lcbtf-mrnet \
	-lcbtf-xml \
        -lcbtf-messages-base -lcbtf-messages-converters-base \
	-lcbtf-core \
	@TARGET_BOOST_UNIT_TEST_FRAMEWORK_LIB@ \
	@TARGET_BOOST_SYSTEM_LIB@ \
	@TARGET_BOOST_THREAD_LIB@ \
	@TARGET_CBTF_LIBS@ \
	@TARGET_BINUTILS_BFD_LIB@ \
	@TARGET_MRNET_LIBS@

testXDR_SOURCES = \
	testXDR.cpp
endif

