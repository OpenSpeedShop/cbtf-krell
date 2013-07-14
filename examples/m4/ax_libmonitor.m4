################################################################################
# Copyright (c) 2006-2013 Krell Institute. All Rights Reserved.
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
# Check for Monitor (http://www.cs.utk.edu/~mucci)
################################################################################

AC_DEFUN([AX_LIBMONITOR], [

    AC_ARG_WITH(libmonitor,
                AC_HELP_STRING([--with-libmonitor=DIR],
                               [libmonitor installation @<:@/usr@:>@]),
                libmonitor_dir=$withval, libmonitor_dir="/usr")

    LIBMONITOR_CPPFLAGS="-I$libmonitor_dir/include"
    LIBMONITOR_LDFLAGS="-L$libmonitor_dir/$abi_libdir"
    LIBMONITOR_LIBS="-lmonitor"
    LIBMONITOR_DIR="$libmonitor_dir"
    LIBMONITOR_LIBDIR="$libmonitor_dir/$abi_libdir"

    libmonitor_saved_CPPFLAGS=$CPPFLAGS
    libmonitor_saved_LDFLAGS=$LDFLAGS
    libmonitor_saved_LIBS=$LIBS

    CPPFLAGS="$CPPFLAGS $LIBMONITOR_CPPFLAGS"
    LDFLAGS="$LDFLAGS $LIBMONITOR_LDFLAGS"
    LIBS="$LIBMONITOR_LIBS -lpthread"

    AC_MSG_CHECKING([for libmonitor library and headers])

    AC_LINK_IFELSE(AC_LANG_PROGRAM([[
        #include <monitor.h>
        ]], [[
        monitor_init_library();
        ]]), [ AC_MSG_RESULT(yes)

            AM_CONDITIONAL(HAVE_LIBMONITOR, true)
            AC_DEFINE(HAVE_LIBMONITOR, 1, [Define to 1 if you have libmonitor.])

        ], [ AC_MSG_RESULT(no)

            AM_CONDITIONAL(HAVE_LIBMONITOR, false)
            LIBMONITOR_CPPFLAGS=""
            LIBMONITOR_LDFLAGS=""
            LIBMONITOR_LIBS=""
            LIBMONITOR_DIR=""

        ]
    )

    CPPFLAGS=$libmonitor_saved_CPPFLAGS
    LDFLAGS=$libmonitor_saved_LDFLAGS
    LIBS=$libmonitor_saved_LIBS

    AC_SUBST(LIBMONITOR_CPPFLAGS)
    AC_SUBST(LIBMONITOR_LDFLAGS)
    AC_SUBST(LIBMONITOR_LIBS)
    AC_SUBST(LIBMONITOR_DIR)
    AC_SUBST(LIBMONITOR_LIBDIR)

])
