#include "istream.h"

using namespace glstreamer;

void const* IStream::requireInternalBuffer ( size_type size )
{
    internalBuffer.resize(size);
    this->readData(internalBuffer.data(), size);
    return internalBuffer.data();
}

IStream& IStream::releaseInternalBuffer ( const void*, size_type )
{
    internalBuffer.clear();
    return *this;
}
