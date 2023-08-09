#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <time.h>

#pragma comment(lib, "ws2_32.lib")

void scan_ports(const char* target, int start_port, int end_port) {
    WSADATA wsa;
    SOCKET s;
    struct sockaddr_in server;
    int port;

    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Failed to initialize Winsock.\n");
        return;
    }

    clock_t start_time = clock();
    for (port = start_port; port <= end_port; port++) {
        s = socket(AF_INET, SOCK_STREAM, 0);
        if (s == INVALID_SOCKET) {
            printf("Socket creation failed.\n");
            return;
        }

        server.sin_family = AF_INET;
        server.sin_addr.s_addr = inet_addr(target);
        server.sin_port = htons(port);

        if (connect(s, (struct sockaddr*)&server, sizeof(server)) != SOCKET_ERROR) {
            printf("Port %d is open.\n", port);
            fflush(stdout);
        }

        closesocket(s);
    }
    clock_t end_time = clock();

    WSACleanup();
    double elapsed_time = (double)(end_time - start_time) * 1000.0 / CLOCKS_PER_SEC;
    printf("Elapsed time: %f ms\n", elapsed_time);
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        printf("Usage: %s start_port end_port ip\n", argv[0]);
        return 1;
    }

    int start_port = atoi(argv[1]);
    int end_port = atoi(argv[2]);

    if (start_port <= 0 || end_port <= 0 || start_port > end_port) {
        printf("Invalid port range.\n");
        return 1;
    }

    const char* ip = argv[3];

    printf("Scanning ip %s for ports %d to %d.\n", ip, start_port, end_port);
    fflush(stdout);

    // Perform the port scan
    scan_ports(ip, start_port, end_port);

    return 0;
}
