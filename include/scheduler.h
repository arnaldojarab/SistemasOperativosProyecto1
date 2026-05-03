#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "truck.h"

#define FIFO 0
#define RR   1

void scheduler_init(int algorithm, int quantum);
void enqueue(Truck* c);
void schedule_next();
void notify_finish(Truck* c);
int get_execution_time(int remaining);

#endif
