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

AC_DEFUN([AX_CBTF_XML], [

    AC_ARG_WITH(cbtf-xml,
                AC_HELP_STRING([--with-cbtf-xml=DIR],
                               [CBTF XML library installation @<:@/usr@:>@]),
                cbtf_xml_dir=$withval, cbtf_xml_dir="/usr")

    CBTF_XML_CPPFLAGS="-I$cbtf_xml_dir/include"
    CBTF_XML_LDFLAGS="-L$cbtf_xml_dir/$abi_libdir"
    CBTF_XML_LIBS="-lcbtf-xml"

    AC_LANG_PUSH(C++)
    AC_REQUIRE_CPP

    cbtf_xml_saved_CPPFLAGS=$CPPFLAGS
    cbtf_xml_saved_LDFLAGS=$LDFLAGS
    cbtf_xml_saved_LIBS=$LIBS

    CPPFLAGS="$CPPFLAGS $CBTF_XML_CPPFLAGS"
    LDFLAGS="$CXXFLAGS $CBTF_XML_LDFLAGS"
    LIBS="$CBTF_XML_LIBS $BOOST_FILESYSTEM_LIB $BOOST_SYSTEM_LIB"

    AC_MSG_CHECKING([for CBTF XML library and headers])

    AC_LINK_IFELSE(AC_LANG_PROGRAM([[
        #include <KrellInstitute/CBTF/XML.hpp>
        ]], [[
        KrellInstitute::CBTF::registerXML("");
        ]]), [ 
            AC_MSG_RESULT(yes)
        ], [
            AC_MSG_RESULT(no)
            #AC_MSG_ERROR([CBTF XML library could not be found.])
        ])

    CPPFLAGS=$cbtf_xml_saved_CPPFLAGS
    LDFLAGS=$cbtf_xml_saved_LDFLAGS
    LIBS=$cbtf_xml_saved_LIBS

    AC_LANG_POP(C++)

    AC_SUBST(CBTF_XML_CPPFLAGS)
    AC_SUBST(CBTF_XML_LDFLAGS)
    AC_SUBST(CBTF_XML_LIBS)

])


AC_DEFUN([AX_TARGET_CBTF_XML], [

    AC_ARG_WITH(target-cbtf-xml,
                AC_HELP_STRING([--with-target-cbtf-xml=DIR],
                               [CBTF XML library installation @<:@/usr@:>@]),
                target_cbtf_xml_dir=$withval, target_cbtf_xml_dir="/usr")

    TARGET_CBTF_XML_CPPFLAGS="-I$target_cbtf_xml_dir/include"
    TARGET_CBTF_XML_LDFLAGS="-L$target_cbtf_xml_dir/$abi_libdir"
    TARGET_CBTF_XML_LIBS="$TARGET_BOOST_SYSTEM_LIB $TARGET_BOOST_FILESYSTEM_LIB $TARGET_BOOST_THREAD_LIB $TARGET_LIBXERCES_C $TARGET_XML_LIBS -lcbtf -lcbtf-xml"
    TARGET_CBTF_XML_DIR="$target_cbtf_xml_dir"

    AC_MSG_CHECKING([for Targetted CBTF XML support])

    found_target_cbtf=0
    if test -f $target_cbtf_xml_dir/$abi_libdir/libcbtf-xml.so -o -f $target_cbtf_xml_dir/$abi_libdir/libcbtf-xml.a ; then
       found_target_cbtf=1
       TARGET_CBTF_XML_LDFLAGS="-L$target_cbtf_xml_dir/$abi_libdir"
    elif test -f  $target_cbtf_xml_dir/$alt_abi_libdir/libcbtf-xml.so -o -f $target_cbtf_xml_dir/$alt_abi_libdir/libcbtf-xml.a ; then
       found_target_cbtf=1
       TARGET_CBTF_XML_LDFLAGS="-L$target_cbtf_xml_dir/$alt_abi_libdir"
    fi

    if test $found_target_cbtf == 0 && test "$target_cbtf_xml_dir" == "/zzz" ; then
      AM_CONDITIONAL(HAVE_TARGET_CBTF_XML, false)
      TARGET_CBTF_XML_CPPFLAGS=""
      TARGET_CBTF_XML_LDFLAGS=""
      TARGET_CBTF_XML_LIBS=""
      TARGET_CBTF_XML_DIR=""
      AC_MSG_RESULT(no)
    elif test $found_target_cbtf == 1 ; then
      AM_CONDITIONAL(HAVE_TARGET_CBTF_XML, true)
      AC_DEFINE(HAVE_TARGET_CBTF_XML, 1, [Define to 1 if you have a target version of CBTF.])
      AC_MSG_RESULT(yes)
    else
      AM_CONDITIONAL(HAVE_TARGET_CBTF_XML, false)
      TARGET_CBTF_XML_CPPFLAGS=""
      TARGET_CBTF_XML_LDFLAGS=""
      TARGET_CBTF_XML_LIBS=""
      TARGET_CBTF_XML_DIR=""
      AC_MSG_RESULT(no)
    fi

    AC_SUBST(TARGET_CBTF_XML_CPPFLAGS)
    AC_SUBST(TARGET_CBTF_XML_LDFLAGS)
    AC_SUBST(TARGET_CBTF_XML_LIBS)
    AC_SUBST(TARGET_CBTF_XML_DIR)

])



