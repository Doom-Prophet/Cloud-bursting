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

    void RegisterPartner(ray::ActorHandle<PingPong> partner) {
        this->partner = partner;
    }

    ray::ObjectRef<void> Ping() {
        ping_count++;
        if (ping_count < 5) {
            std::cout << "Ping from rank " << rank << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
            partner.Task(&PingPong::Pong).Remote();
        }
        return ray::ObjectRef<void>();  // Return an empty object ref.
    }

    ray::ObjectRef<void> Pong() {
        std::cout << "Pong from rank " << rank << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        partner.Task(&PingPong::Ping).Remote();
        return ray::ObjectRef<void>();  // Return an empty object ref.
    }
};

RAY_REMOTE(PingPong::RegisterPartner, PingPong::Ping, PingPong::Pong);

int main(int argc, char **argv) {
    ray::RayConfig config;
    config.local_mode = true;
    ray::Init(config);

    auto alice = ray::ActorRemote(PingPong::Factory, 1);
    auto bob = ray::ActorRemote(PingPong::Factory, 2);

    alice.Task(&PingPong::RegisterPartner, bob).Remote();
    bob.Task(&PingPong::RegisterPartner, alice).Remote();

    alice.Task(&PingPong::Ping).Remote();

    ray::Shutdown();
    return 0;
}
