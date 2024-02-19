// #include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

# define MPI_COMM_WORLD XXX

int MPI_Init(int *argc, char ***argv){
    ...
    // start ray cluster
}

MPI_Abort

MPI_Comm_rank

MPI_Comm_size

MPI_Ssend

MPI_Recv

MPI_Finalize
// stop ray cluster

int main(int argc, char** argv) {

  // Initialization
  MPI_Init(NULL, NULL);

  int world_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
  int world_size;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);

  if (world_size < 2) {
    fprintf(stderr, "World size must be greater than 1 for %s\n", argv[0]);
    MPI_Abort(MPI_COMM_WORLD, 1);
  }

  // Send and recv
  if (world_rank == 0) {
    char stringToSend[] = "Hello world";
    MPI_Ssend(stringToSend, strlen(stringToSend), MPI_CHAR, 1, 0, MPI_COMM_WORLD);
    printf("MPI process %d send string: \"%s\".\n", world_rank, stringToSend);  
  } else if (world_rank == 1) {
    char stringReceived[12];
    stringReceived[11] = '\0';
    MPI_Recv(stringReceived, 11, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    printf("MPI process %d received string: \"%s\".\n", world_rank, stringReceived);
  }

  if (world_rank == 1) {
    char stringToSend[] = "Goodbye world";
    MPI_Ssend(stringToSend, strlen(stringToSend), MPI_CHAR, 0, 0, MPI_COMM_WORLD);
    printf("MPI process %d send string: \"%s\".\n", world_rank, stringToSend);  
  } else if (world_rank == 0) {
    char stringReceived[14];
    stringReceived[13] = '\0';
    MPI_Recv(stringReceived, 13, MPI_CHAR, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    printf("MPI process %d received string: \"%s\".\n", world_rank, stringReceived);
  }

  MPI_Finalize();
}
