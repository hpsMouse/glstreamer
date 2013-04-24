#include "ostream.h"

using namespace glstreamer;

void* OStream::requireInternalBuffer ( size_type size )
{
    this->internalBuffer.resize(size);
    return this->internalBuffer.data();
}

OStream& OStream::pushInternalBuffer ( void*, size_type )
{
    this->writeData(this->internalBuffer.data(), this->internalBuffer.size());
    this->internalBuffer.clear();
    return *this;
}
