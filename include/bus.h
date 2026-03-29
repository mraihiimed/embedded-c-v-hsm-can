#ifndef BUS_H
#define BUS_H

#include "can_types.h"
#include "can.h"
int bus_init(void);
int bus_send_frame(const can_frame_t *frame);
void bus_server_run(uint16_t port_bus, uint16_t port_monitor);

#endif

