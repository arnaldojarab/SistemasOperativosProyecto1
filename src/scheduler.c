#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include "scheduler.h"


#define MAX_QUEUE 100

static Camion* queue[MAX_QUEUE];
static int front = 0;
static int rear = 0;

static pthread_mutex_t queue_mutex;

static int algorithm;
static int quantum;


static int is_empty() {
	return front == rear;
}

static void enqueue_internal(Camion* c) {
	if (rear < MAX_QUEUE) {
		queue[rear++] = c;
	}
}

static Camion* dequeue_internal() {
	if (is_empty()) return NULL;
	return queue[front++];
}

static void schedule_next_internal() {
	Camion* c = dequeue_internal();

	if (c != NULL) {
		sem_post(&c->sem_turno);  
	}
}



void scheduler_init(int alg, int q) {
	algorithm = alg;
	quantum = q;

	pthread_mutex_init(&queue_mutex, NULL);

	front = 0;
	rear = 0;
}

void enqueue(Camion* c) {
	pthread_mutex_lock(&queue_mutex);

	enqueue_internal(c);

	pthread_mutex_unlock(&queue_mutex);
}

void schedule_next() {
	pthread_mutex_lock(&queue_mutex);

	schedule_next_internal();

	pthread_mutex_unlock(&queue_mutex);
}

void notify_finish(Camion* c) {
	pthread_mutex_lock(&queue_mutex);

	if (algorithm == RR && c->tiempo_restante > 0) {
		enqueue_internal(c);
	}

	schedule_next_internal();

	pthread_mutex_unlock(&queue_mutex);
}

int get_execution_time(int remaining) {
	if (algorithm == FIFO) {
		return remaining;
	} else {
		if (remaining < quantum) {
			return remaining;
		} else {
			return quantum;
		}
	}
}
