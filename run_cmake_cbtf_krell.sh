#!/bin/bash

rm -rf  build
mkdir build
pushd build

export CC=gcc
export CXX=g++

export KRELL_ROOT=/opt/STABLE/krellroot_v2.3.0
export DYNINST_ROOT=/opt/STABLE/krellroot_v2.3.0
export CBTF_ROOT=/opt/STABLE/cbtf_v2.3.x
export CBTF_KRELL_PREFIX=/opt/STABLE/cbtf_v2.3.x
export MRNET_ROOT=/opt/STABLE/krellroot_v2.3.0
export XERCESC_ROOT=/opt/STABLE/krellroot_v2.3.0
export BOOST_ROOT=/opt/boost-1.59.0

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
	-DOPENMPI_DIR=/opt/openmpi-2.0.1 \
	-DCBTF_BOOST_ROOT=${BOOST_ROOT}

make clean
make
make install

