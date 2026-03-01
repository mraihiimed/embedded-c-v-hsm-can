#include "ids.h"

#define MAC_ERROR_THRESHOLD        3
#define CRC_ERROR_THRESHOLD        5
#define UNSECURED_FRAME_THRESHOLD  1

ids_state_t ids_update_on_mac_error(ids_counters_t *c) {
    c->mac_errors++;
    if (c->mac_errors >= MAC_ERROR_THRESHOLD) {
        return IDS_FAIL_SAFE;
    }
    return IDS_OK;
}

ids_state_t ids_update_on_crc_error(ids_counters_t *c) {
    c->crc_errors++;
    if (c->crc_errors >= CRC_ERROR_THRESHOLD) {
        return IDS_FAIL_SAFE;
    }
    return IDS_OK;
}

ids_state_t ids_update_on_unsecured(ids_counters_t *c) {
    c->unsecured_frames++;
    if (c->unsecured_frames >= UNSECURED_FRAME_THRESHOLD) {
        return IDS_FAIL_SAFE;
    }
    return IDS_OK;
}

