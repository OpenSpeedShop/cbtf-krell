################################################################################
# Copyright (c) 2006-2015 Krell Institute. All Rights Reserved.
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

    AC_ARG_WITH([libmonitor-libdir],
                AS_HELP_STRING([--with-libmonitor-libdir=LIB_DIR],
                [Force given directory for libmonitor libraries. Note that this will overwrite library path detection, so use this parameter only if default library detection fails and you know exactly where your libmonitor libraries are located.]),
                [
                if test -d $withval
                then
                        ac_libmonitor_lib_path="$withval"
                else
                        AC_MSG_ERROR(--with-libmonitor-libdir expected directory name)
                fi ],
                [ac_libmonitor_lib_path=""])


    if test "x$ac_libmonitor_lib_path" == "x"; then
       LIBMONITOR_LDFLAGS="-L$libmonitor_dir/$abi_libdir"
       LIBMONITOR_LIBDIR="$libmonitor_dir/$abi_libdir"
    else
       LIBMONITOR_LDFLAGS="-L$ac_libmonitor_lib_path"
       LIBMONITOR_LIBDIR="$ac_libmonitor_lib_path"
    fi

    LIBMONITOR_CPPFLAGS="-I$libmonitor_dir/include"
    LIBMONITOR_DIR="$libmonitor_dir"
    LIBMONITOR_LIBS="-lmonitor"

    libmonitor_saved_CPPFLAGS=$CPPFLAGS
    libmonitor_saved_LDFLAGS=$LDFLAGS
    libmonitor_saved_LIBS=$LIBS

    CPPFLAGS="$CPPFLAGS $LIBMONITOR_CPPFLAGS"
    LDFLAGS="$LDFLAGS $LIBMONITOR_LDFLAGS"
    if test -f /usr/$abi_libdir/x86_64-linux-gnu/libpthread.a ; then
       LIBS="$LIBMONITOR_LIBS /usr/$abi_libdir/x86_64-linux-gnu/libpthread.a"
    elif test -f /usr/$alt_abi_libdir/x86_64-linux-gnu/libpthread.a ; then
       LIBS="$LIBMONITOR_LIBS /usr/$alt_abi_libdir/x86_64-linux-gnu/libpthread.a"
    else
       LIBS="$LIBMONITOR_LIBS -lpthread"
    fi

    AC_MSG_CHECKING([for libmonitor library and headers])

    AC_LINK_IFELSE([AC_LANG_PROGRAM([[
        #include <monitor.h>
        ]], [[
        monitor_init_library();
        ]])], [ AC_MSG_RESULT(yes)

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
