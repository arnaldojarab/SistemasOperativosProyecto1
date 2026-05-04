# Proyecto 1 de Sistemas Operativos

## Cómo ejecutar
```bash
make
./terminal -n (cantidad de camiones) -m (cantidad de muelles) -a (fifo/rr) -q (tiempo del quantum si se escogió Round Robin)
```

# Estructura del Proyecto

## ¿Qué hace cada archivo?

### `main.c`
Punto de entrada del programa. Se encarga de inicializar todos los módulos, crear los hilos de los camiones mediante `pthread_create`, lanzar el planificador y esperar a que todos los hilos terminen con `pthread_join`. No contiene lógica de negocio, solo orquesta todo.

### `truck.c` / `truck.h`
Define la estructura `Truck` y su ciclo de vida completo. Maneja los estados NEW → READY → RUNNING → BLOCKED → DONE. Aquí vive la función que ejecuta cada hilo cuando es creado.

### `scheduler.c` / `scheduler.h`
Contiene la lógica de los dos algoritmos de planificación. Implementa la cola de espera de camiones y decide el orden en que acceden a los muelles:
- **FIFO**: atiende por orden de llegada
- **Round Robin**: cada camión tiene un quantum máximo de tiempo en el muelle

### `logger.c` / `logger.h`
Módulo de registro de operaciones global. Protege el acceso al log con un `mutex` para evitar condiciones de carrera cuando varios hilos intentan escribir al mismo tiempo. Todos los demás módulos usan este logger para reportar eventos.

### `Makefile`
Automatiza la compilación. En lugar de compilar cada archivo a mano, basta con correr `make` en la terminal y genera el ejecutable completo enlazando todos los `.c`.

# Regiones Críticas

### logger.c
En la función `logger_log`, cuando un hilo intenta acceder al archivo de log se usa un `mutex_lock` para evitar race condition.

### scheduler.c
Las regiones críticas se encuentran en las funciones `enqueue` y `notify_finish`, donde se modifican variables compartidas como la cola (`queue`, `front`, `rear`) y el contador `active_count`. Estas se protegen mediante el uso del mutex `queue_mutex`, asegurando exclusión mutua y evitando condiciones de carrera.

### truck.c
El acceso al muelle está controlado por un semáforo inicializado con la cantidad de muelles disponibles. Cada camión decrementa el semáforo al entrar y lo incrementa al salir, garantizando que nunca más camiones que muelles disponibles operen simultáneamente.

# Prevención de Interbloqueo (Deadlock)

Durante el desarrollo se identificaron y resolvieron dos situaciones que causaban deadlock:

**1. Activación inicial del planificador**
El primer problema ocurría al arrancar el programa: el planificador intentaba asignar un muelle antes de que ningún camión hubiera llegado a la cola de espera, por lo que no había nadie a quien asignarle el turno. Los camiones quedaban bloqueados esperando una señal que nunca llegaba. La solución fue hacer que el planificador active al siguiente camión en el momento exacto en que ese camión se registra en la cola, verificando si hay muelles disponibles en ese instante. Así se elimina la ventana de tiempo donde existían muelles libres pero ningún hilo era notificado.

**2. Registro doble de camiones en Round Robin**
El segundo problema era específico del algoritmo Round Robin. Cuando un camión agotaba su quantum de tiempo y debía volver a la cola de espera, el sistema lo registraba dos veces: una vez desde el planificador y otra vez desde el propio hilo del camión al reiniciar su ciclo. Tener el mismo camión duplicado en la cola corrompía el contador de camiones activos, haciendo que el sistema creyera que había más camiones ocupando muelles de los que realmente había. Esto eventualmente dejaba hilos bloqueados esperando un turno que el planificador nunca les daba. La solución fue dejar que cada camión se registre en la cola por su propia cuenta al volver a solicitar un muelle, eliminando el registro duplicado desde el planificador.