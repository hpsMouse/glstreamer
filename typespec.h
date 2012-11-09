#ifndef _85E1174E_2674_11E2_82AF_206A8A22A96A
#define _85E1174E_2674_11E2_82AF_206A8A22A96A

#include <cstddef>
#include <cstring>

#include <typeinfo>
#include <utility>

#include "exceptions.h"

namespace glstreamer
{
    class IStream;
    class OStream;
    struct TypeSpec
    {
        virtual ~TypeSpec() = default;
        virtual std::type_info const* id                  ()                             const = 0;
        virtual void*                 create              ()                             const = 0;
        virtual void                  destroy             (void *obj)                    const = 0;
        virtual void                  assign              (void *dst, void const* src)   const = 0;
        virtual void                  clear               (void *obj)                    const = 0;
        virtual void                  context_out         (void *obj)                    const = 0;
        virtual void                  context_in          (void *obj)                    const = 0;
        virtual std::size_t           serialize_size      ()                             const = 0;
        virtual void                  serialize_fixed     (void const* obj, char *data)  const = 0;
        virtual void                  serialize_variable  (void const* obj, OStream &os) const = 0;
        virtual void                  deserialize_fixed   (void *obj, char const* data)  const = 0;
        virtual void                  deserialize_varialbe(void *obj, IStream &is)       const = 0;
    };
    
    template <typename T>
    struct TypeSpecBasic : virtual TypeSpec
    {
        virtual std::type_info const* id() const
        {
            return &typeid(T);
        }
        virtual void* create() const
        {
            return new T();
        }
        virtual void destroy(void* obj) const
        {
            delete ptr(obj);
        }
        virtual void assign(void* dst, const void* src) const
        {
            ref(dst) = ref(src);
        }
        virtual void clear(void* obj) const
        {
            ref(obj) = T();
        }
    private:
        static T*       ptr(void *p)       { return static_cast<T*>(p); }
        static T const* ptr(void const* p) { return static_cast<T const*>(p); }
        static T&       ref(void *p)       { return *ptr(p); }
        static T const& ref(void const* p) { return *ptr(p); }
    };
    
    struct TypeSpecNoContext : virtual TypeSpec
    {
        virtual void context_out(void*) const {}
        virtual void context_in(void*) const{}
    };
    
    struct TypeSpecNoFixedSerialize : virtual TypeSpec
    {
        virtual void serialize_fixed(const void*, char*) const
        {
            throw UnsupportedOperation("serialize_fixed");
        }
        virtual void deserialize_fixed(void*, const char*) const
        {
            throw UnsupportedOperation("deserialize_fixed");
        }
    };
    
    struct TypeSpecNoVariableSerialize : virtual TypeSpec
    {
        virtual void serialize_variable(const void*, OStream&) const
        {
            throw UnsupportedOperation("serialize_variable");
        }
        virtual void deserialize_varialbe(void*, IStream&) const
        {
            throw UnsupportedOperation("deserialize_varialbe");
        }
    };
    
    template <typename T>
    struct TypeSpecPodSerialize : TypeSpecNoVariableSerialize
    {
        virtual std::size_t serialize_size() const
        {
            return sizeof(T);
        }
        virtual void serialize_fixed(const void* obj, char* data) const
        {
            std::memcpy(data, obj, sizeof(T));
        }
        virtual void deserialize_fixed(void* obj, const char* data) const
        {
            std::memcpy(obj, data, sizeof(T));
        }
    };
    
    template <typename T>
    struct DefaultTypeSpec : TypeSpecBasic<T>, TypeSpecNoContext, TypeSpecPodSerialize<T>
    {};
}

#endif
