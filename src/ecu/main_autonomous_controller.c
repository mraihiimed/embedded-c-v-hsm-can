/*#include "ecu.h"
#include "config.h"
#include <unistd.h>
#include <stdio.h>

int main(void) {
    ecu_t ecu;
    if (!ecu_init(&ecu, "autonomous_controller", "127.0.0.1", CAN_PORT_BUS_SERVER)) {
        return 1;
    }

    log_msg(LOG_INFO, "[AUTO] Autonomous controller started\n");

    while (!ecu.fail_safe) {
        can_frame_t frame;
        if (!ecu_recv(&ecu, &frame)) {
            break;
        }

        // Example: react to wheel speed frames
        if (frame.id == 0x100 && frame.dlc >= 1) {
            uint8_t speed = frame.data[0];
            log_msg(LOG_INFO, "[AUTO] Wheel FL speed=%u\n", speed);

            // Simple control: if speed > 100, send brake command
            if (speed > 100) {
                can_frame_t cmd = {0};
                cmd.id = 0x200; // brake command ID
                cmd.dlc = 1;
                cmd.data[0] = 1; // apply brake
                cmd.secured = true;
                ecu_send(&ecu, &cmd);
            }
        }

        if (ecu.fail_safe) {
            log_msg(LOG_ERROR, "[AUTO] FAIL-SAFE: deactivating autonomous control\n");
            // Stop sending any commands, just loop reading if desired
        }
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

        if (send(sock, &brake_frame, sizeof(brake_frame), 0) != sizeof(brake_frame)) {
            printf("[WARN] [AUTO] Failed to send brake cmd\n");
        }
        if (send(sock, &steer_frame, sizeof(steer_frame), 0) != sizeof(steer_frame)) {
            printf("[WARN] [AUTO] Failed to send steer cmd\n");
        }

        // Simple pattern
        brake += 5;
        if (brake > 100) brake = 0;

        steer += steer_step;
        if (steer > 20 || steer < -20) steer_step = -steer_step;
	
	printf("[AUTO] sending brake=%u steer=%d\n", brake, steer);

        usleep(100000); // 100 ms
    }

    return 0;
}


