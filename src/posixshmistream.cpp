#include <cstring>

#include <stdexcept>

#include "posixshmistream.h"

#include "posixshm.h"

using namespace glstreamer;

PosixShmIStream::PosixShmIStream ( PosixShm& shm ):
buffer(static_cast<Buffer*>(shm.map())),
pos(0),
shm(shm)
{}

IStream& PosixShmIStream::readData ( void* data, size_type size )
{
    if(pos + size > buffer->size)
        throw std::invalid_argument(
            "PosixShmIStream::readData: "
            "Read across end of buffer."
        );
    
    std::memcpy(data, buffer->data + pos, size);
    
    pos += size;
    
    return *this;
}
