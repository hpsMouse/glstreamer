cmake_minimum_required(VERSION 2.8)

project(glstreamer)

add_definitions(-std=gnu++11 -Wall -Wextra -Weffc++ -Werror -pthread)

add_library(glstreamer STATIC cancel.cpp defaultspecs.cpp demuxer.cpp fdfiller.cpp fdpump.cpp internalsinglelink.cpp istream.cpp linkbuffer.cpp MemBufferIStream.cpp MemBufferOStream.cpp ostream.cpp posixexception.cpp posixfd.cpp posixsem.cpp posixshm.cpp posixshmistream.cpp posixshmostream.cpp posixunnamedsem.cpp prettyio.cpp processlink.cpp threadedlink.cpp typemgr.cpp typespec.cpp CancellationEnabler.cpp)

add_subdirectory(tests)

