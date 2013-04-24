#include <cstring>

#include <stdexcept>

#include "posixshmostream.h"

#include "posixshm.h"

using namespace glstreamer;

PosixShmOStream::PosixShmOStream ( PosixShm& shm, size_type initialSize ):
buffer(static_cast<Buffer*>(shm.resize(initialSize + sizeof(uint32_t), true))),
_capacity(initialSize),
shm(shm)
{
    buffer->size = 0;
}

void PosixShmOStream::reserve ( size_type newSize )
{
    if(newSize > _capacity)
    {
        size_type newCapacity = _capacity;
        do
            newCapacity *= 2;
        while(newCapacity < newSize);
        expand(newCapacity);
    }
}

OStream& PosixShmOStream::writeData ( const void* data, size_type size )
{
    size_type newSize = buffer->size + size;
    
    reserve(newSize);
    
    std::memcpy(buffer->data + buffer->size, data, size);
    buffer->size = newSize;
    
    return *this;
}

void* PosixShmOStream::requireInternalBuffer ( size_type size )
{
    reserve(buffer->size + size);
    
    return buffer->data + buffer->size;
}

OStream& PosixShmOStream::pushInternalBuffer ( void* buffer, size_type size )
{
    if(buffer != this->buffer->data + this->buffer->size)
        throw std::invalid_argument(
            "PosixShmOStream::pushInternalBuffer: "
            "The ssupplied buffer doesn't match the internal buffer."
        );
    
    if(this->buffer->size + size > _capacity)
        throw std::invalid_argument(
            "PosixShmOStream::pushInternalBuffer: "
            "Pushed buffer size is greater than previously requested and is overflowing capacity."
        );
    
    this->buffer->size += size;
    
    return *this;
}

void PosixShmOStream::expand( size_type newCapacity )
{
    if(newCapacity > _capacity)
    {
        buffer = static_cast<Buffer*>(shm.resize(newCapacity + sizeof(uint32_t), true));
        _capacity = newCapacity;
    }
}
