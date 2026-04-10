#include "network.h"
#include "config.h"
#include <stdio.h>


#include <stdio.h>
#include <string.h>         // memset
#include <unistd.h>         // close()
#include <sys/socket.h>     // socket, bind, listen
#include <netinet/in.h>     // sockaddr_in, INADDR_ANY
#include <arpa/inet.h>      // htons


// ---------------------------------------------------------------------------
// Global state (internal)
// ---------------------------------------------------------------------------
static int g_bus_sock = -1;


// ---------------------------------------------------------------------------
// High-level API
// ---------------------------------------------------------------------------

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

int net_listen_bus(uint16_t port)
{
    int sockfd;
    struct sockaddr_in addr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        return -1;

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
        return -1;

    if (listen(sockfd, 5) < 0)
        return -1;

    return sockfd;
}

int network_send_frame(const can_frame_t *frame)
{
    // Validate input
    if (!frame) {
        printf("[NETWORK] ERROR: NULL frame\n");
        return -1;
    }

    // Check initialization
    if (g_bus_sock < 0) {
        printf("[NETWORK] ERROR: network not initialized\n");
        return -1;
    }

    // Delegate to low-level (mocked in tests)
    if (!net_send_frame(g_bus_sock, frame)) {
        printf("[NETWORK] ERROR: net_send_frame() failed\n");
        return -1;
    }

    return 0;
}


// ---------------------------------------------------------------------------
// Test support (only compiled in unit tests)
// ---------------------------------------------------------------------------
#ifdef UNIT_TEST
void network_reset(void)
{
    g_bus_sock = -1;
}
#endif
