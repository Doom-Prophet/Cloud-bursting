import ray
import time

@ray.remote
class Pinger:
    def __init__(self):
        self.pong_count = 0
        self.rank = 'A'

    def register(self, partner):
        self.partner = partner
    
    def ping(self):
        print(f"Pinger {self.rank} sending ping")
        time.sleep(1) 
        temp1 = self.partner.pong.remote()
        ray.get(temp1)

    def pong(self):
        self.pong_count += 1
        if self.pong_count < 5: 
            print(f"Pinger received pong {self.pong_count}")
            temp2 = self.partner.ping.remote()
            ray.get(temp2)

@ray.remote
class Ponger:
    def __init__(self):
        self.ping_count = 0
        self.rank = 'B'
    
    def register(self, partner):
        self.partner = partner

    def ping(self):
        self.ping_count += 1
        print(f"Ponger received ping {self.ping_count}")
        temp3 = self.partner.pong.remote()
        ray.get(temp3)

    def pong(self):
        print(f"Ponger {self.rank} sending pong")
        time.sleep(1) 
        temp4 = self.partner.ping.remote()
        ray.get(temp4)

def main():
    # ray.init(address="192.168.20.203:6379", _redis_password="12345678")
    ray.init()

    print("Start!")
    alice = Pinger.remote()
    bob = Ponger.remote()

    ray.get(alice.register.remote(bob))
    ray.get(bob.register.remote(alice))

    print("Middle!")

    ray.get(alice.ping.remote())
    print("Complete!")

if __name__ == "__main__":
    main()