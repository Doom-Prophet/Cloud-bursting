#include <ray/api.h>
#include <iostream>
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

    void Ping() {
        ping_count++;
        if (ping_count < 2) {
            std::cout << "Ping from rank " << rank << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
            // partner.Task(&PingPong::Pong).Remote();
            return partner.Task(&PingPong::Pong).Remote();
        }
        // return ray::Nil();
    }

    void Pong() {
        std::cout << "Pong from rank " << rank << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        // partner.Task(&PingPong::Ping).Remote();
        return partner.Task(&PingPong::Ping).Remote();
    }

    void Test() {
        std::cout << "Checkpoint 4 " << rank << std::endl;
        printf("Checkpoint 5");
    }
};

PingPong *CreatePlayer(int rank_input){
        return new PingPong(rank_input);
    }

RAY_REMOTE(CreatePlayer, &PingPong::RegisterPartner, &PingPong::Ping, &PingPong::Pong, &PingPong::Test);

int main(int argc, char **argv) {
    ray::Init();

    printf("Checkpoint 1");

    ray::ActorHandle<PingPong> alice = ray::Actor(CreatePlayer).Remote(1);
    ray::ActorHandle<PingPong> bob = ray::Actor(CreatePlayer).Remote(2);

    printf("Checkpoint 2");

    ray::Get(alice.Task(&PingPong::RegisterPartner).Remote(bob));
    ray::Get(bob.Task(&PingPong::RegisterPartner).Remote(alice));

    printf("Checkpoint 3");

    ray::Get(alice.Task(&PingPong::Test).Remote());

    printf("Checkpoint 6");

    ray::Get(alice.Task(&PingPong::Ping).Remote());

    printf("Checkpoint 7");

    // ray::Shutdown();
    return 0;
}
