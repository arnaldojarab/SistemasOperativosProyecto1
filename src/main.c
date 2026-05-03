#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "truck.h"
#include "scheduler.h"
#include "logger.h"

int algorithm = 0;
int quantum   = 0;
int num_trucks = 5;
int num_docks  = 3;

sem_t docks;

void print_usage(const char *prog) {
    printf("Usage: %s [-n trucks] [-m docks] [-a fifo|rr] [-q quantum]\n", prog);
    exit(1);
}

int main(int argc, char *argv[]) {
    int c;

    while ((c = getopt(argc, argv, "n:m:a:q:")) != -1) {
        switch (c) {
            case 'n':
                num_trucks = atoi(optarg);
                break;
            case 'm':
                num_docks = atoi(optarg);
                break;
            case 'a':
                if (strcmp(optarg, "fifo") == 0) {
                    algorithm = 0;
                } else if (strcmp(optarg, "rr") == 0) {
                    algorithm = 1;
                } else {
                    printf("Invalid algorithm: %s\n", optarg);
                }
                break;
            case 'q':
                quantum = atoi(optarg);
                break;
            default:
                print_usage(argv[0]);
        }
    }

    if (algorithm == 1 && quantum <= 0) {
        printf("Error: Round Robin requires -q (quantum) > 0\n");
        exit(1);
    }

    if (num_trucks <= 0 || num_docks <= 0) {
        printf("Error: -n and -m must be > 0\n");
        exit(1);
    }

    printf("Current configuration: Trucks=%d, Docks=%d, Algorithm=%s, Quantum=%d\n",
           num_trucks, num_docks, algorithm == 0 ? "fifo" : "rr", quantum);

    srand(time(NULL));

    logger_init("terminal.log");
    sem_init(&docks, 0, num_docks);
    scheduler_init(algorithm, quantum);

    Truck *trucks  = malloc(sizeof(Truck) * num_trucks);
    pthread_t *threads = malloc(sizeof(pthread_t) * num_trucks);

    for (int i = 0; i < num_trucks; i++) {
        int load_time = (rand() % 8) + 2;
        truck_init(&trucks[i], i, load_time);
        pthread_create(&threads[i], NULL, run_truck, &trucks[i]);
    }

    int initial = num_docks < num_trucks ? num_docks : num_trucks;
    for (int i = 0; i < initial; i++) {
        schedule_next();
    }

    for (int i = 0; i < num_trucks; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("\n%-8s %-10s %-12s %-12s\n", "Truck", "Load(s)", "Wait(s)", "Turnaround(s)");
    printf("--------------------------------------------\n");

    double total_wait       = 0;
    double total_turnaround = 0;

    for (int i = 0; i < num_trucks; i++) {
        double turnaround = difftime(trucks[i].finish_time, trucks[i].arrival_time);
        double waiting    = turnaround - trucks[i].load_time;
        printf("%-8d %-10d %-12.0f %-12.0f\n",
               trucks[i].id, trucks[i].load_time, waiting, turnaround);
        logger_log("Truck %d: load=%ds wait=%.0fs turnaround=%.0fs",
                   trucks[i].id, trucks[i].load_time, waiting, turnaround);
        total_wait       += waiting;
        total_turnaround += turnaround;
    }

    printf("--------------------------------------------\n");
    printf("Average wait: %.2f s | Average turnaround: %.2f s\n",
           total_wait / num_trucks, total_turnaround / num_trucks);
    logger_log("Average wait: %.2f s | Average turnaround: %.2f s",
               total_wait / num_trucks, total_turnaround / num_trucks);

    sem_destroy(&docks);
    free(trucks);
    free(threads);
    logger_destroy();

    return 0;
}