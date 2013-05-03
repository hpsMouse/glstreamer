#ifndef _6D82001C_44E9_11E2_8050_206A8A22A96A
#define _6D82001C_44E9_11E2_8050_206A8A22A96A

#include <arpa/inet.h>

#include "types.h"

namespace glstreamer
{
    class NetUInt32
    {
    public:
        using value_type = Word32;
        using Self = NetUInt32;
        
        NetUInt32(value_type value = 0):
        value(htonl(value))
        {}
        
        operator value_type () const
        {
            return ntohl(value);
        }
        
        value_type* rawMem()
        { return &value; }
        
        value_type const* rawMem() const
        { return &value; }
        
        value_type& rawValue()
        { return value; }
        
        value_type rawValue() const
        { return value; }
        
    private:
        value_type value;
    };
    
    static_assert(sizeof(NetUInt32) == sizeof(NetUInt32::value_type), "");
}

#endif
