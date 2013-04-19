cmake_minimum_required(VERSION 2.8)

project(glstreamer)

add_definitions(-std=gnu++11 -Wall -Wextra -Weffc++ -Werror -pthread)

add_library(glstreamer SHARED demuxer.cpp fdfiller.cpp fdpump.cpp glstreamer.cpp internalsinglelink.cpp istream.cpp linkbuffer.cpp MemBufferIStream.cpp MemBufferOStream.cpp ostream.cpp posixexception.cpp posixfd.cpp posixsem.cpp posixshm.cpp posixshmistream.cpp posixshmostream.cpp posixunnamedsem.cpp prettyio.cpp processlink.cpp threadedlink.cpp typemgr.cpp typespec.cpp CancellableThread.cpp ProcessorManager.cpp SocketLinkInput.cpp SocketLinkOutput.cpp)

target_link_libraries(glstreamer dl posixpp_shared)

add_subdirectory(tests)

add_subdirectory(core)

add_subdirectory(gl)