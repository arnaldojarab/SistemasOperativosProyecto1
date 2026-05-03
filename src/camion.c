#include "camion.h"
#include "planificador.h"
#include "logger.h"
#include <unistd.h>

void camion_init(Camion* c, int id, int tiempo_carga) {
    c->id              = id;
    c->estado          = NUEVO;
    c->tiempo_carga    = tiempo_carga;
    c->tiempo_restante = tiempo_carga;
    sem_init(&c->sem_espera, 0, 0);
}

void* camion_hilo(void* arg) {
    Camion* c = (Camion*)arg;

    c->estado = LISTO;
    logger_log("Camion %d: NUEVO -> LISTO", c->id);

    while (c->tiempo_restante > 0) {
        c->estado = BLOQUEADO;
        logger_log("Camion %d: LISTO -> BLOQUEADO (esperando muelle libre)", c->id);

        planificador_agregar_a_cola(c);
        sem_wait(&c->sem_espera);

        c->estado = EJECUCION;
        logger_log("Camion %d: BLOQUEADO -> EJECUCION (muelle asignado)", c->id);

        int turno = planificador_tiempo_turno(c->tiempo_restante);
        sleep(turno);
        c->tiempo_restante -= turno;

        planificador_liberar_muelle(c);

        if (c->tiempo_restante > 0) {
            logger_log("Camion %d: tiempo restante = %d s (re-encolando)", c->id, c->tiempo_restante);
        }
    }

    c->estado = TERMINADO;
    logger_log("Camion %d: EJECUCION -> TERMINADO", c->id);

    sem_destroy(&c->sem_espera);
    return NULL;
}
