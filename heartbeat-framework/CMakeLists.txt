cmake_minimum_required(VERSION 3.13)

project(heartbeat-framework VERSION 1.0)

#project(hbconf)

set(CMAKE_CXX_STANDARD 11)

add_executable(heartbeat-framework main.cpp wrap.cpp heartbeat-config.h wrap.h hbconf.cpp hbconf.h)

#add_executable(hbconf hbconf.cpp hbconf.h)

add_subdirectory(ARP)

add_subdirectory(log)

add_subdirectory(plugin-mgr)