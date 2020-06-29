cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_STANDARD 11)

project(check-virtual-ip)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w")

add_executable(check-virtual-ip check-vip.cpp check-vip.h)