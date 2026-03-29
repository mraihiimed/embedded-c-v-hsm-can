#ifndef UTIL_H
#define UTIL_H
#include "config.h"
#include <stdarg.h>
#include <stdio.h>
#include "can.h"
void util_sleep_ms(int ms);
void log_msg(log_level_t level, const char *fmt, ...) ;
#endif
