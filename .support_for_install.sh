#!/bin/bash 
#
# Set up we find the autotools for bootstrapping
#
set -x
export bmode=""
if [ `uname -m` = "x86_64" -o `uname -m` = " x86-64" ]; then
    LIBDIR="lib64"
    ALTLIBDIR="lib"
    echo "UNAME IS X86_64 FAMILY: LIBDIR=$LIBDIR"
    export LIBDIR="lib64"
elif [ `uname -m` = "ppc64" ]; then
   if [ $CBTF_PPC64_BITMODE_32 ]; then
    LIBDIR="lib"
    ALTLIBDIR="lib64"
    echo "UNAME IS PPC64 FAMILY, but 32 bitmode: LIBDIR=$LIBDIR"
    export LIBDIR="lib"
    export bmode="--with-ppc64-bitmod=32"
   else
    LIBDIR="lib64"
    ALTLIBDIR="lib"
    echo "UNAME IS PPC64 FAMILY, with 64 bitmode: LIBDIR=$LIBDIR"
    export LIBDIR="lib64"
    export CFLAGS=" -m64 $CFLAGS "
    export CXXFLAGS=" -m64 $CXXFLAGS "
    export CPPFLAGS=" -m64 $CPPFLAGS "
    export bmode="--with-ppc64-bitmod=64"
   fi
elif [ `uname -m` = "ppc" ]; then
    LIBDIR="lib"
    ALTLIBDIR="lib64"
    echo "UNAME IS PPC FAMILY: LIBDIR=$LIBDIR"
    export LIBDIR="lib"
    export bmode="--with-ppc64-bitmod=32"
else
    LIBDIR="lib"
    ALTLIBDIR="lib64"
    export LIBDIR="lib"
    echo "UNAME IS X86 FAMILY: LIBDIR=$LIBDIR"
fi

#sys=`uname -n | grep -o '[^0-9]\{0,\}'`
sys=`uname -n `
export MACHINE=$sys
echo ""
echo '    machine: ' $sys
                                                                                    
#if [ -z $LD_LIBRARY_PATH ]; then
#  export LD_LIBRARY_PATH=$CBTF_ROOT/$LIBDIR
#else
#  export LD_LIBRARY_PATH=$CBTF_ROOT/$LIBDIR:$LD_LIBRARY_PATH
#fi
#
#if [ -z $PATH ]; then
#  export PATH=$CBTF_ROOT/bin:$PATH
#else
#  export PATH=$CBTF_ROOT/bin:$PATH
#fi

if [ -z "$CBTF_MPI_MPICH2" ]; then
 export CBTF_MPI_MPICH2=/usr
fi

if [ -z "$CBTF_MPI_MVAPICH" ]; then
 export CBTF_MPI_MVAPICH=/usr
fi

if [ -z "$CBTF_MPI_MVAPICH2" ]; then
 export CBTF_MPI_MVAPICH2=/usr
fi

if [ -z "$CBTF_MPI_MPT" ]; then
 export CBTF_MPI_MPT=/usr
fi

if [ -z "$CBTF_MPI_OPENMPI" ]; then
 export CBTF_MPI_OPENMPI=/usr
fi


export CC=`which gcc`
export CXX=`which c++`
export CPLUSPLUS=`which c++`

# you may want to load the latest autotools for the bootstraps to succeed.

echo "-------------------------------------------------------------"
echo "-- BUILDING MESSAGES ----------------------------------"
echo "-------------------------------------------------------------"
cd messages
#./bootstrap

if [ -z "$CBTF_TARGET_ARCH" ];
then
./configure --prefix=$CBTF_PREFIX $bmode --with-cbtf=$CBTF_PREFIX --with-mrnet=$CBTF_MRNET_ROOT --with-cbtf-mrnet=$CBTF_PREFIX --with-boost=$CBTF_BOOST_ROOT --with-boost-libdir=$CBTF_BOOST_ROOT_LIB  --with-libxerces-c-prefix=$CBTF_XERCESC_ROOT --with-cbtf-xml=$CBTF_PREFIX
else
./configure --prefix=$CBTF_PREFIX $bmode --with-cbtf=$CBTF_PREFIX --with-mrnet=$CBTF_MRNET_ROOT --with-cbtf-mrnet=$CBTF_PREFIX --with-boost=$CBTF_BOOST_ROOT --with-boost-libdir=$CBTF_BOOST_ROOT_LIB  --with-libxerces-c-prefix=$CBTF_XERCESC_ROOT --with-cbtf-xml=$CBTF_PREFIX --with-target-os=$CBTF_TARGET_ARCH
fi
echo "-- UNINSTALLING MESSAGES ----------------------------------"
make uninstall
echo "-- CLEANING MESSAGES ----------------------------------"
make clean
echo "-- MAKING MESSAGES ----------------------------------"
make
echo "-- INSTALLING MESSAGES ----------------------------------"
make install
echo "-- FINISHED BUILDING MESSAGES ----------------------------------"

