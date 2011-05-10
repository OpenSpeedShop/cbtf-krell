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

AC_DEFUN([AX_MRNET], [

    AC_ARG_WITH(mrnet,
                AC_HELP_STRING([--with-mrnet=DIR],
                               [MRNet installation @<:@/usr@:>@]),
                mrnet_dir=$withval, mrnet_dir="/usr")

    MRNET_CPPFLAGS="-I$mrnet_dir/include -Dos_linux"
    MRNET_LDFLAGS="-L$mrnet_dir/$abi_libdir"
    MRNET_LIBS="-Wl,--whole-archive -lmrnet -lxplat -Wl,--no-whole-archive"
    MRNET_LIBS="$MRNET_LIBS -lpthread -ldl"

    AC_LANG_PUSH(C++)
    AC_REQUIRE_CPP

    mrnet_saved_CPPFLAGS=$CPPFLAGS
    mrnet_saved_LDFLAGS=$LDFLAGS

    CPPFLAGS="$CPPFLAGS $MRNET_CPPFLAGS $BOOST_CPPFLAGS"
    LDFLAGS="$CXXFLAGS $MRNET_LDFLAGS $MRNET_LIBS"

    AC_MSG_CHECKING([for MRNet library and headers])

    AC_LINK_IFELSE(AC_LANG_PROGRAM([[
        #include <mrnet/MRNet.h>
        ]], [[
        MRN::set_OutputLevel(0);
        ]]), [ 
            AC_MSG_RESULT(yes)
        ], [
            AC_MSG_RESULT(no)
            AC_MSG_ERROR([MRNet could not be found.])
        ])

    CPPFLAGS=$mrnet_saved_CPPFLAGS
    LDFLAGS=$mrnet_saved_LDFLAGS

    AC_LANG_POP(C++)

    AC_SUBST(MRNET_CPPFLAGS)
    AC_SUBST(MRNET_LDFLAGS)
    AC_SUBST(MRNET_LIBS)

])
