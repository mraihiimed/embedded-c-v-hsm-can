#include "bus.h"
#include "network.h"
#include "config.h"
#include <sys/select.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define MAX_CLIENTS 32

void bus_server_run(uint16_t port_bus, uint16_t port_monitor) {
    int listen_fd = net_listen_bus(port_bus);
    int monitor_fd = net_listen_bus(port_monitor);
    if (listen_fd < 0 || monitor_fd < 0) {
        log_msg(LOG_ERROR, "[BUS] Failed to listen\n");
        return;
    }

    int clients[MAX_CLIENTS];
    int monitors[MAX_CLIENTS];
    int num_clients = 0;
    int num_monitors = 0;
    memset(clients, 0, sizeof(clients));
    memset(monitors, 0, sizeof(monitors));

    log_msg(LOG_INFO, "[BUS] Server running on ports %u (ECUs) and %u (monitors)\n",
            port_bus, port_monitor);

    while (1) {
        fd_set rfds;
        FD_ZERO(&rfds);
        int maxfd = 0;

        FD_SET(listen_fd, &rfds);
        if (listen_fd > maxfd) maxfd = listen_fd;

        FD_SET(monitor_fd, &rfds);
        if (monitor_fd > maxfd) maxfd = monitor_fd;

        for (int i = 0; i < num_clients; i++) {
            FD_SET(clients[i], &rfds);
            if (clients[i] > maxfd) maxfd = clients[i];
        }

        int ret = select(maxfd + 1, &rfds, NULL, NULL, NULL);
        if (ret < 0) {
            log_msg(LOG_ERROR, "[BUS] select() failed\n");
            break;
        }

        if (FD_ISSET(listen_fd, &rfds)) {
            int cfd = accept(listen_fd, NULL, NULL);
            if (cfd >= 0 && num_clients < MAX_CLIENTS) {
                clients[num_clients++] = cfd;
                log_msg(LOG_INFO, "[BUS] ECU connected (fd=%d)\n", cfd);
            } else if (cfd >= 0) {
                close(cfd);
            }
        }

        if (FD_ISSET(monitor_fd, &rfds)) {
            int mfd = accept(monitor_fd, NULL, NULL);
            if (mfd >= 0 && num_monitors < MAX_CLIENTS) {
                monitors[num_monitors++] = mfd;
                log_msg(LOG_INFO, "[BUS] Monitor connected (fd=%d)\n", mfd);
            } else if (mfd >= 0) {
                close(mfd);
            }
        }

        for (int i = 0; i < num_clients; i++) {
            int cfd = clients[i];
            if (FD_ISSET(cfd, &rfds)) {
                can_frame_t frame;
                if (!net_recv_frame(cfd, &frame)) {
                    log_msg(LOG_INFO, "[BUS] ECU disconnected (fd=%d)\n", cfd);
                    close(cfd);
                    clients[i] = clients[--num_clients];
                    i--;
                    continue;
                }

                // Broadcast to all other ECUs
                for (int j = 0; j < num_clients; j++) {
                    if (clients[j] != cfd) {
                        net_send_frame(clients[j], &frame);
                    }
                }
                // Send to all monitors
                for (int j = 0; j < num_monitors; j++) {
                    net_send_frame(monitors[j], &frame);
                }
            }
        }
    }

    close(listen_fd);
    close(monitor_fd);
}

