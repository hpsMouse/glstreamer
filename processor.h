#ifndef _57AA767C_2A48_11E2_A5E4_206A8A22A96A
#define _57AA767C_2A48_11E2_A5E4_206A8A22A96A

#include <type_traits>

#include "argblock.h"
#include "typelist.h"

namespace glstreamer
{
    class Processor
    {
    public:
        virtual ~Processor() = default;
        
        virtual void run() = 0;
        
        Slot& inputArg(ArgBlock::size_type i)
        {
            return inputArgs.arg(i);
        }
        
        Slot& inputArg(std::string const& name)
        {
            return inputArgs.arg(name);
        }
        
        Slot& outputArg(ArgBlock::size_type i)
        {
            return outputArgs.arg(i);
        }
        
        Slot& outputArg(std::string const& name)
        {
            return outputArgs.arg(name);
        }
        
    protected:
        ArgBlock inputArgs, outputArgs;
    };
    
    template <typename InputTypes, typename OutputTypes>
    class FixedProcessor;
    
    template <typename ... InputTypes, typename ... OutputTypes>
    class FixedProcessor <TypeList<InputTypes...>, TypeList<OutputTypes...>> : public Processor
    {
    public:
        template <typename ... NameTypes>
        FixedProcessor(NameTypes const& ... names)
        {
            static_assert(sizeof...(NameTypes) == sizeof...(InputTypes) + sizeof...(OutputTypes),
                          "sizeof(inputs) + sizeof(outputs) != sizeof(names)");
            
            addInputSlots<InputTypes...>(names...);
        }
        
        virtual void run() override
        {
            assignInput<0>();
        }
        
        virtual void do_run(InputTypes& ..., OutputTypes& ...) = 0;
        
    private:
        template <typename InputType, typename ... InputTypesTail, typename NameType, typename ... NameTypesTail>
        void addInputSlots(NameType const& name, NameTypesTail const& ... names)
        {
            inputArgs.addSlot<InputType>(name);
            addInputSlots<InputTypesTail...>(names...);
        }
        
        template <typename ... NameTypesTail>
        void addInputSlots(NameTypesTail const& ... names)
        {
            addOutputSlots<OutputTypes...>(names...);
        }
        
        template <typename OutputType, typename ... OutputTypesTail, typename NameType, typename ...NameTypesTail>
        void addOutputSlots(NameType const& name, NameTypesTail const& ... names)
        {
            outputArgs.addSlot<OutputType>(name);
            addOutputSlots<OutputTypesTail...>(names...);
        }
        
        template <typename ...>
        void addOutputSlots()
        {}
        
        template <unsigned i, typename ... UnpackedArgTypes>
        typename std::enable_if<i != sizeof...(InputTypes)>::type
        assignInput(UnpackedArgTypes& ... unpackedArgs)
        {
            assignInput<i+1>(unpackedArgs ..., *static_cast<typename TypeListPickRaw<i, InputTypes...>::type*>(inputArgs.arg(i).arg));
        }
        
        template <unsigned i, typename ... UnpackedArgTypes>
        typename std::enable_if<i == sizeof...(InputTypes)>::type
        assignInput(UnpackedArgTypes& ... unpackedArgs)
        {
            assignOutput<0>(unpackedArgs ...);
        }
        
        template <unsigned i, typename ... UnpackedArgTypes>
        typename std::enable_if<i != sizeof...(OutputTypes)>::type
        assignOutput(UnpackedArgTypes& ... unpackedArgs)
        {
            assignOutput<i+1>(unpackedArgs ..., *static_cast<typename TypeListPickRaw<i, OutputTypes...>::type*>(outputArgs.arg(i).arg));
        }
        
        template <unsigned i, typename ... UnpackedArgTypes>
        typename std::enable_if<i == sizeof...(OutputTypes)>::type
        assignOutput(UnpackedArgTypes& ... unpackedArgs)
        {
            do_run(unpackedArgs ...);
        }
    };
}

#endif
