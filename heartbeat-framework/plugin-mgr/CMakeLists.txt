cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_STANDARD 11)

project(plugin-mgr)

set(LIBRARY_OUTPUT_PATH ${LIBRARY_OUTPUT_PATH}/plugins)

message(${LIBRARY_OUTPUT_PATH})

add_executable(loadtest loadtest.cpp)
target_link_libraries(loadtest dl)
