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

AC_INIT([core], [1.0])
AC_COPYRIGHT([[Copyright (c) 2010-2013 Krell Institute. All Rights Reserved.]])

AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE
AM_CONFIG_HEADER([config.h])

AH_TOP([
/*******************************************************************************
** Copyright (c) 2010-2013 Krell Institute. All Rights Reserved.
**
** This library is free software; you can redistribute it and/or modify it under
** the terms of the GNU Lesser General Public License as published by the Free
** Software Foundation; either version 2.1 of the License, or (at your option)
** any later version.
**
** This library is distributed in the hope that it will be useful, but WITHOUT
** ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
** FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
** details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this library; if not, write to the Free Software Foundation, Inc.,
** 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*******************************************************************************/
])

AC_GNU_SOURCE
AC_PROG_CC
AC_PROG_CXX
AC_PROG_LIBTOOL
AC_PROG_INSTALL

LT_CONFIG_LTDL_DIR([libltdl])
LT_INIT([dlopen])
LTDL_INIT([recursive])

AC_PATH_PROG([RPCGEN], rpcgen)
AC_PROG_LN_S

################################################################################
# Handle PowerPC and X86-64 Special Cases
################################################################################
AC_ARG_WITH(ppc64_bitmode,
            AC_HELP_STRING([--with-ppc64-bitmode=<32,64>],
                           [Specify ppc64 library bit mode @<:@64@:>@]),
            ppc64_bitmode=$withval, ppc64_bitmode="64")

AC_MSG_CHECKING([Configuring for host = $host  with ppc64_bitmode = $ppc64_bitmode])

case "$host" in
    powerpc-*-linux*)
        AC_MSG_CHECKING([Configuring for powerpc-star-linux])

        abi_libdir="lib"
        alt_abi_libdir="lib64"
        if test x"$libdir" == x'${exec_prefix}/lib64'; then
            libdir='${exec_prefix}/lib'
        fi
        LDFLAGS="-DLIB_DIR=lib $LDFLAGS"
        ;;
    powerpc64-*-*)

        AC_MSG_CHECKING([Configuring for powerpc64-star-linux with ppc64_bitmode = $ppc64_bitmode])
        if test "$ppc64_bitmode" = "64" ; then
                AC_MSG_CHECKING([Configuring for 64 bit powerpc64-star-linux host])
                abi_libdir="lib64"
                alt_abi_libdir="lib"
                if test x"$libdir" == x'${exec_prefix}/lib'; then
                    libdir='${exec_prefix}/lib64'
                fi
                LDFLAGS="-DLIB_DIR=lib64 $LDFLAGS"
                CFLAGS="-m64 $CFLAGS"
                CXXFLAGS="-m64 $CXXFLAGS"
                CPPFLAGS="-m64 $CPPFLAGS"
        elif test "$ppc64_bitmode" = "32" ; then
                AC_MSG_CHECKING([Configuring for 32 bit powerpc64-star-linux host])
                abi_libdir="lib"
                alt_abi_libdir="lib64"
                if test x"$libdir" == x'${exec_prefix}/lib64'; then
                    libdir='${exec_prefix}/lib'
                fi
                LDFLAGS="-DLIB_DIR=lib $LDFLAGS"
        fi
        ;;

    x86_64-*-linux*)
        AC_MSG_CHECKING([Configuring for x86_64--star-linux host])
        if test -d /usr/lib/x86_64-linux-gnu ; then
          if test x"$libdir" == x'${exec_prefix}/lib64'; then
            libdir='${exec_prefix}/lib'
          fi
          abi_libdir="lib"
          alt_abi_libdir="lib64"
          LDFLAGS="-DLIB_DIR=lib $LDFLAGS"
        else
          if test x"$libdir" == x'${exec_prefix}/lib'; then
            libdir='${exec_prefix}/lib64'
          fi
          abi_libdir="lib64"
          alt_abi_libdir="lib"
          LDFLAGS="-DLIB_DIR=lib64 $LDFLAGS"
        fi
        ;;

    *)
        AC_MSG_CHECKING([Configuring for default host])
        abi_libdir="lib"
        alt_abi_libdir="lib64"
        LDFLAGS="-DLIB_DIR=lib $LDFLAGS"
        ;;
