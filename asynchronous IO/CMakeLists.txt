cmake_minimum_required(VERSION 3.19)
project(Asynchronous_IO C)

set(CMAKE_C_STANDARD 11)

add_executable(Asynchronous_IO server.c client.c)