# ===========================================================================
#  http://www.nongnu.org/autoconf-archive/ax_boost_unit_test_framework.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_BOOST_UNIT_TEST_FRAMEWORK
#
# DESCRIPTION
#
#   Test for Unit_Test_Framework library from the Boost C++ libraries. The
#   macro requires a preceding call to AX_BOOST_BASE. Further documentation
#   is available at <http://randspringer.de/boost/index.html>.
#
#   This macro calls:
#
#     AC_SUBST(BOOST_UNIT_TEST_FRAMEWORK_LIB)
#
#   And sets:
#
#     HAVE_BOOST_UNIT_TEST_FRAMEWORK
#
# LICENSE
#
#   Copyright (c) 2008 Thomas Porschberg <thomas@randspringer.de>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 8

AC_DEFUN([AX_BOOST_UNIT_TEST_FRAMEWORK],
[
	AC_ARG_WITH([boost-unit-test-framework],
	AS_HELP_STRING([--with-boost-unit-test-framework@<:@=special-lib@:>@],
                   [use the Unit_Test_Framework library from boost - it is possible to specify a certain library for the linker
                        e.g. --with-boost-unit-test-framework=boost_unit_test_framework-gcc ]),
        [
        if test "$withval" = "no"; then
			want_boost="no"
        elif test "$withval" = "yes"; then
            want_boost="yes"
            ax_boost_user_unit_test_framework_lib=""
        else
		    want_boost="yes"
        	ax_boost_user_unit_test_framework_lib="$withval"
		fi
        ],
        [want_boost="yes"]
	)

	if test "x$want_boost" = "xyes"; then
        AC_REQUIRE([AC_PROG_CC])
		CPPFLAGS_SAVED="$CPPFLAGS"
		CPPFLAGS="$CPPFLAGS $BOOST_CPPFLAGS"
		export CPPFLAGS

		LDFLAGS_SAVED="$LDFLAGS"
		LDFLAGS="$LDFLAGS $BOOST_LDFLAGS"
		export LDFLAGS

        AC_CACHE_CHECK(whether the Boost::Unit_Test_Framework library is available,
					   ax_cv_boost_unit_test_framework,
        [AC_LANG_PUSH([C++])
			 AC_COMPILE_IFELSE(AC_LANG_PROGRAM([[@%:@include <boost/test/unit_test.hpp>]],
                                    [[using boost::unit_test::test_suite;
					                 test_suite* test= BOOST_TEST_SUITE( "Unit test example 1" ); return 0;]]),
                   ax_cv_boost_unit_test_framework=yes, ax_cv_boost_unit_test_framework=no)
         AC_LANG_POP([C++])
		])
		if test "x$ax_cv_boost_unit_test_framework" = "xyes"; then
			AC_DEFINE(HAVE_BOOST_UNIT_TEST_FRAMEWORK,,[define if the Boost::Unit_Test_Framework library is available])
            BOOSTLIBDIR=`echo $BOOST_LDFLAGS | sed -e 's/@<:@^\/@:>@*//'`

            if test "x$ax_boost_user_unit_test_framework_lib" = "x"; then
         		saved_ldflags="${LDFLAGS}"
                for monitor_library in `ls $BOOSTLIBDIR/libboost_unit_test_framework*.{so,a}* 2>/dev/null` ; do
                    if test -r $monitor_library ; then
                       libextension=`echo $monitor_library | sed 's,.*/,,' | sed -e 's;^lib\(boost_unit_test_framework.*\)\.so.*$;\1;' -e 's;^lib\(boost_unit_test_framework.*\)\.a*$;\1;'`
                       ax_lib=${libextension}
                       link_unit_test_framework="yes"
                    else
                       link_unit_test_framework="no"
                    fi

        		    if test "x$link_unit_test_framework" = "xyes"; then
                      BOOST_UNIT_TEST_FRAMEWORK_LIB="-l$ax_lib"
                      AC_SUBST(BOOST_UNIT_TEST_FRAMEWORK_LIB)
					  break
				    fi
                done
                if test "x$link_unit_test_framework" != "xyes"; then
                for libextension in `ls $BOOSTLIBDIR/boost_unit_test_framework*.{dll,a}* 2>/dev/null  | sed 's,.*/,,' | sed -e 's;^\(boost_unit_test_framework.*\)\.dll.*$;\1;' -e 's;^\(boost_unit_test_framework.*\)\.a*$;\1;'` ; do
                     ax_lib=${libextension}
				    AC_CHECK_LIB($ax_lib, exit,
                                 [BOOST_UNIT_TEST_FRAMEWORK_LIB="-l$ax_lib"; AC_SUBST(BOOST_UNIT_TEST_FRAMEWORK_LIB) link_unit_test_framework="yes"; break],
                                 [link_unit_test_framework="no"])
  				done
                fi
            else
                link_unit_test_framework="no"
         		saved_ldflags="${LDFLAGS}"
                for ax_lib in boost_unit_test_framework-$ax_boost_user_unit_test_framework_lib $ax_boost_user_unit_test_framework_lib ; do
                   if test "x$link_unit_test_framework" = "xyes"; then
                      break;
                   fi
                   for unittest_library in `ls $BOOSTLIBDIR/lib${ax_lib}.{so,a}* 2>/dev/null` ; do
                   if test -r $unittest_library ; then
                       libextension=`echo $unittest_library | sed 's,.*/,,' | sed -e 's;^lib\(boost_unit_test_framework.*\)\.so.*$;\1;' -e 's;^lib\(boost_unit_test_framework.*\)\.a*$;\1;'`
                       ax_lib=${libextension}
                       link_unit_test_framework="yes"
                    else
                       link_unit_test_framework="no"
                    fi

			        if test "x$link_unit_test_framework" = "xyes"; then
                        BOOST_UNIT_TEST_FRAMEWORK_LIB="-l$ax_lib"
                        AC_SUBST(BOOST_UNIT_TEST_FRAMEWORK_LIB)
					    break
				    fi
                  done
               done
            fi
			if test "x$link_unit_test_framework" != "xyes"; then
				AC_MSG_ERROR(Could not link against $ax_lib !)
			fi
		fi

		CPPFLAGS="$CPPFLAGS_SAVED"
    	LDFLAGS="$LDFLAGS_SAVED"
	fi
])

