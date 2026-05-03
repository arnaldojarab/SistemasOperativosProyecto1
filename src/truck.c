#include "truck.h"
#include "scheduler.h"
#include "logger.h"
#include <unistd.h>

void truck_init(Truck* t, int id, int load_time) {
    t->id             = id;
    t->state          = NEW;
    t->load_time      = load_time;
    t->remaining_time = load_time;
    sem_init(&t->sem_turno, 0, 0);
}

void* run_truck(void* arg) {
    Truck* t = (Truck*)arg;

    t->state = READY;
    logger_log("Truck %d: NEW -> READY", t->id);

    while (t->remaining_time > 0) {
        t->state = BLOCKED;
        logger_log("Truck %d: READY -> BLOCKED (waiting for dock)", t->id);

        enqueue(t);
        sem_wait(&t->sem_turno);

        t->state = RUNNING;
        logger_log("Truck %d: BLOCKED -> RUNNING (dock assigned)", t->id);

        int slot = get_execution_time(t->remaining_time);
        sleep(slot);
        t->remaining_time -= slot;

        notify_finish(t);

        if (t->remaining_time > 0) {
            logger_log("Truck %d: remaining time = %d s (re-queuing)", t->id, t->remaining_time);
        }
    }

    t->state = DONE;
    logger_log("Truck %d: RUNNING -> DONE", t->id);

    sem_destroy(&t->sem_turno);
    return NULL;
}