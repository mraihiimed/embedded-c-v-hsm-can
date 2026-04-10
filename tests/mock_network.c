#include "mock_network.h"

// --- Control values (tests set these) ---
int  mock_connect_return = -1;
int  mock_listen_return  = -1;
bool mock_send_return    = true;
bool mock_recv_return    = true;

// --- Legacy counter required by test_bus ---
int mock_network_send_called = 0;

// --- Captured values ---
int mock_last_sockfd = -1;
can_frame_t mock_last_frame;

// ---------------------------------------------------------------------------
// Low-level mock implementations
// ---------------------------------------------------------------------------

int net_connect_to_bus(const char *host, uint16_t port)
{
    (void)host;
    (void)port;
    return mock_connect_return;
}

int net_listen_bus(uint16_t port)
{
    (void)port;
    return mock_listen_return;
}

bool net_send_frame(int sockfd, const can_frame_t *frame)
{
    mock_network_send_called++;   // required by test_bus

    mock_last_sockfd = sockfd;

    if (frame)
        mock_last_frame = *frame;

    return mock_send_return;
}

bool net_recv_frame(int sockfd, can_frame_t *frame)
{
    (void)sockfd;

    if (frame)
        *frame = mock_last_frame;

    return mock_recv_return;
}

// ---------------------------------------------------------------------------
// High-level API mocks (required by bus_testable, ecu, ids tests)
// ---------------------------------------------------------------------------

int network_send_frame(const can_frame_t *frame)
{
    // High-level wrapper expected by bus_testable.c
    return net_send_frame(1, frame) ? 0 : -1;
}

int network_receive_frame(can_frame_t *frame)
{
    // High-level wrapper expected by ECU/IDS tests
    return net_recv_frame(1, frame) ? 0 : -1;
}

// ---------------------------------------------------------------------------
// Optional standalone test mode
// ---------------------------------------------------------------------------

#ifdef MOCK_NETWORK_STANDALONE
#include <stdio.h>

int main(void)
{
    printf("=== mock_network standalone ===\n");

    mock_connect_return = 10;
    mock_send_return    = true;
    mock_recv_return    = true;

    int sock = net_connect_to_bus("127.0.0.1", 1337);
    printf("net_connect_to_bus() -> %d\n", sock);

    can_frame_t frame = {
        .id = 0x123,
        .dlc = 2,
        .data = {0xAA, 0xBB}
    };

    bool send_ok = net_send_frame(sock, &frame);
    printf("net_send_frame() -> %d\n", send_ok);
    printf("mock_network_send_called = %d\n", mock_network_send_called);
    printf("mock_last_sockfd = %d\n", mock_last_sockfd);
    printf("mock_last_frame.data[0] = 0x%02X\n", mock_last_frame.data[0]);

    can_frame_t recv_frame;
    bool recv_ok = net_recv_frame(sock, &recv_frame);
    printf("net_recv_frame() -> %d\n", recv_ok);
    printf("recv_frame.data[0] = 0x%02X\n", recv_frame.data[0]);

    printf("=== done ===\n");
    return 0;
}
#endif
