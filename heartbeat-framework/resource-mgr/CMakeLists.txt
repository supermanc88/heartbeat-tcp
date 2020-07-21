cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_STANDARD 11)

project(resource-mgr)

include_directories(../)

set(RES_MGR_SRC resource-mgr.cpp resource-mgr.h
        common/custom-functions.h common/custom-functions.cpp)

add_executable(resource-mgr ${RES_MGR_SRC})


add_subdirectory(virtual-ip)