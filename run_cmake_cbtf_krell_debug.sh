rm -rf  build
mkdir build
pushd build

export CC=gcc
export CXX=g++

export KRELL_ROOT=/opt/DEVEL3/krellroot_v2.2.2
export DYNINST_ROOT=/opt/DEVEL3/krellroot_v2.2.2
export CBTF_ROOT=/opt/DEBUG/cbtf_v2.2.2
export CBTF_KRELL_PREFIX=/opt/DEBUG/cbtf_v2.2.2
export MRNET_ROOT=/opt/DEVEL3/krellroot_v2.2.2
export XERCESC_ROOT=/opt/DEVEL3/krellroot_v2.2.2

cmake .. \
        -DCMAKE_BUILD_TYPE=None \
        -DCMAKE_CXX_FLAGS="-g" \
        -DCMAKE_C_FLAGS="-g" \
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
	-DOPENMPI_DIR=/opt/openmpi-1.10.2

make clean
make
make install

