#include "logger.h"
#include <stdio.h>
#include <pthread.h>
#include <stdarg.h>
#include <time.h>

static FILE* log_file = NULL;
static pthread_mutex_t log_mutex;

void logger_init(const char* filename) {
    log_file = fopen(filename, "w");
    if (!log_file) {
        perror("Error abriendo log");
        return;
    }
    pthread_mutex_init(&log_mutex, NULL);
}


void logger_log(const char* format, ...) {
    if ( !log_file) return;

    char message[256];

    va_list args;
    va_start(args, format);
    vsnprintf(message, sizeof(message), format, args);
    va_end(args);

    time_t now = time(NULL);
    struct tm* t = localtime(&now);

    //mutex que evita que dos hilos escriban al mismo tiempo
    pthread_mutex_lock(&log_mutex);

    // Region critica
    fprintf(log_file, "[%02d:%02d:%02d] [T%lu] %s\n",
            t->tm_hour, t->tm_min, t->tm_sec,
            (unsigned long)pthread_self(),
            message);

    fflush(log_file);

    pthread_mutex_unlock(&log_mutex);
}

void logger_destroy() {
    pthread_mutex_destroy(&log_mutex);
    fclose(log_file);
}