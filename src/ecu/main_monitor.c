#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include "network.h"
#include "ecu.h"
#include "can.h"
#include "config.h"
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <sys/socket.h>

#define CAN_ID_SECURITY_EVENT 0x3F0

// Security event types
#define SEC_EVT_MAC_FAIL   1
#define SEC_EVT_SPOOF      2
#define SEC_EVT_DLC_ERR    3
#define SEC_EVT_FAILSAFE   6
int main(void) {

    // -----------------------------
    // 1) Initialize ncurses
    // -----------------------------
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);

    int sock = net_connect_to_bus(CAN_IP, CAN_PORT_BUS_SERVER);
    if (sock < 0) {
        endwin();
        printf("[ERROR] Monitor failed to connect to bus server\n");
        return 1;
    }

    // Colors
    if (has_colors()) {
        start_color();
       //init_pair(1, COLOR_WHITE,  COLOR_RED);    // Alerts
       // init_pair(2, COLOR_GREEN,  COLOR_BLACK);  // Sensors
       //init_pair(3, COLOR_CYAN,   COLOR_BLACK);  // Controller
       // init_pair(4, COLOR_YELLOW, COLOR_BLACK);  // Actuators
       // init_pair(5, COLOR_WHITE,  COLOR_BLACK);  // Default

        init_pair(1, COLOR_RED,    COLOR_BLACK);  // Alerts (red text)
        init_pair(2, COLOR_GREEN,  COLOR_BLACK);  // Sensors
        init_pair(3, COLOR_CYAN,   COLOR_BLACK);  // Controller
        init_pair(4, COLOR_YELLOW, COLOR_BLACK);  // Actuators
        init_pair(5, COLOR_WHITE,  COLOR_BLACK);  // Default
    }

    int rows, cols;
    getmaxyx(stdscr, rows, cols);
    fcntl(sock, F_SETFL, O_NONBLOCK);

    // -----------------------------
    // 2) Frame storage
    // -----------------------------
    can_frame_t wheel_fl = {0};
    can_frame_t wheel_fr = {0};
    can_frame_t wheel_rl = {0};
    can_frame_t wheel_rr = {0};
    can_frame_t steering  = {0};
    can_frame_t engine    = {0};
    can_frame_t brake_cmd = {0};
    can_frame_t steer_cmd = {0};

    // Security event buffer
    char sec_msg_line1[128] = {0};
    char sec_msg_line2[128] = {0};
    int show_security_alert = 0;

    int running = 1;
    int frame_count = 0;

    // -----------------------------
    // 3) Main loop
    // -----------------------------
    while (running) {

        can_frame_t frame;
        int n = recv(sock, &frame, sizeof(frame), 0);

        if (n == sizeof(frame)) {

            // -------------------------
            // 3.1) Handle security events
            // -------------------------
            if (frame.id == CAN_ID_SECURITY_EVENT && frame.dlc >= 3) {

                uint8_t type = frame.data[0];
                uint8_t ecu  = frame.data[1];
                uint8_t bad  = frame.data[2];

                show_security_alert = 1;

                snprintf(sec_msg_line1, sizeof(sec_msg_line1),
                         "[SECURITY] ECU %u reported:", ecu);

                switch (type) {
                    case SEC_EVT_MAC_FAIL:
                        snprintf(sec_msg_line2, sizeof(sec_msg_line2),
                                 "MAC mismatch on frame 0x%02X", bad);
                        break;

                    case SEC_EVT_SPOOF:
                        snprintf(sec_msg_line2, sizeof(sec_msg_line2),
                                 "Spoofed ID detected: 0x%02X", bad);
                        break;

                    case SEC_EVT_DLC_ERR:
                        snprintf(sec_msg_line2, sizeof(sec_msg_line2),
                                 "Malformed DLC on frame 0x%02X", bad);
                        break;

	           case SEC_EVT_FAILSAFE:
    			attron(COLOR_PAIR(4)); // yellow
   		        mvprintw(2, 0, "[FAILSAFE] ECU entered fail-safe mode");
   		        attroff(COLOR_PAIR(4));
   		        break;


                    default:
                        snprintf(sec_msg_line2, sizeof(sec_msg_line2),
                                 "Unknown security event");
                        break;
                }

                continue;
            }

            // -------------------------
            // 3.2) Normal CAN frames
            // -------------------------
            switch (frame.id) {
                case 0x100: wheel_fl = frame; break;
                case 0x101: wheel_fr = frame; break;
                case 0x102: wheel_rl = frame; break;
                case 0x103: wheel_rr = frame; break;
                case 0x110: engine   = frame; break;
                case 0x120: steering = frame; break;
                case 0x300: brake_cmd = frame; break;
                case 0x302: steer_cmd = frame; break;
            }
        }
        else if (n < 0 && errno != EAGAIN && errno != EWOULDBLOCK) {
            endwin();
            perror("[ERROR] recv");
            return 1;
        }

        // -----------------------------
        // 3.3) Keyboard input
        // -----------------------------
        int ch = getch();
        if (ch == 'q' || ch == 'Q') {
            running = 0;
            break;
        }

        // -----------------------------
        // 3.4) Draw UI
        // -----------------------------
        erase();

        attron(COLOR_PAIR(5));
        mvprintw(0, 0, "C-V-HSM-CAN Monitor  |  Press 'q' to quit");
        attroff(COLOR_PAIR(5));

        // -----------------------------
        // 3.5) Security alert banner
        // -----------------------------
        if (show_security_alert) {
            attron(COLOR_PAIR(1));
            mvprintw(2, 0, "%s", sec_msg_line1);
            mvprintw(3, 0, "%s", sec_msg_line2);
            attroff(COLOR_PAIR(1));
        }

        // -----------------------------
        // 3.6) Sensor section
        // -----------------------------
        int y = 5;

        attron(COLOR_PAIR(2));
        mvprintw(y++, 0, "SENSORS (Sending):");

        mvprintw(y++, 0, "[WHEEL_FL] ID:0x%03X Speed:%3d Secured:%d",
                 wheel_fl.id, wheel_fl.data[0], wheel_fl.secured);

        mvprintw(y++, 0, "[WHEEL_FR] ID:0x%03X Speed:%3d Secured:%d",
                 wheel_fr.id, wheel_fr.data[0], wheel_fr.secured);

        mvprintw(y++, 0, "[WHEEL_RL] ID:0x%03X Speed:%3d Secured:%d",
                 wheel_rl.id, wheel_rl.data[0], wheel_rl.secured);

        mvprintw(y++, 0, "[WHEEL_RR] ID:0x%03X Speed:%3d Secured:%d",
                 wheel_rr.id, wheel_rr.data[0], wheel_rr.secured);

        mvprintw(y++, 0, "[ENGINE]   ID:0x%03X RPM:%4d Secured:%d",
                 engine.id, engine.data[0] * 100, engine.secured);

        mvprintw(y++, 0, "[STEER]    ID:0x%03X Angle:%3d Secured:%d",
                 steering.id, steering.data[0], steering.secured);
        attroff(COLOR_PAIR(2));

        // -----------------------------
        // 3.7) Controller section
        // -----------------------------
        attron(COLOR_PAIR(3));
        mvprintw(y++, 0, "CONTROLLER (Autonomous):");

        mvprintw(y++, 0, "[Brake Cmd] ID:0x300 Pressure:%3d Secured:%d",
                 brake_cmd.data[0], brake_cmd.secured);

        mvprintw(y++, 0, "[Steer Cmd] ID:0x302 Angle:%3d Secured:%d",
                 steer_cmd.data[0], steer_cmd.secured);
        attroff(COLOR_PAIR(3));

        // -----------------------------
        // 3.8) Footer
        // -----------------------------
        attron(COLOR_PAIR(5));
        mvprintw(rows - 2, 0, "Frames observed: %d", frame_count);
        attroff(COLOR_PAIR(5));

        refresh();
        frame_count++;
        usleep(100000);
    }

    endwin();
    close(sock);
    return 0;
}
