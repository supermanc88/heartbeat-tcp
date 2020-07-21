cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_STANDARD 11)

project(check-virtual-ip)

include_directories(../../)

set(CHECKVIP_SRC check-vip.cpp check-vip.h
        ../../common/custom-functions.cpp ../../common/custom-functions.h)

add_executable(check-virtual-ip ${CHECKVIP_SRC})