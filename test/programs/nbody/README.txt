
TO BUILD NBODY:

# load MPI module file or dotkit, etc

# Then compile using mpicc or gcc or other

EXAMPLES:
mpicc -g -o nbody nbody-mpi.c -lm

or 

gcc -g -o nbody -I /usr/local/tools/openmpi-gnu-1.4.2/include nbody-mpi.c -lm -L/usr/local/tools/openmpi-gnu-1.4.2/lib -lmpi

etc.

