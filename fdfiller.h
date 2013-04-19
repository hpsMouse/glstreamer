#ifndef _1B600CC0_865C_11E2_8F0C_206A8A22A96A
#define _1B600CC0_865C_11E2_8F0C_206A8A22A96A

#include <memory>
#include <thread>

#include <sys/socket.h>

#include "CancellableThread.h"

#include "types.h"

namespace glstreamer
{
    class FdFiller : private CancellableThread
    {
    public:
        FdFiller(sockaddr const* addr, socklen_t addrLen, Word32 linkId, LinkBuffer* linkBuffer, TypeSpec* typeSpec);
        ~FdFiller();
        
    private:
        FdFiller(FdFiller const&) = delete;
        FdFiller& operator = (FdFiller const&) = delete;
        
        virtual void run() override;
        
        LinkBuffer *linkBuffer;
        int fd;
        std::unique_ptr<char[]> remote_addr;
        
        struct ParameterPack
        {
            ParameterPack() = default;
            socklen_t addrLen;
            Word32 linkId;
            TypeSpec *typeSpec;
        } parameterPack;
    };
}

#endif
