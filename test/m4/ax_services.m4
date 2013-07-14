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
# Check for EPYDOC (http://epydoc.sourceforge.net/)   
################################################################################

AC_DEFUN([AC_PKG_EPYDOC], [

	AC_MSG_CHECKING([for epydoc binary])
	if epydoc --version >/dev/null 2>/dev/null ; then
      		AC_MSG_CHECKING([found epydoc binary])
		      AC_MSG_RESULT(yes)
		      AM_CONDITIONAL(HAVE_EPYDOC, true)
		      AC_DEFINE(HAVE_EPYDOC, 1, [Define to 1 if you have EPYDOC.])
	else
                AM_CONDITIONAL(HAVE_EPYDOC, false)
	fi
])

################################################################################
# Check for CBTF SERVICES 
################################################################################

AC_DEFUN([AX_CBTF_SERVICES], [

    AC_ARG_WITH(cbtf-services,
            AC_HELP_STRING([--with-cbtf-services=DIR],
                           [CBTF services library installation @<:@/usr@:>@]),
                services_dir=$withval, services_dir="/usr"
		)

    CBTF_SERVICES_CPPFLAGS="-I$services_dir/include"
    CBTF_SERVICES_LDFLAGS="-L$services_dir/$abi_libdir"
    CBTF_SERVICES_BINUTILS_LIBS="-lcbtf-services-binutils"
    CBTF_SERVICES_COMMON_LIBS="-lcbtf-services-common"
    CBTF_SERVICES_DATA_LIBS="-lcbtf-services-data"
    CBTF_SERVICES_FILEIO_LIBS="-lcbtf-services-fileio"
    CBTF_SERVICES_FPE_LIBS="-lcbtf-services-fpe"
    CBTF_SERVICES_MONITOR_LIBS="-lcbtf-services-monitor"
    CBTF_SERVICES_MRNET_LIBS="-lcbtf-services-mrnet"
    CBTF_SERVICES_OFFLINE_LIBS="-lcbtf-services-offline"
    CBTF_SERVICES_PAPI_LIBS="-lcbtf-services-papi"
    CBTF_SERVICES_SEND_LIBS="-lcbtf-services-send"
    CBTF_SERVICES_TIMER_LIBS="-lcbtf-services-timer"
    CBTF_SERVICES_UNWIND_LIBS="-lcbtf-services-unwind"


    CBTF_SERVICES_LIBS="$CBTF_SERVICES_BINUTILS_LIBS $CBTF_SERVICES_COMMON_LIBS $CBTF_SERVICES_DATA_LIBS $CBTF_SERVICES_FILEIO_LIBS $CBTF_SERVICES_FPE_LIBS $CBTF_SERVICES_MONITOR_LIBS $CBTF_SERVICES_MRNET_LIBS $CBTF_SERVICES_OFFLINE_LIBS $CBTF_SERVICES_PAPI_LIBS $CBTF_SERVICES_SEND_LIBS $CBTF_SERVICES_TIMER_LIBS $CBTF_SERVICES_UNWIND_LIBS"

    services_saved_CPPFLAGS=$CPPFLAGS
    services_saved_LDFLAGS=$LDFLAGS
    services_saved_LIBS=$LIBS

    CPPFLAGS="$CPPFLAGS $CBTF_SERVICES_CPPFLAGS"
    LDFLAGS="$LDFLAGS $CBTF_SERVICES_LDFLAGS"
    LIBS="$CBTF_SERVICES_COMMON_LIBS -lrt -lpthread"

    AC_MSG_CHECKING([for CBTF SERVICES library and headers])

    AC_SEARCH_LIBS(CBTF_GetPCFromContext, cbtf-services-common[], 
        [ AC_MSG_RESULT(yes)

        ], [ AC_MSG_RESULT(no)
            #AC_MSG_ERROR([CBTF services library could not be found.])
        ]
    )

    CPPFLAGS=$services_saved_CPPFLAGS
    LDFLAGS=$services_saved_LDFLAGS
    LIBS=$services_saved_LIBS

    AC_SUBST(CBTF_SERVICES_CPPFLAGS)
    AC_SUBST(CBTF_SERVICES_LDFLAGS)
    AC_SUBST(CBTF_SERVICES_LIBS)
    AC_SUBST(CBTF_SERVICES_BINUTILS_LIBS)
    AC_SUBST(CBTF_SERVICES_COMMON_LIBS)
    AC_SUBST(CBTF_SERVICES_DATA_LIBS)
    AC_SUBST(CBTF_SERVICES_FILEIO_LIBS)
    AC_SUBST(CBTF_SERVICES_FPE_LIBS)
    AC_SUBST(CBTF_SERVICES_MONITOR_LIBS)
    AC_SUBST(CBTF_SERVICES_MRNET_LIBS)
    AC_SUBST(CBTF_SERVICES_PAPI_LIBS)
    AC_SUBST(CBTF_SERVICES_SEND_LIBS)
    AC_SUBST(CBTF_SERVICES_TIMER_LIBS)
    AC_SUBST(CBTF_SERVICES_UNWIND_LIBS)

])

