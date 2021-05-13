cmake_minimum_required(VERSION 3.19)
project(unix_domain_socket_communication C)

set(CMAKE_C_STANDARD 11)

add_executable(unix_domain_socket_communication server.c client.c)