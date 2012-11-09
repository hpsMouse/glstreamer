#ifndef _5784FE2E_274B_11E2_B935_206A8A22A96A
#define _5784FE2E_274B_11E2_B935_206A8A22A96A

#include <map>
#include <string>
#include <vector>

#include "exceptions.h"
#include "typemgr.h"

namespace glstreamer
{
    class TypeSpec;
    
    struct Slot
    {
        Slot(std::string const& name, TypeSpec *typeSpec):
        name(name),
        arg(nullptr),
        typeSpec(typeSpec)
        {}
        
        Slot(Slot const& slot):
        name(slot.name),
        arg(nullptr),
        typeSpec(slot.typeSpec)
        {}
        
        Slot& operator = (Slot const&) = delete;
        
        std::string const name;
        void *arg;
        TypeSpec* const typeSpec;
    };
    
    class ArgBlock
    {
    public:
        typedef std::vector<Slot>::size_type size_type;
        
        ArgBlock() = default;
        
        ArgBlock(ArgBlock const&) = delete;
        ArgBlock& operator = (ArgBlock const&) = delete;
        
        size_type argCount() const
        {
            return args.size();
        }
        
        Slot& arg(size_type i)
        {
            return args[i];
        }
        
        Slot const& arg(size_type i) const
        {
            return args[i];
        }
        
        Slot& arg(std::string const& name)
        {
            auto indexIter = name2index.find(name);
            if(indexIter == name2index.end())
                throw NotFound("argument \"" + name + "\"");
            return arg(indexIter->second);
        }
        
        Slot const& arg(std::string const& name) const
        {
            auto indexIter = name2index.find(name);
            if(indexIter == name2index.end())
                throw NotFound("argument \"" + name + "\"");
            return arg(indexIter->second);
        }
        
        template <typename T>
        ArgBlock& addSlot(std::string const& name)
        {
            if(name2index.find(name) != name2index.end())
                throw DefinationConflict(name);
            size_type index = args.size();
            args.push_back(Slot(name, TypeManager::getTypeSpec<T>()));
            name2index.insert(std::make_pair(name, index));
            return *this;
        }
        
        template <typename ... SlotTypes, typename ... NameTypes>
        ArgBlock& addSlots(NameTypes const& ... names)
        {
            static_assert(sizeof...(SlotTypes) == sizeof...(NameTypes), "sizeof(slots) != sizeof(names)");
            doAddSlots<SlotTypes...>(names...);
            return *this;
        }
        
    private:
        template <typename SlotType, typename ... SlotTypes, typename NameType, typename ... NameTypes>
        void doAddSlots(NameType const& name, NameTypes const& ... names)
        {
            addSlot<SlotType>(name);
            doAddSlots<SlotTypes...>(names...);
        }
        
        template <typename ...>
        void doAddSlots()
        {}
        
        std::vector<Slot> args;
        std::map<std::string, size_type> name2index;
    };
}

#endif
