#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define VISUALIZER_PORT 8888
#define VISUALIZER_IP "127.0.0.1"

void send_telemetry(const char* json_data) {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) return;

    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(VISUALIZER_PORT);
    servaddr.sin_addr.s_addr = inet_addr(VISUALIZER_IP);

    sendto(sock, json_data, strlen(json_data), 0, (const struct sockaddr *)&servaddr, sizeof(servaddr));
    close(sock);
}
