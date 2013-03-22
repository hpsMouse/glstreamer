#include <cstdio>
#include <cstring>

#include <sys/socket.h>

#include "fdpump.h"

#include "exceptions.h"
#include "linkbuffer.h"
#include "prettyio.h"
#include "protocoltypes.h"
#include "typespec.h"

using namespace glstreamer;

FdPump::FdPump():
linkBuffer(),
fd(-1),
typeName(nullptr),
fixedLen(0),
linkHeader(),
remoteTypeName(),
stateLock(),
condResourceReady(),
runner([this]{this->run();})
{
}

FdPump::~FdPump()
{
    if(fd >= 0)
        shutdown(fd, SHUT_RDWR);
    runner.join();
    close(fd);
    fd = -1;
}

void FdPump::assignLink ( int fd, const LinkHeader& header )
{
    if(fd < 0)
        throw std::invalid_argument(wrap("Negative fd."));
    
    std::unique_lock<std::mutex> stateLocker(stateLock);
    
    if(this->fd >= 0)
        throw ResourceConflict(wrap("Duplicate assignLink() call."));
    
    this->fd = fd;
    this->linkHeader = header;
    
    this->tryEmitReady();
}

LinkBuffer* FdPump::allocateBuffer ( size_type bufferCount, TypeSpec* typeSpec )
{
    if(bufferCount == 0)
        throw std::invalid_argument(wrap("Zero-sized bufferCount."));
    
    std::unique_lock<std::mutex> stateLocker(stateLock);
    
    if(this->linkBuffer.get() != nullptr)
        throw ResourceConflict(wrap("Duplicate allocateBuffer() call."));
    
    this->linkBuffer.reset(new LinkBuffer(bufferCount));
    this->typeName = typeSpec->id().name();
    this->fixedLen = typeSpec->serialize_size();
    
    this->tryEmitReady();
    
    return this->linkBuffer.get();
}

void FdPump::run ()
{
    std::unique_lock<std::mutex> stateLocker(stateLock);
    
    while(!resourceReady())
        condResourceReady.wait(stateLocker);
    
    try
    {
        if(linkHeader.fixedLen != fixedLen)
        {
            writeError(ErrorCode::errType);
            return;
        }
        
        writeError(ErrorCode::errNone);
        
        remoteTypeName.resize(linkHeader.typeNameLen);
        readFd(fd, remoteTypeName.data(), linkHeader.typeNameLen);
        remoteTypeName.push_back('\0');
        
        if(std::strcmp(remoteTypeName.data(), typeName) != 0)
        {
            writeError(ErrorCode::errType);
            return;
        }
        
        writeError(ErrorCode::errNone);
        
        while(true)
        {
            std::vector<char>& buffer = linkBuffer->getEmpty();
            DataHeader dataHeader;
            readFd(fd, dataHeader);
            
            if(dataHeader.magic != dataHeader.magicValue)
            {
                writeError(ErrorCode::errMagic);
                break;
            }
            if(dataHeader.crc != crc32(dataHeader))
            {
                writeError(ErrorCode::errCrc);
                break;
            }
            if(fixedLen != 0 && dataHeader.varLen != 0)
            {
                writeError(ErrorCode::errSize);
                break;
            }
            writeError(ErrorCode::errNone);
            
            size_type dataLen = fixedLen != 0 ? fixedLen : size_type(dataHeader.varLen);
            buffer.resize(dataLen);
            readFd(fd, buffer.data(), dataLen);
            linkBuffer->putFull();
            writeError(ErrorCode::errNone);
        }
    }
    catch(EOFError const&)
    {
        try {
            writeError(ErrorCode::errPacket);
        } catch(...)
        {}
    }
    catch(std::exception const& e)
    {
        std::fprintf(stderr, "%s\n", e.what());
    }
}

bool FdPump::resourceReady()
{
    return fd >= 0 && linkBuffer.get() != nullptr;
}

void FdPump::tryEmitReady()
{
    if(resourceReady())
        this->condResourceReady.notify_one();
}

void FdPump::writeError ( Word32 errCode )
{
    writeFd(fd, NetUInt32(errCode));
}
