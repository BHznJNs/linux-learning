#include <stdarg.h>
#include <stdio.h>

void logger(const char* type, const char* fmt, ...);
void panic(const char *fmt, ...);

// --- --- --- --- --- ---

void logger(const char* type, const char* fmt, ...) {
    va_list ap;
    char msg[256];

    if (!fmt) return;
    va_start(ap, fmt);
    vsnprintf(msg, sizeof(msg), fmt, ap);
    va_end(ap);

    printf("[%s] %s\n", type, msg);
}

void panic(const char *fmt, ...) {
    va_list ap;
    char msg[256];

    if (!fmt) return;
    va_start(ap, fmt);
    vsnprintf(msg, sizeof(msg), fmt, ap);
    va_end(ap);

    perror(msg);
    exit(1);
}

