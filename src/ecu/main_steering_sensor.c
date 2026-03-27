/*#include "ecu.h"
#include "config.h"
#include <unistd.h>

int main(void) {
    ecu_t ecu;
    if (!ecu_init(&ecu, "steering_sensor", "127.0.0.1", CAN_PORT_BUS_SERVER)) {
        return 1;
    }

    log_msg(LOG_INFO, "[STEER_SENS] Steering sensor started\n");

    int8_t angle = 0;

    while (!ecu.fail_safe) {
        can_frame_t frame = {0};
        frame.id = 0x150;
        frame.dlc = 1;
        frame.data[0] = (uint8_t)angle;
        frame.secured = true;

        ecu_send(&ecu, &frame);

        angle = (angle + 5) % 90;
        usleep(100000);
    }

    return 0;
}*/

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>

#include "config.h"
#include "can.h"
#include "network.h"

int main(void) {
    printf("[INFO] [STEER_SENS] Steering sensor starting\n");

    int sock = net_connect_to_bus(CAN_IP, CAN_PORT_BUS_SERVER);
    if (sock < 0) {
        printf("[ERROR] [STEER_SENS] Failed to connect to bus server\n");
        return 1;
    }

    int8_t angle = -30;
    int8_t step  = 5;

    while (1) {
        can_frame_t frame;
        memset(&frame, 0, sizeof(frame));

        frame.id  = CAN_ID_STEER;
        frame.dlc = 1;
        frame.data[0] = (uint8_t)angle;  // signed angle in monitor
        frame.secured = false;           // unsecured for now

        if (send(sock, &frame, sizeof(frame), 0) != sizeof(frame)) {
            printf("[WARN] [STEER_SENS] Failed to send frame\n");
        }

        angle += step;
        if (angle > 30 || angle < -30) step = -step;

        usleep(100000); // 100 ms
    }

    return 0;
}
