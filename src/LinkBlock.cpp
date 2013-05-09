#include "LinkBlock.h"

#include "link.h"

using namespace glstreamer;

LinkBlock::LinkBlock():
links()
{}

LinkBlock::~LinkBlock()
{}

LinkBlock& LinkBlock::addLink ( Link* link )
{
    links.emplace_back(link);
    return *this;
}
