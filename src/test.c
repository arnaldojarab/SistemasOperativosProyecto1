#include "logger.h"
#include <pthread.h>

void* camion(void* arg) {
    int id = *(int*)arg;

    logger_log("Camión %d intentando acceder al muelle", id);
    logger_log("Camión %d entrando al muelle", id);
    logger_log("Camión %d saliendo del muelle", id);

    return NULL;
}

int main() {
    logger_init("log.txt");

    pthread_t hilos[3];
    int ids[3] = {1, 2, 3};

    for (int i = 0; i < 3; i++) {
        pthread_create(&hilos[i], NULL, camion, &ids[i]);
    }

    for (int i = 0; i < 3; i++) {
        pthread_join(hilos[i], NULL);
    }

    logger_destroy();
    return 0;
}