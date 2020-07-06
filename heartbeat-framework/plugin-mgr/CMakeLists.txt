cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_STANDARD 11)

#set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -w")

project(plugin-mgr)
add_executable(plugin-mgr plugin-manager.cpp plugin-manager.h)
target_link_libraries(plugin-mgr dl)


set(LIBRARY_OUTPUT_PATH ${LIBRARY_OUTPUT_PATH}/plugins)

message(${LIBRARY_OUTPUT_PATH})

#project(test-lib)
#add_library(test-lib SHARED test-lib.cpp test-lib.h hb-plugin.h)

add_subdirectory(plugins/ICMP)

add_subdirectory(plugins/NetSignStatus)