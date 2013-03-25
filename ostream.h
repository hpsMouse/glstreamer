#ifndef _3D73D4B2_428D_11E2_9A41_206A8A22A96A
#define _3D73D4B2_428D_11E2_9A41_206A8A22A96A

#include <type_traits>
#include <vector>

#include "types.h"

namespace glstreamer
{
    class OStream
    {
    public:
        OStream():
        internalBuffer()
        {}
        
        virtual ~OStream() = default;
        
        /**
         * \brief Write an object into the stream.
         * 
         * \note A trivially copyable object should be enough, and POD is a
         * slightly stronger requirement. The POD requirement is used because
         * g++ 4.7 hasn't implement \c std::is_trivially_copyalbe yet. In the
         * future this requirement will be losen.
         */
        template <typename T>
        typename std::enable_if<std::is_pod<T>::value, OStream&>::type
        operator << (T const& obj)
        {
            return writeData(&obj, sizeof(obj));
        }
        
        virtual OStream& writeData(void const* data, size_type size) = 0;
        
        virtual void* requireInternalBuffer(size_type size);
        virtual OStream& pushInternalBuffer(void *buffer, size_type size);
        
    private:
        OStream(OStream const&) = delete;
        OStream& operator = (OStream const&) = delete;
        
        std::vector<char> internalBuffer;
    };
}

#endif
