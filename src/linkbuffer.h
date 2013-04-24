#ifndef _2DF28F64_45EA_11E2_91EA_206A8A22A96A
#define _2DF28F64_45EA_11E2_91EA_206A8A22A96A

#include <memory>
#include <vector>

#include "types.h"

#include "posixunnamedsem.h"

namespace glstreamer
{
    struct LinkBuffer
    {
        LinkBuffer(size_type bufferCount);
        ~LinkBuffer() noexcept;
        
        std::vector<char>& getFull()
        {
            fullSem.wait();
            std::vector<char>& result = buffers[readBuffer];
            readBuffer = (readBuffer + 1) % bufferCount;
            return result;
        }
        
        std::vector<char>& getEmpty()
        {
            if(currentEmpty != nullptr)
                return *currentEmpty;
            
            emptySem.wait();
            std::vector<char>& result = buffers[writeBuffer];
            writeBuffer = (writeBuffer + 1) % bufferCount;
            
            currentEmpty = &result;
            return result;
        }
        
        void putFull()
        {
            if(currentEmpty == nullptr)
                return;
            
            currentEmpty = nullptr;
            fullSem.post();
        }
        
        void putEmpty()
        {
            emptySem.post();
        }
        
    private:
        LinkBuffer(LinkBuffer const&) = delete;
        LinkBuffer& operator = (LinkBuffer const&) = delete;
        
        PosixUnnamedSem emptySem, fullSem;
        
        std::unique_ptr<std::vector<char>[]> buffers;
        
        size_type readBuffer, writeBuffer;
        size_type bufferCount;
        
        std::vector<char>* currentEmpty;
    };
}

#endif
