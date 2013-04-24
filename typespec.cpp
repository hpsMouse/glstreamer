#include "typespec.h"

#include "istream.h"
#include "ostream.h"

using namespace glstreamer;

void TypeSpec::serialize_auto ( const void* obj, LocalArgBase* localArg, OStream& os )
{
    auto fixedSize = this->serialize_size();
    if(fixedSize != 0) // Fixed
    {
        void* internalBuffer = os.requireInternalBuffer(fixedSize);
        this->serialize_fixed(obj, localArg, static_cast<char*>(internalBuffer));
        os.pushInternalBuffer(internalBuffer, fixedSize);
    }
    else // Variable
        this->serialize_variable(obj, localArg, os);
}

void TypeSpec::deserialize_auto ( void* obj, LocalArgBase* localArg, IStream& is )
{
    auto fixedSize = this->serialize_size();
    if(fixedSize != 0) // Fixed
    {
        void const* internalBuffer = is.requireInternalBuffer(fixedSize);
        this->deserialize_fixed(obj, localArg, static_cast<char const*>(internalBuffer));
        is.releaseInternalBuffer(internalBuffer, fixedSize);
    }
    else // Variable
        this->deserialize_varialbe(obj, localArg, is);
}
