#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define PINGPONG_COUNT 1000
#define MESSAGE_SIZE 1024

int main(int argc, char **argv) {
    int rank, size, i;
    char sendbuf[MESSAGE_SIZE], recvbuf[MESSAGE_SIZE];
    MPI_Status status;
    double start, end, duration;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (size != 2) {
        printf("The size is:",size);
        if (rank == 0) {
            printf("This benchmark should be run with two processes!\n");
        }
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    // Warm up
    for (i = 0; i < 10; i++) {
        if (rank == 0) {
            MPI_Send(sendbuf, MESSAGE_SIZE, MPI_CHAR, 1, 0, MPI_COMM_WORLD);
            MPI_Recv(recvbuf, MESSAGE_SIZE, MPI_CHAR, 1, 0, MPI_COMM_WORLD, &status);
        } else {
            MPI_Recv(recvbuf, MESSAGE_SIZE, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &status);
            MPI_Send(sendbuf, MESSAGE_SIZE, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
        }
    }

    // Benchmark
    MPI_Barrier(MPI_COMM_WORLD);
    start = MPI_Wtime();

    for (i = 0; i < PINGPONG_COUNT; i++) {
        if (rank == 0) {
            MPI_Send(sendbuf, MESSAGE_SIZE, MPI_CHAR, 1, 0, MPI_COMM_WORLD);
            MPI_Recv(recvbuf, MESSAGE_SIZE, MPI_CHAR, 1, 0, MPI_COMM_WORLD, &status);
        } else {
            MPI_Recv(recvbuf, MESSAGE_SIZE, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &status);
            MPI_Send(sendbuf, MESSAGE_SIZE, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
        }
    }

    end = MPI_Wtime();
    duration = (end - start) / (2 * PINGPONG_COUNT);  // Divide by 2 because of ping and pong

    if (rank == 0) {
        printf("Average latency for message size %d: %f ms\n", MESSAGE_SIZE, duration * 1000);
    }

    MPI_Finalize();
    return 0;
}
