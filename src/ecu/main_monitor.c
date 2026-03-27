#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include "network.h"
#include "ecu.h"
#include "can.h"
#include "config.h"
#include <fcntl.h>
#include <stdio.h>   // for printf
#include <errno.h>   // for errno
#include <sys/socket.h>

int main(void) {
    // 1) Initialize ncurses
    initscr();              // Start curses mode
    cbreak();               // Disable line buffering
    noecho();               // Don't echo typed keys
    curs_set(0);            // Hide cursor
    nodelay(stdscr, TRUE);  // getch() non-blocking
    keypad(stdscr, TRUE);   // Enable function keys

    int sock = net_connect_to_bus(CAN_IP,     CAN_PORT_BUS_SERVER);
    if (sock < 0) {
    endwin();
    printf("[ERROR] Monitor failed to connect to bus  server\n");
    return 1;
}

    // 2) Enable colors
    if (has_colors()) {
        start_color();
        init_pair(1, COLOR_WHITE,  COLOR_RED);    // Alerts
        init_pair(2, COLOR_GREEN,  COLOR_BLACK);  // Sensors
        init_pair(3, COLOR_CYAN,   COLOR_BLACK);  // Controller
        init_pair(4, COLOR_YELLOW, COLOR_BLACK);  // Actuators
        init_pair(5, COLOR_WHITE,  COLOR_BLACK);  // Default
    }

    int rows, cols;
    getmaxyx(stdscr, rows, cols);
    
    fcntl(sock, F_SETFL, O_NONBLOCK);
    
    can_frame_t wheel_fl = {0};
    can_frame_t wheel_fr = {0};
    can_frame_t wheel_rl = {0};
    can_frame_t wheel_rr = {0};
    can_frame_t steering  = {0};
    can_frame_t engine    = {0};
    can_frame_t brake_cmd = {0};
    can_frame_t steer_cmd = {0};

    // 3) Main loop
    int running = 1;
    int frame_count = 0;
    int attack_detected = 0; // later: set from real IDS state

    while (running) {
    
    
    can_frame_t frame;
    int n = recv(sock, &frame, sizeof(frame), 0);

    if (n == sizeof(frame)) {
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

   /* if (!frame.secured) {
        attack_detected = 1;
    }*/
}
else if(n < 0 && errno != EAGAIN && errno != EWOULDBLOCK)
{
   //REAL ERROR
   endwin();
   perror("[ERROR] recv");
   return 1;
}

        // 3.1) Handle keyboard input
        int ch = getch();
        if (ch == 'q' || ch == 'Q') {
            running = 0;
            break;
        }

        // 3.2) Clear screen
        erase();

        // 3.3) Header line
        attron(COLOR_PAIR(5));
        mvprintw(0, 0, "C-V-HSM-CAN Monitor (C version)  |  Press 'q' to quit");
        attroff(COLOR_PAIR(5));

        // 3.4) Attack banner (simulate for now if attack_detected == 1)
        if (attack_detected) {
            attron(COLOR_PAIR(1));
            mvprintw(2, 0, "ATTACK DETECTED: unsecured frames detected on bus");
            mvprintw(3, 0, "AUTONOMOUS CONTROLLER DEACTIVATED - RESTART REQUIRED");
            attroff(COLOR_PAIR(1));
        }

        // 3.5) Sections titles
        int y = 5;
        attron(COLOR_PAIR(2));
        mvprintw(y++, 0, "SENSORS (Sending):");
        
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


        attron(COLOR_PAIR(3));
        mvprintw(y++, 0, "CONTROLLER (Autonomous):");

        mvprintw(y++, 0, "[Brake Cmd] ID:0x300 Pressure:%3d Secured:%d",
         brake_cmd.data[0], brake_cmd.secured);

        mvprintw(y++, 0, "[Steer Cmd] ID:0x302 Angle:%3d Secured:%d",
         steer_cmd.data[0], steer_cmd.secured);
        attroff(COLOR_PAIR(3));


        attron(COLOR_PAIR(4));
        mvprintw(y++, 0, "ACTUATORS (Receiving):");
        attroff(COLOR_PAIR(4));

        // 3.6) Example dynamic info (for now just a counter)
        attron(COLOR_PAIR(5));
        mvprintw(rows - 2, 0, "Frames observed: %d", frame_count);
        attroff(COLOR_PAIR(5));

        // 3.7) Refresh screen
        refresh();

        // 3.8) Simulate work
        frame_count++;
        usleep(100000); // 100 ms
    }
    endwin();
    close(sock);
    // 4) Cleanup
    return 0;
}
