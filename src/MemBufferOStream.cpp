#include <cstring>

#include "MemBufferOStream.h"

using namespace glstreamer;

MemBufferOStream::MemBufferOStream ( std::vector< char >& buffer ):
buffer(buffer)
{}

OStream& MemBufferOStream::writeData ( const void* data, size_type size )
{
    auto currentSize = buffer.size();
    buffer.resize(currentSize + size);
    std::memcpy(buffer.data() + currentSize, data, size);
    return *this;
}

void* MemBufferOStream::requireInternalBuffer ( size_type size )
{
    auto currentSize = buffer.size();
    buffer.resize(currentSize + size);
    return buffer.data() + currentSize;
}

OStream& MemBufferOStream::pushInternalBuffer ( void*, size_type )
{
    return *this;
}
