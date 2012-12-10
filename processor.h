#ifndef _57AA767C_2A48_11E2_A5E4_206A8A22A96A
#define _57AA767C_2A48_11E2_A5E4_206A8A22A96A

#include "argblock.h"
#include "slottypes.h"

namespace glstreamer
{
    class Processor
    {
    public:
        Processor():
        inputArgs(this, Input),
        outputArgs(this, Output)
        {}
        virtual ~Processor() = default;
        
        void execute()
        {
            outputArgs.clearArgs();
            inputArgs.fetchArgs();
            run();
            outputArgs.pushArgs();
        }
        
        size_type inputArgCount() const
        {
            return inputArgs.argCount();
        }
        
        size_type outputArgCount() const
        {
            return outputArgs.argCount();
        }
        
        InputSlot inputArg(size_type i)
        {
            return InputSlot(inputArgs.fullSlot(i));
        }
        
        InputSlot inputArg(std::string const& name)
        {
            return InputSlot(inputArgs.fullSlot(name));
        }
        
        OutputSlot outputArg(size_type i)
        {
            return OutputSlot(outputArgs.fullSlot(i));
        }
        
        OutputSlot outputArg(std::string const& name)
        {
            return OutputSlot(outputArgs.fullSlot(name));
        }
        
    protected:
        virtual void run() = 0;
        
        ArgBlock inputArgs, outputArgs;
    };
}

#endif
