cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_STANDARD 11)



set(LIBRARY_DIRECTORY ${LIBRARY_OUTPUT_PATH})

set(LIBRARY_OUTPUT_PATH ${LIBRARY_OUTPUT_PATH}/plugins)

link_directories(${LIBRARY_DIRECTORY})

project(plug_icmp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

message("lib dir: ${LIBRARY_DIRECTORY}")

include_directories(../../../)

set(ICMP_SRC loadconfig.h loadconfig.cpp
        plug-icmp.cpp plug-icmp.h
        ../../../heartbeat-config.h
        ../../../hbconf.cpp ../../../hbconf.h
        ../../../common/custom-functions.cpp ../../../common/custom-functions.h
        )


add_library(plug_icmp SHARED ${ICMP_SRC})

target_link_libraries(plug_icmp hb-log)