cd ..

if [ -f $CBTF_PREFIX/$LIBDIR/libcbtf-messages-base.so -a -f $CBTF_PREFIX/$LIBDIR/libcbtf-messages-converters-perfdata.so ]; then
   echo "CBTF MESSAGES BUILT SUCCESSFULLY into $CBTF_PREFIX."
else
   echo "CBTF MESSAGES FAILED TO BUILD - TERMINATING BUILD SCRIPT.  Please check for errors."
   exit
fi

#
echo "-------------------------------------------------------------"
echo "-- BUILDING SERVICES ----------------------------------"
echo "-------------------------------------------------------------"
cd services
#./bootstrap
echo "-- CONFIGURING SERVICES ----------------------------------"
if [ -z "$CBTF_TARGET_ARCH" ];
then
  ./configure --prefix=$CBTF_PREFIX $bmode --with-mrnet=$CBTF_MRNET_ROOT --with-cbtf-messages=$CBTF_PREFIX --with-libmonitor=$CBTF_LIBMONITOR_ROOT --with-libunwind=$CBTF_LIBUNWIND_ROOT --with-papi=$CBTF_PAPI_ROOT  --with-tls=implicit  --with-binutils=$CBTF_BINUTILS_ROOT
else
  ./configure --prefix=$CBTF_PREFIX $bmode --with-mrnet=$CBTF_MRNET_ROOT --with-cbtf-messages=$CBTF_PREFIX --with-libmonitor=$CBTF_LIBMONITOR_ROOT --with-libunwind=$CBTF_LIBUNWIND_ROOT --with-papi=$CBTF_PAPI_ROOT  --with-tls=implicit  --with-binutils=$CBTF_BINUTILS_ROOT --with-target-os=$CBTF_TARGET_ARCH
fi
echo "-- UNINSTALLING SERVICES ----------------------------------"
make uninstall
echo "-- CLEANING SERVICES ----------------------------------"
make clean
echo "-- MAKING SERVICES ----------------------------------"
make
echo "-- INSTALLING SERVICES ----------------------------------"
make install
echo "-- FINISHED BUILDING SERVICES ----------------------------------"


if [ -f $CBTF_PREFIX/$LIBDIR/libcbtf-services-common.so -a -f $CBTF_PREFIX/$LIBDIR/libcbtf-services-unwind.so ]; then
   echo "CBTF SERVICES BUILT SUCCESSFULLY into $CBTF_PREFIX."
else
   echo "CBTF SERVICES FAILED TO BUILD - TERMINATING BUILD SCRIPT.  Please check for errors."
   exit
fi

cd ..

echo "-------------------------------------------------------------"
echo "-- STARTING TO BUILD CORE ----------------------------------"
echo "-------------------------------------------------------------"
cd core
#./bootstrap

