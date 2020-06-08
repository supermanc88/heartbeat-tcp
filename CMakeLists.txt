cmake_minimum_required(VERSION 3.13)
project(heartbeat-tcp)

set(CMAKE_CXX_STANDARD 11)

#add_executable(heartbeat-tcp main.cpp)

#设置库文件存放路径
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/build/lib)

#设置执行文件存放路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/build/bin)

add_subdirectory(heartbeat-framework)