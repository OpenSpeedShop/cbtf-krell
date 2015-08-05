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
# Check for Libunwind (http://www.hpl.hp.com/research/linux/libunwind)
################################################################################

AC_DEFUN([AX_LIBUNWIND], [

    AC_ARG_WITH(libunwind,
                AC_HELP_STRING([--with-libunwind=DIR],
                               [libunwind installation @<:@/usr@:>@]),
                libunwind_dir=$withval, libunwind_dir="/usr")

    AC_ARG_WITH([libunwind-libdir],
                AS_HELP_STRING([--with-libunwind-libdir=LIB_DIR],
                [Force given directory for libunwind libraries. Note that this will overwrite library path detection, so use this parameter only if default library detection fails and you know exactly where your libunwind libraries are located.]),
                [
                if test -d $withval
                then
                        ac_libunwind_lib_path="$withval"
                else
                        AC_MSG_ERROR(--with-libunwind-libdir expected directory name)
                fi ],
                [ac_libunwind_lib_path=""])


    if test "x$ac_libunwind_lib_path" == "x"; then
       LIBUNWIND_LDFLAGS="-L$libunwind_dir/$abi_libdir"
       LIBUNWIND_LIBDIR="$libunwind_dir/$abi_libdir"
    else
       LIBUNWIND_LDFLAGS="-L$ac_libunwind_lib_path"
       LIBUNWIND_LIBDIR="$ac_libunwind_lib_path"
    fi

    LIBUNWIND_CPPFLAGS="-I$libunwind_dir/include -DUNW_LOCAL_ONLY"
    LIBUNWIND_LIBS="-lunwind"
    LIBUNWIND_DIR="$libunwind_dir"

    libunwind_saved_CPPFLAGS=$CPPFLAGS
    libunwind_saved_LDFLAGS=$LDFLAGS
    libunwind_saved_LIBS=$LIBS

    CPPFLAGS="$CPPFLAGS $LIBUNWIND_CPPFLAGS"
    LDFLAGS="$LDFLAGS $LIBUNWIND_LDFLAGS"
    LIBS="$LIBUNWIND_LIBS"

    AC_MSG_CHECKING([for libunwind library and headers])

    AC_LINK_IFELSE([AC_LANG_PROGRAM([[
        #include <libunwind.h>
        ]], [[
        unw_init_local((void*)0, (void*)0);
        ]])], [ AC_MSG_RESULT(yes)

            AM_CONDITIONAL(HAVE_LIBUNWIND, true)
            AC_DEFINE(HAVE_LIBUNWIND, 1, [Define to 1 if you have libunwind.])

        ], [ AC_MSG_RESULT(no)

            AM_CONDITIONAL(HAVE_LIBUNWIND, false)
            LIBUNWIND_CPPFLAGS=""
            LIBUNWIND_LDFLAGS=""
            LIBUNWIND_LIBS=""
            LIBUNWIND_DIR=""
            LIBUNWIND_LIBDIR=""

        ]
    )

    CPPFLAGS=$libunwind_saved_CPPFLAGS
    LDFLAGS=$libunwind_saved_LDFLAGS
    LIBS=$libunwind_saved_LIBS

    AC_SUBST(LIBUNWIND_CPPFLAGS)
    AC_SUBST(LIBUNWIND_LDFLAGS)
    AC_SUBST(LIBUNWIND_LIBS)
    AC_SUBST(LIBUNWIND_DIR)
    AC_SUBST(LIBUNWIND_LIBDIR)

])

