#include "config.h"
#include <stdarg.h>
#include <stdio.h>
#include "can.h"

#ifndef UTIL_H
#define UTIL_H
#include "config.h"
#endif
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

