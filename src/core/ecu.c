#include "ecu.h"
#include "network.h"
#include "config.h"
#include <string.h>
#include <unistd.h>

bool ecu_init(ecu_t *ecu, const char *name, const char *bus_host, uint16_t bus_port) {
    memset(ecu, 0, sizeof(*ecu));
    strncpy(ecu->name, name, sizeof(ecu->name) - 1);
    ecu->sockfd = net_connect_to_bus(bus_host, bus_port);
    if (ecu->sockfd < 0) {
        log_msg(LOG_ERROR, "[ECU %s] Failed to connect to bus\n", name);
        return false;
    }
    if (!hsm_init(&ecu->hsm, name)) {
        close(ecu->sockfd);
        return false;
    }
    if (strcmp(name, "wheel_fl") == 0) ecu->ecu_id = 1;
    else if (strcmp(name, "wheel_fr") == 0) ecu->ecu_id = 2;
    else if (strcmp(name, "wheel_rl") == 0) ecu->ecu_id = 3;
    else if (strcmp(name, "wheel_rr") == 0) ecu->ecu_id = 4;
    else if (strcmp(name, "engine") == 0)   ecu->ecu_id = 5;
    else if (strcmp(name, "steering_sensor") == 0) ecu->ecu_id = 6;
    else if (strcmp(name, "steering_controller") == 0) ecu->ecu_id = 7;
    else if (strcmp(name, "brake_controller") == 0) ecu->ecu_id = 8;
    else ecu->ecu_id = 99; // unknown

    ecu->fail_safe = false;
    return true;
}

bool ecu_send(ecu_t *ecu, can_frame_t *frame) {
    if (ecu->fail_safe) return false;

    if (!frame->secured) {
        // unsecured frame – immediate fail-safe
        ids_state_t st = ids_update_on_unsecured(&ecu->ids);
        if (st == IDS_FAIL_SAFE) {
            ecu->fail_safe = true;
            log_msg(LOG_ERROR, "[ECU %s] FAIL-SAFE due to unsecured frame\n", ecu->name);
            return false;
        }
    } else {
        hsm_protect_frame(&ecu->hsm, frame);
    }

    return net_send_frame(ecu->sockfd, frame);
}

bool ecu_recv(ecu_t *ecu, can_frame_t *frame) {
    if (!net_recv_frame(ecu->sockfd, frame)) {
        return false;
    }

    if (!frame->secured) {
        ids_state_t st = ids_update_on_unsecured(&ecu->ids);
        if (st == IDS_FAIL_SAFE) {
            ecu->fail_safe = true;
            log_msg(LOG_ERROR, "[ECU %s] FAIL-SAFE due to unsecured incoming frame\n", ecu->name);
        }
        return true;
    }

    if (!hsm_verify_frame(&ecu->hsm, frame)) {
        // MAC or CRC error – treat as MAC error for simplicity
        ids_state_t st = ids_update_on_mac_error(&ecu->ids);
        if (st == IDS_FAIL_SAFE) {
            ecu->fail_safe = true;
            log_msg(LOG_ERROR, "[ECU %s] FAIL-SAFE due to MAC/CRC errors\n", ecu->name);
        }
    }
    return true;
}

