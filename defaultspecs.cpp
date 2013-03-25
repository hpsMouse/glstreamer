#include <memory>
#include <string>
#include <vector>

#include "defaultspecs.h"

#include "defaulttypespec.h"
#include "istream.h"
#include "ostream.h"
#include "typemgr.h"

using namespace glstreamer;

namespace
{
    struct StringSpec : public TypeSpecBasic<std::string>, public TypeSpecNoContext, public TypeSpecNoFixedSerialize
    {
        virtual void serialize_variable ( const void* obj, OStream& os ) const override
        {
            std::string const* str = static_cast<std::string const*>(obj);
            os << Word64(str->size());
            os.writeData(str->data(), str->size());
        }
        
        virtual void deserialize_varialbe ( void* obj, IStream& is ) const override
        {
            std::string* str = static_cast<std::string*>(obj);
            Word64 size;
            is >> size;
            std::unique_ptr<char[]> buffer(new char[size]);
            is.readData(buffer.get(), size);
            str->assign(buffer.get(), buffer.get() + size);
        }
    };
    
    template <typename T>
    struct VectorSpec : public TypeSpecBasic<std::vector<T>>, public TypeSpecNoContext, public TypeSpecNoFixedSerialize
    {
        VectorSpec():
        elementSpec(TypeManager::getTypeSpec<T>())
        {}
        
        VectorSpec(VectorSpec<T> const&) = default;
        VectorSpec& operator = (VectorSpec<T> const&) = default;
        
        virtual void serialize_variable ( const void* obj, OStream& os ) const override
        {
            std::vector<T> const* vec = static_cast<std::vector<T> const*>(obj);
            os << Word64(vec->size());
            for(T const& elem : *vec)
                elementSpec->serialize_auto(&elem, os);
        }
        
        virtual void deserialize_varialbe ( void* obj, IStream& is ) const override
        {
            std::vector<T> *vec = static_cast<std::vector<T>*>(obj);
            Word64 size;
            is >> size;
            vec->resize(size);
            for(Word64 i = 0; i < size; ++i)
                elementSpec->deserialize_auto(&(*vec)[i], is);
        }
        
    private:
        TypeSpec* elementSpec;
    };
}

template <typename T, typename Spec>
static void registerType()
{
    static Spec spec;
    TypeManager::registerTypeSpec<T>(&spec);
}

template <typename T>
static void registerDefaultTypeSpec()
{
    static DefaultTypeSpec<T> spec;
    TypeManager::registerTypeSpec<T>(&spec);
}

template <typename T>
static void registerVectorTypeSpec()
{
    static VectorSpec<T> spec;
    TypeManager::registerTypeSpec<std::vector<T>>(&spec);
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
    registerType<std::string, StringSpec>();
    registerVectorTypeSpec<std::string>();
}
