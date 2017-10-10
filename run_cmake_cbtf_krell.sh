#!/bin/bash

rm -rf  build
mkdir build
pushd build

export CC=gcc
export CXX=g++

export KRELL_ROOT=~/OSS/krellroot_v2.3.1
export DYNINST_ROOT=~/OSS/krellroot_v2.3.1
export CBTF_ROOT=~/OSS/cbtf_v2.3.1
export CBTF_KRELL_PREFIX=~/OSS/cbtf_v2.3.1
export MRNET_ROOT=~/OSS/krellroot_v2.3.1
export XERCESC_ROOT=~/OSS/krellroot_v2.3.1
export BOOST_ROOT=~/OSS/krellroot_v2.3.1

cmake .. \
        -DCMAKE_BUILD_TYPE=None \
        -DCMAKE_CXX_FLAGS="-g -O2" \
        -DCMAKE_C_FLAGS="-g -O2" \
	-DCMAKE_INSTALL_PREFIX=${CBTF_KRELL_PREFIX} \
	-DCMAKE_PREFIX_PATH=${CBTF_ROOT} \
	-DCBTF_DIR=${CBTF_ROOT} \
	-DBINUTILS_DIR=${KRELL_ROOT} \
	-DLIBMONITOR_DIR=${KRELL_ROOT} \
	-DLIBUNWIND_DIR=${KRELL_ROOT} \
	-DPAPI_DIR=${KRELL_ROOT} \
	-DMRNET_DIR=${MRNET_ROOT} \
	-DDYNINST_DIR=${DYNINST_ROOT} \
	-DXERCESC_DIR=${XERCESC_ROOT} \
        -DLIBDWARF_DIR=${KRELL_ROOT} \
        -DLIBELF_DIR=${KRELL_ROOT} \
	-DOPENMPI_DIR=/opt/OPENMPI/2.1.1/xl64 \
	-DCBTF_BOOST_ROOT=${BOOST_ROOT}

make clean
make VERBOSE=1
make install

