#include "network.h"
#include "config.h"
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdio.h>

// Global socket used by all ECUs
static int g_bus_sock = -1;


int network_init(void)
{
    g_bus_sock = net_connect_to_bus(BUS_SERVER_HOST, BUS_SERVER_PORT);
    if (g_bus_sock < 0) {
        printf("[NETWORK] ERROR: Could not connect to bus server %s:%d\n",
               BUS_SERVER_HOST, BUS_SERVER_PORT);
        return -1;
    }

    printf("[NETWORK] Connected to bus server at %s:%d\n",
           BUS_SERVER_HOST, BUS_SERVER_PORT);
    return 0;
}


int network_send_frame(const can_frame_t *frame)
{
    if (!frame) {
        printf("[NETWORK] ERROR: NULL frame\n");
        return -1;
    }

    if (g_bus_sock < 0) {
        printf("[NETWORK] ERROR: network not initialized\n");
        return -1;
    }

    if (!net_send_frame(g_bus_sock, frame)) {
        printf("[NETWORK] ERROR: net_send_frame() failed\n");
        return -1;
    }

    return 0;
}

int net_connect_to_bus(const char *host, uint16_t port) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) return -1;

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if (inet_pton(AF_INET, host, &addr.sin_addr) <= 0) {
        close(sockfd);
        return -1;
    }

    if (connect(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        close(sockfd);
        return -1;
    }
    return sockfd;
}

int net_listen_bus(uint16_t port) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) return -1;

    int opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        close(sockfd);
        return -1;
    }
    if (listen(sockfd, 16) < 0) {
        close(sockfd);
        return -1;
    }
    return sockfd;
}

bool net_send_frame(int sockfd, const can_frame_t *frame) {
    ssize_t n = send(sockfd, frame, sizeof(*frame), 0);
    return n == (ssize_t)sizeof(*frame);
}

bool net_recv_frame(int sockfd, can_frame_t *frame) {
    ssize_t n = recv(sockfd, frame, sizeof(*frame), 0);
    return n == (ssize_t)sizeof(*frame);
}
