cmake_minimum_required(VERSION 3.16)

project(FileTransfer)

set(CMAKE_CXX_STANDARD 17)

include_directories(common)

add_executable(server server/server.cpp)
add_executable(client client/client.cpp)

if(WIN32)
    target_link_libraries(server ws2_32)
    target_link_libraries(client ws2_32)
endif()