#############################################################################################
# Check for Boost Unit Test Framework for Target Architecture 
#############################################################################################

AC_DEFUN([AC_PKG_TARGET_BOOST_UNIT_TEST_FRAMEWORK], [

    AC_ARG_WITH(target-boost-unit-test-framework,
                AC_HELP_STRING([--with-target-boost-unit-test-framework=DIR],
                               [Boost unit framework target architecture installation @<:@/opt@:>@]),
                target_boost_unit_test_framework_dir=$withval, target_boost_unit_test_framework_dir="/zzz")

    AC_MSG_CHECKING([for Targetted Boost Unit Framework support])

    found_target_boost_unit_test_framework=0
    if test -f $target_boost_unit_test_framework_dir/$abi_libdir/libboost_unit_test_framework.so -o -f $target_boost_unit_test_framework_dir/$abi_libdir/libboost_unit_test_framework.a; then
       found_target_boost_unit_test_framework=1
       TARGET_BOOST_UNIT_TEST_FRAMEWORK_LDFLAGS="-L$target_boost_unit_test_framework_dir/$abi_libdir"
       TARGET_BOOST_UNIT_TEST_FRAMEWORK_LIB="$target_boost_unit_test_framework_dir/$abi_libdir"
    elif test -f $target_boost_unit_test_framework_dir/$alt_abi_libdir/libboost_unit_test_framework.so -o -f $target_boost_unit_test_framework_dir/$alt_abi_libdir/libboost_unit_test_framework.a; then
       found_target_boost_unit_test_framework=1
       TARGET_BOOST_UNIT_TEST_FRAMEWORK_LDFLAGS="-L$target_boost_unit_test_framework_dir/$alt_abi_libdir"
       TARGET_BOOST_UNIT_TEST_FRAMEWORK_LIB="$target_boost_unit_test_framework_dir/$abi_libdir"
    fi

    if test $found_target_boost_unit_test_framework == 0 && test "$target_boost_unit_test_framework_dir" == "/zzz" ; then
      AM_CONDITIONAL(HAVE_TARGET_BOOST_UNIT_TEST_FRAMEWORK, false)
      TARGET_BOOST_UNIT_TEST_FRAMEWORK_CPPFLAGS=""
      TARGET_BOOST_UNIT_TEST_FRAMEWORK_LDFLAGS=""
      TARGET_BOOST_UNIT_TEST_FRAMEWORK_LIBS=""
      TARGET_BOOST_UNIT_TEST_FRAMEWORK_LIB=""
      TARGET_BOOST_UNIT_TEST_FRAMEWORK_DIR=""
      AC_MSG_RESULT(no)
    elif test $found_target_boost_unit_test_framework == 1 ; then
      AC_MSG_RESULT(yes)
      AM_CONDITIONAL(HAVE_TARGET_BOOST_UNIT_TEST_FRAMEWORK, true)
      AC_DEFINE(HAVE_TARGET_BOOST_UNIT_TEST_FRAMEWORK, 1, [Define to 1 if you have a target version of BOOST_UNIT_TEST_FRAMEWORK.])
      TARGET_BOOST_UNIT_TEST_FRAMEWORK_CPPFLAGS="-I$target_boost_unit_test_framework_dir/include/boost"
      TARGET_BOOST_UNIT_TEST_FRAMEWORK_LIBS="-lboost_unit_test_framework"
      TARGET_BOOST_UNIT_TEST_FRAMEWORK_LIB="-lboost_unit_test_framework"
      TARGET_BOOST_DIR="$target_boost_unit_test_framework_dir"
    else 
      AM_CONDITIONAL(HAVE_TARGET_BOOST_UNIT_TEST_FRAMEWORK, false)
      TARGET_BOOST_UNIT_TEST_FRAMEWORK_CPPFLAGS=""
      TARGET_BOOST_UNIT_TEST_FRAMEWORK_LDFLAGS=""
      TARGET_BOOST_UNIT_TEST_FRAMEWORK_LIBS=""
      TARGET_BOOST_UNIT_TEST_FRAMEWORK_LIB=""
      TARGET_BOOST_UNIT_TEST_FRAMEWORK_DIR=""
      AC_MSG_RESULT(no)
    fi

    AC_SUBST(TARGET_BOOST_UNIT_TEST_FRAMEWORK_LIB)
    AC_SUBST(TARGET_BOOST_UNIT_TEST_FRAMEWORK_CPPFLAGS)
    AC_SUBST(TARGET_BOOST_UNIT_TEST_FRAMEWORK_LDFLAGS)
    AC_SUBST(TARGET_BOOST_UNIT_TEST_FRAMEWORK_LIBS)
    AC_SUBST(TARGET_BOOST_UNIT_TEST_FRAMEWORK_DIR)

])

