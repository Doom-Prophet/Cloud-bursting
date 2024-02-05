#include <ray/api.h>
#include <iostream>
#include <thread>
#include <chrono>

class PingPong {
public:
    static int ping_count;
    static int rank;
    static ray::ActorHandle<PingPong> partner;

    PingPong(int rank) { PingPong::rank = rank; }

    static void RegisterPartner(ray::ActorHandle<PingPong> partner) {
        PingPong::partner = partner;
    }

    static ray::ObjectRef<void> Ping() {
        ping_count++;
        if (ping_count < 5) {
            std::cout << "Ping from rank " << rank << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
            return partner.Task(&PingPong::Pong).Remote();
        }
        return ray::ObjectRef<void>();
    }

    static ray::ObjectRef<void> Pong() {
        std::cout << "Pong from rank " << rank << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        return partner.Task(&PingPong::Ping).Remote();
    }
};

// Initialize static members
int PingPong::ping_count = 0;
int PingPong::rank = 0;
ray::ActorHandle<PingPong> PingPong::partner;

RAY_REMOTE(PingPong::RegisterPartner, PingPong::Ping, PingPong::Pong);

int main(int argc, char **argv) {
    ray::Init();

    auto alice = ray::Actor(PingPong::Factory, 1).Remote();
    auto bob = ray::Actor(PingPong::Factory, 2).Remote();

    alice.Task(&PingPong::RegisterPartner, bob).Remote();
    bob.Task(&PingPong::RegisterPartner, alice).Remote();

    alice.Task(&PingPong::Ping).Remote();

    ray::Shutdown();
    return 0;
}
