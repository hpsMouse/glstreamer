#ifndef _78073BC2_42A2_11E2_BBBB_206A8A22A96A
#define _78073BC2_42A2_11E2_BBBB_206A8A22A96A

#include "istream.h"

namespace glstreamer
{
    class PosixShmIStream : public IStream
    {
    public:
        explicit PosixShmIStream(PosixShm& shm);
        
        virtual ~PosixShmIStream() noexcept = default;
        
        virtual IStream& readData ( void* data, size_type size ) override;
        
        size_type remainingBytes() const
        { return buffer->size - pos; }
        
    private:
        struct Buffer
        {
            uint32_t size;
            char     data[];
        };
        
        PosixShmIStream(PosixShmIStream const&) = delete;
        PosixShmIStream& operator = (PosixShmIStream const&) = delete;
        
        Buffer *buffer;
        size_type pos;
        PosixShm& shm;
    };
}

#endif
