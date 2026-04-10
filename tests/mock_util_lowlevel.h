#ifndef MOCK_UTIL_LOWLEVEL_H
#define MOCK_UTIL_LOWLEVEL_H

#include <unistd.h>   // for useconds_t
#include <stdint.h>

extern int mock_usleep_called;
extern int mock_usleep_value;   // <- what tests expect

int usleep(useconds_t usec);

#endif
