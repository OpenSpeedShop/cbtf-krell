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

AC_DEFUN([AX_CORE], [

    AC_ARG_WITH(cbtf-core,
            AC_HELP_STRING([--with-cbtf-core=DIR],
                           [CBTF core library installation @<:@/usr@:>@]),
                core_dir=$withval, core_dir="/usr")

    CORE_CPPFLAGS="-I$core_dir/include"
    CORE_LDFLAGS="$DYNINST_LDFLAGS $LIBDWARF_LDFLAGS -L$core_dir/$abi_libdir"
    CORE_LIBS="$DYNINST_LIBS $LIBDWARF_LIBS -lcbtf-core"

    core_saved_CPPFLAGS=$CPPFLAGS
    core_saved_LDFLAGS=$LDFLAGS

    CPPFLAGS="$CPPFLAGS $CORE_CPPFLAGS -I/usr/include"
    LDFLAGS="$LDFLAGS $CORE_LDFLAGS $CORE_LIBS $BINUTILS_LDFLAGS $BINUTILS_BFD_LIB"

    AC_MSG_CHECKING([for CBTF CORE library and headers])

    AC_LANG_PUSH(C++)
    AC_LINK_IFELSE([AC_LANG_PROGRAM([[
        #include <KrellInstitute/Core/TotallyOrdered.hpp>
        ]], [[
        ]])], [ 
            AC_MSG_RESULT(yes)
        ], [
            AC_MSG_RESULT(no)
            #AC_MSG_ERROR([CBTF core library could not be found.])
        ])

    AC_LANG_POP
    CPPFLAGS=$core_saved_CPPFLAGS
    LDFLAGS=$core_saved_LDFLAGS

    AC_SUBST(CORE_CPPFLAGS)
    AC_SUBST(CORE_LDFLAGS)
    AC_SUBST(CORE_LIBS)

])



AC_DEFUN([AX_TARGET_CORE], [

    AC_ARG_WITH(target-cbtf-core,
            AC_HELP_STRING([--with-target-cbtf-core=DIR],
                           [Targeted CBTF core library installation @<:@/usr@:>@]),
                target_core_dir=$withval, target_core_dir="/usr")

    TARGET_CORE_CPPFLAGS="-I$target_core_dir/include"
    TARGET_CORE_LDFLAGS="$DYNINST_LDFLAGS $LIBDWARF_LDFLAGS -L$target_core_dir/$abi_libdir"
    TARGET_CORE_LIBS="$DYNINST_LIBS $LIBDWARF_LIBS -lcbtf-core"

    AC_MSG_CHECKING([for Targeted CBTF CORE library and headers])


    if test -f $target_core_dir/include/KrellInstitute/Core/TotallyOrdered.hpp; then

            AC_MSG_RESULT(yes)
    else
            AC_MSG_RESULT(no)
            #AC_MSG_ERROR([Targeted CBTF core library could not be found.])
    fi

    AC_SUBST(TARGET_CORE_CPPFLAGS)
    AC_SUBST(TARGET_CORE_LDFLAGS)
    AC_SUBST(TARGET_CORE_LIBS)

])
