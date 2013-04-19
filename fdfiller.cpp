#include <cstdio>
#include <cstring>

#include <sstream>
#include <string>

#include <unistd.h>

#include "fdfiller.h"

#include "exceptions.h"
#include "linkbuffer.h"
#include "posixexception.h"
#include "prettyio.h"
#include "protocoltypes.h"
#include "typespec.h"

using namespace glstreamer;

FdFiller::FdFiller ( const sockaddr* addr, socklen_t addrLen, Word32 linkId, LinkBuffer* linkBuffer, TypeSpec* typeSpec ):
linkBuffer(linkBuffer),
fd(-1),
remote_addr(new char[addrLen]),
parameterPack()
{
    fd = socket(addr->sa_family, SOCK_STREAM, 0);
    if(fd < 0)
        throw_posix(socket);
    
    std::memcpy(remote_addr.get(), addr, addrLen);
    
    this->parameterPack.addrLen = addrLen;
    this->parameterPack.linkId = linkId;
    this->parameterPack.typeSpec = typeSpec;
    
    this->start();
}

FdFiller::~FdFiller()
{
    linkBuffer->getEmpty().clear();
    linkBuffer->putFull();
    this->waitForFinish();
    close(fd);
    fd = -1;
}

template <typename T>
static std::string toString(T const& obj)
{
    std::ostringstream conv;
    conv << obj;
    return conv.str();
}

void FdFiller::run ()
{
    sockaddr *addr = reinterpret_cast<sockaddr*>(this->remote_addr.get());
    socklen_t addrLen = this->parameterPack.addrLen;
    Word32 linkId = this->parameterPack.linkId;
    TypeSpec *typeSpec = this->parameterPack.typeSpec;
    try
    {
        CancellationEnabler cancellation_on;
        int err = cancel_point(connect(fd, addr, addrLen), ::glstreamer::nonzeroerrno);
        if(err)
            throw_posix(connect);
        
        char const* typeName = typeSpec->id().name();
        auto const typeNameLen = std::strlen(typeName);
        auto const fixedLen = typeSpec->serialize_size();
        LinkHeader linkHeader(linkId, typeNameLen, fixedLen);
        NetUInt32 response;
        
        writeFd(fd, linkHeader);
        
        readFd(fd, response);
        if(response != ErrorCode::errNone)
            throw ProtocolError(toString(static_cast<Word32>(response)));
        
        writeFd(fd, typeName, typeNameLen);
        
        readFd(fd, response);
        if(response != ErrorCode::errNone)
            throw ProtocolError(toString(static_cast<Word32>(response)));
        
        while(true)
        {
            auto const& buffer = linkBuffer->getFull();
            if(buffer.empty())
                break;
            if(fixedLen == 0) // Variable size
            {
                DataHeader dataHeader(buffer.size());
                writeFd(fd, dataHeader);
                
                readFd(fd, response);
                if(response != ErrorCode::errNone)
                    throw ProtocolError(toString(static_cast<Word32>(response)));
                
                writeFd(fd, buffer.data(), buffer.size());
            }
            else // Fixed size
            {
                if(buffer.size() != fixedLen)
                    throw InternalError("Buffer size doesn't match declared serialize_size.");
                
                DataHeader dataHeader(0);
                writeFd(fd, dataHeader);
                
                readFd(fd, response);
                if(response != ErrorCode::errNone)
                    throw ProtocolError(toString(static_cast<Word32>(response)));
                
                writeFd(fd, buffer.data(), fixedLen);
            }
            
            readFd(fd, response);
            if(response != ErrorCode::errNone)
                throw ProtocolError(toString(static_cast<Word32>(response)));
            
            linkBuffer->putEmpty();
        }
    }
    catch(CancelException const&)
    {}
    catch(std::exception const& e)
    {
        std::fprintf(stderr, "%s\n", e.what());
    }
}
