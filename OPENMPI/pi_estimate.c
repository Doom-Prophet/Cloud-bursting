#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define NUM_SAMPLES 100000000

int main(int argc, char **argv) {
    int rank;
    double x, y;
    long long int i, count = 0;
    double pi;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    unsigned int seed = rank;  // Seed the random number generator
    for (i = 0; i < NUM_SAMPLES; i++) {
        x = (double)rand_r(&seed) / RAND_MAX;
        y = (double)rand_r(&seed) / RAND_MAX;
        if (x * x + y * y <= 1.0) count++;
    }

    pi = 4.0 * count / NUM_SAMPLES;

    printf("Estimated value of Pi = %lf\n", pi);

    MPI_Finalize();
    return 0;
}
