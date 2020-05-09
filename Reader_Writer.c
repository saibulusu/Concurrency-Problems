#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <signal.h>
#define SIZE 10
#define MAX 5

int buffer[SIZE];
int fill = 0, cur = 0;

sem_t mutex, writeLock;
int readers_count = 0;

// Acquire the read lock
void aquire_readlock() {
    sem_wait(&mutex);
    ++readers_count;
    if (readers_count == 1) {
        sem_wait(&writeLock);
    }
    sem_post(&mutex);
}

// Release the read lock
void release_readlock() {
    sem_wait(&mutex);
    --readers_count;
    if (readers_count == 0) {
        sem_post(&writeLock);
    }
    sem_post(&mutex);
}

// Acquire the write lock
void acquire_writelock() {
    sem_wait(&writeLock);
}

// Release the write lock
void release_writelock() {
    sem_post(&writeLock);
}

// Read multiple values from the buffer
void *read() {
    aquire_readlock();
    for (int i = 0; i < fill; ++i) {
        printf("reading %d: %d\n", i, buffer[i]);
        fflush(stdout);
    }
    release_readlock();

    return NULL;
}

// Write a value into the buffer
void *write() {
    while (true) {
        acquire_writelock();
        buffer[fill] = cur;
        printf("writing %d: %d\n", fill, cur);
        fflush(stdout);
        ++fill;
        fill %= SIZE;
        ++cur;
        release_writelock();
    }
    return NULL;
}

// Run the program
int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Incorrect number of arguments. Terminating...\n");
        fflush(stdout);
        exit(1);
    }

    sem_init(&mutex, 0, 1);
    sem_init(&writeLock, 0, 1);

    int num_writers = atoi(argv[1]);
    int num_readers = atoi(argv[2]);

    pthread_t writers[num_writers];
    for (int i = 0; i < num_writers; ++i) {
        pthread_create(&writers[i], NULL, write, NULL);
    }

    pthread_t readers[num_readers];
    for (int i = 0; i < num_readers; ++i) {
        pthread_create(&readers[i], NULL, read, NULL);
    }

    // for (int i = 0; i < num_writers; ++i) {
    //     pthread_join(writers[i], NULL);
    // }

    // for (int i = 0; i < num_readers; ++i) {
    //     pthread_join(readers[i], NULL);
    // }

    while (cur < 200) {

    }
    for (int i = 0; i < num_readers; ++i) {
        pthread_kill(readers[i], 0);
    }

    for (int i = 0; i < num_writers; ++i) {
        pthread_kill(writers[i], 0);
    }

    printf("completed execution\n");
    fflush(stdout);
}