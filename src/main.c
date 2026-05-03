#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "truck.h"
#include "scheduler.h"

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

    printf("Current configuration: Trucks=%d, Docks=%d, Algorithm=%s, Quantum=%d\n", num_trucks,  num_docks, algorithm == 0 ? "fifo" : "rr", quantum);

	srand(time(NULL));

	
	
	sem_init(&docks, 0, num_docks);
    scheduler_init(algorithm, quantum);

	Truck *trucks = malloc(sizeof(Truck) * num_trucks);
    pthread_t *threads = malloc(sizeof(pthread_t) * num_trucks);

	for (int i = 0; i < num_trucks; i++) {
		int load_time = (rand() % 8) + 2;

    	truck_init(&trucks[i], i, load_time); 

    	pthread_create(&threads[i], NULL, run_truck, &trucks[i]);
    }
	
	schedule_next();
	
	for (int i = 0; i < num_trucks; i++) {
		pthread_join(threads[i], NULL);
	}



	sem_destroy(&docks);
	free(trucks);
	free(threads);
	

    return 0;
}
