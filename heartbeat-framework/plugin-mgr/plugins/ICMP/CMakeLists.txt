cmake_minimum_required(VERSION 3.7)

set(CMAKE_CXX_STANDARD 11)

include_directories(../../../log)

project(plug_icmp)


#link_libraries(/tmp/heartbeat/build/lib)
#link_libraries(${LIBRARY_OUTPUT_PATH}/../)

set(LIBRARY_DIRECTORY ${LIBRARY_OUTPUT_PATH}/../)

set(LIBRARY_OUTPUT_PATH ${LIBRARY_OUTPUT_PATH}/plugins)

message(${LIBRARY_DIRECTORY})

link_libraries(${LIBRARY_DIRECTORY})

#add_executable(plug_icmp plug-icmp.cpp plug-icmp.h)
add_library(plug_icmp SHARED plug-icmp.cpp plug-icmp.h)

target_link_libraries(plug_icmp hb-log)
