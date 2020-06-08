cmake_minimum_required(VERSION 3.7)

set(CMAKE_CXX_STANDARD 11)

project(plug_icmp)

add_library(plug_icmp SHARED plug-icmp.cpp plug-icmp.h)