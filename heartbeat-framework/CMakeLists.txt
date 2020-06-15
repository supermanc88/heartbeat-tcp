cmake_minimum_required(VERSION 3.7)

project(heartbeat-framework VERSION 1.0)

#project(hbconf)

set(CMAKE_CXX_STANDARD 11)

set(SRC_PATH
        main.cpp
        wrap.cpp
        heartbeat-config.h
        wrap.h
        hbconf.cpp
        hbconf.h
        resource-mgr/resource-mgr.cpp)

add_executable(heartbeat-framework  ${SRC_PATH})

#add_executable(hbconf hbconf.cpp hbconf.h)

# 单个模块测试
#add_subdirectory(ARP)
#
#add_subdirectory(log)
#
#add_subdirectory(plugin-mgr)
#
#add_subdirectory(resource-mgr)