echo "-- CONFIGURING CORE ----------------------------------"
if [ -z "$CBTF_TARGET_ARCH" ];
then
  ./configure --prefix=$CBTF_PREFIX $bmode --with-cbtf=$CBTF_PREFIX --with-cbtf-messages=$CBTF_PREFIX --with-mrnet=$CBTF_MRNET_ROOT --with-cbtf-mrnet=$CBTF_PREFIX --with-boost=$CBTF_BOOST_ROOT --with-boost-libdir=$CBTF_BOOST_ROOT_LIB --with-cbtf-xml=$CBTF_PREFIX --with-papi=$CBTF_PAPI_ROOT --with-libmonitor=$CBTF_LIBMONITOR_ROOT --with-libunwind=$CBTF_LIBUNWIND_ROOT --with-cbtf-services=$CBTF_PREFIX --with-tls=implicit --with-libelf=$CBTF_LIBELF_ROOT --with-libdwarf=$CBTF_LIBDWARF_ROOT --with-libdwarf-libdir=$CBTF_LIBDWARF_ROOT_LIB --with-dyninst=$CBTF_DYNINST_ROOT --with-dyninst-libdir=$CBTF_DYNINST_ROOT_LIB --with-dyninst-version=$CBTF_DYNINST_VERS --with-libxerces-c-prefix=$CBTF_XERCESC_ROOT --with-binutils=$CBTF_BINUTILS_ROOT --with-mpich2=$CBTF_MPI_MPICH2 --with-mvapich=$CBTF_MPI_MVAPICH --with-mvapich2=$CBTF_MPI_MVAPICH2 --with-openmpi=$CBTF_MPI_OPENMPI --with-mpt=$CBTF_MPI_MPT --with-alps=$CBTF_ALPS_ROOT
else
  ./configure --prefix=$CBTF_PREFIX $bmode --with-cbtf=$CBTF_PREFIX --with-cbtf-messages=$CBTF_PREFIX --with-mrnet=$CBTF_MRNET_ROOT --with-cbtf-mrnet=$CBTF_PREFIX --with-boost=$CBTF_BOOST_ROOT --with-boost-libdir=$CBTF_BOOST_ROOT_LIB --with-cbtf-xml=$CBTF_PREFIX --with-papi=$CBTF_PAPI_ROOT --with-libmonitor=$CBTF_LIBMONITOR_ROOT --with-libunwind=$CBTF_LIBUNWIND_ROOT --with-cbtf-services=$CBTF_PREFIX --with-tls=implicit --with-libelf=$CBTF_LIBELF_ROOT --with-libdwarf=$CBTF_LIBDWARF_ROOT --with-libdwarf-libdir=$CBTF_LIBDWARF_ROOT_LIB --with-dyninst=$CBTF_DYNINST_ROOT --with-dyninst-libdir=$CBTF_DYNINST_ROOT_LIB --with-dyninst-version=$CBTF_DYNINST_VERS --with-libxerces-c-prefix=$CBTF_XERCESC_ROOT --with-binutils=$CBTF_BINUTILS_ROOT --with-target-os=$CBTF_TARGET_ARCH --with-mpich2=$CBTF_MPI_MPICH2 --with-mvapich=$CBTF_MPI_MVAPICH --with-mvapich2=$CBTF_MPI_MVAPICH2 --with-openmpi=$CBTF_MPI_OPENMPI --with-mpt=$CBTF_MPI_MPT --with-alps=$CBTF_ALPS_ROOT
fi
echo "-- UNINSTALLING CORE ----------------------------------"
make uninstall
echo "-- CLEANING CORE ----------------------------------"
make clean
echo "-- MAKING CORE ----------------------------------"
make
echo "-- INSTALLING CORE ----------------------------------"
make install
echo "-- FINISHED BUILDING CORE ----------------------------------"

if [ -f $CBTF_PREFIX/$LIBDIR/libcbtf-core-symtabapi.so -a -f $CBTF_PREFIX/$LIBDIR/libcbtf-core.so -a -f $CBTF_PREFIX/$LIBDIR/KrellInstitute/Components/CollectionPlugin.so ]; then
   echo "CBTF CORE BUILT SUCCESSFULLY into $CBTF_PREFIX."
else
   echo "CBTF CORE FAILED TO BUILD - TERMINATING BUILD SCRIPT.  Please check for errors."
   exit
fi

cd ..

