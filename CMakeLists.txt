cmake_minimum_required(VERSION 2.8)

project(glstreamer)

add_definitions(-std=gnu++11 -Wall -Wextra -pthread)

add_library(glstreamer SHARED defaultspecs.cpp internalsinglelink.cpp posixexception.cpp posixsem.cpp threadedlink.cpp typemgr.cpp)

add_executable(test main.cpp testfactory.cpp)

target_link_libraries(test pthread rt glstreamer)
