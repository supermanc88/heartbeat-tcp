cmake_minimum_required(VERSION 2.8)

project(hb-log)

set(CMAKE_CXX_STANDARD 11)

# 没有这句的话  提示编译的时候需要 -fPIC
#set(CMAKE_POSITION_INDEPENDENT_CODE 1)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

include_directories(../)

set(LOG_SRC hb-log.h hb-log.cpp
        ../common/custom-functions.h ../common/custom-functions.cpp)

add_library(hb-log STATIC ${LOG_SRC})

#add_executable(logtest logtest.cpp hb-log.cpp ../common/custom-functions.h ../common/custom-functions.cpp)