#ifndef _85E1174E_2674_11E2_82AF_206A8A22A96A
#define _85E1174E_2674_11E2_82AF_206A8A22A96A

#include <cstddef>

#include <typeindex>

#include "types.h"

namespace glstreamer
{
    struct TypeSpec
    {
        virtual ~TypeSpec() = default;
        
        virtual std::type_index const& id                  ()                             const = 0;
        virtual void*                  create              ()                             const = 0;
        virtual void                   destroy             (void *obj)                    const = 0;
        virtual void                   assign              (void *dst, void const* src)   const = 0;
        virtual void                   clear               (void *obj)                    const = 0;
        virtual void                   context_out         (void *obj)                    const = 0;
        virtual void                   context_in          (void *obj)                    const = 0;
        virtual size_type              serialize_size      ()                             const = 0;
        virtual void                   serialize_fixed     (void const* obj, char *data)  const = 0;
        virtual void                   serialize_variable  (void const* obj, OStream &os) const = 0;
        virtual void                   deserialize_fixed   (void *obj, char const* data)  const = 0;
        virtual void                   deserialize_varialbe(void *obj, IStream &is)       const = 0;
    };
    
    struct Destroyer
    {
        Destroyer(): typeSpec(nullptr) {}
        
        Destroyer(TypeSpec *typeSpec): typeSpec(typeSpec) {}
        
        void operator() (void* obj){ typeSpec->destroy(obj); }
        
    private:
        TypeSpec *typeSpec;
    };
}

#endif
