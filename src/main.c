#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int algoritmo = 0; 
int quantum   = 2;
int num_camiones = 5;
int num_muelles  = 3;

void print_usage(const char *prog) {
	printf("Uso: %s [-n camiones] [-m muelles] [-a fifo|rr] [-q quantum] \n", prog);
    	exit(1);
}

int main(int argc, char *argv[]) {
    	int c;
    
	while ((c = getopt(argc, argv, "n:m:a:q")) != -1) {
		switch (c) {
	    		case 'n':
				num_camiones = atoi(optarg);
				break;
	    		case 'm':
				num_muelles = atoi(optarg);
				break;
	    		case 'a':
				if (strcmp(optarg, "fifo") == 0) {
		    			algoritmo = 0;
				} else if (strcmp(optarg, "rr") == 0) {
		    			algoritmo = 1; 
		 		} else {
		    			printf("Algoritmo invalido: %s\n", optarg);
				}
				break;
	    		case 'q':
				quantum = atoi(optarg);
				break;
	    		default:
				print_usage(argv[0]);
		}
    	}
	printf("Configuracion actual: Camiones=%d, Muelles=%d, Algoritmo=%s, Quantum=%d\n", num_camiones, num_muelles, algoritmo == 0 ? "fifo" : "rr", quantum);
}
