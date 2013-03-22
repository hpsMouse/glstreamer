#ifndef _72F1AC6C_46BC_11E2_9A0D_206A8A22A96A
#define _72F1AC6C_46BC_11E2_9A0D_206A8A22A96A

#include <type_traits>

#include "types.h"

namespace glstreamer
{
    void readFd(int fd, void *buf, size_type size);
    void writeFd(int fd, void const* buf, size_type size);
    
    template <typename T>
    inline void readFd(int fd, T& obj)
    {
        readFd(fd, static_cast<void*>(&obj), sizeof(T));
    }
    
    template <typename T>
    inline void writeFd(int fd, T const& obj)
    {
        writeFd(fd, static_cast<void const*>(&obj), sizeof(T));
    }
}

#endif
