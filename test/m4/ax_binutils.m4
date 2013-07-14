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
# Check for Binutils (http://www.gnu.org/software/binutils)
################################################################################

AC_DEFUN([AX_BINUTILS], [

    AC_ARG_WITH(binutils,
                AC_HELP_STRING([--with-binutils=DIR],
                               [binutils installation @<:@/usr@:>@]),
                binutils_dir=$withval, binutils_dir="/usr")


    vers_info_needed=1
    if test -f  $binutils_dir/$abi_libdir/libbfd.so && test -x $binutils_dir/$abi_libdir/libbfd.so; then
		vers_info_needed=0
    fi

    AC_ARG_WITH(binutils-version,
                AC_HELP_STRING([--with-binutils-version=VERS],
                               [binutils-version installation @<:@@:>@]),
                binutils_vers=-$withval, binutils_vers="")

    if test x"$binutils_vers" == x"" && test $vers_info_needed == 1; then
	binutils_vers="help"
        binutils_vers="`ls $binutils_dir/$abi_libdir/libbfd*.so | cut -d/ -f 4 | cut -c7-99 | sed 's/.so//'`"
    fi

# Chose the pic version over the normal version - binutils-2.20 divides the main library into pic and non-pic
    if test -f $binutils_dir/$abi_libdir/libiberty_pic.a; then
       BINUTILS_IBERTY_LIB="-liberty_pic"
       BINUTILS_IBERTY_LDFLAGS="-L$binutils_dir/$abi_libdir"
       binutils_iberty_lib="-liberty_pic"
    elif test -f $binutils_dir/$abi_libdir/libiberty.a; then
       BINUTILS_IBERTY_LIB="-liberty"
       BINUTILS_IBERTY_LDFLAGS="-L$binutils_dir/$abi_libdir"
       binutils_iberty_lib="-liberty"
    elif test -f $binutils_dir/$alt_abi_libdir/libiberty_pic.a; then
       BINUTILS_IBERTY_LIB="-liberty_pic"
       BINUTILS_IBERTY_LDFLAGS="-L$binutils_dir/$alt_abi_libdir"
       binutils_iberty_lib="-liberty_pic"
    elif test -f $binutils_dir/$alt_abi_libdir/libiberty.a; then
       BINUTILS_IBERTY_LIB="-liberty"
       BINUTILS_IBERTY_LDFLAGS="-L$binutils_dir/$alt_abi_libdir"
       binutils_iberty_lib="-liberty"
    else
       BINUTILS_IBERTY_LIB=
       BINUTILS_IBERTY_LDFLAGS=
       binutils_iberty_lib=
    fi

    case "$host" in
	ia64-*-linux*)
	    binutils_required="true"
            BINUTILS_DIR="$binutils_dir"
	    BINUTILS_CPPFLAGS="-I$binutils_dir/include"
	    BINUTILS_LDFLAGS="-L$binutils_dir/$abi_libdir"
	    BINUTILS_LIBS="-lopcodes$binutils_vers -lbfd$binutils_vers $binutils_iberty_lib"
	    BINUTILS_OPCODES_LIB="-lopcodes$binutils_vers"
	    BINUTILS_BFD_LIB="-lbfd$binutils_vers"
            ;;
	x86_64-*-linux*)
	    binutils_required="true"
            BINUTILS_DIR="$binutils_dir"
	    BINUTILS_CPPFLAGS="-I$binutils_dir/include"
	    BINUTILS_LDFLAGS="-L$binutils_dir/$abi_libdir"
	    BINUTILS_LIBS="-lopcodes$binutils_vers -lbfd$binutils_vers $binutils_iberty_lib"
	    BINUTILS_OPCODES_LIB="-lopcodes$binutils_vers"
	    BINUTILS_BFD_LIB="-lbfd$binutils_vers"
            ;;
	*)
	    binutils_required="true"
            BINUTILS_DIR="$binutils_dir"
	    BINUTILS_CPPFLAGS="-I$binutils_dir/include"
	    BINUTILS_LDFLAGS="-L$binutils_dir/$abi_libdir"
	    BINUTILS_LIBS="-lopcodes$binutils_vers -lbfd$binutils_vers $binutils_iberty_lib"
	    BINUTILS_OPCODES_LIB="-lopcodes$binutils_vers"
	    BINUTILS_BFD_LIB="-lbfd$binutils_vers"
            ;;
    esac

    binutils_saved_CPPFLAGS=$CPPFLAGS
    binutils_saved_LDFLAGS=$LDFLAGS
    binutils_saved_LIBS=$LIBS

    if test "x$BINUTILS_DIR" == "x/usr"; then
	BINUTILS_IBERTY_LDFLAGS=""
	BINUTILS_LDFLAGS=""
	BINUTILS_CPPFLAGS=""
    fi

    CPPFLAGS="$CPPFLAGS $BINUTILS_CPPFLAGS"
    LDFLAGS="$LDFLAGS $BINUTILS_LDFLAGS"
    LIBS="$BINUTILS_LIBS"

    AC_MSG_CHECKING([for binutils librarys and headers])

    AC_SEARCH_LIBS(bfd_init, [bfd$binutils_vers], 
        [ AC_MSG_RESULT(yes)

            AM_CONDITIONAL(HAVE_BINUTILS, true)
            AC_DEFINE(HAVE_BINUTILS, 1, [Define to 1 if you have BINUTILS.])

        ], [ AC_MSG_RESULT(no)

	    if test x"$binutils_required" == x"true"; then
		AM_CONDITIONAL(HAVE_BINUTILS, false)
	    else
		AM_CONDITIONAL(HAVE_BINUTILS, true)
	    fi
            BINUTILS_CPPFLAGS=""
            BINUTILS_LDFLAGS=""
            BINUTILS_LIBS=""
	    BINUTILS_OPCODES_LIB=""
	    BINUTILS_BFD_LIB=""
            BINUTILS_DIR=""
            BINUTILS_IBERTY_LIB=""
            BINUTILS_IBERTY_LDFLAGS=""

        ]
    )

    CPPFLAGS=$binutils_saved_CPPFLAGS
    LDFLAGS=$binutils_saved_LDFLAGS
    LIBS=$binutils_saved_LIBS


    AC_SUBST(BINUTILS_CPPFLAGS)
    AC_SUBST(BINUTILS_LDFLAGS)
    AC_SUBST(BINUTILS_LIBS)
    AC_SUBST(BINUTILS_OPCODES_LIB)
    AC_SUBST(BINUTILS_BFD_LIB)
    AC_SUBST(BINUTILS_DIR)
    AC_SUBST(BINUTILS_IBERTY_LIB)
    AC_SUBST(BINUTILS_IBERTY_LDFLAGS)

])
