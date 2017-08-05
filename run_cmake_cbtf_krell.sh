#!/bin/bash

rm -rf  build
mkdir build
pushd build

export CC=gcc
export CXX=g++

export KRELL_ROOT=/u/glschult/OSS/osscbtf_v2.3.1
export DYNINST_ROOT=/u/glschult/OSS/osscbtf_v2.3.1
export MRNET_ROOT=/u/glschult/OSS/osscbtf_v2.3.1
export XERCESC_ROOT=/u/glschult/OSS/osscbtf_v2.3.1
export BOOST_ROOT=/u/glschult/OSS/osscbtf_v2.3.1
export CBTF_ROOT=/u/glschult/OSS/osscbtf_v2.3.1
export CBTF_KRELL_PREFIX=/u/glschult/OSS/osscbtf_v2.3.1
export OPENMPI_INSTALL_ROOT=/nasa/openmpi/1.6.5/gcc
export CUDA_DIR=/nasa/cuda/7.5

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
	-DOPENMPI_DIR=${OPENMPI_INSTALL_ROOT} \
	-DCBTF_BOOST_ROOT=${BOOST_ROOT} \
        -DCUDA_TOOLKIT_ROOT_DIR=${CUDA_DIR}

make clean
make
make install

