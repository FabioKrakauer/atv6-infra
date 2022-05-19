#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>

# define TRUE 1

int size = 0;
int *buffer;

int totalConsumers = 0;
int totalProducers = 0;
int limit = 0;
int in, out = 0;

int xV = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
sem_t empty, full;

void *producer(void *id) {
    int *producer_id = id;
    while(TRUE) {
        sem_wait(&empty);
        pthread_mutex_lock(&mutex);

        if(xV > limit) {
            xV = 0;
        }
        int value = 2 * xV + 1;
        xV = (xV + 1) % limit + 1;
        buffer[in] = value;

        printf("Produtor %d produzindo %d na posicao %d\n", *producer_id, value, in);

        in = (in + 1)%size;
        pthread_mutex_unlock(&mutex);
        sem_post(&full);
        sleep(1);
        
    }
}
void consumer(void* id){
    
    int* consumerId = id; 
    while(1){
        sem_wait(&full);
        pthread_mutex_lock(&mutex);

        xV = buffer[out];
        printf("Consumidor %d consumindo %d na posição %d\n", *consumerId, xV, out);
        out = (out + 1) % size;
        
        pthread_mutex_unlock(&mutex);
        sem_post(&empty);
        sleep(1);
        
    }
}
int main() {
    scanf("%d %d %d %d", &totalProducers, &totalConsumers, &limit, &size);
    buffer = calloc(size, sizeof(int));
    int consumers[size];
    int producers[size];
    pthread_t consumersThreads[totalConsumers];
    pthread_t producersThreads[totalProducers];

    sem_init(&empty, 0, size);
    sem_init(&full, 0, 0);
    //start producers and consumers
    for(int i = 0; i < totalConsumers; i++) {
        consumers[i] = i;
    }
    for(int i = 0; i < totalProducers; i++) {
        producers[i] = i;
    }
    // Start threads
    for(int i = 0; i < totalProducers; i++) {
        pthread_create(&(producersThreads[i]), NULL, (void *)producer, &producers[i]);
    }
    for(int i = 0; i < totalConsumers; i++) {
        pthread_create(&(consumersThreads[i]), NULL, (void *)consumer, &consumers[i]);
    }

    // Join threads
    for(int i = 0; i < totalProducers; i++) {
        pthread_join(producersThreads[i], NULL);
    }
    for(int i = 0; i < totalConsumers; i++) {
        pthread_join(consumersThreads[i], NULL);
    }

    printf("success!");
    return 0;
}