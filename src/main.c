#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int algorithm = 0; 
int quantum   = 2;
int num_trucks = 5;
int num_docks  = 3;

void print_usage(const char *prog) {
    printf("Usage: %s [-n trucks] [-m docks] [-a fifo|rr] [-q quantum]\n", prog);
    exit(1);
}

int main(int argc, char *argv[]) {
    int c;
    
    while ((c = getopt(argc, argv, "n:m:a:q")) != -1) {
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

    printf("Current configuration: Trucks=%d, Docks=%d, Algorithm=%s, Quantum=%d\n", num_trucks,  num_docks, algorithm == 0 ? "fifo" : "rr", quantum);

    return 0;
}
