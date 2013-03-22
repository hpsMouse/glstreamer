#include <cstdio>
#include <cstring>

#include <sstream>
#include <string>

#include <unistd.h>

#include "fdfiller.h"

#include "cancel.h"
#include "exceptions.h"
#include "linkbuffer.h"
#include "posixexception.h"
#include "prettyio.h"
#include "protocoltypes.h"
#include "typespec.h"

#include "CancellationEnabler.h"

using namespace glstreamer;

FdFiller::FdFiller ( const sockaddr* addr, socklen_t addrLen, Word32 linkId, LinkBuffer* linkBuffer, TypeSpec* typeSpec ):
linkBuffer(linkBuffer),
fd(-1),
remote_addr(new char[addrLen]),
runner()
{
    fd = socket(addr->sa_family, SOCK_STREAM, 0);
    if(fd < 0)
        throw_posix(socket);
    
    std::memcpy(remote_addr.get(), addr, addrLen);
    
    runner = std::thread([=]{this->run(reinterpret_cast<sockaddr*>(remote_addr.get()), addrLen, linkId, typeSpec);});
}

FdFiller::~FdFiller()
{
    linkBuffer->getEmpty().clear();
    linkBuffer->putFull();
    runner.join();
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

void FdFiller::run ( sockaddr* addr, socklen_t addrLen, Word32 linkId, TypeSpec* typeSpec )
{
    try
    {
        posixpp::CancellationEnabler cancellation_on;
        int err = cancel_point(connect(fd, addr, addrLen), posixpp::nonzeroerrno);
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
    catch(posixpp::CancelException const&)
    {}
    catch(std::exception const& e)
    {
        std::fprintf(stderr, "%s\n", e.what());
    }
}
