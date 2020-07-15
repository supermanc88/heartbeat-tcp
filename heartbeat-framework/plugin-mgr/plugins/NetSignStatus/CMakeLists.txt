cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_STANDARD 11)



set(LIBRARY_DIRECTORY ${LIBRARY_OUTPUT_PATH})

set(LIBRARY_OUTPUT_PATH ${LIBRARY_OUTPUT_PATH}/plugins)

link_directories(${LIBRARY_DIRECTORY})

project(plug_netsignstatus)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

message(${LIBRARY_DIRECTORY})

include_directories(../../../log)

set(NETSIGN_SRC plug-netsignstatus.cpp plug-netsignstatus.h
        Detect.h Detect.cpp
        ErrMsg.h
        MessageIdentifiers.h
        MonitorSrv.cpp MonitorSrv.h
        MyFunc.cpp MyFunc.h
        MyLog.cpp MyLog.h
        SlimXml.cpp SlimXml.h)

add_library(plug_netsignstatus SHARED ${NETSIGN_SRC})

target_link_libraries(plug_netsignstatus hb-log)

add_executable(netsigntest ${NETSIGN_SRC})
target_link_libraries(netsigntest hb-log)