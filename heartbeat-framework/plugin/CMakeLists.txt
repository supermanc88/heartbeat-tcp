cmake_minimum_required(VERSION 3.7)

set(CMAKE_CXX_STANDARD 11)

project(test-lib)
add_library(test-lib SHARED test-lib.cpp test-lib.h hb-plugin.h)


project(plugin-test)
add_executable(plugin-test plugin-manager.cpp plugin-manager.h)
target_link_libraries(plugin-test dl)

