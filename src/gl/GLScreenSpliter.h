#ifndef _DD103008_B4AF_11E2_BA46_206A8A22A96A
#define _DD103008_B4AF_11E2_BA46_206A8A22A96A

#include <vector>

#include "../processor.h"

#include "parameter_types.h"

namespace glstreamer_gl
{
    /**
     * \brief Split a viewport into multiple parts
     * 
     * Input and outputs: Viewport(width, height), Frustum_or_Ortho(left, right, bottom, top, near, far)
     */
    class GLScreenSplitter : public glstreamer::Processor
    {
    public:
        GLScreenSplitter(ScreenPart const* parts, std::size_t nParts);
        GLScreenSplitter(std::vector<ScreenPart> const& parts);
        GLScreenSplitter(std::vector<ScreenPart>&& parts);
        
        static GLViewport split(GLViewport const& input, ScreenPart const& part);
        
        static std::vector<ScreenPart> makeGrid(unsigned xsplit, unsigned ysplit);
        
    private:
        std::vector<ScreenPart> parts;
        
        virtual void run();
        
        void init();
    };
}

#endif
