#ifndef _43064A30_2FB5_11E2_8F82_206A8A22A96A
#define _43064A30_2FB5_11E2_8F82_206A8A22A96A

#include <cstddef>

namespace glstreamer
{
    class ArgBlock;
    class Destroyer;
    class FullSlot;
    class InputSlot;
    class InternalSingleLink;
    class IStream;
    class Link;
    class OStream;
    class OutputSlot;
    class PosixException;
    class PosixSem;
    class PosixShm;
    class ProcessLink;
    class Processor;
    class SimpleSlot;
    class ThreadedLink;
    class TypeManager;
    class TypeSpec;
    
    using size_type = std::size_t;
    
    enum Direction {Input, Output};
}

#endif
