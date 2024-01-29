// #include <ray/api.h>
#include <iostream>
#include <thread>
#include <chrono>

class PingPong {
    public:
        PingPong(int rank) : ping_count(0), rank(rank) {}

        void registerPartner(ray::ActorHandle<PingPong> partner) {
            this->partner = partner;
        }

        ray::ObjectRef<void> ping() {
            ping_count++;
            if (ping_count < 5) {
                std::cout << "Ping from rank " << rank << std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(1));
                return partner.Task(&PingPong::pong).Remote();
            }
            return ray::Nil();
        }

        ray::ObjectRef<void> pong() {
            std::cout << "Pong from rank " << rank << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
            return partner.Task(&PingPong::ping).Remote();
        }

    private:
        int ping_count;
        int rank;
        ray::ActorHandle<PingPong> partner;
    };

int main(int argc, char **argv) {
    ray::Init();

    auto alice = ray::Actor(PingPong::Factory, 1).Remote();
    auto bob = ray::Actor(PingPong::Factory, 2).Remote();

    alice.Task(&PingPong::registerPartner, bob).Remote();
    bob.Task(&PingPong::registerPartner, alice).Remote();

    auto task = alice.Task(&PingPong::ping).Remote();
    ray::Wait({task});

    ray::Shutdown();
    return 0;
}
