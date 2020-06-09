cmake_minimum_required(VERSION 3.7)

set(CMAKE_CXX_STANDARD 11)


#link_libraries(${LIBRARY_OUTPUT_PATH}/../)

set(LIBRARY_DIRECTORY ${LIBRARY_OUTPUT_PATH})

link_libraries(${LIBRARY_DIRECTORY})

project(plug_netsignstatus)

include_directories(../../../log)

add_library(plug_netsignstatus SHARED plug-netsignstatus.cpp plug-netsignstatus.h)

target_link_libraries(plug_netsignstatus hb-log)