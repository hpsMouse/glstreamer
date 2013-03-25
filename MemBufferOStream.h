#ifndef _5A26FAC4_951E_11E2_A37C_206A8A22A96A
#define _5A26FAC4_951E_11E2_A37C_206A8A22A96A

#include <vector>

#include "ostream.h"

namespace glstreamer
{
    class MemBufferOStream : public OStream
    {
    public:
        MemBufferOStream(std::vector<char> &buffer);
        
        virtual OStream& writeData ( const void* data, size_type size ) override;
        
        virtual void* requireInternalBuffer ( size_type size ) override;
        virtual OStream& pushInternalBuffer ( void* buffer, size_type size ) override;
        
    private:
        std::vector<char> &buffer;
    };
}

#endif
