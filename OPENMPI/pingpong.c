#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

typedef struct {
    int ping_count;
    int rank;
    pthread_mutex_t lock;
} PingPong;

void *ping(void *arg);
void *pong(void *arg);

int main() {
    pthread_t thread1, thread2;
    PingPong alice, bob;

    alice.rank = 1;
    alice.ping_count = 0;
    pthread_mutex_init(&alice.lock, NULL);

    bob.rank = 2;
    bob.ping_count = 0;
    pthread_mutex_init(&bob.lock, NULL);

    printf("Start!\n");

    pthread_create(&thread1, NULL, ping, (void *)&alice);
    pthread_create(&thread2, NULL, pong, (void *)&bob);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    pthread_mutex_destroy(&alice.lock);
    pthread_mutex_destroy(&bob.lock);

    printf("Complete!\n");
    return 0;
}

void *ping(void *arg) {
    PingPong *pp = (PingPong *)arg;
    while (1) {
        pthread_mutex_lock(&pp->lock);
        if (pp->ping_count >= 5) {
            pthread_mutex_unlock(&pp->lock);
            break;
        }
        printf("Ping from rank %d\n", pp->rank);
        pp->ping_count++;
        pthread_mutex_unlock(&pp->lock);
        sleep(1);
    }
    return NULL;
}

void *pong(void *arg) {
    PingPong *pp = (PingPong *)arg;
    while (1) {
        pthread_mutex_lock(&pp->lock);
        if (pp->ping_count >= 5) {
            pthread_mutex_unlock(&pp->lock);
            break;
        }
        printf("Pong from rank %d\n", pp->rank);
        pp->ping_count++;
        pthread_mutex_unlock(&pp->lock);
        sleep(1);
    }
    return NULL;
}