esac
topdir_execprefix=${exec_prefix}
AC_SUBST(topdir_execprefix)

abi_libdir_name=${abi_libdir}
AC_SUBST(abi_libdir_name)
alt_abi_libdir_name=${alt_abi_libdir}
AC_SUBST(alt_abi_libdir_name)

cbtf_platform=`uname -i`
cbtf_hardware_platform=$cbtf_platform
AC_MSG_CHECKING([Checking for cbtf_hardware_platform=$cbtf_hardware_platform])
AC_SUBST(cbtf_hardware_platform)


################################################################################
# Check for POSIX Threads (ISO/IEC 9945-1:1996)
################################################################################

AC_CHECK_HEADER(pthread.h, [ AC_CHECK_LIB(pthread, pthread_create, [
    AC_DEFINE(HAVE_POSIX_THREADS, 1, [Define to 1 if you have POSIX threads.])
], [
    AC_MSG_FAILURE([cannot locate POSIX thread library and/or headers.])
]) ])

################################################################################
# Check for TLS option, default to implicit
################################################################################

AC_ARG_WITH(tls,
            AC_HELP_STRING([--with-tls=TYPE],
                           [tls (implicit, explicit) @<:@implicit@:>@]),
            TLS=$withval, TLS="implicit")

case "$TLS" in
    implicit)
        AM_CONDITIONAL(USE_EXPLICIT_TLS, false)
        ;;

    explicit)
        AM_CONDITIONAL(USE_EXPLICIT_TLS, true)
        AC_DEFINE(USE_EXPLICIT_TLS, 1, [Define to 1 if you want to use an explicit TLS implementation instead of the __thread keyword.])
        ;;

    *)
        AC_MSG_ERROR([invalid tls specification "$TLS".])
        ;;
esac

################################################################################
# Begin Target O/S Specification
################################################################################

target_os=$host
AC_MSG_CHECKING(for Target O/S)
AC_ARG_WITH(target-os,
    AC_HELP_STRING([--with-target-os=TARGET_OS],
                   [target os specification @<:@$host@:>@]),
                    TARGET_OS=$withval, TARGET_OS="$host")


AM_CONDITIONAL(HAVE_TARGET_STATIC, false)
AM_CONDITIONAL(BUILD_TARGETED, false)

build_targeted=0
case "$TARGET_OS" in
    cray-xk)
	AC_DEFINE(TARGET_OS_CRAYXK, 1, [Do not define if not building for targeted cray-xk.])
	AC_DEFINE_UNQUOTED(TARGET_OS,cray-xk,
			   [Name targeted OS])
	;; 
    cray-xe)
	AC_DEFINE(TARGET_OS_CRAYXE, 1, [Do not define if not building for targeted cray-xe.])
	AC_DEFINE_UNQUOTED(TARGET_OS,cray-xe,
			   [Name targeted OS])
	;; 
    cray-xt5)
	AC_DEFINE(TARGET_OS_CRAYXT5, 1, [Do not define if not building for targeted cray-xt5.])
	AC_DEFINE_UNQUOTED(TARGET_OS,cray-xt5,
			   [Name targeted OS])
	;; 
    cray-xt4)
	AC_DEFINE(TARGET_OS_CRAYXT4, 1, [Do not define if not building for targeted cray-xt4.])
	AC_DEFINE_UNQUOTED(TARGET_OS,cray-xt4,
			   [Name targeted OS])
	;; 
    bgl)
	AM_CONDITIONAL(HAVE_TARGET_STATIC, true)
	AC_DEFINE([HAVE_TARGET_STATIC], [1],[Define to 1 targeted static builds.])
	AM_CONDITIONAL(BUILD_TARGETED, true)
	AC_DEFINE(BUILD_TARGETED, 1, [Define to 0 if not building a targeted runtime.])
	AC_DEFINE(TARGET_OS_BGL, 1, [Do not define if not building for targeted bgl.])
	AC_DEFINE_UNQUOTED(TARGET_OS,bgl,
			   [Name targeted OS])
	build_targeted=1
	;; 
    bgp)
	AM_CONDITIONAL(HAVE_TARGET_STATIC, true)
	AC_DEFINE([HAVE_TARGET_STATIC], [1],[Define to 1 targeted static builds.])
	AM_CONDITIONAL(BUILD_TARGETED, true)
	AC_DEFINE(BUILD_TARGETED, 1, [Define to 0 if not building a targeted runtime.])
	AC_DEFINE(TARGET_OS_BGP, 1, [Do not define if not building for targeted bgp.])
	AC_DEFINE_UNQUOTED(TARGET_OS,bgp,
			   [Name targeted OS])
	build_targeted=1
	;; 
    bgq)
	AM_CONDITIONAL(HAVE_TARGET_STATIC, true)
	AC_DEFINE([HAVE_TARGET_STATIC], [1],[Define to 1 targeted static builds.])
        AM_CONDITIONAL(BUILD_TARGETED, true)
        AC_DEFINE(BUILD_TARGETED, 1, [Define to 0 if not building a targeted runtime.])
        AC_DEFINE(TARGET_OS_BGQ, 1, [Do not define if not building for targeted bgq.])
        AC_DEFINE_UNQUOTED(TARGET_OS,bgq,
                           [Name targeted OS])
	build_targeted=1
        ;;
    *)
	AM_CONDITIONAL(HAVE_TARGET_STATIC, false)
	AC_DEFINE([HAVE_TARGET_STATIC], [0],[Define to 0 for targeted non-static builds.])
	AM_CONDITIONAL(BUILD_TARGETED, false)
	AC_DEFINE(BUILD_TARGETED, 0, [Define to 1 if building a targeted runtime.])
	AC_DEFINE_UNQUOTED(TARGET_OS,$host,
			   [Name targeted OS])
	build_targeted=0
	;; 
