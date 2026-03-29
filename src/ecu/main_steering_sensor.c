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
