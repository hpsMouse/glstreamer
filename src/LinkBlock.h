#ifndef _D80844FE_B871_11E2_9526_206A8A22A96A
#define _D80844FE_B871_11E2_9526_206A8A22A96A

#include <deque>
#include <memory>
#include <utility>

#include "types.h"

namespace glstreamer
{
    class LinkBlock
    {
    public:
        LinkBlock();
        ~LinkBlock();
        
        LinkBlock& addLink(Link* link);
        
        template <typename LinkType, typename ... Args>
        LinkBlock& link(Args&& ... args)
        {
            return addLink(new LinkType(std::forward<Args>(args)...));
        }
        
    private:
        LinkBlock(LinkBlock const&) = delete;
        LinkBlock& operator = (LinkBlock const&) = delete;
        
        std::deque<std::unique_ptr<Link>> links;
    };
}

#endif
