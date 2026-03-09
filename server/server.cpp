#include <iostream>
#include <fstream>
#include <chrono>
#include <cstring>

#include "../common/protocol.h"

bool recv_all(socket_t sock, void* buffer, size_t size)
{
    size_t total = 0;

    while (total < size)
    {
        int n = recv(sock, (char*)buffer + total, size - total, 0);

        if (n <= 0)
            return false;

        total += n;
    }

    return true;
}

int main()
{
    socket_init();

    socket_t server_sock = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    bind(server_sock, (sockaddr*)&server_addr, sizeof(server_addr));

    listen(server_sock, 1);

    std::cout << "Server listening on port " << PORT << "\n";

    sockaddr_in client_addr{};
    socklen_t client_len = sizeof(client_addr);

    socket_t client_sock =
        accept(server_sock, (sockaddr*)&client_addr, &client_len);

    std::cout << "Client connected\n\n";

    uint64_t path_len;
    recv_all(client_sock, &path_len, sizeof(path_len));

    std::string path(path_len, '\0');
    recv_all(client_sock, path.data(), path_len);

    uint64_t filesize;
    recv_all(client_sock, &filesize, sizeof(filesize));

    uint64_t total_chunks =
        (filesize + CHUNK_SIZE - 1) / CHUNK_SIZE;

    std::ofstream outfile(path, std::ios::binary);

    std::cout << "Receiving file: " << path << "\n";
    std::cout << "File size: " << filesize << " bytes\n";
    std::cout << "Chunk size: " << CHUNK_SIZE << " bytes\n";
    std::cout << "Expected chunks: " << total_chunks << "\n";
    std::cout << "Saving to: " << path << "\n\n";

    uint64_t chunks_received = 0;
    int last_progress = -1;

    auto start = std::chrono::high_resolution_clock::now();

    while (chunks_received < total_chunks)
    {
        chunk_t chunk;

        if (!recv_all(client_sock, &chunk, sizeof(chunk)))
            break;

        uint32_t check =
            calculate_checksum(chunk.data, chunk.size);

        if (check != chunk.checksum)
        {
            std::cerr << "Checksum mismatch!\n";
            continue;
        }

        outfile.write(chunk.data, chunk.size);

        chunks_received++;

        int progress =
            (chunks_received * 100) / total_chunks;

        if (progress != last_progress)
        {
            std::cout << "\rProgress: "
                      << progress << "%"
                      << std::flush;
            last_progress = progress;
        }
    }

    std::cout << "\n";

    auto end = std::chrono::high_resolution_clock::now();

    double seconds =
        std::chrono::duration<double>(end - start).count();

    double speed =
        (filesize / 1024.0 / 1024.0) / seconds;

    std::cout << "\nFile transfer complete\n";
    std::cout << "Chunks received: "
              << chunks_received << "\n";
    std::cout << "Time taken: "
              << seconds << " seconds\n";
    std::cout << "Average speed: "
              << speed << " MB/s\n";

    socket_close(client_sock);
    socket_close(server_sock);

    socket_cleanup();
}