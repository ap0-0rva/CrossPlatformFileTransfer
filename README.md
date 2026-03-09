# Cross Platform File Transfer (C++ TCP)

A client–server program written in **C++** that transfers files over **TCP sockets**.

The client reads a file and sends it to the server in **fixed-size chunks**.
The server receives the chunks, verifies them using a checksum, and writes the file to disk.

During the transfer, the program displays **progress, total chunks, transfer time, and average speed**.

---

## Chunk Size

Files are divided into chunks before being sent.

Current chunk size:

**256 KB (262144 bytes)**

Using larger chunks reduces the number of network operations and improves performance for large file transfers.

---

## Project Structure

```
common/   -> shared protocol definitions
client/   -> client program (sends files)
server/   -> server program (receives files)
```

---

## Build Instructions

From the project root:

```
mkdir build
cd build
cmake ..
make
```

This will generate two executables:

```
server
client
```

---

## Running

Start the server:

```
./server
```

Then run the client:

```
./client
```

The client will ask for the file path, server IP, and destination path on the server.

---

## Example

Client:

```
ap0.0rva@MacBook build % ./client
Enter file path to send: /Users/ap0.0rva/Pictures/file.zip
Enter server IP: 127.0.0.1
Enter destination path on server: /Users/ap0.0rva/z.zip
```

Server:

```
ap0.0rva@MacBook build % ./server
Server listening on port 9000
Client connected
Receiving file...
Progress: 100%
File transfer complete
```
