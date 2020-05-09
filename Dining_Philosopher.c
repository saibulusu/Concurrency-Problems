#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

sem_t* forks;
int* num_philosopers;
int iterations = 5000;

// Acquire both the left and right forks
void getforks(int id) {
    if (id != 0) {
        sem_wait(&forks[id]);
        sem_wait(&forks[(id + 1) % *num_philosopers]);
    } else {
        sem_post(&forks[(id + 1) % *num_philosopers]);
        sem_post(&forks[id]);
    }
}

// Release both the left and right forks
void putforks(int id) {
    sem_post(&forks[id]);
    sem_post(&forks[(id + 1) % *num_philosopers]);
}

// Eat using both forks
void eat(int id) {
    printf("philosopher %d eating\n", id);
    fflush(stdout);
}

// Think using neither fork
void think(int id) {
    printf("philosopher %d thinking\n", id);
    fflush(stdout);
}

// Loop while acquring and releasing forks
void *routine(void* args) {
    int id = *(int*)args;
    free(args);

    while (iterations > 0) {
        think(id);
        getforks(id);
        eat(id);
        putforks(id);
        --iterations;
    }

    return NULL;
}

// Run the program
int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Incorrect number of arguments. Terminating...\n");
        fflush(stdout);
        exit(1);
    }

    num_philosopers = malloc(sizeof(int));
    *num_philosopers = atoi(argv[1]);
    forks = malloc(sizeof(sem_t) * *num_philosopers);

    for (int i = 0;  i < *num_philosopers; ++i) {
        sem_init(&forks[i], 0, 1);
    }

    pthread_t philosophers[*num_philosopers];
    for (int i = 0; i < *num_philosopers; ++i) {
        int* arg = malloc(sizeof(int));
        *arg = i;
        pthread_create(&philosophers[i], NULL, routine, (void *)arg);
    }

    for (int i = 0; i < *num_philosopers; ++i) {
        pthread_join(philosophers[i], NULL);
    }

    free(forks);
    free(num_philosopers);
}