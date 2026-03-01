#ifndef IDS_H
#define IDS_H

#include <stdint.h>
#include <stdbool.h>
#include "can.h"

typedef struct {
    uint32_t mac_errors;
    uint32_t crc_errors;
    uint32_t unsecured_frames;
} ids_counters_t;

typedef enum {
    IDS_OK = 0,
    IDS_FAIL_SAFE
} ids_state_t;

ids_state_t ids_update_on_mac_error(ids_counters_t *counters);
ids_state_t ids_update_on_crc_error(ids_counters_t *counters);
ids_state_t ids_update_on_unsecured(ids_counters_t *counters);

#endif

