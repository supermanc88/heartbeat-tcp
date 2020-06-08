cmake_minimum_required(VERSION 3.7)

set(CMAKE_CXX_STANDARD 11)

project(plugin-mgr)
add_executable(plugin-mgr plugin-manager.cpp plugin-manager.h)
target_link_libraries(plugin-mgr dl)





set(LIBRARY_OUTPUT_PATH ${LIBRARY_OUTPUT_PATH}/plugins)

#project(test-lib)
#add_library(test-lib SHARED test-lib.cpp test-lib.h hb-plugin.h)

add_subdirectory(plugins/ICMP)

add_subdirectory(plugins/NetSignStatus)