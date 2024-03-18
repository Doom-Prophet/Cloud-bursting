// #include <mpi.h>
#include <map>
#include <tuple>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <ray/api.h>
#include <vector>

#define MPI_Datatype int
#define MPI_Comm int
#define MPI_Status int

class MPI_Worker{
    int WorkerRank;
    int WorldSize;

public:
    MPI_Worker(int rank_input, int size){
        WorkerRank = rank_input;
        WorldSize = size;
    }

    int MPI_Comm_rank(MPI_Comm comm, int *rank){
    // int MPI_Comm_rank(MPI_Worker worker){
      *rank = WorkerRank;
      return 0;
    }

    int MPI_Comm_size(MPI_Comm comm, int *size){
    // int MPI_Comm_size(){
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

    static MPI_Worker *CreateWorker(int rank_input, int size){
      return new MPI_Worker(rank_input, size);
    }
};

std::vector<ray::ActorHandle<MPI_Worker>> workers;

std::vector<ray::ObjectRef<std::vector<int>>> obj_refs_int;

std::vector<ray::ObjectRef<std::vector<std::string>>> obj_refs_str;

// std::map<int, ray::ObjectRef<void*>> obj_refs_map;

int MPI_Init(int *argc, char ***argv){
// std::vector<ray::ActorHandle<MPI_Worker>> MPI_Init(int size){
    int size = atoi(*argv[1]);
    // int size = 2;
    ray::Init();
    
    for (int i = 0; i < size; ++i) {
        ray::ActorHandle<MPI_Worker> worker = ray::Actor(MPI_Worker::CreateWorker).Remote(i + 1, size);
        workers.push_back(worker);
    }
    return 0;
}

int MPI_Finalize(void){
// void MPI_Finalize(){
  ray::Shutdown();
  return 0;
}

int MPI_Abort(MPI_Comm comm, int errorcode){
// int MPI_Abort(int errorcode){
  if(errorcode==1){
    std::cout << "Wrong world size" << std::endl;
  }
  std::cout << "Unknown bug code" << std::endl;
  return 0;
}

int MPI_Send(const void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm){
// auto MPI_Send(std::vector<ray::ActorHandle<MPI_Worker>> workers, int source, const void *buf){
  auto obj_ref = workers[source].Task(&MPI_Worker::Send).Remote(buf);
  // tag=-1 means error
  if (tag==-1){
    MPI_Abort(comm, 1);
  }
  // tag=0 means int
  if(tag==0){
    obj_refs_int.push_back(obj_ref);
  }
  // tag=1 means string
  if(tag==1){
    obj_refs_str.push_back(obj_ref);
  }
  return 0;
}

auto MPI_Recv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Status *status) {
    // Make this into a loop, keep waiting for valid obj_ref
    
    while(true){
      if (tag==-1){
        MPI_Abort(comm, 1);
        break;
      }

      if (tag==0) {
          std::vector<int> recv_buf[];
          auto &objectList = *obj_refs_int.front();
          obj_refs_int.erase(obj_refs_int.begin());

          for(const auto& obj_ref : objectList) {
            auto value = *(ray::Get(workers[source].Task(&MPI_Worker::Recv).Remote(obj_ref)));
            for(const auto& integer : value) {
              std::cout << integer << std::endl;
              recv_buf.push_back(integer);
            }
          }
          return recv_buf;
      }

      if (tag==1){
          std::vector<std::string> recv_buf[];
          auto &objectList = *obj_refs_str.front();
          obj_refs_str.erase(obj_refs_str.begin());

          for(const auto& obj_ref : objectList) {
            auto value = *(ray::Get(workers[source].Task(&MPI_Worker::Recv).Remote(obj_ref)));
            for(const auto& str : value) {
              std::cout << str << std::endl;
              recv_buf.push_back(str);
            }
          }
          return recv_buf;
      }
    }
    return -1;
}

RAY_REMOTE(MPI_Worker::CreateWorker, &MPI_Worker::MPI_Comm_rank, &MPI_Worker::MPI_Comm_size, &MPI_Worker::Send, &MPI_Worker::Recv);

//////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv) {
  MPI_Datatype datatype = 0;
  MPI_Comm comm = 0;
  MPI_Status *status = 0;

  // Initialization
  auto workers = MPI_Init(argc,argv);
  // auto workers = MPI_Init(2);

  // if (*(ray::Get(workers[0].Task(&MPI_Worker::MPI_Comm_size).Remote())) < 2) {
  //   MPI_Abort(1);
  // }

  int cnt = strlen("Hello world");
  int tag = 1;
  std::vector<std::string> stringToSend = {"Hello","I'm","Zicheng Ma","Nice to meet you!"};
  std::vector<std::string> void_buf[];

  int send_package = MPI_Send(stringToSend, cnt, datatype, 1, tag, comm);
  auto recv_package = MPI_Recv(void_buf, cnt, datatype, 2, tag, comm, status);
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
