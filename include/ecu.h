#ifndef ECU_H
#define ECU_H

#include "hsm.h"
#include "ids.h"
#include "can_types.h"
#include "can.h"

typedef struct {
    char          name[32];
    int           sockfd;
    hsm_context_t hsm;
    ids_counters_t ids;
    bool          fail_safe;
} ecu_t;

bool ecu_init(ecu_t *ecu, const char *name, const char *bus_host, uint16_t bus_port);
bool ecu_send(ecu_t *ecu, can_frame_t *frame);
bool ecu_recv(ecu_t *ecu, can_frame_t *frame);

#endif

