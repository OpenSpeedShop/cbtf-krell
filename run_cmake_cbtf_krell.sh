#!/bin/bash

rm -rf  build
mkdir build
pushd build

export CC=gcc
export CXX=g++

export KRELL_ROOT=/opt/DEVEL/krellroot_v2.2.3
export DYNINST_ROOT=/opt/DEVEL/krellroot_v2.2.3
export CBTF_ROOT=/opt/DEVEL3/cbtf_v2.3.1
export CBTF_KRELL_PREFIX=/opt/DEVEL3/cbtf_v2.3.1
export MRNET_ROOT=/opt/DEVEL/krellroot_v2.2.3
export XERCESC_ROOT=/opt/DEVEL/krellroot_v2.2.3
export BOOST_ROOT=/usr

cmake .. \
        -DCMAKE_BUILD_TYPE=None \
        -DCMAKE_CXX_FLAGS="-g -O2 -m64 -mcmodel=medium" \
        -DCMAKE_C_FLAGS="-g -O2 -m64 -mcmodel=medium" \
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

