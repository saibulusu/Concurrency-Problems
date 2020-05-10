#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <signal.h>

// 0 - Tobacco holder, 1 - Paper holder, 2 - Match holder
pthread_t smokers[3];
sem_t locks[3];
sem_t finished;

// Tobacco holder
void *t() {
    while (true) {
        sem_wait(&locks[0]);

        printf("Tobacco holder smoking\n");
        fflush(stdout);

        sem_post(&finished);
    }

    return NULL;
}

// Paper holder
void *p() {
    while (true) {
        sem_wait(&locks[1]);

        printf("Paper holder smoking\n");
        fflush(stdout);

        sem_post(&finished);
    }

    return NULL;
}

// Match holder
void *m() {
    while (true) {
        sem_wait(&locks[2]);

        printf("Match holder smoking\n");
        fflush(stdout);

        sem_post(&finished);
    }

    return NULL;
}

// Agent creates 100 cigarettes
void *a() {            
    for (int i = 0; i < 3; ++i) {
        sem_init(&locks[i], 0, 1);
        sem_wait(&locks[i]);
    }
    sem_init(&finished, 0, 0);

    pthread_create(&smokers[0], NULL, t, NULL);
    pthread_create(&smokers[1], NULL, p, NULL);
    pthread_create(&smokers[2], NULL, m, NULL);

    int *val = malloc(sizeof(int));

    for (int i = 0; i < 100; ++i) {
        printf("creating cigarette %d\n", i);
        fflush(stdout);
        int choose = rand() % 3;
        sem_post(&locks[choose]);
        sem_wait(&finished);
    }

    for (int i = 0; i < 3; ++i) {
        pthread_kill(smokers[i], 0);
    }

    free(val);
    return NULL;
}

// Run the program
int main(int argc, char* argv[]) {
    pthread_t agent;
    pthread_create(&agent, NULL, a, NULL);

    pthread_join(agent, NULL);

    printf("completed execution\n");
    return 0;
}