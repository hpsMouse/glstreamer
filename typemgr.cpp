#include "typemgr.h"

#include "exceptions.h"

using namespace std;
using namespace glstreamer;

map<type_index, TypeSpec*> TypeManager::typeSpecs;

TypeSpec* TypeManager::getTypeSpec(const type_index& id)
{
    auto iter = typeSpecs.find(id);
    if(iter == typeSpecs.end())
        throw NotFound(string("TypeSpec for ") + id.name());
    return iter->second;
}

void TypeManager::registerTypeSpec(const type_index& id, TypeSpec* spec, TypeManager::ConflictAction conflict)
{
    auto currentSpecIter = typeSpecs.find(id);
    if(currentSpecIter != typeSpecs.end())
    {
        switch(conflict)
        {
            case Error:
                throw DefinitionConflict(string("TypeSpec for ") + id.name());
                break;
            case Ignore:
                break;
            case Replace:
                currentSpecIter->second = spec;
                break;
        }
    }
    else
        typeSpecs.insert(make_pair(id, spec));
}
