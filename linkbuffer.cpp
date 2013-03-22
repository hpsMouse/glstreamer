#include "linkbuffer.h"

#include "exceptions.h"

using namespace glstreamer;

LinkBuffer::LinkBuffer( size_type bufferCount ):
emptySem(bufferCount),
fullSem(0),
buffers(new std::vector<char>[bufferCount]),
readBuffer(0),
writeBuffer(0),
bufferCount(bufferCount),
currentEmpty(nullptr)
{
    if(bufferCount == 0)
        throw UnsupportedOperation("Allocating zero-sized buffer");
}

LinkBuffer::~LinkBuffer() noexcept
{}
