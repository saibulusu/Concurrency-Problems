#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include "LinkedList.c"

struct ListNode* list;
pthread_mutex_t* lock;

sem_t* mutex;
sem_t* empty;
sem_t* full;

int cur = 0;
int limit = 100;

void put() {
    pthread_mutex_lock(lock);
    insert(list, cur);
    ++cur;
    pthread_mutex_unlock(lock);
}

void get() {
    printf("calling get\n");
    fflush(stdout);
    pthread_mutex_lock(lock);
    delete(list);
    pthread_mutex_unlock(lock);
	printf("ending get\n");
	fflush(stdout);
}

void* producer() {
	printf("staring producer\n");
	fflush(stdout);
    while (cur < limit) {
        sem_wait(empty);
		sem_wait(mutex);
		put();
    	sem_post(mutex);
		sem_post(full);
	}
    printf("cur reached limit");
    fflush(stdout);
    return NULL;
}

void* consumer() {
    while (cur < limit) {
        sem_wait(full);
        sem_wait(mutex);
		get();
    	sem_post(mutex);
		sem_post(empty);
	}
    return NULL;
}

int main(int argc, char* argv[]) {
	insert(list, 0);
	printf("%d\n", list->value);
}

int main2(int argc, char* argv[]) {
    int num_producers = 1;
    int num_consumers = 1;
    int MAX = 10;

	lock = malloc(sizeof(pthread_mutex_t));

    mutex = malloc(sizeof(sem_t));
    empty = malloc(sizeof(sem_t));
    full = malloc(sizeof(sem_t));

    sem_init(mutex, 0, 1);
   	sem_init(empty, 0, MAX);
    sem_init(full, 0, 0);

    pthread_t producers[num_producers];
    for (int i = 0; i < num_producers; ++i) {
        pthread_create(&producers[i], NULL, producer, NULL);
    }

    pthread_t consumers[num_consumers];
    for (int i = 0; i < num_consumers; ++i) {
        pthread_create(&consumers[i], NULL, consumer, NULL);
    }

	printf("created all threads\n");
	fflush(stdout);

    for (int i = 0; i < num_producers; ++i) {
         pthread_join(producers[i], NULL);
    }

    for (int i = 0; i < num_consumers; ++i) {
        pthread_join(consumers[i], NULL);
    }

	printf("execution done\n");
	fflush(stdout);

    return 0;
}
