# Proyecto 1 de Sistemas Operativos

# Estructura del Proyecto

## ¿Qué hace cada archivo?

### `main.c`
Punto de entrada del programa. Se encarga de inicializar todos los módulos, crear los hilos de los camiones mediante `pthread_create`, lanzar el planificador y esperar a que todos los hilos terminen con `pthread_join`. No contiene lógica de negocio, solo orquesta todo.

### `camion.c` / `camion.h`
Define la estructura `Camion` y su ciclo de vida completo. Maneja los estados NUEVO → LISTO → EJECUCIÓN → BLOQUEADO → TERMINADO. Aquí vive la función que ejecuta cada hilo cuando es creado.

### `planificador.c` / `planificador.h`
Contiene la lógica de los dos algoritmos de planificación. Implementa la cola de espera de camiones y decide el orden en que acceden a los muelles:
- **FIFO**: atiende por orden de llegada
- **Round Robin**: cada camión tiene un quantum máximo de tiempo en el muelle

### `logger.c` / `logger.h`
Módulo de registro de operaciones global. Protege el acceso al log con un `mutex` para evitar condiciones de carrera cuando varios hilos intentan escribir al mismo tiempo. Todos los demás módulos usan este logger para reportar eventos.

### `Makefile`
Automatiza la compilación. En lugar de compilar cada archivo a mano, basta con correr `make` en la terminal y genera el ejecutable completo enlazando todos los `.c`.