#
echo "-------------------------------------------------------------"
echo "-- BUILDING EXAMPLES ----------------------------------"
echo "-------------------------------------------------------------"
cd examples
#./bootstrap
if [ -z "$CBTF_TARGET_ARCH" ];
then
  ./configure --prefix=$CBTF_PREFIX $bmode --with-cbtf=$CBTF_PREFIX --with-cbtf-xml=$CBTF_PREFIX --with-mrnet=$CBTF_MRNET_ROOT --with-cbtf-messages=$CBTF_PREFIX --with-libmonitor=$CBTF_LIBMONITOR_ROOT --with-libunwind=$CBTF_LIBUNWIND_ROOT --with-papi=$CBTF_PAPI_ROOT  --with-tls=implicit  --with-binutils=$CBTF_BINUTILS_ROOT  --with-boost=$CBTF_BOOST_ROOT --with-boost-libdir=$CBTF_BOOST_ROOT_LIB --with-libxerces-c-prefix=$CBTF_XERCESC_ROOT --with-libdwarf=$CBTF_LIBDWARF_ROOT --with-libdwarf-libdir=$CBTF_LIBDWARF_ROOT_LIB --with-dyninst=$CBTF_DYNINST_ROOT --with-dyninst-libdir=$CBTF_DYNINST_ROOT_LIB --with-dyninst-version=$CBTF_DYNINST_VERS --with-python=$CBTF_PYTHON_ROOT
else
  ./configure --prefix=$CBTF_PREFIX $bmode --with-cbtf=$CBTF_PREFIX --with-cbtf-xml=$CBTF_PREFIX --with-mrnet=$CBTF_MRNET_ROOT --with-cbtf-messages=$CBTF_PREFIX --with-libmonitor=$CBTF_LIBMONITOR_ROOT --with-libunwind=$CBTF_LIBUNWIND_ROOT --with-papi=$CBTF_PAPI_ROOT  --with-tls=implicit  --with-binutils=$CBTF_BINUTILS_ROOT  --with-boost=$CBTF_BOOST_ROOT --with-boost-libdir=$CBTF_BOOST_ROOT_LIB --with-libxerces-c-prefix=$CBTF_XERCESC_ROOT --with-libdwarf=$CBTF_LIBDWARF_ROOT --with-libdwarf-libdir=$CBTF_LIBDWARF_ROOT_LIB --with-dyninst=$CBTF_DYNINST_ROOT --with-dyninst-libdir=$CBTF_DYNINST_ROOT_LIB --with-dyninst-version=$CBTF_DYNINST_VERS --with-target-os=$CBTF_TARGET_ARCH --with-python=$CBTF_PYTHON_ROOT
fi

make uninstall; make clean; make; make install

if [ -f $CBTF_PREFIX/share/KrellInstitute/demos/xml/pcsampDemo.xml ]; then
   echo "CBTF EXAMPLES BUILT SUCCESSFULLY into $CBTF_PREFIX."
else
   echo "CBTF EXAMPLES FAILED TO BUILD - TERMINATING BUILD SCRIPT.  Please check for errors."
   exit
fi

cd ..
#
echo "-------------------------------------------------------------"
echo "-- BUILDING TEST ----------------------------------"
echo "-------------------------------------------------------------"
cd test
#./bootstrap
if [ -z "$CBTF_TARGET_ARCH" ];
then
  ./configure --prefix=$CBTF_PREFIX $bmode --with-cbtf=$CBTF_PREFIX --with-cbtf-xml=$CBTF_PREFIX --with-mrnet=$CBTF_MRNET_ROOT --with-libdwarf=$CBTF_LIBDWARF_ROOT --with-libdwarf-libdir=$CBTF_LIBDWARF_ROOT_LIB --with-dyninst=$CBTF_DYNINST_ROOT --with-dyninst-libdir=$CBTF_DYNINST_ROOT_LIB --with-dyninst-version=$CBTF_DYNINST_VERS --with-cbtf-messages=$CBTF_PREFIX --with-cbtf-core=$CBTF_PREFIX --with-libmonitor=$CBTF_LIBMONITOR_ROOT --with-libunwind=$CBTF_LIBUNWIND_ROOT --with-papi=$CBTF_PAPI_ROOT  --with-tls=implicit  --with-binutils=$CBTF_BINUTILS_ROOT  --with-boost=$CBTF_BOOST_ROOT --with-boost-libdir=$CBTF_BOOST_ROOT_LIB --with-libxerces-c-prefix=$CBTF_XERCESC_ROOT --with-cbtf-mrnet=$CBTF_PREFIX 
