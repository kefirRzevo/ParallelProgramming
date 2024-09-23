#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

// mpicc task2.c -o build/MPI_Intro/MPI_Intro_Task2
// mpirun --map-by :oversubscribe -n 15 build/MPI_Intro/MPI_Intro_Task2 10

int main(int argc, char* argv[]) {
    if (argc != 2) {
        return -1;
    }
    MPI_Init(&argc, &argv);

    int size = 0, rank = 0;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int n = atoi(argv[1]);
    int not_first = (size == 1 ? 0 : n + size - n % size) / (size + 1);
    int first = n - not_first * (size - 1);
    int max = first + rank * not_first;
    int min = (rank == 0 ? 1 : max - not_first + 1);
    double sum = 0;
    for (int i = min; i <= max; ++i) {
        sum += 1. / i;
    }

    if (rank == 0) {
        for (int i = 0; i < size - 1; ++i) {
            double add = 0;
            MPI_Recv(&add, 1, MPI_DOUBLE, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, NULL);
            sum += add;
        }
        printf("Sum: %f\n", sum);
    } else {
        MPI_Send(&sum, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return EXIT_SUCCESS;
}
