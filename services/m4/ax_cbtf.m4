################################################################################
# Copyright (c) 2010-2013 Krell Institute. All Rights Reserved.
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

AC_DEFUN([AX_CBTF], [

    AC_ARG_WITH(cbtf,
                AC_HELP_STRING([--with-cbtf=DIR],
                               [CBTF library installation @<:@/usr@:>@]),
                cbtf_dir=$withval, cbtf_dir="/usr")

    CBTF_CPPFLAGS="-I$cbtf_dir/include"
    CBTF_LDFLAGS="-L$cbtf_dir/$abi_libdir"
    CBTF_LIBS="-lcbtf"

    AC_LANG_PUSH(C++)
    AC_REQUIRE_CPP

    cbtf_saved_CPPFLAGS=$CPPFLAGS
    cbtf_saved_LDFLAGS=$LDFLAGS
    cbtf_saved_LIBS=$LIBS

    CPPFLAGS="$CPPFLAGS $CBTF_CPPFLAGS"
    LDFLAGS="$CXXFLAGS $CBTF_LDFLAGS"
    LIBS="$CBTF_LIBS"

    AC_MSG_CHECKING([for CBTF library and headers])

    AC_LINK_IFELSE(AC_LANG_PROGRAM([[
        #include <KrellInstitute/CBTF/Type.hpp>
        #include <typeinfo>
        ]], [[
        KrellInstitute::CBTF::Type type(typeid(int));
        ]]), [ 
            AC_MSG_RESULT(yes)
        ], [
            AC_MSG_RESULT(no)
            #AC_MSG_ERROR([CBTF library could not be found.])
        ])

    CPPFLAGS=$cbtf_saved_CPPFLAGS
    LDFLAGS=$cbtf_saved_LDFLAGS
    LIBS=$cbtf_saved_LIBS

    AC_LANG_POP(C++)

    AC_SUBST(CBTF_CPPFLAGS)
    AC_SUBST(CBTF_LDFLAGS)
    AC_SUBST(CBTF_LIBS)

])



################################################################################
# Check for CBTF for Target Architecture 
################################################################################

AC_DEFUN([AX_TARGET_CBTF], [

    AC_ARG_WITH(target-cbtf,
                AC_HELP_STRING([--with-target-cbtf=DIR],
                               [CBTF library installation @<:@/opt@:>@]),
                target_cbtf_dir=$withval, target_cbtf_dir="/zzz")

    TARGET_CBTF_CPPFLAGS="-I$target_cbtf_dir/include"
    TARGET_CBTF_LDFLAGS="-L$target_cbtf_dir/$abi_libdir"
    TARGET_CBTF_LIBS="-lcbtf"
    TARGET_CBTF_DIR="$target_cbtf_dir"

    AC_MSG_CHECKING([for Targetted CBTF support])

    found_target_cbtf=0
    if test -f $target_cbtf_dir/$abi_libdir/libcbtf.so -o -f $target_cbtf_dir/$abi_libdir/libcbtf.a ; then
       found_target_cbtf=1
       TARGET_CBTF_LDFLAGS="-L$target_cbtf_dir/$abi_libdir"
    elif test -f  $target_cbtf_dir/$alt_abi_libdir/libcbtf.so -o -f $target_cbtf_dir/$alt_abi_libdir/libcbtf.a ; then
       found_target_cbtf=1
       TARGET_CBTF_LDFLAGS="-L$target_cbtf_dir/$alt_abi_libdir"
    fi

    if test $found_target_cbtf == 0 && test "$target_cbtf_dir" == "/zzz" ; then
      AM_CONDITIONAL(HAVE_TARGET_CBTF, false)
      TARGET_CBTF_CPPFLAGS=""
      TARGET_CBTF_LDFLAGS=""
      TARGET_CBTF_LIBS=""
      TARGET_CBTF_DIR=""
      AC_MSG_RESULT(no)
    elif test $found_target_cbtf == 1 ; then
      AM_CONDITIONAL(HAVE_TARGET_CBTF, true)
      AC_DEFINE(HAVE_TARGET_CBTF, 1, [Define to 1 if you have a target version of CBTF.])
      AC_MSG_RESULT(yes)
    else
      AM_CONDITIONAL(HAVE_TARGET_CBTF, false)
      TARGET_CBTF_CPPFLAGS=""
      TARGET_CBTF_LDFLAGS=""
      TARGET_CBTF_LIBS=""
      TARGET_CBTF_DIR=""
      AC_MSG_RESULT(no)
    fi

    AC_SUBST(TARGET_CBTF_CPPFLAGS)
    AC_SUBST(TARGET_CBTF_LDFLAGS)
    AC_SUBST(TARGET_CBTF_LIBS)
    AC_SUBST(TARGET_CBTF_DIR)

])



