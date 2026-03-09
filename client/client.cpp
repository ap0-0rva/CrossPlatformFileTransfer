#include <iostream>
#include <fstream>
#include <chrono>
#include <cstring>

#include "../common/protocol.h"

bool send_all(socket_t sock, const void* buffer, size_t size)
{
    size_t total = 0;

    while (total < size)
    {
        int n = send(sock, (const char*)buffer + total, size - total, 0);
        if (n <= 0) return false;
        total += n;
    }

    return true;
}

int main()
{
    socket_init();

    std::string filepath;
    std::string server_ip;
    std::string dest_path;

    std::cout << "Enter file path to send: ";
    std::getline(std::cin, filepath);

    std::cout << "Enter server IP: ";
    std::getline(std::cin, server_ip);

    std::cout << "Enter destination path on server: ";
    std::getline(std::cin, dest_path);

    std::ifstream file(filepath, std::ios::binary);

    if (!file)
    {
        std::cerr << "Cannot open file\n";
        return 1;
    }

    file.seekg(0, std::ios::end);
    uint64_t filesize = file.tellg();
    file.seekg(0);

    uint64_t total_chunks = (filesize + CHUNK_SIZE - 1) / CHUNK_SIZE;

    std::cout << "\nFile: " << filepath << "\n";
    std::cout << "File size: " << filesize << " bytes\n";
    std::cout << "Chunk size: " << CHUNK_SIZE << " bytes\n";
    std::cout << "Total chunks: " << total_chunks << "\n";

    socket_t sock = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

#ifdef _WIN32
    inet_pton(AF_INET, server_ip.c_str(), &server_addr.sin_addr);
#else
    inet_pton(AF_INET, server_ip.c_str(), &server_addr.sin_addr);
#endif

    std::cout << "\nConnecting to server...\n";

    connect(sock, (sockaddr*)&server_addr, sizeof(server_addr));

    std::cout << "Connected\n";

    uint64_t path_len = dest_path.size();

    send_all(sock, &path_len, sizeof(path_len));
    send_all(sock, dest_path.c_str(), path_len);
    send_all(sock, &filesize, sizeof(filesize));

    uint64_t chunks_sent = 0;
    int last_progress = -1;

    auto start = std::chrono::high_resolution_clock::now();

    while (file)
    {
        chunk_t chunk;

        file.read(chunk.data, CHUNK_SIZE);
        chunk.size = file.gcount();

        if (chunk.size <= 0)
            break;

        chunk.checksum = calculate_checksum(chunk.data, chunk.size);

        send_all(sock, &chunk, sizeof(chunk));

        chunks_sent++;

        int progress = (chunks_sent * 100) / total_chunks;

        if (progress != last_progress)
        {
            std::cout << "\rProgress: " << progress << "%" << std::flush;
            last_progress = progress;
        }
    }

    std::cout << "\n";

    auto end = std::chrono::high_resolution_clock::now();

    double seconds =
        std::chrono::duration<double>(end - start).count();

    double speed =
        (filesize / 1024.0 / 1024.0) / seconds;

    std::cout << "\nTransfer complete\n";
    std::cout << "Total chunks sent: " << chunks_sent << "\n";
    std::cout << "Time taken: " << seconds << " seconds\n";
    std::cout << "Average speed: " << speed << " MB/s\n";

    socket_close(sock);
    socket_cleanup();
}