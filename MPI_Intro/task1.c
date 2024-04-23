#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

// mpicc task1.c -O3 -o a1.out
// mpirun --map-by :oversubscribe -n 15 a1.out

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    int size = 0, rank = 0;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    printf("Hello, World! Size: %d, Rank: %d\n", size, rank);

    MPI_Finalize();
    return EXIT_SUCCESS;
}
