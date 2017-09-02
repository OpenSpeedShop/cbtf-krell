/*******************************************************************************
** Copyright (c) 2017 Argo Navis Technologies. All Rights Reserved.
**
** This library is free software; you can redistribute it and/or modify it under
** the terms of the GNU Lesser General Public License as published by the Free
** Software Foundation; either version 2.1 of the License, or (at your option)
** any later version.
**
** This library is distributed in the hope that it will be useful, but WITHOUT
** ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
** FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
** details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this library; if not, write to the Free Software Foundation, Inc.,
** 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*******************************************************************************/

#include <mpi.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    MPI_Init(&argc, &argv);
    int rank = -1, size = -1;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    fprintf(stdout, "%s: %d of %d\n", argv[0], rank + 1, size);
    fflush(stdout);
    sleep(1);
    MPI_Finalize();
    return 0;
}
