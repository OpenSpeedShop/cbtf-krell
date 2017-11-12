#!/bin/bash

rm -rf  build
mkdir build
pushd build

export CC=gcc
export CXX=g++

export KRELL_ROOT=/u/glschult/OSS/krellroot_v2.3.1
export DYNINST_ROOT=/u/glschult/OSS/osscbtf_v2.3.1
export MRNET_ROOT=/u/glschult/OSS/osscbtf_v2.3.1
export XERCESC_ROOT=/u/glschult/OSS/osscbtf_v2.3.1
export BOOST_ROOT=/u/glschult/OSS/osscbtf_v2.3.1
#export BOOST_ROOT=/nasa/pkgsrc/sles12/2016Q4/views/boost/1.62
export CBTF_ROOT=/u/glschult/OSS/osscbtf_v2.3.1
export CBTF_KRELL_PREFIX=/u/glschult/OSS/osscbtf_v2.3.1
export OPENMPI_INSTALL_ROOT=/nasa/openmpi/1.6.5/gcc
#export CUDA_DIR=/nasa/cuda/7.5
export CUDA_DIR=/nasa/cuda/sles12/8.0
export ICU_PATH=/nasa/pkgsrc/sles12/2016Q4
#export ICU_PATH=/nasa/pkgsrc/2016Q2

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
	-DOPENMPI_DIR=${OPENMPI_INSTALL_ROOT} \
	-DCMAKE_FIND_ROOT_PATH=${BOOST_ROOT} \
        -DCUDA_TOOLKIT_ROOT_DIR=${CUDA_DIR}

make clean
make
make install

