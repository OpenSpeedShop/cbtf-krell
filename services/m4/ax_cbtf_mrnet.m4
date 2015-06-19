################################################################################
# Copyright (c) 2011-2013 Krell Institute. All Rights Reserved.
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

    AC_LANG_PUSH(C++)
    AC_REQUIRE_CPP

    cbtf_mrnet_saved_CPPFLAGS=$CPPFLAGS

    CPPFLAGS="$CPPFLAGS $CBTF_CPPFLAGS $CBTF_MRNET_CPPFLAGS $MRNET_CPPFLAGS"

    AC_MSG_CHECKING([for CBTF MRNet header KrellInstitute/CBTF/Impl/MessageTags.h])

    if test -f $cbtf_mrnet_dir/include/KrellInstitute/CBTF/Impl/MessageTags.h ; then
      AC_MSG_RESULT(yes)
    else
      AC_MSG_RESULT(no)
    fi


    CPPFLAGS=$cbtf_mrnet_saved_CPPFLAGS

    AC_LANG_POP(C++)

    AC_SUBST(CBTF_MRNET_CPPFLAGS)
])
