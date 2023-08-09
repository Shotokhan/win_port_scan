#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <time.h>

#pragma comment(lib, "ws2_32.lib")

typedef struct {
    const char* target;
    int start_port;
    int end_port;
} ThreadData;

void scan_ports(const char* target, int start_port, int end_port) {
    WSADATA wsa;
    SOCKET s;
    struct sockaddr_in server;
    int port;
    int timeout = 1000; // 1 second

    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Failed to initialize Winsock.\n");
        fflush(stdout);
        return;
    }

    printf("[DEBUG] - Thread scanning ports from %d to %d: started\n", start_port, end_port);
    fflush(stdout);

    for (port = start_port; port <= end_port; port++) {
        if ((((port - start_port) % 10) == 0) && (port > start_port)){
            printf("[DEBUG] - Thread scanning ports from %d to %d: scanned %d ports\n", start_port, end_port, port - start_port);
            fflush(stdout);
        }
        s = socket(AF_INET, SOCK_STREAM, 0);
        if (s == INVALID_SOCKET) {
            printf("Socket creation failed.\n");
            fflush(stdout);
            return;
        }
        setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));
        setsockopt(s, SOL_SOCKET, SO_SNDTIMEO, (const char*)&timeout, sizeof(timeout));

        server.sin_family = AF_INET;
        server.sin_addr.s_addr = inet_addr(target);
        server.sin_port = htons(port);

        if (connect(s, (struct sockaddr*)&server, sizeof(server)) != SOCKET_ERROR) {
            printf("Port %d is open.\n", port);
            fflush(stdout);
        }

        closesocket(s);
    }

    printf("[DEBUG] - Thread scanning ports from %d to %d: finished\n", start_port, end_port);

    WSACleanup();
}

DWORD WINAPI ThreadFunc(LPVOID lpParam) {
    ThreadData* thread_data = (ThreadData*)lpParam;
    scan_ports(thread_data->target, thread_data->start_port, thread_data->end_port);
    return 0;
}

int main(int argc, char* argv[]) {
    if (argc < 5) {
        printf("Usage: %s start_port end_port ip num_threads\n", argv[0]);
        return 1;
    }

    int start_port = atoi(argv[1]);
    int end_port = atoi(argv[2]);

    if (start_port <= 0 || end_port <= 0 || start_port > end_port) {
        printf("Invalid port range.\n");
        return 1;
    }

    const char* ip = argv[3];
    int num_threads = atoi(argv[4]);

    int total_ports = end_port - start_port + 1;
    int ports_per_thread = total_ports / num_threads;
    int remaining_ports = total_ports % num_threads;
    if (remaining_ports > 0) {
        num_threads += 1;
    }
    int i, start, end;

    printf("Scanning ip %s for ports %d to %d using %d threads.\n", ip, start_port, end_port, num_threads);
    fflush(stdout);

    HANDLE* thread_handles = (HANDLE*)malloc(num_threads * sizeof(HANDLE));
    ThreadData* thread_data = (ThreadData*)malloc(num_threads * sizeof(ThreadData));

    clock_t start_time = clock();

    for (i = 0; i < num_threads; i++) {
        start = start_port + i * ports_per_thread;
        end = start_port + (i + 1) * ports_per_thread - 1;

        if (end > end_port) {
            end = end_port;
        }

        thread_data[i].target = ip;
        thread_data[i].start_port = start;
        thread_data[i].end_port = end;

        thread_handles[i] = CreateThread(NULL, 0, ThreadFunc, &thread_data[i], 0, NULL);
        if (thread_handles[i] == NULL) {
            printf("Error creating thread %d\n", i);
            fflush(stdout);
            return 1;
        }
    }

    // Wait for all threads to finish
    WaitForMultipleObjects(num_threads, thread_handles, TRUE, INFINITE);

    clock_t end_time = clock();
    double elapsed_time = (double)(end_time - start_time) * 1000.0 / CLOCKS_PER_SEC;
    printf("Elapsed time: %f ms\n", elapsed_time);
    // Close thread handles
    for (i = 0; i < num_threads; i++) {
        CloseHandle(thread_handles[i]);
    }

    free(thread_handles);
    free(thread_data);

    return 0;
}
