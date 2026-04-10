#include "mock_util_lowlevel.h"

int mock_usleep_called = 0;
int mock_usleep_value = 0;

int usleep(useconds_t usec) {
    mock_usleep_called++;
    mock_usleep_value = (int)usec;
    return 0;
}
