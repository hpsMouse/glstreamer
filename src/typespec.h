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
        
        virtual std::type_index const& id                  ()                                                     const = 0;
        virtual void*                  create              ()                                                     const = 0;
        virtual LocalArgBase*          createLocal         ()                                                     const = 0;
        virtual void                   destroy             (void *obj)                                            const = 0;
        virtual void                   assign              (void *dst, void const* src)                           const = 0;
        virtual void                   clear               (void *obj)                                            const = 0;
        virtual void                   context_out         (void *obj, LocalArgBase *localArg)                    const = 0;
        virtual void                   context_in          (void *obj, LocalArgBase *localArg)                    const = 0;
        virtual size_type              serialize_size      ()                                                     const = 0;
        virtual void                   serialize_fixed     (void const* obj, LocalArgBase *localArg, char *data)  const = 0;
        virtual void                   serialize_variable  (void const* obj, LocalArgBase *localArg, OStream &os) const = 0;
        virtual void                   deserialize_fixed   (void *obj, LocalArgBase *localArg, char const* data)  const = 0;
        virtual void                   deserialize_varialbe(void *obj, LocalArgBase *localArg, IStream &is)       const = 0;
        
        void serialize_auto(void const* obj, LocalArgBase *localArg, OStream &os);
        void deserialize_auto(void *obj, LocalArgBase *localArg, IStream &is);
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
