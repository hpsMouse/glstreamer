#ifndef _43064A30_2FB5_11E2_8F82_206A8A22A96A
#define _43064A30_2FB5_11E2_8F82_206A8A22A96A

#include <cstddef>
#include <cstdint>

namespace glstreamer
{
    class ArgBlock;
    class DataHeader;
    class Demuxer;
    class Destroyer;
    class FdFiller;
    class FdPump;
    class FullSlot;
    class InputSlot;
    class InternalSingleLink;
    class IStream;
    class Link;
    class LinkBuffer;
    class LinkHeader;
    class MemBufferIStream;
    class MemBufferOStream;
    class Muxer;
    class NetUInt32;
    class OStream;
    class OutputSlot;
    class PosixException;
    class PosixFd;
    class PosixSem;
    class PosixShm;
    class PosixShmIStream;
    class PosixShmOStream;
    class PosixUnnamedSem;
    class ProcessLink;
    class Processor;
    class ProcessorManager;
    class SimpleSlot;
    class SocketLinkOutput;
    class ThreadedLink;
    class TypeManager;
    class TypeSpec;
    
    using size_type = std::size_t;
    using Word16 = std::uint16_t;
    using Word32 = std::uint32_t;
    using Word64 = std::uint64_t;
    
    enum Direction {Input, Output};
}

#endif
