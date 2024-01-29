#include <ray/api.h>
#include <iostream>
#include <thread>
#include <chrono>

class PingPong {
public:
    int ping_count;
    int rank;
    ray::ActorHandle<PingPong> partner;

    PingPong(int rank) : ping_count(0), rank(rank) {}

    static void RegisterPartner(ray::ActorHandle<PingPong>& self, ray::ActorHandle<PingPong>& partner) {
        self->registerPartner(partner);
    }

    void registerPartner(ray::ActorHandle<PingPong>& partner) {
        this->partner = partner;
    }

    static ray::ObjectRef<void> Ping(ray::ActorHandle<PingPong>& self) {
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

    static ray::ObjectRef<void> Pong(ray::ActorHandle<PingPong>& self) {
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

    auto alice = ray::Actor(PingPong::Factory, 1).Remote();
    auto bob = ray::Actor(PingPong::Factory, 2).Remote();

    ray::Task(PingPong::RegisterPartner, alice, bob).Remote();
    ray::Task(PingPong::RegisterPartner, bob, alice).Remote();

    ray::Task(PingPong::Ping, alice).Remote();

    ray::Shutdown();
    return 0;
}
