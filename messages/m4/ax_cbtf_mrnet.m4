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

AC_DEFUN([AX_CBTF_MRNET], [

    AC_ARG_WITH(cbtf-mrnet,
                AC_HELP_STRING([--with-cbtf-mrnet=DIR],
                               [CBTF MRNet library installation @<:@/usr@:>@]),
                cbtf_mrnet_dir=$withval, cbtf_mrnet_dir="/usr")

    CBTF_MRNET_CPPFLAGS="-I$cbtf_mrnet_dir/include"
    CBTF_MRNET_LDFLAGS="-L$cbtf_mrnet_dir/$abi_libdir"
    CBTF_MRNET_LIBS="$BOOST_SYSTEM_LIB $BOOST_FILESYSTEM_LIB $BOOST_THREAD_LIB $LIBXERCES_C $MRNET_LIBS -lcbtf -lcbtf-mrnet -lcbtf-xml"

    AC_LANG_PUSH(C++)
    AC_REQUIRE_CPP

    cbtf_mrnet_saved_CPPFLAGS=$CPPFLAGS
    cbtf_mrnet_saved_LDFLAGS=$LDFLAGS

    CPPFLAGS="$CPPFLAGS $CBTF_MRNET_CPPFLAGS $BOOST_CPPFLAGS $MRNET_CPPFLAGS"
    LDFLAGS="$CXXFLAGS $CBTF_MRNET_LDFLAGS $CBTF_MRNET_LIBS $BOOST_LDFLAGS $MRNET_LDFLAGS"

    AC_MSG_CHECKING([for CBTF MRNet library and headers])

    AC_LINK_IFELSE(AC_LANG_PROGRAM([[
        #include <KrellInstitute/CBTF/XDR.hpp>
        ]], [[
        ]]), [ 
            AC_MSG_RESULT(yes)
        ], [
            AC_MSG_RESULT(no)
            #AC_MSG_ERROR([CBTF MRNet library could not be found.])
        ])

    CPPFLAGS=$cbtf_mrnet_saved_CPPFLAGS
    LDFLAGS=$cbtf_mrnet_saved_LDFLAGS

    AC_LANG_POP(C++)

    AC_SUBST(CBTF_MRNET_CPPFLAGS)
    AC_SUBST(CBTF_MRNET_LDFLAGS)
    AC_SUBST(CBTF_MRNET_LIBS)

])

AC_DEFUN([AX_TARGET_CBTF_MRNET], [

    AC_ARG_WITH(target-cbtf-mrnet,
                AC_HELP_STRING([--with-target-cbtf-mrnet=DIR],
                               [CBTF MRNet library installation @<:@/usr@:>@]),
                target_cbtf_mrnet_dir=$withval, target_cbtf_mrnet_dir="/usr")

    TARGET_CBTF_MRNET_CPPFLAGS="-I$target_cbtf_mrnet_dir/include"
    TARGET_CBTF_MRNET_LDFLAGS="-L$target_cbtf_mrnet_dir/$abi_libdir"
    TARGET_CBTF_MRNET_LIBS="$TARGET_BOOST_SYSTEM_LIB $TARGET_BOOST_FILESYSTEM_LIB $TARGET_BOOST_THREAD_LIB $TARGET_LIBXERCES_C $TARGET_MRNET_LIBS -lcbtf -lcbtf-mrnet -lcbtf-xml"
    TARGET_CBTF_MRNET_DIR="$target_cbtf_mrnet_dir"

    AC_MSG_CHECKING([for Targetted CBTF MRNet support])

    found_target_cbtf=0
    if test -f $target_cbtf_mrnet_dir/$abi_libdir/libcbtf-mrnet.so -o -f $target_cbtf_mrnet_dir/$abi_libdir/libcbtf-mrnet.a ; then
       found_target_cbtf=1
       TARGET_CBTF_MRNET_LDFLAGS="-L$target_cbtf_mrnet_dir/$abi_libdir"
    elif test -f  $target_cbtf_mrnet_dir/$alt_abi_libdir/libcbtf-mrnet.so -o -f $target_cbtf_mrnet_dir/$alt_abi_libdir/libcbtf-mrnet.a ; then
       found_target_cbtf=1
       TARGET_CBTF_MRNET_LDFLAGS="-L$target_cbtf_mrnet_dir/$alt_abi_libdir"
    fi

    if test $found_target_cbtf == 0 && test "$target_cbtf_mrnet_dir" == "/zzz" ; then
      AM_CONDITIONAL(HAVE_TARGET_CBTF_MRNET, false)
      TARGET_CBTF_MRNET_CPPFLAGS=""
      TARGET_CBTF_MRNET_LDFLAGS=""
      TARGET_CBTF_MRNET_LIBS=""
      TARGET_CBTF_MRNET_DIR=""
      AC_MSG_RESULT(no)
    elif test $found_target_cbtf == 1 ; then
      AM_CONDITIONAL(HAVE_TARGET_CBTF_MRNET, true)
      AC_DEFINE(HAVE_TARGET_CBTF_MRNET, 1, [Define to 1 if you have a target version of CBTF.])
      AC_MSG_RESULT(yes)
    else
      AM_CONDITIONAL(HAVE_TARGET_CBTF_MRNET, false)
      TARGET_CBTF_MRNET_CPPFLAGS=""
      TARGET_CBTF_MRNET_LDFLAGS=""
      TARGET_CBTF_MRNET_LIBS=""
      TARGET_CBTF_MRNET_DIR=""
      AC_MSG_RESULT(no)
    fi

    AC_SUBST(TARGET_CBTF_MRNET_CPPFLAGS)
    AC_SUBST(TARGET_CBTF_MRNET_LDFLAGS)
    AC_SUBST(TARGET_CBTF_MRNET_LIBS)
    AC_SUBST(TARGET_CBTF_MRNET_DIR)

])



