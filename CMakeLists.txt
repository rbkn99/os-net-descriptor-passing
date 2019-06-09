cmake_minimum_required(VERSION 3.10)
project(os-net-descriptor-passing)

set(CMAKE_CXX_STANDARD 14)

add_executable(os-net-descriptor-passing-server server.cpp utils.cpp utils.h)
add_executable(os-net-descriptor-passing-client client.cpp utils.cpp utils.h)
