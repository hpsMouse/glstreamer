cmake_minimum_required(VERSION 2.8)

project(glstreamer)

add_definitions(-std=gnu++11 -Wall -Wextra -pthread)

add_executable(glstreamer defaultspecs.cpp internalsinglelink.cpp threadedlink.cpp typemgr.cpp main.cpp testfactory.cpp)

target_link_libraries(glstreamer pthread)
