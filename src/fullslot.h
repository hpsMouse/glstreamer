#ifndef _796836E2_2FB5_11E2_9115_206A8A22A96A
#define _796836E2_2FB5_11E2_9115_206A8A22A96A

#include <memory>
#include <string>
#include <utility>

#include "types.h"

#include "LocalArg.h"

namespace glstreamer
{
    struct FullSlot
    {
        SimpleSlot *simpleSlot;
        TypeSpec *typeSpec;
        std::shared_ptr<LocalArgBase> localArg;
        std::string name;
        Processor *processor;
        Direction direction;
        
        FullSlot() = default;
        
        template <typename Name>
        FullSlot(TypeSpec *typeSpec, std::shared_ptr<LocalArgBase> &&localArg, Name&& name, Processor *processor, Direction direction):
        simpleSlot(nullptr),
        typeSpec(typeSpec),
        localArg(std::move(localArg)),
        name(std::forward<Name>(name)),
        processor(processor),
        direction(direction)
        {}
        
        FullSlot(FullSlot const&) = delete;
        FullSlot& operator = (FullSlot const&) = delete;
        FullSlot& operator = (FullSlot&&) = delete;
        
        FullSlot(FullSlot&& src):
        simpleSlot(src.simpleSlot),
        typeSpec(src.typeSpec),
        localArg(std::move(src.localArg)),
        name(std::move(src.name)),
        processor(src.processor),
        direction(src.direction)
        {
            src.simpleSlot = nullptr;
            src.typeSpec = nullptr;
            src.processor = nullptr;
        }
    };
}

#endif
