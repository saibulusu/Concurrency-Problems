#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#define SIZE 20
#define MAX 10

int buffer[SIZE];
int fill = 0, use = 0;

sem_t empty, full;
pthread_mutex_t mutex;

// Put a value into the end of the buffer
void put(int value) {
    buffer[fill % SIZE] = value;
    ++fill;
}

// Get the first available value from the buffer
int get() {
    int tmp = buffer[use % SIZE];
    ++use;
    return tmp;
}

// Consumer thread operation
void *consumer(void *arg) {
    for (int i = 0; i < MAX; ++i) {
        sem_wait(&full);
        pthread_mutex_lock(&mutex);
        get();
        printf("use = %d\n", use);
        fflush(stdout);
        pthread_mutex_unlock(&mutex);
        sem_post(&empty);
    }
    return NULL;
}

// Producer thread operation
void *producer(void *arg) {
    for (int i = 0; i < MAX; ++i) {
        sem_wait(&empty);
        pthread_mutex_lock(&mutex);
        put(i);
        printf("fill = %d\n", fill);
        fflush(stdout);
        pthread_mutex_unlock(&mutex);
        sem_post(&full);
    }
    return NULL;
}

// Run the program, indicate the number of producers and consumers
int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Incorrect number of arguments. Terminating...\n");
        fflush(stdout);
        exit(1);
    }

    sem_init(&empty, 0, SIZE);
    sem_init(&full, 0, 0);
    pthread_mutex_init(&mutex, NULL);

    int num_producers = atoi(argv[1]);
    int num_consumers = atoi(argv[2]);

    pthread_t producers[num_producers];
    for (int i = 0; i < num_producers; ++i) {
        pthread_create(&producers[i], NULL, producer, NULL);
    }

    pthread_t consumers[num_consumers];
    for (int i = 0; i < num_consumers; ++i) {
        pthread_create(&consumers[i], NULL, consumer, NULL);
    }

    for (int i = 0; i < num_producers; ++i) {
        pthread_join(producers[i], NULL);
    }

    for (int i = 0; i < num_consumers; ++i) {
        pthread_join(consumers[i], NULL);
    }

    printf("completed execution\n");
    fflush(stdout);

    return 0;
}