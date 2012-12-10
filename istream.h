#ifndef _D16BCCEC_42A1_11E2_817F_206A8A22A96A
#define _D16BCCEC_42A1_11E2_817F_206A8A22A96A

#include <type_traits>

#include "types.h"

namespace glstreamer
{
    class IStream
    {
    public:
        IStream() = default;
        virtual ~IStream() = default;
        
        /**
         * \brief Read an object from the stream.
         * 
         * \note See OStream::operator<<() for more information about the POD
         * requirement.
         */
        template <typename T>
        typename std::enable_if<std::is_pod<T>::value, IStream&>::type
        operator >> (T& obj)
        {
            return readData(&obj, sizeof(obj));
        }
        
        virtual IStream& readData(void* data, size_type size) = 0;
        
    private:
        IStream(IStream const&) = delete;
        IStream& operator = (IStream const&) = delete;
    };
}

#endif
