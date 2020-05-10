#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <stdbool.h>
#include <unistd.h>
#define WAITING_MAX 10

sem_t waiting_room;
sem_t barber_ready, customer_ready;
int num_waiting = 0;
int num_serviced = 0;

// Barber sleeps until there is someone to service
void *barber_routine() {
    while (true) {
        if (num_waiting == 0) {
            printf("barber is sleeping\n");
            fflush(stdout);
        }
        sem_wait(&customer_ready);
        sem_post(&barber_ready);
        --num_waiting;
    }

    return NULL;
}

// Customer checks to see if there is an available seat then waits for the barber
void *customer_routine(void *arg) {
    int id = *(int *)arg;
    free(arg);
    if (num_waiting >= WAITING_MAX) {
        printf("customer %d leaving\n", id);
        fflush(stdout);
        return NULL;
    }
    sem_wait(&waiting_room);
    ++num_waiting;
    sem_post(&customer_ready);
    sem_wait(&barber_ready);
    sem_post(&waiting_room);
    printf("customer %d getting a haircut\n", id);
    fflush(stdout);
    ++num_serviced;
    return NULL;
}

// Run the program
int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Incorrect number of arguments. Exiting...\n");
        fflush(stdout);
        exit(1);
    }

    sem_init(&waiting_room, 0, WAITING_MAX);
    sem_init(&customer_ready, 0, 0);
    sem_init(&barber_ready, 0, 1);

    pthread_t barber;
    pthread_create(&barber, NULL, barber_routine, NULL);

    sleep(3);

    int num_customers = atoi(argv[1]);
    pthread_t customers[num_customers];
    for (int i = 0; i < num_customers; ++i) {
        int *arg = malloc(sizeof(int));
        *arg = i;
        pthread_create(&customers[i], NULL, customer_routine, (void *)arg);
    }

    for (int i = 0; i < num_customers; ++i) {
        pthread_join(customers[i], NULL);
    }

    pthread_kill(barber, 0);

    printf("execution complete\n");
    fflush(stdout);
    return 0;
}