#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

// mpicc task3.c -O3 -o a3.out
// mpirun --map-by :oversubscribe -n 15 a3.out

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int size = 0, rank = 0;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int var = 43;
    if (rank == 0) {
        MPI_Send(&var, 1, MPI_INT, (size + rank + 1) % size, 0, MPI_COMM_WORLD);
        MPI_Recv(&var, 1, MPI_INT, (size + rank - 1) % size, 0, MPI_COMM_WORLD, NULL);
        printf("My rank %d. I received %d\n", rank, var);
    } else {
        MPI_Recv(&var, 1, MPI_INT, (size + rank - 1) % size, 0, MPI_COMM_WORLD, NULL);
        printf("My rank %d. I received %d\n", rank, var++);
        MPI_Send(&var, 1, MPI_INT, (size + rank + 1) % size, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return EXIT_SUCCESS;
}
