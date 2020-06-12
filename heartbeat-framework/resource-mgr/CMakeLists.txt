cmake_minimum_required(VERSION 3.7)

set(CMAKE_CXX_STANDARD 11)

include_directories(../)

project(resource-mgr)

add_executable(resource-mgr resource-mgr.cpp resource-mgr.h)


add_subdirectory(virtual-ip)