else
  ./configure --prefix=$CBTF_PREFIX $bmode --with-cbtf=$CBTF_PREFIX --with-cbtf-xml=$CBTF_PREFIX --with-mrnet=$CBTF_MRNET_ROOT --with-libdwarf=$CBTF_LIBDWARF_ROOT --with-libdwarf-libdir=$CBTF_LIBDWARF_ROOT_LIB --with-dyninst=$CBTF_DYNINST_ROOT --with-dyninst-libdir=$CBTF_DYNINST_ROOT_LIB --with-dyninst-version=$CBTF_DYNINST_VERS --with-cbtf-messages=$CBTF_PREFIX --with-cbtf-core=$CBTF_PREFIX --with-libmonitor=$CBTF_LIBMONITOR_ROOT --with-libunwind=$CBTF_LIBUNWIND_ROOT --with-papi=$CBTF_PAPI_ROOT  --with-tls=implicit  --with-binutils=$CBTF_BINUTILS_ROOT  --with-boost=$CBTF_BOOST_ROOT --with-boost-libdir=$CBTF_BOOST_ROOT_LIB --with-libxerces-c-prefix=$CBTF_XERCESC_ROOT --with-cbtf-mrnet=$CBTF_PREFIX  --with-target-os=$CBTF_TARGET_ARCH
fi

make uninstall; make clean; make; make install

if [ -f src/pcsamp_xdr/testXDR ]; then
   echo "CBTF TEST BUILT SUCCESSFULLY into $CBTF_PREFIX."
else
   echo "CBTF TEST FAILED TO BUILD - TERMINATING BUILD SCRIPT.  Please check for errors."
   exit
fi

cd ..

echo "-------------------------------------------------------------"
echo "-- BUILDING LIBKRELL-BASE -----------------------------------"
echo "-------------------------------------------------------------"

cd libkrell-base

if [ ! -d "build" ]; then
   mkdir build
fi

cd build

export CC=`which gcc`
export CXX=`which c++`
export CPLUSPLUS=`which c++`

cmake -DCMAKE_INSTALL_PREFIX=$CBTF_PREFIX -DLIB_SUFFIX=64 \
    -DCMAKE_LIBRARY_PATH=$CBTF_PREFIX/lib64 \
    -DCMAKE_MODULE_PATH=$CBTF_PREFIX/share/KrellInstitute/cmake ..

echo "-------------------------------------------------------------"
echo "-- INSTALLING LIBKRELL-BASE ---------------------------------"
echo "-------------------------------------------------------------"

make install


if [ -f $CBTF_PREFIX/$LIBDIR/libkrell-base.so ]; then
   echo "CBTF LIBKRELL-BASE BUILT SUCCESSFULLY into $CBTF_PREFIX."
else
   echo "CBTF LIBKRELL-BASE FAILED TO BUILD - TERMINATING BUILD SCRIPT.  Please check for errors."
   exit
fi

cd ../..

echo "-------------------------------------------------------------"
echo "-- BUILDING LIBKRELL-SYMTAB ---------------------------------"
echo "-------------------------------------------------------------"

cd libkrell-symtab

if [ ! -d "build" ]; then
   mkdir build
fi

cd build

export CC=`which gcc`
export CXX=`which c++`
export CPLUSPLUS=`which c++`

cmake -DCMAKE_INSTALL_PREFIX=$CBTF_PREFIX -DLIB_SUFFIX=64 \
    -DCMAKE_LIBRARY_PATH=$CBTF_PREFIX/lib64 \
    -DCMAKE_MODULE_PATH=$CBTF_PREFIX/share/KrellInstitute/cmake ..

echo "-------------------------------------------------------------"
echo "-- INSTALLING LIBKRELL-SYMTAB -------------------------------"
echo "-------------------------------------------------------------"

make install


if [ -f $CBTF_PREFIX/$LIBDIR/libkrell-symtab.so ]; then
   echo "CBTF LIBKRELL-SYMTAB BUILT SUCCESSFULLY into $CBTF_PREFIX."
else
   echo "CBTF LIBKRELL-SYMTAB FAILED TO BUILD - TERMINATING BUILD SCRIPT.  Please check for errors."
   exit
fi

cd ../..


echo "-------------------------------------------------------------"
echo "-- END OF BUILDING CBTF  ------------------------------------"
echo "-------------------------------------------------------------"
echo "-------------------------------------------------------------"
echo "-- END OF BUILDING CBTF  ------------------------------------"
echo "-------------------------------------------------------------"
