#include "mock_log.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

char mock_log_buffer[MOCK_LOG_BUFFER_SIZE];
int mock_log_index = 0;

void log_msg(log_level_t level, const char *fmt, ...)
{
    (void)level;

    va_list args;
    va_start(args, fmt);

    mock_log_index += vsnprintf(
        mock_log_buffer + mock_log_index,
        MOCK_LOG_BUFFER_SIZE - mock_log_index,
        fmt,
        args
    );

    va_end(args);
}
