#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>

#include "config.h"
#include "can.h"
#include "network.h"

int main(void) {

    printf("[INFO] [AUTO] Autonomous controller started\n");

    int sock = net_connect_to_bus(CAN_IP, CAN_PORT_BUS_SERVER);
    if (sock < 0) {
        printf("[ERROR] [AUTO] Failed to connect to bus server\n");
        return 1;
    }

    uint8_t brake = 0;
    int8_t steer  = 0;
    int8_t steer_step = 2;

    while (1) {
        // Brake command
        can_frame_t brake_frame;
        memset(&brake_frame, 0, sizeof(brake_frame));
        brake_frame.id  = CAN_ID_BRAKE_CMD;
        brake_frame.dlc = 1;
        brake_frame.data[0] = brake;
        brake_frame.secured = false;

        // Steer command
        can_frame_t steer_frame;
        memset(&steer_frame, 0, sizeof(steer_frame));
        steer_frame.id  = CAN_ID_STEER_CMD;
        steer_frame.dlc = 1;
        steer_frame.data[0] = (uint8_t)steer;
        steer_frame.secured = false;
	// Send Frames
        if (send(sock, &brake_frame, sizeof(brake_frame), 0) != sizeof(brake_frame)) {
            printf("[WARN] [AUTO] Failed to send brake cmd\n");
        }
        if (send(sock, &steer_frame, sizeof(steer_frame), 0) != sizeof(steer_frame)) {
            printf("[WARN] [AUTO] Failed to send steer cmd\n");
        }

        // Simple motion pattern
        brake += 5;
        if (brake > 100) brake = 0;

        steer += steer_step;
        if (steer > 20 || steer < -20) steer_step = -steer_step;
	printf("[AUTO] sending brake=%u steer=%d\n", brake, steer);

        usleep(100000); // 100 ms
    }

    return 0;
}
