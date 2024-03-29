#include <iostream>
#include <ray/api.h>
#include <thread>
#include <chrono>

class PingPong {
    int ping_count = 0;
    int rank;

public:
    ray::ActorHandle<PingPong> partner;

    PingPong(int rank_input){
        rank = rank_input;
    }

    int RegisterPartner(ray::ActorHandle<PingPong> partner) {
        this->partner = partner;
        return 0;
    }
    
    auto Ping() {
        ping_count++;
        if (ping_count < 2) {
            std::cout << "Ping from rank " << rank << std::endl;
            // std::this_thread::sleep_for(std::chrono::seconds(1));
            // partner.Task(&PingPong::Pong).Remote();
            // return partner.Task(&PingPong::Pong).Remote();
        }
        return 0;
    }

    auto Pong() {
        std::cout << "Pong from rank " << rank << std::endl;
        // std::this_thread::sleep_for(std::chrono::seconds(1));
        // partner.Task(&PingPong::Ping).Remote();
        // return partner.Task(&PingPong::Ping).Remote();
        // return 0;
    }

    int Test() {
        std::cout << "Checkpoint 4 \n" << std::endl;
        printf("Checkpoint 5\n");
        return 0;
    }
};

PingPong *CreatePlayer(int rank_input){
        return new PingPong(rank_input);
    }

RAY_REMOTE(CreatePlayer, &PingPong::RegisterPartner, &PingPong::Ping, &PingPong::Pong, &PingPong::Test);

int main(int argc, char **argv) {
    // int ping_count = 0;

    ray::Init();

    printf("Checkpoint 1\n");

    ray::ActorHandle<PingPong> alice = ray::Actor(CreatePlayer).Remote(1);
    ray::ActorHandle<PingPong> bob = ray::Actor(CreatePlayer).Remote(2);

    printf("Checkpoint 2\n");

    ray::Get(alice.Task(&PingPong::RegisterPartner).Remote(bob));
    ray::Get(bob.Task(&PingPong::RegisterPartner).Remote(alice));

    printf("Checkpoint 3\n");

    auto res0 = ray::Get(alice.Task(&PingPong::Test).Remote());

    printf("Checkpoint 6\n");
    
    // auto res1 = ray::Get(alice.Task(&PingPong::Ping).Remote());
    // while(res1 != NULL){
    //     // ping_count++;
    //     auto res1 = ray::Get(alice.Task(&PingPong::Ping).Remote());
    //     // auto res2 = ray::Get(bob.Task(&PingPong::Pong).Remote());
    // }

    // printf("Checkpoint 7");

    // ray::Shutdown();
    return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// /// This is an example of Ray C++ application. Please visit
// /// `https://docs.ray.io/en/master/ray-core/walkthrough.html#installation`
// /// for more details.

// /// including the `<ray/api.h>` header
// #include <ray/api.h>

// /// common function
// int Plus(int x, int y) { return x + y; }
// /// Declare remote function
// RAY_REMOTE(Plus);

// /// class
// class Counter {
//  public:
//   int count;

//   Counter(int init) { count = init; }
//   /// static factory method
//   static Counter *FactoryCreate(int init) { return new Counter(init); }

//   /// non static function
//   int Add(int x) {
//     count += x;
//     return count;
//   }
// };
// /// Declare remote function
// RAY_REMOTE(Counter::FactoryCreate, &Counter::Add);

// int main(int argc, char **argv) {
//   /// initialization
//   ray::Init();

//   /// put and get object
//   auto object = ray::Put(100);
//   auto put_get_result = *(ray::Get(object));
//   std::cout << "put_get_result = " << put_get_result << std::endl;

//   /// common task
//   auto task_object = ray::Task(Plus).Remote(1, 2);
//   int task_result = *(ray::Get(task_object));
//   std::cout << "task_result = " << task_result << std::endl;

//   /// actor
//   ray::ActorHandle<Counter> actor = ray::Actor(Counter::FactoryCreate).Remote(0);
//   /// actor task
//   auto actor_object = actor.Task(&Counter::Add).Remote(3);
//   int actor_task_result = *(ray::Get(actor_object));
//   std::cout << "actor_task_result = " << actor_task_result << std::endl;
//   /// actor task with reference argument
//   auto actor_object2 = actor.Task(&Counter::Add).Remote(task_object);
//   int actor_task_result2 = *(ray::Get(actor_object2));
//   std::cout << "actor_task_result2 = " << actor_task_result2 << std::endl;

//   /// shutdown
//   ray::Shutdown();
//   return 0;
// }