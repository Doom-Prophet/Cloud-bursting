#include <ray/api.h>
#include <iostream>
#include <thread>
#include <chrono>

class PingPong {
    int ping_count;
    int rank;

public:
    ray::ActorHandle<PingPong> partner;

    PingPong(int rank_input){
        ping_count = 0;
        rank = rank_input;
    }

    Pingpong *CreatePlayer(int rank_input){
        return new Pingpong(rank_input);
    }

    static void RegisterPartner(ray::ActorHandle<PingPong>& self, ray::ActorHandle<PingPong>& partner) {
        self->registerPartner(partner);
    }

    void registerPartner(ray::ActorHandle<PingPong>& partner) {
        this->partner = partner;
    }

    static ray::ObjectRef<void> Ping(ray::ActorHandle<PingPong>& self, ray::ActorHandle<PingPong>& partner) {
        return self->ping();
    }

    ray::ObjectRef<void> ping() {
        ping_count++;
        if (ping_count < 5) {
            std::cout << "Ping from rank " << rank << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
            return partner.Task(&PingPong::Pong, partner).Remote();
        }
        return ray::Nil();
    }

    static ray::ObjectRef<void> Pong(ray::ActorHandle<PingPong>& self, ray::ActorHandle<PingPong>& partner) {
        return self->pong();
    }

    ray::ObjectRef<void> pong() {
        std::cout << "Pong from rank " << rank << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        return partner.Task(&PingPong::Ping, partner).Remote();
    }
};

RAY_REMOTE(PingPong::RegisterPartner, PingPong::Ping, PingPong::Pong);

int main(int argc, char **argv) {
    ray::Init();

    ActorHandle<PingPong> alice = ray::Actor(CreatePlayer).Remote(1);
    ActorHandle<PingPong> bob = ray::Actor(CreatePlayer).Remote(2);

    ray::Task(PingPong::RegisterPartner, alice, bob).Remote();
    ray::Task(PingPong::RegisterPartner, bob, alice).Remote();

    ray::Task(PingPong::Ping, alice).Remote();

    ray::Shutdown();
    return 0;
}
