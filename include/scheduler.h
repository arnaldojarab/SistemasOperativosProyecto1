#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "truck.h"

#define FIFO 0
#define RR   1

extern sem_t docks;

void scheduler_init(int algorithm, int quantum, int num_docks);
void enqueue(Truck* t);
void notify_finish(Truck* t);
int get_execution_time(int remaining);

#endif
