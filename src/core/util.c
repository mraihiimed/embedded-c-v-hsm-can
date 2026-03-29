#include "config.h"
#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>
#include "util.h"

void util_sleep_ms(int ms)
{
    if (ms <= 0)
        return;

    usleep(ms * 1000);   // convert ms → microseconds
}


void log_msg(log_level_t level, const char *fmt, ...) {
    const char *tag = "";
    switch (level) {
        case LOG_DEBUG: tag = "DEBUG"; break;
        case LOG_INFO:  tag = "INFO";  break;
        case LOG_WARN:  tag = "WARN";  break;
        case LOG_ERROR: tag = "ERROR"; break;
    }
    va_list ap;
    va_start(ap, fmt);
    fprintf(stderr, "[%s] ", tag);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
}

