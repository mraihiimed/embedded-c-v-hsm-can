#include "mock_net_lowlevel.h"

// Control values
int  mock_connect_return = -1;
bool mock_send_return    = true;
bool mock_recv_return    = true;

// Captured values
int        mock_last_sockfd    = -1;
can_frame_t mock_last_sent_frame;

// Match signatures from network.c

int net_connect_to_bus(const char *host, uint16_t port)
{
    (void)host;
    (void)port;
    return mock_connect_return;
}

bool net_send_frame(int sockfd, const can_frame_t *frame)
{
    mock_last_sockfd = sockfd;

    if (frame)
        mock_last_sent_frame = *frame;

    return mock_send_return;
}

bool net_recv_frame(int sockfd, can_frame_t *frame)
{
    (void)sockfd;

    if (frame)
        *frame = mock_last_sent_frame;

    return mock_recv_return;
}
