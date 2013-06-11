#################################################################################
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
#################################################################################

################################################################################
# Check for alps  library and include files
################################################################################

AC_DEFUN([AX_ALPS], [

    AC_ARG_WITH(alps,
                AC_HELP_STRING([--with-alps=DIR],
                               [alps installation @<:@/usr@:>@]),
                alps_dir=$withval, alps_dir="/usr")

    AC_ARG_WITH([alps-libdir],
                AS_HELP_STRING([--with-alps-libdir=LIB_DIR],
                [Force given directory for alps libraries. Note that this will overwrite library path detection, so use this parameter only if default library detection fails and you know exactly where your alps libraries are located.]),
                [
                if test -d $withval 
                then
                        ac_alps_lib_path="$withval"
                else
                        AC_MSG_ERROR(--with-alps-libdir expected directory name)
                fi ], 
                [ac_alps_lib_path=""])


    ALPS_CPPFLAGS="-I$alps_dir/include"

    if test "x$ac_alps_lib_path" == "x"; then
       ALPS_LDFLAGS="-L$alps_dir/lib/alps"
    else
       ALPS_LDFLAGS="-L$ac_alps_lib_path"
    fi


    found_alps=0

    ALPS_LIBS="-lalps -lalpslli -lalpsutil"

    alps_saved_CPPFLAGS=$CPPFLAGS
    alps_saved_LDFLAGS=$LDFLAGS
    alps_saved_LIBS=$LIBS

    CPPFLAGS="$CPPFLAGS $ALPS_CPPFLAGS"
    LDFLAGS="$LDFLAGS $ALPS_LDFLAGS $LIBELF_LDFLAGS"
    LIBS="$ALPS_LIBS $LIBELF_LIBS -lpthread"

    AC_MSG_CHECKING([for alps library and headers])
    AC_LANG_PUSH(C++)

    AC_LINK_IFELSE(AC_LANG_PROGRAM([[
        #include <alps/alps.h>
        ]], [[
        if ( ALPS_XT_NID ) {
        }
        ]]), [ found_alps=1 ], [ found_alps=0 ])

    if test $found_alps -eq 1; then
        AC_MSG_RESULT(yes)
        AM_CONDITIONAL(HAVE_ALPS, true)
        AC_DEFINE(HAVE_ALPS, 1, [Define to 1 if you have ALPS.])
    else
        AC_MSG_RESULT(no)
        AM_CONDITIONAL(HAVE_ALPS, false)
        AC_DEFINE(HAVE_ALPS, 0, [Define to 0 if you do not have ALPS.])
        ALPS_CPPFLAGS=""
        ALPS_LDFLAGS=""
        ALPS_LIBS=""
    fi

    CPPFLAGS=$alps_saved_CPPFLAGS 
    LDFLAGS=$alps_saved_LDFLAGS
    LIBS=$alps_saved_LIBS

    AC_LANG_POP(C++)

    AC_SUBST(ALPS_CPPFLAGS)
    AC_SUBST(ALPS_LDFLAGS)
    AC_SUBST(ALPS_LIBS)

])
