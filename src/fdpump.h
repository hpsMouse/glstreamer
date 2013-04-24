#ifndef _ABC3D5C2_46AF_11E2_B790_206A8A22A96A
#define _ABC3D5C2_46AF_11E2_B790_206A8A22A96A

#include <condition_variable>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

#include "types.h"

#include "protocoltypes.h"
#include "nettypes.h"

namespace glstreamer
{
    class FdPump
    {
    public:
        FdPump();
        ~FdPump();
        
        void assignLink(int fd, LinkHeader const& header);
        LinkBuffer* allocateBuffer(size_type bufferCount, TypeSpec *typeSpec);
        
    private:
        FdPump(FdPump const&) = delete;
        FdPump& operator = (FdPump const&) = delete;
        
        void run();
        
        bool resourceReady();
        void tryEmitReady();
        
        void writeError(Word32 errCode);
        
        std::unique_ptr<LinkBuffer> linkBuffer;
        int fd;
        
        char const* typeName;
        size_type fixedLen;
        LinkHeader linkHeader;
        std::vector<char> remoteTypeName;
        
        std::mutex stateLock;
        std::condition_variable condResourceReady;
        
        std::thread runner;
    };
}

#endif
