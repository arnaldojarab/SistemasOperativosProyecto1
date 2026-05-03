#ifndef TRUCK_H
#define TRUCK_H

#include <semaphore.h>
#include <time.h>

typedef enum {
    NEW,
    READY,
    RUNNING,
    BLOCKED,
    DONE
} TruckState;

typedef struct {
    int id;
    TruckState state;
    int load_time;
    int remaining_time;
    sem_t  sem_turn;
    time_t arrival_time;
    time_t start_time;
    time_t finish_time;
} Truck;

void  truck_init(Truck* t, int id, int load_time);
void* run_truck(void* arg);

#endif