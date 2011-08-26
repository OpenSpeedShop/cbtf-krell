################################################################################
# Copyright (c) 2010 Krell Institute All Rights Reserved.
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

dist_bin_SCRIPTS = cbtfrun
BUILT_FILES = cbtfrun

CLEANFILES = $(BUILT_FILES)


monitor_dir=$(LIBMONITOR_DIR)
unwind_dir=$(LIBUNWIND_DIR)
papi_dir=$(PAPI_DIR)
mrnet_dir=$(MRNET_DIR)

cbtfrun: cbtfrun.in

	@rm -f cbtfrun
	@sed \
		-e 's,@@machine_dependent_libdir@@,$(abi_libdir_name),' \
		-e 's,@@alt_machine_dependent_libdir@@,$(alt_abi_libdir_name),' \
		-e 's,@@cbtfinstalldir@@,$(prefix),' \
		-e 's,@@libmonitordir@@,$(monitor_dir),' \
		-e 's,@@libunwinddir@@,$(unwind_dir),' \
		-e 's,@@libmrnetdir@@,$(mrnet_dir),' \
		-e 's,@@libpapidir@@,$(papi_dir),' \
		-e 's,@@lamdir@@,$(LAM_DIR),' \
		-e 's,@@lampidir@@,$(LAMPI_DIR),' \
		-e 's,@@mpichdir@@,$(MPICH_DIR),' \
		-e 's,@@mpich2dir@@,$(mpich2_dir),' \
		-e 's,@@mvapichdir@@,$(MVAPICH_DIR),' \
		-e 's,@@mvapich2dir@@,$(MVAPICH2_DIR),' \
		-e 's,@@mptdir@@,$(MPT_DIR),' \
		-e 's,@@openmpidir@@,$(OPENMPI_DIR),' \
		-e 's,@@cbtfdefaultmpi@@,$(DEFAULT_MPI_IMPL),' \
			$(srcdir)/cbtfrun.in > cbtfrun
	@chmod 755 cbtfrun
EXTRA_DIST = cbtfrun.in
