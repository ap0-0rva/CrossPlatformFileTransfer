#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <cstdint>

#define PORT 9000
#define CHUNK_SIZE 262144 // 256 KB

#ifdef _WIN32

#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

typedef SOCKET socket_t;

inline void socket_init()
{
    WSADATA wsa;
    WSAStartup(MAKEWORD(2,2), &wsa);
}

inline void socket_cleanup()
{
    WSACleanup();
}

inline void socket_close(socket_t s)
{
    closesocket(s);
}

#else

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

typedef int socket_t;

inline void socket_init() {}
inline void socket_cleanup() {}

inline void socket_close(socket_t s)
{
    close(s);
}

#endif

struct chunk_t
{
    uint32_t size;
    uint32_t checksum;
    char data[CHUNK_SIZE];
};

inline uint32_t calculate_checksum(const char* data, uint32_t size)
{
    uint32_t sum = 0;

    for (uint32_t i = 0; i < size; i++)
        sum += (unsigned char)data[i];

    return sum;
}

#endif