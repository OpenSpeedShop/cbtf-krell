################################################################################
## Copyright (c) 2011-2013 Krell Institute. All Rights Reserved.
##
## This program is free software; you can redistribute it and/or modify it under
## the terms of the GNU General Public License as published by the Free Software
## Foundation; either version 2 of the License, or (at your option) any later
## version.
##
## This program is distributed in the hope that it will be useful, but WITHOUT
## ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
## FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
## details.
##
## You should have received a copy of the GNU General Public License along with
## this program; if not, write to the Free Software Foundation, Inc., 59 Temple
## Place, Suite 330, Boston, MA  02111-1307  USA
#################################################################################

AC_DEFUN([AX_XERCESC], [

    AC_ARG_WITH(libxerces-c-prefix,
                AC_HELP_STRING([--with-libxerces-c-prefix=DIR],
                               [XERCES installation @<:@/usr@:>@]),
                xerces_dir=$withval, xerces_dir="/usr")


    LIBXERCES_C_DIR="$xerces_dir"
    LIBXERCES_C_LIBSDIR="$xerces_dir/$abi_libdir"
    if test "x$xerces_dir" != "x/usr"; then
        LIBXERCES_C_CPPFLAGS="-I$xerces_dir/include"
        LIBXERCES_C_LDFLAGS="-L$xerces_dir/$abi_libdir"
    else
        LIBXERCES_C_CPPFLAGS=""
        LIBXERCES_C_LDFLAGS=""
    fi
    LIBXERCES_C="-lxerces-c"
    LTLIBXERCES_C="-lxerces-c"

    AC_LANG_PUSH(C++)
    AC_REQUIRE_CPP

    xerces_saved_CPPFLAGS=$CPPFLAGS
    xerces_saved_LDFLAGS=$LDFLAGS

    CPPFLAGS="$CPPFLAGS $LIBXERCES_C_CPPFLAGS"
    LDFLAGS="$LDFLAGS $LIBXERCES_C_LDFLAGS $LIBXERCES_C"

    AC_MSG_CHECKING([for XERCES support])

    foundXERCES=0
    if test -f  $xerces_dir/$abi_libdir/libxerces-c.so; then
       AC_MSG_CHECKING([found xerces library])
       foundXERCES=1
    else 
	if test -f $xerces_dir/$alt_abi_libdir/libxerces-c.so; then
          AC_MSG_CHECKING([found xerces library])
          LIBXERCES_C_LIBSDIR="$xerces_dir/$alt_abi_libdir"
          LIBXERCES_C_LDFLAGS="-L$xerces_dir/$alt_abi_libdir"
          foundXERCES=1
          AC_MSG_CHECKING([found xerces lib (so)])
       else
          foundXERCES=0
          AC_MSG_CHECKING([did not find xerces lib (so)])
       fi
    fi

    if test $foundXERCES == 1 && test -f  $xerces_dir/include/xercesc/util/XMLString.hpp; then
       AC_MSG_CHECKING([found xerces headers])
       foundXERCES=1
    else
       foundXERCES=0
       AC_MSG_CHECKING([did not fine xerces headers])
    fi

    if test $foundXERCES == 1; then
      AC_MSG_CHECKING([found all xerces headers, libraries and supporting libz library])
      AC_MSG_RESULT(yes)
      AM_CONDITIONAL(HAVE_XERCES, true)
      AC_DEFINE(HAVE_XERCES, 1, [Define to 1 if you have XERCES.])
    else
      AC_MSG_RESULT(no)
      AM_CONDITIONAL(HAVE_XERCES, false)
      LIBXERCES_C_DIR=""
      LIBXERCES_C_LIBSDIR=""
      LIBXERCES_C_CPPFLAGS=""
      LIBXERCES_C_LDFLAGS=""
      LIBXERCES_C=""
      LTLIBXERCES_C=""
    fi


    CPPFLAGS=$xerces_saved_CPPFLAGS
    LDFLAGS=$xerces_saved_LDFLAGS

    AC_LANG_POP(C++)

    AC_SUBST(LIBXERCES_C_DIR)
    AC_SUBST(LIBXERCES_C_LIBSDIR)
    AC_SUBST(LIBXERCES_C_CPPFLAGS)
    AC_SUBST(LIBXERCES_C_LDFLAGS)
    AC_SUBST(LIBXERCES_C)
    AC_SUBST(LTLIBXERCES_C)

])

