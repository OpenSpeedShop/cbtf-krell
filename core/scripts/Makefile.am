################################################################################
# Copyright (c) 2010-2013 Krell Institute All Rights Reserved.
#
# This library is free software; you can redistribute it and/or modify it under
# the terms of the GNU Lesser General Public License as published by the Free
# Software Foundation; either version 2.1 of the License, or (at your option)
# any later version.
#
# This library is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
# details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this library; if not, write to the Free Software Foundation, Inc.,
# 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
################################################################################

dist_sbin_SCRIPTS = cbtf_mrnet_commnode cbtf_libcbtf_mrnet_backend
dist_bin_SCRIPTS = 
BUILT_FILES = cbtf_mrnet_commnode cbtf_libcbtf_mrnet_backend

if !BUILD_RUNTIME
dist_bin_SCRIPTS += cbtfrun cbtflink
BUILT_FILES += cbtfrun cbtflink
endif

CLEANFILES = $(BUILT_FILES)

monitor_dir=$(LIBMONITOR_DIR)
monitor_libdir=$(LIBMONITOR_LIBDIR)

unwind_dir=$(LIBUNWIND_DIR)
unwind_libdir=$(LIBUNWIND_LIBDIR)

papi_dir=$(PAPI_DIR)
papi_libdir=$(PAPI_LIBDIR)

mrnet_dir=$(MRNET_DIR)
mrnet_libdir=$(MRNET_LIBDIR)

dyninst_dir=$(DYNINST_DIR)
dyninst_libdir=$(DYNINST_LIBDIR)

xercesc_dir=$(LIBXERCES_C_DIR)
xercesc_libdir=$(LIBXERCES_C_LIBSDIR)

boost_dir=$(BOOST_DIR)
boost_libdir=$(BOOST_LIBDIR)

if HAVE_OMPT
libiomp_dir=$(LIBIOMP_DIR)
else
libiomp_dir=
endif


if !BUILD_RUNTIME
cbtfrun: cbtfrun.in
endif
	@rm -f cbtfrun
	@sed \
		-e 's,@machine_dependent_libdir@,$(abi_libdir_name),' \
		-e 's,@alt_machine_dependent_libdir@,$(alt_abi_libdir_name),' \
		-e 's,@cbtfkdir@,$(prefix),' \
		-e 's,@cbtfklibdir@,$(libdir),' \
		-e 's,@libmonitordir@,$(monitor_dir),' \
		-e 's,@libmonitorlibdir@,$(monitor_libdir),' \
		-e 's,@libunwinddir@,$(unwind_dir),' \
		-e 's,@libunwindlibdir@,$(unwind_libdir),' \
		-e 's,@libmrnetdir@,$(mrnet_dir),' \
		-e 's,@libmrnetlibdir@,$(mrnet_libdir),' \
		-e 's,@libpapidir@,$(papi_dir),' \
		-e 's,@libpapilibdir@,$(papi_libdir),' \
		-e 's,@libiompdir@,$(libiomp_dir),' \
		-e 's,@lamdir@,$(LAM_DIR),' \
		-e 's,@lampidir@,$(LAMPI_DIR),' \
		-e 's,@mpichdir@,$(MPICH_DIR),' \
		-e 's,@mpich2dir@,$(mpich2_dir),' \
		-e 's,@mvapichdir@,$(MVAPICH_DIR),' \
		-e 's,@mvapich2dir@,$(MVAPICH2_DIR),' \
		-e 's,@mptdir@,$(MPT_DIR),' \
		-e 's,@openmpidir@,$(OPENMPI_DIR),' \
                -e 's,@libiompdir@,$(libiomp_dir),' \
		-e 's,@cbtfdefaultmpi@,$(DEFAULT_MPI_IMPL),' \
			$(srcdir)/cbtfrun.in > cbtfrun
	@chmod 755 cbtfrun

cbtflink: cbtflink.in
	@rm -f cbtflink
	@sed \
		-e 's,@default_mpi_impl@,$(DEFAULT_MPI_IMPL),' \
		-e 's,@cbtfkdir@,$(prefix),' \
		-e 's,@cbtfklibdir@,$(libdir),' \
		-e 's,@libmonitordir@,$(monitor_dir),' \
		-e 's,@libmonitorlibdir@,$(monitor_libdir),' \
		-e 's,@libmrnetdir@,$(mrnet_dir),' \
		-e 's,@libmrnetlibdir@,$(mrnet_libdir),' \
		-e 's,@libunwinddir@,$(unwind_dir),' \
		-e 's,@libunwindlibdir@,$(unwind_libdir),' \
		-e 's,@cbtftargetos@,$(TARGET_OS),' \
		-e 's,@libpapidir@,$(papi_dir),' \
		-e 's,@libpapilibdir@,$(papi_libdir),' \
			$(srcdir)/cbtflink.in > cbtflink
	@chmod 755 cbtflink

cbtf_mrnet_commnode: cbtf_mrnet_commnode.in
	@rm -f cbtf_mrnet_commnode
	@sed \
		-e 's,@machine_dependent_libdir@,$(abi_libdir_name),' \
		-e 's,@alt_machine_dependent_libdir@,$(alt_abi_libdir_name),' \
		-e 's,@libmrnetdir@,$(mrnet_dir),' \
		-e 's,@libmrnetlibdir@,$(mrnet_libdir),' \
		-e 's,@dyninstdir@,$(dyninst_dir),' \
		-e 's,@dyninstlibdir@,$(dyninst_libdir),' \
		-e 's,@boostinstalldir@,$(boost_dir),' \
		-e 's,@boostlibdir@,$(boost_libdir),' \
		-e 's,@xercescdir@,$(xercesc_dir),' \
		-e 's,@xercesclibdir@,$(xercesc_libdir),' \
		-e 's,@cbtfdir@,$(CBTF_DIR),' \
		-e 's,@cbtflibdir@,$(CBTF_LIBDIR),' \
		-e 's,@cbtfkdir@,$(prefix),' \
		-e 's,@cbtfklibdir@,$(libdir),' \
			$(srcdir)/cbtf_mrnet_commnode.in > cbtf_mrnet_commnode
	@chmod 755 cbtf_mrnet_commnode

cbtf_libcbtf_mrnet_backend: cbtf_libcbtf_mrnet_backend.in
	@rm -f cbtf_libcbtf_mrnet_backend
	@sed \
		-e 's,@machine_dependent_libdir@,$(abi_libdir_name),' \
		-e 's,@libmrnetdir@,$(mrnet_dir),' \
		-e 's,@libmrnetlibdir@,$(mrnet_libdir),' \
		-e 's,@cbtfdir@,$(CBTF_DIR),' \
		-e 's,@cbtflibdir@,$(CBTF_LIBDIR),' \
			$(srcdir)/cbtf_libcbtf_mrnet_backend.in > cbtf_libcbtf_mrnet_backend
	@chmod 755 cbtf_libcbtf_mrnet_backend


EXTRA_DIST = cbtfrun.in cbtflink.in cbtf_mrnet_commnode.in cbtf_libcbtf_mrnet_backend.in
