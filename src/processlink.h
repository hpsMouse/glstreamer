#ifndef _4DE98482_3D28_11E2_9EC3_206A8A22A96A
#define _4DE98482_3D28_11E2_9EC3_206A8A22A96A

#include <memory>
#include <string>

#include "link.h"

#include "posixsem.h"
#include "posixshm.h"
#include "typespec.h"

#include "types.h"

namespace glstreamer
{
    class ProcessLink : public Link
    {
    public:
        ProcessLink(OutputSlot const& srcSlot, std::string const& name, size_type queueSize);
        ProcessLink(std::string const& name, InputSlot const& dstSlot, size_type queueSize);
        
        virtual ~ProcessLink() noexcept;
        
        virtual void push ( SimpleSlot* src );
        virtual void fetch ( SimpleSlot* dst );
        
    private:
        ProcessLink(ProcessLink const&) = delete;
        ProcessLink& operator = (ProcessLink const&) = delete;
        
        void openResources(std::string const& name);
        
        TypeSpec *typeSpec;
        FullSlot *slot;
        
        PosixSem emptyLeft;
        PosixSem fullLeft;
        
        PosixShm ctrlBlock;
        std::unique_ptr<PosixShm[]> buffers;
        size_type currentBufferId;
        size_type queueSize;
        
        std::unique_ptr<void, Destroyer> arg;
        
        Direction direction;
    };
}

#endif
