cmake_minimum_required(VERSION 2.8)

project(glstreamer)

add_definitions(-std=gnu++11 -Wall -Wextra)

add_executable(glstreamer internallink.cpp defaultspecs.cpp typemgr.cpp main.cpp)

