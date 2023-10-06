#include <stdio.h>
#include <mpi.h>

#define PINGPONG_COUNT 10
#define MESSAGE_SIZE 1024

int main(int argc, char **argv) {
    int rank, size, i;
    char sendbuf[MESSAGE_SIZE], recvbuf[MESSAGE_SIZE];
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (size != 2) {
        printf("This benchmark should be run with two processes!\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    // Initialize buffers
    for (i = 0; i < MESSAGE_SIZE; i++) {
        sendbuf[i] = 'a';
        recvbuf[i] = 'b';
    }

    MPI_Barrier(MPI_COMM_WORLD);
    double start_time = MPI_Wtime();

    if (rank == 0) {
        for (i = 0; i < PINGPONG_COUNT; i++) {
            MPI_Send(sendbuf, MESSAGE_SIZE, MPI_CHAR, 1, 0, MPI_COMM_WORLD);
            MPI_Recv(recvbuf, MESSAGE_SIZE, MPI_CHAR, 1, 0, MPI_COMM_WORLD, &status);
        }
    } else if (rank == 1) {
        for (i = 0; i < PINGPONG_COUNT; i++) {
            MPI_Recv(recvbuf, MESSAGE_SIZE, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &status);
            MPI_Send(sendbuf, MESSAGE_SIZE, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
        }
    }

    double end_time = MPI_Wtime();

    if (rank == 0) {
        double elapsed_time = (end_time - start_time) / (2 * PINGPONG_COUNT);  // divide by 2 because it's a round trip
        printf("Ping-pong average latency: %f seconds\n", elapsed_time);
    }

    MPI_Finalize();
    return 0;
}