esac

AC_SUBST(BUILD_TARGETED)
AC_SUBST(TARGET_OS)
################################################################################
# End Target O/S Specification
################################################################################

################################################################################
# build runtime collector software only
################################################################################
#
# Option: 
#
AC_MSG_CHECKING(for building runtime collectors only)
AC_ARG_ENABLE([runtime-only],
    [AS_HELP_STRING([--enable-runtime-only],
        [Enable support building just the runtime collectors with no client [default=no]])],
    [], [enable_runtime_only=no])

if test "x$enable_runtime_only" = xyes ; then
    AM_CONDITIONAL(BUILD_RUNTIME, true)
    AC_DEFINE(BUILD_RUNTIME, 1, [Define to 1 if you do want only collector runtime support built.])
else
    AM_CONDITIONAL(BUILD_RUNTIME, false)
    AC_DEFINE(BUILD_RUNTIME, 0, [Define to 1 if you do not want only collector runtime support built.])
fi
AC_SUBST(BUILD_RUNTIME)


################################################################################
# Check for Additional Packages
################################################################################

AX_BINUTILS()
AX_LIBMONITOR()
AX_LIBUNWIND()
AX_PAPI()

AX_ALPS()

AX_BOOST_BASE([1.35.0])
AX_BOOST_SYSTEM()
AX_BOOST_THREAD()
AX_BOOST_DATE_TIME()
AX_BOOST_FILESYSTEM()
AX_BOOST_PROGRAM_OPTIONS()

AX_XERCESC()
# This helps with targeted builds when there is no xercesc
if test "$LIBXERCES_C" = "-lxerces-c" ; then
  AX_XERCESC_VERSION()
fi

AX_MRNET()
AX_CBTF()
AX_CBTF_XML()
AX_CBTF_MRNET()
AX_LIBELF()
AX_LIBDWARF()
AX_DYNINST()
AX_PKG_MPI()

AX_MESSAGES()
AX_CBTF_SERVICES()
AX_LIBIOMP()

AC_CONFIG_FILES([
    Makefile
    include/Makefile
    libltdl/Makefile
    collectionTool/Makefile
    collectors/Makefile
    collectors/pcsamp/Makefile
    collectors/usertime/Makefile
    collectors/io/Makefile
    collectors/hwc/Makefile
    collectors/hwcsamp/Makefile
    collectors/hwctime/Makefile
    collectors/mem/Makefile
    collectors/mpi/Makefile
    collectors/pthreads/Makefile
    components/Makefile
    daemonTool/Makefile
    scripts/Makefile
    src/Makefile
])

AC_OUTPUT
