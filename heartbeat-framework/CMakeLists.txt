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
        resource-mgr/resource-mgr.cpp
        plugin-mgr/plugin-manager.cpp)

add_executable(heartbeat-framework  ${SRC_PATH})

target_link_libraries(heartbeat-framework dl)

#add_executable(hbconf hbconf.cpp hbconf.h)

# 编译检测本机vip
add_subdirectory(resource-mgr/virtual-ip)
# 编译arp工具
add_subdirectory(ARP)
# 编译日志库
add_subdirectory(log)
# 编译ICMP插件
add_subdirectory(plugin-mgr/plugins/ICMP)

# 单个模块测试
#
#add_subdirectory(log)
#
#add_subdirectory(plugin-mgr)
#
#add_subdirectory(resource-mgr)