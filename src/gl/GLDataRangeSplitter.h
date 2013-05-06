#ifndef _FC3453CE_B5F9_11E2_B1B0_206A8A22A96A
#define _FC3453CE_B5F9_11E2_B1B0_206A8A22A96A

#include <vector>

#include "../processor.h"

#include "parameter_types.h"

namespace glstreamer_gl
{
    class GLDataRangeSplitter : public glstreamer::Processor
    {
    public:
        GLDataRangeSplitter(GLDataRange const* parts, std::size_t nParts);
        GLDataRangeSplitter(std::vector<GLDataRange> const& parts);
        GLDataRangeSplitter(std::vector<GLDataRange>&& parts);
        
        static std::vector<GLDataRange> makeEvenSplit(unsigned nParts);
        
    private:
        std::vector<GLDataRange> parts;
        
        virtual void run();
        
        void init();
    };
}

#endif
