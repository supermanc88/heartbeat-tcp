cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_STANDARD 11)

project(resource-mgr)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w")

add_executable(resource-mgr resource-mgr.cpp resource-mgr.h)


add_subdirectory(virtual-ip)