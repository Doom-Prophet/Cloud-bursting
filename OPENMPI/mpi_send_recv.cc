// #include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ray/api.h>

// #define MPI_CHAR XXX
// #define MPI_COMM_WORLD XXX
// #define MPI_STATUS_IGNORE XXX

class MPI_Worker{
    int WorkerRank;
    int WorldSize;

public:
    MPI_Worker(int rank_input, int size){
        WorkerRank = rank_input;
        WorldSize = size;
    }

    // int MPI_Comm_rank(MPI_Comm comm, int *rank);
    int MPI_Comm_rank(MPI_Worker worker){
      return worker.WorkerRank;
    }

    // int MPI_Comm_size(MPI_Comm comm, int *size);
    int MPI_Comm_size(){
      return WorldSize;
    }

    auto Send(const void *buf){
      auto obj_ref = ray::Put(buf);
      return obj_ref;
    }

    auto Recv(auto obj_ref){
      auto result = ray::Get(obj_ref);
      return result;
    }

    MPI_Worker *CreateWorker(int rank_input, int size){
      return new MPI_Worker(rank_input, size);
    }
};

// int MPI_Init(int *argc, char ***argv);
std::vector<ray::ActorHandle<MPI_Worker>> MPI_Init(int size){
    ray::Init();

    std::vector<ray::ActorHandle<MPI_Worker>> workers;
    for (int i = 0; i < size; ++i) {
        auto worker = ray::Actor(CreateWorker).Remote(i + 1, size);
        workers.push_back(worker);
    }
    return workers;
}

// int MPI_Finalize(void);
void MPI_Finalize(){
  ray::Shutdown();
}

// int MPI_Abort(MPI_Comm comm, int errorcode);
int MPI_Abort(int errorcode){
  if(errorcode==1){
    std::cout << "Wrong world size" << std::endl;
  }
  std::cout << "Unknown bug code" << std::endl;
  return 0;
}

// int MPI_Send(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm);
auto MPI_Send(std::vector<ray::ActorHandle<MPI_Worker>> workers, int source, const void *buf){
  return ray::Get(workers[source].Task(&MPI_Worker::Send).Remote(buf));
}

// int MPI_Recv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Status *status);
auto MPI_Recv(std::vector<ray::ActorHandle<MPI_Worker>> workers, int source, auto obj_ref){
  return *(ray::Get(workers[source].Task(&MPI_Worker::Recv).Remote(obj_ref)));
}

RAY_REMOTE(&MPI_Worker::CreateWorker, &MPI_Worker::MPI_Comm_rank, &MPI_Worker::MPI_Comm_size, &MPI_Worker::Send, &MPI_Worker::Recv);

//////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv) {

  // Initialization
  auto workers = MPI_Init(argv[3]);

  // Access each player using the vector index
  // auto player1 = players[0];
  // auto player2 = players[1];

  if (workers[0].MPI_Comm_size() < 2) {
    MPI_Abort(1);
  }

  char stringToSend[] = "Hello world";

  auto send_package = MPI_Send(workers, 1, stringToSend);
  auto recv_package = MPI_Recv(workers, 2, send_package);
  std::cout << "recv_package = " << recv_package << std::endl;

  // Following parts not yet updated!
  // if (world_rank == 0) {
  //   char stringToSend[] = "Hello world";
  //   MPI_Send(stringToSend, strlen(stringToSend), MPI_CHAR, 1, 0, MPI_COMM_WORLD);
  //   printf("MPI process %d send string: \"%s\".\n", world_rank, stringToSend);  
  // } else if (world_rank == 1) {
  //   char stringReceived[12];
  //   stringReceived[11] = '\0';
  //   MPI_Recv(stringReceived, 11, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  //   printf("MPI process %d received string: \"%s\".\n", world_rank, stringReceived);
  // }

  // if (world_rank == 1) {
  //   char stringToSend[] = "Goodbye world";
  //   MPI_Send(stringToSend, strlen(stringToSend), MPI_CHAR, 0, 0, MPI_COMM_WORLD);
  //   printf("MPI process %d send string: \"%s\".\n", world_rank, stringToSend);  
  // } else if (world_rank == 0) {
  //   char stringReceived[14];
  //   stringReceived[13] = '\0';
  //   MPI_Recv(stringReceived, 13, MPI_CHAR, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  //   printf("MPI process %d received string: \"%s\".\n", world_rank, stringReceived);
  // }

  MPI_Finalize();
}
