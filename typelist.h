#ifndef _1454C614_2A4A_11E2_9F25_206A8A22A96A
#define _1454C614_2A4A_11E2_9F25_206A8A22A96A

#include <utility>

namespace glstreamer
{
    template <typename ... Types>
    struct TypeList
    {};
    
    template <typename T, typename U>
    struct Const
    {
        using type = T;
    };
    
    template <unsigned i, typename ... Types>
    struct TypeListPickRaw;
    
    template <typename Type, typename ... Types>
    struct TypeListPickRaw<0, Type, Types...>
    {
        typedef Type type;
    };
    
    template <unsigned i, typename Type, typename ... Types>
    struct TypeListPickRaw<i, Type, Types...>
    {
        typedef typename TypeListPickRaw<i-1, Types...>::type type;
    };
    
    inline void sequence()
    {}
    
    template <typename Func, typename ... Funcs>
    inline void sequence(Func&& func, Funcs&& ... funcs)
    {
        func();
        sequence(std::forward<Funcs>(funcs)...);
    }
}

#endif
