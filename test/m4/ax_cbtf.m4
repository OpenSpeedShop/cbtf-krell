################################################################################
# Copyright (c) 2010 Krell Institute. All Rights Reserved.
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

    CPPFLAGS="$CPPFLAGS $CBTF_CPPFLAGS $BOOST_CPPFLAGS"
    LDFLAGS="$CXXFLAGS $CBTF_LDFLAGS $CBTF_LIBS"

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
            AC_MSG_ERROR([CBTF library could not be found.])
        ])

    CPPFLAGS=$cbtf_saved_CPPFLAGS
    LDFLAGS=$cbtf_saved_LDFLAGS

    AC_LANG_POP(C++)

    AC_SUBST(CBTF_CPPFLAGS)
    AC_SUBST(CBTF_LDFLAGS)
    AC_SUBST(CBTF_LIBS)

])
