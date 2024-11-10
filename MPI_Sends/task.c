#include <mpi.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// mpicc task1.c -o build/MPI_Sends/MPI_Sends_Task
// mpirun --map-by :oversubscribe -n 2 build/MPI_Sends/MPI_Sends_Task 1024

enum send_type
{
    mpi_send = 0,
    mpi_ssend = 1,
    mpi_rsend = 2,
    mpi_bsend = 3,
};

int main(int argc, char *argv[]) {
    if (argc != 3) {
        return -1;
    }
    MPI_Init(&argc, &argv);

    int size = 0, rank = 0;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int n = atoi(argv[1]);
    char* buf = malloc(sizeof(char) * (n + 24));
    if (!buf) {
        printf("Can't allocate %d bytes\n", n);
    }
    MPI_Barrier(MPI_COMM_WORLD);
    if (rank == 0) {
        for (int i = 0; i < n; ++i) {
            buf[i] = 47;
        }
        MPI_Buffer_attach(buf, n + 24);
        int type = atoi(argv[2]);
        clock_t start = clock();
        switch (type) {
        case mpi_send:
            MPI_Send(buf, n, MPI_CHAR, 1, 0, MPI_COMM_WORLD);
            break;
        case mpi_ssend:
            MPI_Ssend(buf, n, MPI_CHAR, 1, 0, MPI_COMM_WORLD);
            break;
        case mpi_rsend:
            MPI_Rsend(buf, n, MPI_CHAR, 1, 0, MPI_COMM_WORLD);
            break;
        case mpi_bsend:
            MPI_Bsend(buf, n, MPI_CHAR, 1, 0, MPI_COMM_WORLD);
            break;
        default:
            return -1;
        }
        clock_t end = clock();
        MPI_Buffer_detach(buf, &n);
        printf("%lf\n", ((double)(end - start)) / CLOCKS_PER_SEC);
    } else if (rank == 1) {
        usleep(2*1000*1000);
        MPI_Recv(buf, n, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
    MPI_Finalize();
    free(buf);
    return EXIT_SUCCESS;
}
