#ifndef _C5B88F14_953C_11E2_9568_206A8A22A96A
#define _C5B88F14_953C_11E2_9568_206A8A22A96A

#include <memory>

#include <sys/socket.h>

#include "link.h"

#include "fdfiller.h"
#include "linkbuffer.h"
#include "types.h"
#include "typespec.h"

namespace glstreamer
{
    class SocketLinkOutput : public Link
    {
    public:
        SocketLinkOutput(OutputSlot const& srcSlot,
                         sockaddr const* addr,
                         socklen_t addrLen,
                         Word32 linkId,
                         size_type bufferCount = 2
                        );
        
        virtual ~SocketLinkOutput() noexcept;
        
        virtual void push ( SimpleSlot* src );
        virtual void fetch ( SimpleSlot* dst );
        
    private:
        SocketLinkOutput(SocketLinkOutput const&) = delete;
        SocketLinkOutput& operator = (SocketLinkOutput const&) = delete;
        
        FullSlot* slot;
        TypeSpec* typeSpec;
        size_type fixedLen;
        std::unique_ptr<void, Destroyer> arg;
        LinkBuffer linkBuffer;
        std::unique_ptr<FdFiller> filler;
    };
}

#endif
