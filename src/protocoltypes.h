#ifndef _88286688_4785_11E2_B703_206A8A22A96A
#define _88286688_4785_11E2_B703_206A8A22A96A

#include <type_traits>

#include <boost/crc.hpp>

#include "nettypes.h"

namespace glstreamer
{
    inline Word32
    crc32(void const* obj, std::size_t size)
    {
        boost::crc_32_type crc;
        crc.process_bytes(obj, size);
        return crc.checksum();
    }
    
    template <typename T>
    inline
    typename std::enable_if<!std::is_pointer<T>::value && (sizeof(T) > 0), Word32>::type
    crc32(T const& obj)
    {
        return crc32(static_cast<void const*>(&obj), sizeof(T) - sizeof(Word32));
    }
    
    struct LinkHeader
    {
        constexpr static Word32 magicValue = 0x94195aefu;
        NetUInt32 magic;
        NetUInt32 linkId;
        NetUInt32 typeNameLen;
        NetUInt32 fixedLen;
        NetUInt32 crc;
        
        LinkHeader():
        magic(),
        linkId(),
        typeNameLen(),
        fixedLen(),
        crc()
        {}
        
        LinkHeader(Word32 linkId, Word32 typeNameLen, Word32 fixedLen):
        magic(magicValue),
        linkId(linkId),
        typeNameLen(typeNameLen),
        fixedLen(fixedLen),
        crc(crc32(*this))
        {}
    };
    
    struct DataHeader
    {
        constexpr static Word32 magicValue = 0xe1037937u;
        NetUInt32 magic;
        NetUInt32 varLen;
        NetUInt32 crc;
        
        DataHeader():
        magic(),
        varLen(),
        crc()
        {}
        
        DataHeader(Word32 varLen):
        magic(magicValue),
        varLen(varLen),
        crc(crc32(*this))
        {}
        
        void updateVarLen(Word32 newLen)
        {
            varLen = newLen;
            crc = crc32(*this);
        }
    };
    
    struct ErrorCode
    {
        constexpr static Word32 errNone = 0x00;   // No error
        constexpr static Word32 errMagic = 0x01;  // Magic number error
        constexpr static Word32 errCrc = 0x02;    // CRC error
        constexpr static Word32 errType = 0x03;   // Type mismatch
        constexpr static Word32 errPacket = 0x04; // Packet truncated
        constexpr static Word32 errSize = 0x05;   // Size specification conflict
        constexpr static Word32 errExist = 0x06;  // Link already exist
    };
}

#endif
