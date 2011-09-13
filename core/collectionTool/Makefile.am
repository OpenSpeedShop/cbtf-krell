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

xmldir = $(datadir)/KrellInstitute/xml

bin_PROGRAMS = collectionTool

collectionTool_CXXFLAGS = \
	-DLIBDIR="\"$(libdir)\"" \
	-DXMLDIR="\"$(xmldir)\"" \
	@BOOST_CPPFLAGS@ \
	@CBTF_CPPFLAGS@ \
	@CBTF_MRNET_CPPFLAGS@ \
	@CBTF_XML_CPPFLAGS@ \
	@MRNET_CPPFLAGS@

collectionTool_LDFLAGS = \
	@BOOST_LDFLAGS@ \
	@CBTF_LDFLAGS@ \
	@CBTF_MRNET_LDFLAGS@ \
	@CBTF_XML_LDFLAGS@ \
	@MRNET_LDFLAGS@

collectionTool_LDADD = \
	-lboost_program_options \
	@BOOST_SYSTEM_LIB@ \
	@BOOST_THREAD_LIB@ \
	@CBTF_LIBS@ \
	@CBTF_MRNET_LIBS@\
	@CBTF_XML_LIBS@\
	@MRNET_LIBS@

collectionTool_SOURCES = \
	collectionTool.cpp
