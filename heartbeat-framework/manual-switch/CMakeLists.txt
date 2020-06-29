cmake_minimum_required(VERSION 2.8)

project(hb_manual)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w")

set(HB_MANUAL hb_manually.cpp hb_manually.h)

add_executable(hb_manual ${HB_MANUAL})