################################################################################
# Copyright (c) 2013 Krell Institute. All Rights Reserved.
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

contribxmldir = $(datadir)/KrellInstitute/contrib/xml
contribplugindir = $(datadir)/KrellInstitute/contrib/Components

BUILT_FILES = pyexample.xml

nobase_dist_contribxml_DATA = \
        pyexample.xml

contribplugin_LTLIBRARIES = pyExample.la

pyExample_la_SOURCES = pyExample.cpp mrnetConverters.cpp

pyExample_la_CXXFLAGS = \
	-DLIBPYTHON=\"lib$(PYTHON_LIB).so\" \
	@BOOST_CPPFLAGS@ \
	@MRNET_CPPFLAGS@ \
	@PYTHON_CPPFLAGS@ \
	@CBTF_CPPFLAGS@

pyExample_la_LDFLAGS = \
	-L$(top_srcdir) \
	-module -avoid-version -shared \
	@BOOST_LDFLAGS@ \
	@MRNET_LDFLAGS@ \
	@PYTHON_EXTRA_LDFLAGS@ \
	@PYTHON_LDFLAGS@ \
	@CBTF_LDFLAGS@

pyExample_la_LIBADD = \
	-ldl \
	@MRNET_LIBS@ \
	-l@BOOST_PYTHON_LIB@ \
	@PYTHON_EXTRA_LIBS@ \
	@CBTF_LIBS@

CLEANFILES = $(BUILT_FILES)

pyexample.xml: pyexample.xml.in
	@rm -f pyexample.xml
	@sed \
                -e 's,@@contriblib_location@@,$(contribplugindir),' \
                $(srcdir)/pyexample.xml.in > pyexample.xml

EXTRA_DIST = pyexample.xml.in
