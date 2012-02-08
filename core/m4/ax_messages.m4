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

AC_DEFUN([AX_MESSAGES], [

    AC_ARG_WITH(cbtf-messages,
            AC_HELP_STRING([--with-cbtf-messages=DIR],
                           [CBTF message library installation @<:@/usr@:>@]),
                messages_dir=$withval, messages_dir="/usr")

    MESSAGES_CPPFLAGS="-I$messages_dir/include"
    MESSAGES_LDFLAGS="-L$messages_dir/$abi_libdir"
    MESSAGES_LIBS="-lcbtf-messages-base -lcbtf-messages-collector -lcbtf-messages-events -lcbtf-messages-instrumentation -lcbtf-messages-perfdata -lcbtf-messages-symtab -lcbtf-messages-thread"
    MESSAGES_BASE_LIBS="-lcbtf-messages-base"
    MESSAGES_COLLECTOR_LIBS="-lcbtf-messages-collector"
    MESSAGES_EVENTS_LIBS="-lcbtf-messages-events"
    MESSAGES_INSTRUMENTATION_LIBS="-lcbtf-messages-instrumentation"
    MESSAGES_PERFDATA_LIBS="-lcbtf-messages-perfdata"
    MESSAGES_SYMTAB_LIBS="-lcbtf-messages-symtab"
    MESSAGES_THREAD_LIBS="-lcbtf-messages-thread"

    messages_saved_CPPFLAGS=$CPPFLAGS
    messages_saved_LDFLAGS=$LDFLAGS

    CPPFLAGS="$CPPFLAGS $MESSAGES_CPPFLAGS"
    LDFLAGS="$LDFLAGS $MESSAGES_LDFLAGS $MESSAGES_BASE_LIBS"

    AC_MSG_CHECKING([for CBTF MESSAGES library and headers])

    AC_LINK_IFELSE(AC_LANG_PROGRAM([[
        #include <KrellInstitute/Messages/Address.h>
        ]], [[
        ]]), [ 
            AC_MSG_RESULT(yes)
        ], [
            AC_MSG_RESULT(no)
            #AC_MSG_ERROR([CBTF messages library could not be found.])
        ])

    CPPFLAGS=$messages_saved_CPPFLAGS
    LDFLAGS=$messages_saved_LDFLAGS

    AC_SUBST(MESSAGES_CPPFLAGS)
    AC_SUBST(MESSAGES_LDFLAGS)
    AC_SUBST(MESSAGES_LIBS)
    AC_SUBST(MESSAGES_BASE_LIBS)
    AC_SUBST(MESSAGES_COLLECTOR_LIBS)
    AC_SUBST(MESSAGES_EVENTS_LIBS)
    AC_SUBST(MESSAGES_INSTRUMENTATION_LIBS)
    AC_SUBST(MESSAGES_PERFDATA_LIBS)
    AC_SUBST(MESSAGES_THREAD_LIBS)

])


AC_DEFUN([AX_TARGET_MESSAGES], [

    AC_ARG_WITH(target-cbtf-messages,
            AC_HELP_STRING([--with-target-cbtf-messages=DIR],
                           [CBTF target message library installation @<:@/opt@:>@]),
                target_cbtf_messages_dir=$withval, target_cbtf_messages_dir="/zzz")

    TARGET_MESSAGES_CPPFLAGS="-I$target_cbtf_messages_dir/include"
    TARGET_MESSAGES_LDFLAGS="-L$target_cbtf_messages_dir/$abi_libdir"
    TARGET_MESSAGES_LIBS="-lcbtf-messages-base -lcbtf-messages-collector -lcbtf-messages-events -lcbtf-messages-instrumentation -lcbtf-messages-perfdata -lcbtf-messages-symtab -lcbtf-messages-thread"
    TARGET_MESSAGES_BASE_LIBS="-lcbtf-messages-base"
    TARGET_MESSAGES_COLLECTOR_LIBS="-lcbtf-messages-collector"
    TARGET_MESSAGES_EVENTS_LIBS="-lcbtf-messages-events"
    TARGET_MESSAGES_INSTRUMENTATION_LIBS="-lcbtf-messages-instrumentation"
    TARGET_MESSAGES_PERFDATA_LIBS="-lcbtf-messages-perfdata"
    TARGET_MESSAGES_SYMTAB_LIBS="-lcbtf-messages-symtab"
    TARGET_MESSAGES_THREAD_LIBS="-lcbtf-messages-thread"

    target_messages_saved_CPPFLAGS=$CPPFLAGS
    target_messages_saved_LDFLAGS=$LDFLAGS

    CPPFLAGS="$TARGET_MESSAGES_CPPFLAGS"
    LDFLAGS="$TARGET_MESSAGES_LDFLAGS $TARGET_MESSAGES_BASE_LIBS"

    AC_MSG_CHECKING([for CBTF TARGET_MESSAGES library and headers])

    if [ test -f $target_cbtf_messages_dir/include/KrellInstitute/Messages/Address.h ]; then
            AC_MSG_RESULT(yes)
    else
            AC_MSG_RESULT(no)
#            AC_MSG_ERROR([CBTF target messages library could not be found.])
    fi
        ])

    CPPFLAGS=$target_messages_saved_CPPFLAGS
    LDFLAGS=$target_messages_saved_LDFLAGS

    AC_SUBST(TARGET_MESSAGES_CPPFLAGS)
    AC_SUBST(TARGET_MESSAGES_LDFLAGS)
    AC_SUBST(TARGET_MESSAGES_LIBS)
    AC_SUBST(TARGET_MESSAGES_BASE_LIBS)
    AC_SUBST(TARGET_MESSAGES_COLLECTOR_LIBS)
    AC_SUBST(TARGET_MESSAGES_EVENTS_LIBS)
    AC_SUBST(TARGET_MESSAGES_INSTRUMENTATION_LIBS)
    AC_SUBST(TARGET_MESSAGES_PERFDATA_LIBS)
    AC_SUBST(TARGET_MESSAGES_THREAD_LIBS)

])



