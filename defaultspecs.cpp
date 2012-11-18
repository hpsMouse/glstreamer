#include "defaultspecs.h"

#include "defaulttypespec.h"
#include "typemgr.h"

template <typename T>
inline static void registerDefaultTypeSpec()
{
    using namespace glstreamer;
    static DefaultTypeSpec<T> spec;
    TypeManager::registerTypeSpec<T>(&spec);
}

static void registerBasicTypes()
{
    registerDefaultTypeSpec<char>();
    
    registerDefaultTypeSpec<signed char>();
    registerDefaultTypeSpec<short int>();
    registerDefaultTypeSpec<int>();
    registerDefaultTypeSpec<long int>();
    registerDefaultTypeSpec<long long int>();
    
    registerDefaultTypeSpec<unsigned char>();
    registerDefaultTypeSpec<unsigned short int>();
    registerDefaultTypeSpec<unsigned int>();
    registerDefaultTypeSpec<unsigned long int>();
    registerDefaultTypeSpec<unsigned long long int>();
    
    registerDefaultTypeSpec<wchar_t>();
    registerDefaultTypeSpec<char16_t>();
    registerDefaultTypeSpec<char32_t>();
    
    registerDefaultTypeSpec<bool>();
    
    registerDefaultTypeSpec<float>();
    registerDefaultTypeSpec<double>();
    registerDefaultTypeSpec<long double>();
}

void glstreamer::registerDefaultTypes()
{
    registerBasicTypes();
}
