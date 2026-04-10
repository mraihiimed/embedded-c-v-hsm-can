#ifndef MOCK_LOG_H
#define MOCK_LOG_H

#include "util.h"

#define MOCK_LOG_BUFFER_SIZE 1024

extern char mock_log_buffer[MOCK_LOG_BUFFER_SIZE];
extern int mock_log_index;

void log_msg(log_level_t level, const char *fmt, ...);

#endif
