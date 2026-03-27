#include "bus.h"
#include "config.h"

int main(void) {
    bus_server_run(CAN_PORT_BUS_SERVER, CAN_PORT_MONITOR);
    return 0;
}

