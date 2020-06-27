cmake_minimum_required(VERSION 3.7)

set(CMAKE_CXX_STANDARD 11)


#link_libraries(${LIBRARY_OUTPUT_PATH}/../)

set(LIBRARY_DIRECTORY ${LIBRARY_OUTPUT_PATH}/../)

link_libraries(${LIBRARY_DIRECTORY})

project(plug_netsignstatus)

include_directories(../../../log)

set(NETSIGN_SRC plug-netsignstatus.cpp plug-netsignstatus.h
        Detect.h Detect.cpp
        ErrMsg.h
        MessageIdentifiers.h
        MonitorSrv.cpp MonitorSrv.h
        MyFunc.cpp MyFunc.h
        MyLog.cpp MyLog.h
        Ping.cpp Ping.h
        SlimXml.cpp SlimXml.h)

add_library(plug_netsignstatus SHARED ${NETSIGN_SRC})

target_link_libraries(plug_netsignstatus hb-log)