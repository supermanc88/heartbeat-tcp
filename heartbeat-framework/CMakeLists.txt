cmake_minimum_required(VERSION 2.8)

project(heartbeat-framework)

#project(hbconf)

set(CMAKE_CXX_STANDARD 11)

#include_directories(protobuf-src)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w")

set(SRC_PATH
        main.cpp
        wrap.cpp
        heartbeat-config.h
        wrap.h
        hbconf.cpp
        hbconf.h
        telegram.pb.cc
        resource-mgr/resource-mgr.cpp
        plugin-mgr/plugin-manager.cpp
        make-telegram.cpp
        make-telegram.h hares.cpp hares.h
        common/custom-functions.cpp)

add_executable(heartbeat-framework  ${SRC_PATH})

target_link_libraries(heartbeat-framework dl)

target_link_libraries(heartbeat-framework protobuf pthread)

#add_executable(hbconf hbconf.cpp hbconf.h)

# 编译检测本机vip
add_subdirectory(resource-mgr/virtual-ip)
# 编译arp工具
add_subdirectory(ARP)
# 编译日志库
add_subdirectory(log)
# 编译ICMP插件
add_subdirectory(plugin-mgr/plugins/ICMP)

# 编译netsign插件
add_subdirectory(plugin-mgr/plugins/NetSignStatus)


# 编译hb_manual 手动切换
add_subdirectory(manual-switch)

# 单个模块测试
#
#add_subdirectory(log)
#
# 用来测试动态库加载
add_subdirectory(plugin-mgr)
#
#add_subdirectory(resource-mgr)