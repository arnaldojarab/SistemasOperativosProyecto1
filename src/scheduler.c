#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include "scheduler.h"

#define MAX_QUEUE 100

static Truck* queue[MAX_QUEUE];
static int front = 0;
static int rear  = 0;

static pthread_mutex_t queue_mutex;

static int algorithm;
static int quantum;
static int num_docks;
static int active_count = 0;


static int is_empty() {
	return front == rear;
}

static void enqueue_internal(Truck* t) {
	if (rear < MAX_QUEUE) {
		queue[rear++] = t;
	}
}

static Truck* dequeue_internal() {
	if (is_empty()) return NULL;
	return queue[front++];
}

static void schedule_next_internal() {
	Truck* t = dequeue_internal();
	if (t != NULL) {
		active_count++;
		sem_post(&t->sem_turn);
	}
}

void scheduler_init(int alg, int q, int ndocks) {
	algorithm    = alg;
	quantum      = q;
	num_docks    = ndocks;
	active_count = 0;

	pthread_mutex_init(&queue_mutex, NULL);

	front = 0;
	rear  = 0;
}

void enqueue(Truck* t) {
	pthread_mutex_lock(&queue_mutex);

	enqueue_internal(t);

	if (active_count < num_docks) {
		schedule_next_internal();
	}

	pthread_mutex_unlock(&queue_mutex);
}

void notify_finish(Truck* t) {
	(void)t;
	pthread_mutex_lock(&queue_mutex);

	active_count--;

	if (!is_empty()) {
		schedule_next_internal();
	}

	pthread_mutex_unlock(&queue_mutex);
}

int get_execution_time(int remaining) {
	if (algorithm == FIFO) {
		return remaining;
	} else {
		return remaining < quantum ? remaining : quantum;
	}
}