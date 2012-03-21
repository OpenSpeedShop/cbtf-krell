################################################################################
# Copyright (c) 2010-2012 Krell Institute. All Rights Reserved.
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

AC_DEFUN([AX_LW_MRNET], [

    foundMRNET=0
    AC_ARG_WITH(mrnet,
                AC_HELP_STRING([--with-mrnet=DIR],
                               [MRNet installation @<:@/usr@:>@]),
                mrnet_dir=$withval, mrnet_dir="/usr")

    MRNET_CPPFLAGS="-I$mrnet_dir/include -Dos_linux"
    MRNET_LDFLAGS="-L$mrnet_dir/$abi_libdir"
    MRNET_LW_LIBS="-Wl,--whole-archive -lmrnet_lightweight -lxplat_lightweight -Wl,--no-whole-archive"
    MRNET_LW_LIBS="$MRNET_LW_LIBS -lpthread -ldl"
    MRNET_DIR="$mrnet_dir"

    AC_REQUIRE_CPP

    mrnet_saved_CPPFLAGS=$CPPFLAGS
    mrnet_saved_LDFLAGS=$LDFLAGS

    CPPFLAGS="$CPPFLAGS $MRNET_CPPFLAGS"
    LDFLAGS="$CXXFLAGS $MRNET_LDFLAGS $MRNET_LIBS"

    AC_MSG_CHECKING([for Lightweight MRNet library and headers])

    AC_LINK_IFELSE(AC_LANG_PROGRAM([[
        #include <mrnet_lightweight/MRNet.h>
        ]], [[
        ]]), [ 
            AC_MSG_RESULT(yes)

	    foundMRNET=1

        ], [
            AC_MSG_RESULT(no)
            #AC_MSG_ERROR([LW MRNet could not be found.])

	    foundMRNET=0

        ])

    CPPFLAGS=$mrnet_saved_CPPFLAGS
    LDFLAGS=$mrnet_saved_LDFLAGS

    AC_SUBST(MRNET_CPPFLAGS)
    AC_SUBST(MRNET_LDFLAGS)
    AC_SUBST(MRNET_LW_LIBS)
    AC_SUBST(MRNET_DIR)

    if test $foundMRNET == 1; then
        AM_CONDITIONAL(HAVE_MRNET, true)
        AC_DEFINE(HAVE_MRNET, 1, [Define to 1 if you have MRNet.])
    else
        AM_CONDITIONAL(HAVE_MRNET, false)
    fi

])

AC_DEFUN([AX_TARGET_LW_MRNET], [

    foundMRNET=0
    AC_ARG_WITH(target-mrnet,
                AC_HELP_STRING([--with-target-mrnet=DIR],
                               [MRNet installation @<:@/opt@:>@]),
                target_mrnet_dir=$withval, target_mrnet_dir="/zzz")

    if test -f $target_mrnet_dir/$abi_libdir/mrnet_config.h ; then
         TARGET_MRNET_CPPFLAGS="-I$target_mrnet_dir/include -I$target_mrnet_dir/$abi_libdir -Dos_linux"
    elif test -f $target_mrnet_dir/$alt_abi_libdir/mrnet_config.h ; then
         TARGET_MRNET_CPPFLAGS="-I$target_mrnet_dir/include -I$target_mrnet_dir/$alt_abi_libdir -Dos_linux"
    else
         TARGET_MRNET_CPPFLAGS="-I$target_mrnet_dir/include -Dos_linux"
    fi

    TARGET_MRNET_LDFLAGS="-L$target_mrnet_dir/$abi_libdir"
    TARGET_MRNET_LW_LIBS="-Wl,--whole-archive -lmrnet_lightweight -lxplat_lightweight -Wl,--no-whole-archive"
    TARGET_MRNET_LW_LIBS="$TARGET_MRNET_LW_LIBS -lpthread -ldl"
    TARGET_MRNET_DIR="$target_mrnet_dir"

    AC_REQUIRE_CPP

    target_mrnet_saved_CPPFLAGS=$CPPFLAGS
    target_mrnet_saved_LDFLAGS=$LDFLAGS

    CPPFLAGS="$CPPFLAGS $TARGET_MRNET_CPPFLAGS"
    LDFLAGS="$CXXFLAGS $TARGET_MRNET_LDFLAGS $TARGET_MRNET_LIBS"

    AC_MSG_CHECKING([for Lightweight MRNet targeted library and headers])

    AC_LINK_IFELSE(AC_LANG_PROGRAM([[
        #include <mrnet_lightweight/MRNet.h>
        ]], [[
        ]]), [ 
            AC_MSG_RESULT(yes)

	    foundMRNET=1

        ], [
            AC_MSG_RESULT(no)
            #AC_MSG_ERROR([LW MRNet could not be found.])

	    foundMRNET=0

        ])

    CPPFLAGS=$target_mrnet_saved_CPPFLAGS
    LDFLAGS=$target_mrnet_saved_LDFLAGS

    AC_SUBST(TARGET_MRNET_CPPFLAGS)
    AC_SUBST(TARGET_MRNET_LDFLAGS)
    AC_SUBST(TARGET_MRNET_LW_LIBS)
    AC_SUBST(TARGET_MRNET_DIR)

    if test $foundMRNET == 1; then
        AM_CONDITIONAL(HAVE_TARGET_MRNET, true)
        AC_DEFINE(HAVE_TARGET_MRNET, 1, [Define to 1 if you have MRNet.])
    else
        AM_CONDITIONAL(HAVE_TARGET_MRNET, false)
    fi

])



