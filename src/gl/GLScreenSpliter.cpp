#include <sstream>

#include "GLScreenSpliter.h"

using namespace glstreamer_gl;

GLScreenSplitter::GLScreenSplitter ( const ScreenPart* parts, std::size_t nParts ):
parts(parts, parts + nParts)
{
    init();
}

GLScreenSplitter::GLScreenSplitter ( const std::vector< ScreenPart >& parts ):
parts(parts)
{
    init();
}

GLScreenSplitter::GLScreenSplitter ( std::vector< ScreenPart >&& parts ):
parts(std::move(parts))
{
    init();
}

GLViewport GLScreenSplitter::split ( const GLViewport& input, const ScreenPart& part )
{
    GLViewport output;
    
    unsigned leftP   = input.width * part.left;
    unsigned rightP  = input.width * part.right;
    
    unsigned bottomP = input.height * part.bottom;
    unsigned topP    = input.height * part.top;
    
    output.width = rightP - leftP;
    output.height = topP - bottomP;
    
    double
    leftN   = double  (leftP) / input.width,
    rightN  = double (rightP) / input.width,
    bottomN = double(bottomP) / input.height,
    topN    = double   (topP) / input.height;
    
    double widthD = input.right - input.left;
    double heightD = input.top - input.bottom;
    
    output.left = input.left + widthD * leftN;
    output.right = input.left + widthD * rightN;
    output.bottom = input.bottom + heightD * bottomN;
    output.top = input.bottom + heightD * topN;
    output.near = input.near;
    output.far = input.far;
    
    return output;
}

std::vector< ScreenPart > GLScreenSplitter::makeGrid ( unsigned int xsplit, unsigned int ysplit )
{
    std::vector<ScreenPart> result;
    result.reserve(xsplit * ysplit);
    
    for(unsigned y = 0; y < ysplit; ++y)
    {
        for(unsigned x = 0; x < xsplit; ++x)
            result.push_back(ScreenPart({double(x) / xsplit, double(x+1) / xsplit, double(y) / ysplit, double(y+1) / ysplit}));
    }
    
    return result;
}

void GLScreenSplitter::run()
{
    GLViewport const& input = *static_cast<GLViewport*>(inputArgs.simpleSlot(0).arg);
    auto nParts = parts.size();
    for(decltype(nParts) i = 0; i < nParts; ++i)
    {
        ScreenPart const& part = parts[i];
        GLViewport& output = *static_cast<GLViewport*>(outputArgs.simpleSlot(i).arg);
        output = split(input, part);
    }
}

void GLScreenSplitter::init()
{
    auto nParts = parts.size();
    inputArgs.addSlot<GLViewport>("inputViewport");
    for(std::size_t i = 0; i < nParts; ++i)
    {
        std::ostringstream name;
        name << "outputViewport_" << i;
        outputArgs._addSlot<GLViewport>(name.str());
    }
    outputArgs.refreshSimpleSlots();
}
