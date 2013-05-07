#ifndef _5784FE2E_274B_11E2_B935_206A8A22A96A
#define _5784FE2E_274B_11E2_B935_206A8A22A96A

#include <functional>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "types.h"

#include "exceptions.h"
#include "fullslot.h"
#include "link.h"
#include "simpleslot.h"
#include "typelist.h"
#include "typemgr.h"
#include "typespec.h"

namespace glstreamer
{
    class ArgBlock
    {
    public:
        ArgBlock(Processor *processor, Direction direction):
        simpleSlots(),
        fullSlots(),
        fullSlotsRef(),
        processor(processor),
        direction(direction)
        {}
        
        ArgBlock(ArgBlock const&) = delete;
        ArgBlock(ArgBlock&&) = delete;
        ArgBlock& operator = (ArgBlock const&) = delete;
        ArgBlock& operator = (ArgBlock&&) = delete;
        
        size_type argCount() const
        {
            return simpleSlots.size();
        }
        
        SimpleSlot& simpleSlot(size_type index)
        {
            return simpleSlots[index];
        }
        
        FullSlot& fullSlot(size_type index)
        {
            return *fullSlotsRef[index];
        }
        
        FullSlot& fullSlot(std::string const& name)
        {
            auto const& iter = fullSlots.find(name);
            if(iter == fullSlots.end())
                throw NotFound("Slot " + name);
            return iter->second;
        }
        
        template <typename T>
        ArgBlock& _addSlot(std::string const& name)
        {
            TypeSpec* typeSpec = TypeManager::getTypeSpec<T>();
            auto const& result = fullSlots.insert(std::make_pair(name, FullSlot(typeSpec, std::shared_ptr<LocalArgBase>(typeSpec->createLocal()), name, processor, direction)));
            if(!result.second)
                throw DefinitionConflict("Slot " + name);
            fullSlotsRef.push_back(&result.first->second);
            return *this;
        }
        
        template <typename T>
        ArgBlock& addSlot(std::string const& name)
        {
            _addSlot<T>(name);
            refreshSimpleSlots();
            return *this;
        }
        
        template <typename ... Types>
        ArgBlock& _addSlots(typename Const<std::string const&, Types>::type ... names)
        {
            sequence(std::bind(&ArgBlock::_addSlot<Types>, this, names)...);
            return *this;
        }
        
        template <typename ... Types>
        ArgBlock& addSlots(typename Const<std::string const&, Types>::type ... names)
        {
            _addSlots<Types...>(names...);
            refreshSimpleSlots();
            return *this;
        }
        
        void refreshSimpleSlots()
        {
            std::vector<SimpleSlot> newSimpleSlots;
            newSimpleSlots.reserve(fullSlotsRef.size());
            for(FullSlot* slot : fullSlotsRef)
            {
                if(slot->simpleSlot != nullptr)
                    newSimpleSlots.push_back(*slot->simpleSlot);
                else
                    newSimpleSlots.emplace_back(slot->typeSpec);
                slot->simpleSlot = &newSimpleSlots.back();
            }
            simpleSlots = std::move(newSimpleSlots);
        }
        
        void fetchArgs()
        {
            for(SimpleSlot& slot : simpleSlots)
                slot.link->fetch(&slot);
        }
        
        void pushArgs()
        {
            for(SimpleSlot& slot : simpleSlots)
                slot.link->push(&slot);
        }
        
        void clearArgs()
        {
            for(SimpleSlot& slot : simpleSlots)
                slot.typeSpec->clear(slot.arg);
        }
        
    private:
        std::vector<SimpleSlot> simpleSlots;
        std::map<std::string, FullSlot> fullSlots;
        std::vector<FullSlot*> fullSlotsRef;
        Processor *processor;
        Direction direction;
    };
}

#endif
