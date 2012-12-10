#ifndef _5D42AD08_4293_11E2_AFEE_206A8A22A96A
#define _5D42AD08_4293_11E2_AFEE_206A8A22A96A

#include "ostream.h"

namespace glstreamer
{
    class PosixShmOStream : public OStream
    {
    public:
        /**
         * \brief Constructor
         * 
         * \param shm Underlying shared memeory object.
         * \param initialSize The initial size of shared memory, must be greater
         *                    than zero.
         */
        explicit PosixShmOStream(PosixShm& shm, size_type initialSize = 4096);
        
        virtual ~PosixShmOStream() noexcept override = default;
        
        void reserve(size_type newSize);
        
        virtual OStream& writeData ( const void* data, size_type size ) override;
        
        virtual void* requireInternalBuffer ( size_type size ) override;
        virtual OStream& pushInternalBuffer ( void* buffer, size_type size ) override;
        
    private:
        struct Buffer
        {
            uint32_t size;
            char     data[];
        };
        
        PosixShmOStream(PosixShmOStream const&) = delete;
        PosixShmOStream& operator = (PosixShmOStream const&) = delete;
        
        void expand( glstreamer::size_type newCapacity );
        
        Buffer *buffer;
        size_type _capacity;
        PosixShm& shm;
    };
}

#endif
