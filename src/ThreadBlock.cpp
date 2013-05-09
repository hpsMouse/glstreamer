#include "ThreadBlock.h"

#include "exceptions.h"
#include "processor.h"

using namespace glstreamer;

ThreadBlock::ThreadBlock():
processors(),
index()
{}

ThreadBlock::~ThreadBlock()
{}

ThreadBlock& ThreadBlock::addProcessor ( const std::string& name, Processor* processor )
{
    std::unique_ptr<Processor> ptr(processor);
    if(!index.insert(std::make_pair(name, processor)).second)
        throw DefinitionConflict("Block-scope processor name: " + name);
    processors.push_back(std::move(ptr));
    return *this;
}

void ThreadBlock::execute()
{
    for(std::unique_ptr<Processor>& processor: processors)
        processor->execute();
}

Processor& ThreadBlock::processor ( const std::string& name )
{
    auto const& iter = index.find(name);
    if(iter == index.end())
        throw NotFound(name);
    return *iter->second;
}
