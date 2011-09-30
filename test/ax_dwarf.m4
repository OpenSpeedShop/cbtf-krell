################################################################################
# Check for libdwarf (http://www.reality.sgiweb.org/davea/dwarf.html)
################################################################################

AC_DEFUN([AX_LIBDWARF], [

    AC_ARG_WITH(libdwarf,
                AC_HELP_STRING([--with-libdwarf=DIR],
                               [libdwarf installation @<:@/usr@:>@]),
                libdwarf_dir=$withval, libdwarf_dir="/usr")

    found_libdwarf=0

    LIBDWARF_CPPFLAGS="-I$libdwarf_dir/$abi_libdir/libdwarf/include"
    LIBDWARF_LDFLAGS="-L$libdwarf_dir/$abi_libdir/libdwarf/$abi_libdir"
    LIBDWARF_LIBS="-ldwarf"

    libdwarf_saved_CPPFLAGS=$CPPFLAGS
    libdwarf_saved_LDFLAGS=$LDFLAGS

    CPPFLAGS="$CPPFLAGS $LIBDWARF_CPPFLAGS"
    LDFLAGS="$LDFLAGS $LIBDWARF_LDFLAGS $LIBDWARF_LIBS -lelf -lpthread"

    AC_MSG_CHECKING([for libdwarf library and headers])

    AC_LINK_IFELSE(AC_LANG_PROGRAM([[
        #include <dwarf.h>
        ]], [[
        if (DW_ID_up_case != DW_ID_down_case) {
           int mycase = DW_ID_up_case;
        }
        ]]), [ found_libdwarf=1 ], [ found_libdwarf=0 ])

    if test $found_libdwarf -eq 1; then
        AC_MSG_RESULT(yes)
        AM_CONDITIONAL(HAVE_LIBDWARF, true)
        AC_DEFINE(HAVE_LIBDWARF, 1, [Define to 1 if you have LIBDWARF.])
    else
# Try again with the traditional path instead
         found_libdwarf=0
         LIBDWARF_CPPFLAGS="-I$libdwarf_dir/include"
         LIBDWARF_LDFLAGS="-L$libdwarf_dir/$abi_libdir"

         CPPFLAGS="$CPPFLAGS $LIBDWARF_CPPFLAGS"
         LDFLAGS="$LDFLAGS $LIBDWARF_LDFLAGS $LIBDWARF_LIBS -lelf -lpthread"

         AC_MSG_CHECKING([for libdwarf library and headers])

         AC_LINK_IFELSE(AC_LANG_PROGRAM([[
             #include <dwarf.h>
             ]], [[
             if (DW_ID_up_case != DW_ID_down_case) {
                int mycase = DW_ID_up_case;
             }
             ]]), [ found_libdwarf=1

             ], [ found_libdwarf=0 ])

         if test $found_libdwarf -eq 1; then
             AC_MSG_RESULT(yes)
             AM_CONDITIONAL(HAVE_LIBDWARF, true)
             AC_DEFINE(HAVE_LIBDWARF, 1, [Define to 1 if you have LIBDWARF.])
         else
             AC_MSG_RESULT(no)
             AM_CONDITIONAL(HAVE_LIBDWARF, false)
             AC_DEFINE(HAVE_LIBDWARF, 0, [Define to 0 if you do not have LIBDWARF.])
             LIBDWARF_CPPFLAGS=""
             LIBDWARF_LDFLAGS=""
             LIBDWARF_LIBS=""
         fi
    fi

    CPPFLAGS=$libdwarf_saved_CPPFLAGS LDFLAGS=$libdwarf_saved_LDFLAGS

    AC_SUBST(LIBDWARF_CPPFLAGS)
    AC_SUBST(LIBDWARF_LDFLAGS)
    AC_SUBST(LIBDWARF_LIBS)

])
