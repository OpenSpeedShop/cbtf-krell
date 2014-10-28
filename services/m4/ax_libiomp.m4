#############################################################################################
# Check for libiomp
#############################################################################################

AC_DEFUN([AX_LIBIOMP], [

    AC_ARG_WITH(libiomp,
                AC_HELP_STRING([--with-libiomp=DIR],
                               [libiomp installation @<:@/opt@:>@]),
                libiomp_dir=$withval, libiomp_dir="/zzz")

    AC_MSG_CHECKING([for libiomp support])

    found_libiomp=0
    found_ompt_includes=0
    if test -f $libiomp_dir/$abi_libdir/libiomp5.a || test -f $libiomp_dir/$abi_libdir/libiomp5.so; then
       found_libiomp=1
       LIBIOMP_LDFLAGS="-L$libiomp_dir/$abi_libdir"
    elif test -f $libiomp_dir/$alt_abi_libdir/libiomp5.a || test -f $libiomp_dir/$alt_abi_libdir/libiomp5.so; then
       found_libiomp=1
       LIBIOMP_LDFLAGS="-L$libiomp_dir/$alt_abi_libdir"
    fi

    if test -f $libiomp_dir/include/ompt.h; then
	found_ompt_includes=1
	AM_CONDITIONAL(HAVE_OMPT, true)
    else
	AM_CONDITIONAL(HAVE_OMPT, false)
    fi

    if test $found_libiomp == 0 && test "$libiomp_dir" == "/zzz" ; then
      AM_CONDITIONAL(HAVE_LIBIOMP, false)
      LIBIOMP_CPPFLAGS=""
      LIBIOMP_LDFLAGS=""
      LIBIOMP_LIBS=""
      LIBIOMP_DIR=""
      AC_MSG_RESULT(no)
    elif test $found_libiomp == 1 ; then
      AM_CONDITIONAL(HAVE_LIBIOMP, true)
      AC_DEFINE(HAVE_LIBIOMP, 1, [Define to 1 if you have LIBIOMP.])
      LIBIOMP_CPPFLAGS="-I$libiomp_dir/include"
      LIBIOMP_LIBS="-liomp5"
      LIBIOMP_DIR="$libiomp_dir"
      AC_MSG_RESULT(yes)
    else
      AM_CONDITIONAL(HAVE_LIBIOMP, false)
      LIBIOMP_CPPFLAGS=""
      LIBIOMP_LDFLAGS=""
      LIBIOMP_LIBS=""
      LIBIOMP_DIR=""
      AC_MSG_RESULT(no)
    fi


    AC_SUBST(LIBIOMP_CPPFLAGS)
    AC_SUBST(LIBIOMP_LDFLAGS)
    AC_SUBST(LIBIOMP_LIBS)
    AC_SUBST(LIBIOMP_DIR)

])



