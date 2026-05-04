#include "truck.h"
#include "scheduler.h"
#include "logger.h"
#include <unistd.h>

void truck_init(Truck* t, int id, int load_time) {
    t->id             = id;
    t->state          = NEW;
    t->load_time      = load_time;
    t->remaining_time = load_time;
    t->arrival_time   = time(NULL);
    t->start_time     = 0;
    t->finish_time    = 0;
    sem_init(&t->sem_turn, 0, 0);
}

void* run_truck(void* arg) {
    Truck* t = (Truck*)arg;

    t->state = READY;
    logger_log("Truck %d: NEW -> READY", t->id);

    while (t->remaining_time > 0) {
        const char* prev = (t->state == READY) ? "READY" : "RUNNING";
        t->state = BLOCKED;
        logger_log("Truck %d: %s -> BLOCKED (waiting for dock)", t->id, prev);

        enqueue(t);
        // critical region
        sem_wait(&t->sem_turn);
        sem_wait(&docks);

        t->state = RUNNING;
        if (t->start_time == 0) t->start_time = time(NULL);
        logger_log("Truck %d: BLOCKED -> RUNNING (dock assigned)", t->id);

        int slot = get_execution_time(t->remaining_time);
        sleep(slot);
        t->remaining_time -= slot;

        sem_post(&docks);
        notify_finish(t);

        if (t->remaining_time > 0) {
            logger_log("Truck %d: remaining time = %d s (re-queuing)", t->id, t->remaining_time);
        }
    }

    t->finish_time = time(NULL);
    t->state = DONE;
    logger_log("Truck %d: RUNNING -> DONE", t->id);

    sem_destroy(&t->sem_turn);
    return NULL;
}