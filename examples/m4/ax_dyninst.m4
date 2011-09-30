################################################################################
# Copyright (c) 2010 Krell Institute. All Rights Reserved.
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


################################################################################
# Check for Dyninst (http://www.dyninst.org)
################################################################################

AC_DEFUN([AX_DYNINST], [

    AC_ARG_WITH(dyninst,
                AC_HELP_STRING([--with-dyninst=DIR],
                               [Dyninst installation @<:@/usr@:>@]),
                dyninst_dir=$withval, dyninst_dir="/usr")

    AC_ARG_WITH(dyninst-version,
                AC_HELP_STRING([--with-dyninst-version=VERS],
                               [dyninst-version installation @<:@7.0@:>@]),
                dyninst_vers=$withval, dyninst_vers="7.0")

    DYNINST_CPPFLAGS="-I$dyninst_dir/include/dyninst"
    DYNINST_LDFLAGS="-L$dyninst_dir/$abi_libdir"
    DYNINST_DIR="$dyninst_dir" 
    DYNINST_VERS="$dyninst_vers"

#   The default is to use 6.0 dyninst cppflags and libs.  
#   Change that (the default case entry) when you change the default vers to something other than 6.0
    case "$dyninst_vers" in
	"5.1")
            DYNINST_CPPFLAGS="$DYNINST_CPPFLAGS -DUSE_STL_VECTOR -DIBM_BPATCH_COMPAT"
            DYNINST_LIBS="-ldyninstAPI -lcommon"
            ;;
	"5.2")
            DYNINST_CPPFLAGS="$DYNINST_CPPFLAGS -DUSE_STL_VECTOR"
            DYNINST_LIBS="-ldyninstAPI -lcommon -lsymtabAPI" 
            ;;
	"6.0")
            DYNINST_CPPFLAGS="$DYNINST_CPPFLAGS -DUSE_STL_VECTOR"
            DYNINST_LIBS="-ldyninstAPI -lcommon -lsymtabAPI -linstructionAPI" 
            ;;
	"6.1")
            DYNINST_CPPFLAGS="$DYNINST_CPPFLAGS -DUSE_STL_VECTOR"
            DYNINST_LIBS="-ldyninstAPI -lcommon -lsymtabAPI -linstructionAPI" 
            ;;
	"7.0")
            DYNINST_CPPFLAGS="$DYNINST_CPPFLAGS -DUSE_STL_VECTOR"
            DYNINST_LIBS="-ldyninstAPI -lcommon -lsymtabAPI -linstructionAPI -lparseAPI" 
            ;;
	*)
            DYNINST_CPPFLAGS="$DYNINST_CPPFLAGS -DUSE_STL_VECTOR"
            DYNINST_LIBS="-ldyninstAPI -lcommon -lsymtabAPI -linstructionAPI" 
            ;;
    esac


    AC_LANG_PUSH(C++)
    AC_REQUIRE_CPP

    dyninst_saved_CPPFLAGS=$CPPFLAGS
    dyninst_saved_LDFLAGS=$LDFLAGS

    CPPFLAGS="$CPPFLAGS $DYNINST_CPPFLAGS"
    LDFLAGS="$CXXFLAGS $DYNINST_LDFLAGS $DYNINST_LIBS $BINUTILS_LDFLAGS -liberty $LIBDWARF_LDFLAGS $LIBDWARF_LIBS"

    AC_MSG_CHECKING([for Dyninst API library and headers])

    AC_LINK_IFELSE(AC_LANG_PROGRAM([[
	#include <BPatch.h>
        ]], [[
	BPatch bpatch();
        ]]), AC_MSG_RESULT(yes), [ AC_MSG_RESULT(no)
	# for offline only builds, dyninst is not installed.
	# do not die.
        #AC_MSG_FAILURE(cannot locate Dyninst API library and/or headers.) ]
    )

    CPPFLAGS=$dyninst_saved_CPPFLAGS
    LDFLAGS=$dyninst_saved_LDFLAGS

    AC_LANG_POP(C++)

    AC_SUBST(DYNINST_CPPFLAGS)
    AC_SUBST(DYNINST_LDFLAGS)
    AC_SUBST(DYNINST_LIBS)
    AC_SUBST(DYNINST_DIR)
    AC_SUBST(DYNINST_VERS)

    AC_DEFINE(HAVE_DYNINST, 1, [Define to 1 if you have Dyninst.])

])
