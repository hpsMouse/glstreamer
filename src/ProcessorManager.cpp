#include "ProcessorManager.h"

#include "exceptions.h"

using namespace glstreamer;

std::map<std::string, ProcessorManager::ProcessorMaker> ProcessorManager::makers;

void ProcessorManager::registerProcessor ( const std::string& name, ProcessorManager::ProcessorMaker maker )
{
    if(!makers.insert(std::make_pair(name, maker)).second)
        throw DefinitionConflict("Processor " + name);
}

ProcessorManager::ProcessorMaker ProcessorManager::findProcessor ( const std::string& name )
{
    auto const& iter = makers.find(name);
    if(iter == makers.end())
        throw NotFound("Processor " + name);
    return iter->second;
}

std::unique_ptr< Processor > ProcessorManager::makeProcessor ( const std::string& name, const std::map< std::string, boost::any >& args )
{
    return findProcessor(name)(args);
}
