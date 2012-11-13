#ifndef _8642DBDE_2D6B_11E2_846D_206A8A22A96A
#define _8642DBDE_2D6B_11E2_846D_206A8A22A96A

#include <string>

#include "link.h"

namespace glstreamer
{
    class TypeSpec;
    
    struct Slot
    {
        Slot(std::string const& name, TypeSpec *typeSpec):
        name(name),
        arg(nullptr),
        typeSpec(typeSpec),
        link(nullptr)
        {}
        
        Slot(Slot const& slot):
        name(slot.name),
        arg(nullptr),
        typeSpec(slot.typeSpec),
        link(nullptr)
        {}
        
        Slot& operator = (Slot const&) = delete;
        
        void fetch()
        {
            link->fetch(this);
        }
        void push()
        {
            link->push(this);
        }
        
        std::string const name;
        void *arg;
        TypeSpec* const typeSpec;
        Link *link;
    };
}

#endif
