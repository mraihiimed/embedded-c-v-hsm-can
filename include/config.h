#ifndef CONFIG_H
#define CONFIG_H
#include "can.h"

// CAN Bus server network configuration
#define CAN_IP "127.0.0.1"
#define CAN_MAX_DATA_LEN 8
//#define CAN_PORT_BUS_SERVER 5000
#define CAN_PORT_MONITOR   5001
#define CAN_PORT_BUS_SERVER 6000
#define ECU_NAME_MAX_LEN   32

// Simple log levels
typedef enum {
    LOG_DEBUG = 0,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR
} log_level_t;

void log_msg(log_level_t level, const char *fmt, ...);

#endif

