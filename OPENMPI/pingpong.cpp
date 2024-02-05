#include <ray/api.h>
#include <cassert>
// #include <iostream>
// #include <thread>
// #include <chrono>

// class PingPong {
//     // int ping_count = 0;
//     int rank;

// public:
//     ray::ActorHandle<PingPong> partner;

//     PingPong(int rank_input){
//         rank = rank_input;
//     }

//     int RegisterPartner(ray::ActorHandle<PingPong> partner) {
//         this->partner = partner;
//         return 0;
//     }
    
//     int Ping() {
//         // ping_count++;
//         // if (ping_count < 2) {
//         std::cout << "Ping from rank " << rank << std::endl;
//         // std::this_thread::sleep_for(std::chrono::seconds(1));
//         // partner.Task(&PingPong::Pong).Remote();
//         // partner.Task(&PingPong::Pong).Remote();
//         // }
//         return 0;
//     }

//     int Pong() {
//         std::cout << "Pong from rank " << rank << std::endl;
//         // std::this_thread::sleep_for(std::chrono::seconds(1));
//         // partner.Task(&PingPong::Ping).Remote();
//         // partner.Task(&PingPong::Ping).Remote();
//         return 0;
//     }

//     int Test() {
//         std::cout << "Checkpoint 4 " << std::endl;
//         printf("Checkpoint 5");
//         return 0;
//     }
// };

// PingPong *CreatePlayer(int rank_input){
//         return new PingPong(rank_input);
//     }

// RAY_REMOTE(CreatePlayer, &PingPong::RegisterPartner, &PingPong::Ping, &PingPong::Pong, &PingPong::Test);

// int main(int argc, char **argv) {
//     int ping_count = 0;

//     ray::Init();

//     printf("Checkpoint 1");

//     ray::ActorHandle<PingPong> alice = ray::Actor(CreatePlayer).Remote(1);
//     ray::ActorHandle<PingPong> bob = ray::Actor(CreatePlayer).Remote(2);

//     printf("Checkpoint 2");

//     ray::Get(alice.Task(&PingPong::RegisterPartner).Remote(bob));
//     ray::Get(bob.Task(&PingPong::RegisterPartner).Remote(alice));

//     printf("Checkpoint 3");

//     auto res0 = ray::Get(alice.Task(&PingPong::Test).Remote());

//     printf("Checkpoint 6");
    
//     while(ping_count < 5){
//         ping_count++;
//         auto res1 = ray::Get(alice.Task(&PingPong::Ping).Remote());
//         auto res2 = ray::Get(bob.Task(&PingPong::Pong).Remote());
//     }

//     printf("Checkpoint 7");

//     // ray::Shutdown();
//     return 0;
// }

// A regular C++ function.
int MyFunction() {
  return 1;
}
// Register as a remote function by `RAY_REMOTE`.
RAY_REMOTE(MyFunction);

// Invoke the above method as a Ray task.
// This will immediately return an object ref (a future) and then create
// a task that will be executed on a worker process.
auto res = ray::Task(MyFunction).Remote();

// The result can be retrieved with ``ray::ObjectRef::Get``.
assert(*res.Get() == 1);