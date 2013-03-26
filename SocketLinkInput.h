#ifndef _332E7A3E_95F8_11E2_976D_206A8A22A96A
#define _332E7A3E_95F8_11E2_976D_206A8A22A96A

#include <memory>

#include <sys/socket.h>

#include "link.h"

#include "types.h"
#include "typespec.h"

namespace glstreamer
{
    class SocketLinkInput : Link
    {
    public:
        SocketLinkInput(Demuxer *demuxer,
                        Word32 linkId,
                        InputSlot const& dest,
                        size_type bufferCount = 2
                       );
        
        virtual ~SocketLinkInput() noexcept;
        
        virtual void push ( SimpleSlot* src ) override;
        virtual void fetch ( SimpleSlot* dst ) override;
        
    private:
        SocketLinkInput(SocketLinkInput const&) = delete;
        SocketLinkInput& operator = (SocketLinkInput const&) = delete;
        
        FullSlot *slot;
        TypeSpec *typeSpec;
        size_type fixedLen;
        std::unique_ptr<void, Destroyer> arg;
        LinkBuffer* linkBuffer;
    };
}

#endif
