#ifndef _2D2EC7DE_2E19_11E2_A1A9_206A8A22A96A
#define _2D2EC7DE_2E19_11E2_A1A9_206A8A22A96A

#include <cstddef>
#include <cstring>

#include "typespec.h"

#include "exceptions.h"

namespace glstreamer
{
    template <typename T>
    struct TypeSpecBasic : virtual TypeSpec
    {
        TypeSpecBasic(): _id(typeid(T)) {}
        virtual std::type_index const& id() const override
        {
            return _id;
        }
        virtual void* create() const override
        {
            return new T();
        }
        virtual void destroy(void* obj) const override
        {
            delete ptr(obj);
        }
        virtual void assign(void* dst, const void* src) const override
        {
            ref(dst) = ref(src);
        }
        virtual void clear(void* obj) const override
        {
            ref(obj) = T();
        }
    private:
        std::type_index const _id;
        
        static T*       ptr(void *p)       { return static_cast<T*>(p); }
        static T const* ptr(void const* p) { return static_cast<T const*>(p); }
        static T&       ref(void *p)       { return *ptr(p); }
        static T const& ref(void const* p) { return *ptr(p); }
    };
    
    struct TypeSpecNoContext : virtual TypeSpec
    {
        virtual void context_out(void*) const override {}
        virtual void context_in(void*) const override {}
    };
    
    struct TypeSpecNoFixedSerialize : virtual TypeSpec
    {
        virtual size_type serialize_size() const override
        {
            return 0;
        }
        virtual void serialize_fixed(const void*, char*) const override
        {
            throw UnsupportedOperation("serialize_fixed");
        }
        virtual void deserialize_fixed(void*, const char*) const override
        {
            throw UnsupportedOperation("deserialize_fixed");
        }
    };
    
    struct TypeSpecNoVariableSerialize : virtual TypeSpec
    {
        virtual void serialize_variable(const void*, OStream&) const override
        {
            throw UnsupportedOperation("serialize_variable");
        }
        virtual void deserialize_varialbe(void*, IStream&) const override
        {
            throw UnsupportedOperation("deserialize_varialbe");
        }
    };
    
    template <typename T>
    struct TypeSpecPodSerialize : TypeSpecNoVariableSerialize
    {
        virtual size_type serialize_size() const override
        {
            return sizeof(T);
        }
        virtual void serialize_fixed(const void* obj, char* data) const override
        {
            std::memcpy(data, obj, sizeof(T));
        }
        virtual void deserialize_fixed(void* obj, const char* data) const override
        {
            std::memcpy(obj, data, sizeof(T));
        }
    };
    
    template <typename T>
    struct DefaultTypeSpec : TypeSpecBasic<T>, TypeSpecNoContext, TypeSpecPodSerialize<T>
    {};
}

#endif
