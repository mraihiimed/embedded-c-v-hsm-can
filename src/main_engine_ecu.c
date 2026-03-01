#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>

#include "config.h"
#include "can.h"
#include "network.h"

int main(void) {
    printf("[INFO] [ENGINE] Engine ECU starting\n");

    int sock = net_connect_to_bus(CAN_IP, CAN_PORT_BUS_SERVER);
    if (sock < 0) {
        printf("[ERROR] [ENGINE] Failed to connect to bus server\n");
        return 1;
    }

    uint8_t rpm_step = 8;   // 8 * 100 = 800 RPM
    while (1) {
        can_frame_t frame;
        memset(&frame, 0, sizeof(frame));

        frame.id  = CAN_ID_ENGINE;
        frame.dlc = 1;
        frame.data[0] = rpm_step;   // interpret as rpm_step * 100 in monitor
        frame.secured = false;      // unsecured for now

        if (send(sock, &frame, sizeof(frame), 0) != sizeof(frame)) {
            printf("[WARN] [ENGINE] Failed to send frame\n");
        }

        rpm_step += 4;
        if (rpm_step > 40) rpm_step = 8;  // 800..4000 RPM

        usleep(100000); // 100 ms
    }

    return 0;
}
