cmake_minimum_required(VERSION 3.7)

set(CMAKE_CXX_STANDARD 11)

project(plug_netsignstatus)

add_library(plug_netsignstatus SHARED plug-netsignstatus.cpp plug-netsignstatus.h)