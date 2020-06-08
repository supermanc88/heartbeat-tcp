cmake_minimum_required(VERSION 3.7)

set(CMAKE_CXX_STANDARD 11)

project(plug_netsignstatus)

link_libraries(${LIBRARY_OUTPUT_PATH}/../)

add_library(plug_netsignstatus SHARED plug-netsignstatus.cpp plug-netsignstatus.h)

target_link_libraries(plug_netsignstatus hb-log)