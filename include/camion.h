#ifndef CAMION_H
#define CAMION_H

#include <semaphore.h>

typedef enum {
    NUEVO,
    LISTO,
    EJECUCION,
    BLOQUEADO,
    TERMINADO
} EstadoCamion;

typedef struct {
    int id;
    EstadoCamion estado;
    int tiempo_carga;
    int tiempo_restante;
    sem_t sem_espera;
} Camion;

void  camion_init(Camion* c, int id, int tiempo_carga);
void* camion_hilo(void* arg);

#endif
