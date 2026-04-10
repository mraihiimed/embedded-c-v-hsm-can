#include "mock_net_lowlevel.h"
#include <string.h>

// ---------------------------------------------------------------------------
// Control values (set by tests)
// ---------------------------------------------------------------------------
int  mock_connect_return = -1;
bool mock_send_return    = true;
bool mock_recv_return    = true;

// ---------------------------------------------------------------------------
// Captured values (for assertions)
// ---------------------------------------------------------------------------
int         mock_last_sockfd = -1;
can_frame_t mock_last_sent_frame;

const char *mock_last_host = NULL;
uint16_t    mock_last_port = 0;

// ---------------------------------------------------------------------------
// Reset function (IMPORTANT for test isolation)
// ---------------------------------------------------------------------------
void mock_net_lowlevel_reset(void)
{
    mock_connect_return = -1;
    mock_send_return    = true;
    mock_recv_return    = true;

    mock_last_sockfd = -1;
    memset(&mock_last_sent_frame, 0, sizeof(mock_last_sent_frame));

    mock_last_host = NULL;
    mock_last_port = 0;
}

// ---------------------------------------------------------------------------
// Mock implementations (override real low-level)
// ---------------------------------------------------------------------------

int net_connect_to_bus(const char *host, uint16_t port)
{
    mock_last_host = host;
    mock_last_port = port;

    return mock_connect_return;
}

bool net_send_frame(int sockfd, const can_frame_t *frame)
{
    mock_last_sockfd = sockfd;

    if (frame) {
        mock_last_sent_frame = *frame;
    }

    return mock_send_return;
}

bool net_recv_frame(int sockfd, can_frame_t *frame)
{
    (void)sockfd;

    if (frame) {
        *frame = mock_last_sent_frame;
    }

    return mock_recv_return;
}
