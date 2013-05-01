#ifndef _743D6BAA_9771_11E2_8970_206A8A22A96A
#define _743D6BAA_9771_11E2_8970_206A8A22A96A

#include <vector>

#include "../defaulttypespec.h"
#include "../istream.h"
#include "../ostream.h"
#include "../typemgr.h"
#include "../types.h"

namespace glstreamer_core
{
    using namespace glstreamer;
    template <typename T>
    struct VectorSpec : public TypeSpecBasic<std::vector<T>>, public TypeSpecNoContext, public TypeSpecNoFixedSerialize
    {
        VectorSpec():
        elementSpec(TypeManager::getTypeSpec<T>())
        {}
        
        VectorSpec(VectorSpec<T> const&) = default;
        VectorSpec& operator = (VectorSpec<T> const&) = default;
        
        virtual void clear ( void* obj ) const
        {
            std::vector<T> *vec = static_cast<std::vector<T>*>(obj);
            vec->clear();
        }
        
        virtual void serialize_variable ( const void* obj, LocalArgBase*, OStream& os ) const override
        {
            std::vector<T> const* vec = static_cast<std::vector<T> const*>(obj);
            os << Word64(vec->size());
            for(T const& elem : *vec)
                elementSpec->serialize_auto(&elem, nullptr, os);
        }
        
        virtual void deserialize_varialbe ( void* obj, LocalArgBase*, IStream& is ) const override
        {
            std::vector<T> *vec = static_cast<std::vector<T>*>(obj);
            Word64 size;
            is >> size;
            vec->resize(size);
            for(Word64 i = 0; i < size; ++i)
                elementSpec->deserialize_auto(vec->data() + i, nullptr, is);
        }
        
    private:
        TypeSpec* elementSpec;
    };
}

#endif
