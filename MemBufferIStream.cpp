#include <cstring>

#include "MemBufferIStream.h"

#include "exceptions.h"

using namespace glstreamer;

MemBufferIStream::MemBufferIStream ( const char* buffer, size_type size ):
buffer(buffer),
size(size)
{}

IStream& MemBufferIStream::readData ( void* data, size_type size )
{
    if(size > this->size)
        throw EOFError();
    
    std::memcpy(data, this->buffer, size);
    
    this->buffer += size;
    this->size -= size;
    
    return *this;
}

void const* MemBufferIStream::requireInternalBuffer ( size_type size )
{
    if(size > this->size)
        throw EOFError();
    
    void const* result = this->buffer;
    
    this->buffer += size;
    this->size -= size;
    
    return result;
}

IStream& MemBufferIStream::releaseInternalBuffer ( const void*, size_type )
{
    return *this;
}
