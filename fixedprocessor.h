#ifndef _A9EE8832_2E18_11E2_8A17_206A8A22A96A
#define _A9EE8832_2E18_11E2_8A17_206A8A22A96A

#include <type_traits>
#include <utility>

#include "processor.h"

#include "typelist.h"

namespace glstreamer
{
    template <typename InputTypes, typename OutputTypes>
    class FixedProcessor;
    
    template <typename ... InputTypes, typename ... OutputTypes>
    class FixedProcessor <TypeList<InputTypes...>, TypeList<OutputTypes...>> : public Processor
    {
    public:
        FixedProcessor(typename Const<std::string const&, InputTypes>::type ... inputNames,
                       typename Const<std::string const&, OutputTypes>::type ... outputNames)
        {
            inputArgs.addSlots<InputTypes...>(inputNames...);
            outputArgs.addSlots<OutputTypes...>(outputNames...);
        }
        
    protected:
        virtual void do_run(InputTypes& ... inputs, OutputTypes& ... outputs) = 0;
        
    private:
        virtual void run() override final
        {
            assignInputs<0, InputTypes...>();
        }
        
        template <size_type i, typename Head, typename ... Tail, typename ... Assigned>
        void assignInputs(Assigned& ... args)
        {
            assignInputs<i+1, Tail...>(args ..., *static_cast<Head*>(inputArgs.simpleSlot(i).arg));
        }
        
        template <size_type i, typename ... Tail, typename ... Assigned>
        typename std::enable_if<sizeof...(Tail) == 0>::type
        assignInputs(Assigned& ... args)
        {
            assignOutputs<0, OutputTypes...>(args ...);
        }
        
        template <size_type i, typename Head, typename ... Tail, typename ... Assigned>
        void assignOutputs(Assigned& ... args)
        {
            assignOutputs<i+1, Tail...>(args ..., *static_cast<Head*>(outputArgs.simpleSlot(i).arg));
        }
        
        template <size_type i, typename ... Tail, typename ... Assigned>
        typename std::enable_if<sizeof...(Tail) == 0>::type
        assignOutputs(Assigned& ... args)
        {
            do_run(args ...);
        }
    };
}

#endif
