cmake_minimum_required(VERSION 3.13)

project(heartbeat-framework VERSION 1.0)

set(CMAKE_CXX_STANDARD 11)

add_subdirectory(ARP)

add_executable(heartbeat-framework main.cpp wrap.c heartbeat-config.h wrap.h)
