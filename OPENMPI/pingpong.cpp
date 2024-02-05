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

    void RegisterPartner(ray::ActorHandle<PingPong> partner) {
        this->partner = partner;
    }

    void Ping() {
        ping_count++;
        if (ping_count < 5) {
            std::cout << "Ping from rank " << rank << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
            partner.Task(&PingPong::Pong).Remote();
            // return partner.Task(&PingPong::Pong, partner).Remote();
        }
        // return ray::Nil();
    }

    void Pong() {
        std::cout << "Pong from rank " << rank << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        partner.Task(&PingPong::Ping).Remote();
        // return partner.Task(&PingPong::Ping, partner).Remote();
    }
};

PingPong *CreatePlayer(int rank_input){
        return new PingPong(rank_input);
    }

RAY_REMOTE(CreatePlayer, &PingPong::RegisterPartner, &PingPong::Ping, &PingPong::Pong);

int main(int argc, char **argv) {
    ray::Init();

    ray::ActorHandle<PingPong> alice = ray::Actor(CreatePlayer).Remote(1);
    ray::ActorHandle<PingPong> bob = ray::Actor(CreatePlayer).Remote(2);

    alice.Task(&PingPong::RegisterPartner).Remote(bob);
    bob.Task(&PingPong::RegisterPartner).Remote(alice);

    alice.Task(&PingPong::Ping).Remote();

    ray::Shutdown();
    return 0;
}
