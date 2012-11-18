#ifndef _796836E2_2FB5_11E2_9115_206A8A22A96A
#define _796836E2_2FB5_11E2_9115_206A8A22A96A

#include <string>
#include <utility>

#include "types.h"

namespace glstreamer
{
    struct FullSlot
    {
        SimpleSlot *simpleSlot;
        TypeSpec *typeSpec;
        std::string name;
        Processor *processor;
        enum Direction { Input, Output } direction;
        
        FullSlot() = default;
        
        template <typename Name>
        FullSlot(TypeSpec *typeSpec, Name&& name, Processor *processor, Direction direction):
        simpleSlot(nullptr),
        typeSpec(typeSpec),
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
        name(src.name),
        processor(src.processor),
        direction(direction)
        {
            src.simpleSlot = nullptr;
            src.typeSpec = nullptr;
            src.name.clear();
            src.processor = nullptr;
        }
    };
}

#endif
