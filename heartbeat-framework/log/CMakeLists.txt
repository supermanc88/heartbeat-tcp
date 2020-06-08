cmake_minimum_required(VERSION 3.7)

project(hb-log)

set(CMAKE_CXX_STANDARD 11)

#add_executable(hb-log hb-log.cpp hb-log.h)
add_library(hb-log STATIC hb-log.h hb-log.cpp)