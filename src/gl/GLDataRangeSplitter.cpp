#include <cstdio>

#include <utility>

#include "GLDataRangeSplitter.h"

using namespace glstreamer;
using namespace glstreamer_gl;

GLDataRangeSplitter::GLDataRangeSplitter ( const GLDataRange* parts, std::size_t nParts ):
parts(parts, parts + nParts)
{
    init();
}

GLDataRangeSplitter::GLDataRangeSplitter ( const std::vector< GLDataRange >& parts ):
parts(parts)
{
    init();
}

GLDataRangeSplitter::GLDataRangeSplitter ( std::vector< GLDataRange >&& parts ):
parts(std::move(parts))
{
    init();
}

void GLDataRangeSplitter::init()
{
    std::size_t nParts = parts.size();
    inputArgs.addSlot<GLDataRange>("inputRange");
    for(std::size_t i = 0; i < nParts; ++i)
    {
        char name[64];
        std::snprintf(name, 64, "outputRange_%zu", i);
        outputArgs._addSlot<GLDataRange>(name);
    }
    outputArgs.refreshSimpleSlots();
}

void GLDataRangeSplitter::run()
{
    std::size_t nParts = parts.size();
    GLDataRange const& input = *static_cast<GLDataRange*>(inputArgs.simpleSlot(0).arg);
    double length = input.end - input.start;
    for(std::size_t i = 0; i < nParts; ++i)
    {
        GLDataRange const& part = parts[i];
        GLDataRange& output = *static_cast<GLDataRange*>(outputArgs.simpleSlot(i).arg);
        output.start = input.start + part.start * length;
        output.end = input.start + part.end * length;
    }
}

std::vector< GLDataRange > GLDataRangeSplitter::makeEvenSplit ( unsigned int nParts )
{
    std::vector<GLDataRange> result;
    result.reserve(nParts);
    for(unsigned i = 0; i < nParts; ++i)
        result.push_back(GLDataRange( {double(i) / nParts, double(i+1) / nParts} ));
    return result;
}
