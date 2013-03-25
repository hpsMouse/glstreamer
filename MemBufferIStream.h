#ifndef _70B7EBA6_951C_11E2_B7AE_206A8A22A96A
#define _70B7EBA6_951C_11E2_B7AE_206A8A22A96A

#include "types.h"

#include "istream.h"

namespace glstreamer
{
    class MemBufferIStream : public IStream
    {
    public:
        MemBufferIStream(char const* buffer, size_type size);
        
        virtual IStream& readData ( void* data, size_type size ) override;
        
        virtual void const* requireInternalBuffer ( size_type size ) override;
        virtual IStream& releaseInternalBuffer ( const void* buffer, size_type size ) override;
        
    private:
        MemBufferIStream(MemBufferIStream const&) = delete;
        MemBufferIStream& operator = (MemBufferIStream const&) = delete;
        
        char const* buffer;
        size_type size;
    };
}

#endif
