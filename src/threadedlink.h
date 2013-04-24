#ifndef _A9CA0176_2E1B_11E2_9CAC_206A8A22A96A
#define _A9CA0176_2E1B_11E2_9CAC_206A8A22A96A

#include <condition_variable>
#include <memory>
#include <mutex>

#include "link.h"

#include "types.h"
#include "typespec.h"

namespace glstreamer
{
    class ThreadedLink : public Link
    {
    public:
        ThreadedLink(OutputSlot const& srcSlot, InputSlot const& dstSlot, size_type queueSize);
        virtual ~ThreadedLink() noexcept;
        
        ThreadedLink(ThreadedLink const&) = delete;
        ThreadedLink& operator = (ThreadedLink const&) = delete;
        
        virtual void push(SimpleSlot* src) override;
        virtual void fetch(SimpleSlot* dst) override;
        
    private:
        FullSlot* src;
        FullSlot* dst;
        size_type queueSize;
        
        TypeSpec *typeSpec;
        std::unique_ptr<std::unique_ptr<void, Destroyer>[]> argBuffers;
        size_type currentSize, indexIn, indexOut;
        
        std::mutex queueLock;
        std::condition_variable condPush, condFetch;
    };
}

#endif
