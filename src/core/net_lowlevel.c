#include "network.h"
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int net_connect_to_bus(const char *host, uint16_t port)
{
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

bool net_send_frame(int sockfd, const can_frame_t *frame)
{
    ssize_t n = send(sockfd, frame, sizeof(*frame), 0);
    return n == (ssize_t)sizeof(*frame);
}

bool net_recv_frame(int sockfd, can_frame_t *frame)
{
    ssize_t n = recv(sockfd, frame, sizeof(*frame), 0);
    return n == (ssize_t)sizeof(*frame);
}
