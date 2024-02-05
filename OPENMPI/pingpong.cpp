#include <iostream>
#include <ray/api.h>
#include <thread>
#include <chrono>

// class PingPong {
//     int ping_count = 0;
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
//         ping_count++;
//         if (ping_count < 2) {
//             std::cout << "Ping from rank " << rank << std::endl;
//         // std::this_thread::sleep_for(std::chrono::seconds(1));
//         // partner.Task(&PingPong::Pong).Remote();
//             return partner.Task(&PingPong::Pong).Remote();
//         }
//         return 0;
//     }

//     int Pong() {
//         std::cout << "Pong from rank " << rank << std::endl;
//         // std::this_thread::sleep_for(std::chrono::seconds(1));
//         // partner.Task(&PingPong::Ping).Remote();
//         return partner.Task(&PingPong::Ping).Remote();
//         // return 0;
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
    
//     auto res1 = ray::Get(alice.Task(&PingPong::Ping).Remote());
//     while(res1 != None){
//         // ping_count++;
//         auto res1 = ray::Get(alice.Task(&PingPong::Ping).Remote());
//         // auto res2 = ray::Get(bob.Task(&PingPong::Pong).Remote());
//     }

//     printf("Checkpoint 7");

//     // ray::Shutdown();
//     return 0;
// }

class Counter {
    int count;
public:
    Counter(int init) {count = init;}
    int Add(int x) {return x + 1;}
    std::cout << "Hi" << std::endl;
};

Counter *CreateCounter(int init) {
    return new Counter(init);
}
RAY_REMOTE(CreateCounter, &Counter::Add);

int main(){
    std::cout << "Hi 1" << std::endl;
    // Create a actor
    ray::ActorHandle<Counter> actor = ray::Actor(CreateCounter).Remote(0);

    std::cout << "Hi 2" << std::endl;
    // Call the actor's remote function
    auto result = actor.Task(&Counter::Add).Remote(1);
    int value = *result.Get(); // Retrieve the result of the remote function call
    // printf("%d\n", value); // Correctly print the integer result

    std::cout << "Hi 3" << std::endl;
    return 0;
}