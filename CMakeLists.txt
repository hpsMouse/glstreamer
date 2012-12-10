cmake_minimum_required(VERSION 2.8)

project(glstreamer)

add_definitions(-std=gnu++11 -Wall -Wextra -pthread)

add_library(glstreamer SHARED defaultspecs.cpp internalsinglelink.cpp posixexception.cpp posixsem.cpp posixshm.cpp posixshmistream.cpp posixshmostream.cpp processlink.cpp threadedlink.cpp typemgr.cpp)

add_subdirectory(tests)
