#ifndef _ADF5165C_274D_11E2_B505_206A8A22A96A
#define _ADF5165C_274D_11E2_B505_206A8A22A96A

#include <map>
#include <type_traits>
#include <typeindex>

#include "types.h"

namespace glstreamer
{
    class TypeManager
    {
    public:
        enum ConflictAction { Error, Ignore, Replace };
        
        template <typename T>
        static TypeSpec* getTypeSpec()
        {
            return getTypeSpec(typeid(typename std::remove_cv<typename std::remove_reference<T>::type>::type));
        }
        
        template <typename T>
        static void registerTypeSpec(TypeSpec* spec, ConflictAction conflict = Error)
        {
            registerTypeSpec(typeid(typename std::remove_cv<typename std::remove_reference<T>::type>::type), spec, conflict);
        }
        
        template <typename T, typename Spec>
        static void registerType(ConflictAction conflict = Error)
        {
            static Spec spec;
            registerTypeSpec<T>(&spec, conflict);
        }
        
    private:
        static TypeSpec* getTypeSpec(std::type_index const& id);
        static void registerTypeSpec(std::type_index const& id, TypeSpec* spec, ConflictAction conflict);
        
        static std::map<std::type_index, TypeSpec*> typeSpecs;
    };
}

#endif
