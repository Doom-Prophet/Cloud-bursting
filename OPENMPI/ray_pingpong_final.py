import ray
import time

@ray.remote
class PingPong:
    def __init__(self, rank):
        self.ping_count = 0
        self.rank = rank

    def register(self, partner):
        self.partner = partner
    
    def ping(self):
        self.ping_count += 1
        if self.ping_count < 5: 
            print(f"Ping from rank {self.rank}")
            time.sleep(1) 
            return self.partner.pong.remote()

    def pong(self):
        print(f"Pong from rank {self.rank}")
        time.sleep(1) 
        return self.partner.ping.remote()

def main():
    # ray.init(address="192.168.20.203:6379", _redis_password="12345678")
    ray.init()

    print("Start!")
    alice = PingPong.remote(1)
    bob = PingPong.remote(2)

    ray.get(alice.register.remote(bob))
    ray.get(bob.register.remote(alice))

    print("Middle!")

    to_do = alice.ping.remote()
    while to_do != None:
        to_do = ray.get(to_do)
    
    print("Complete!")

if __name__ == "__main__":
    main()