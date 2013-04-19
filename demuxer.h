#ifndef _33C886B8_4825_11E2_87EF_206A8A22A96A
#define _33C886B8_4825_11E2_87EF_206A8A22A96A

#include <map>
#include <memory>
#include <mutex>

#include <sys/types.h>
#include <sys/socket.h>

#include "CancellableThread.h"

#include "types.h"

#include "posixfd.h"

namespace glstreamer
{
    class Demuxer : private CancellableThread
    {
    public:
        Demuxer(sockaddr const* bindAddr, socklen_t addrLen);
        ~Demuxer();
        
        LinkBuffer* getLink(Word32 linkId, size_type bufferCount, TypeSpec* typeSpec);
        
    private:
        Demuxer(Demuxer const&) = delete;
        Demuxer& operator = (Demuxer const&) = delete;
        
        virtual void run() override;
        
        PosixFd listenfd;
        std::map<Word32, std::unique_ptr<FdPump>> pumps;
        
        std::mutex stateLock;
    };
}

#endif
