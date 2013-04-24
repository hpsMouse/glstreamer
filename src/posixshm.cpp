#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <fcntl.h>
#include <unistd.h>

#include "posixshm.h"

#include "posixexception.h"

using namespace glstreamer;

PosixShm::PosixShm() noexcept:
shm(-1),
mapped(nullptr),
_mappedSize(0),
_name(),
owns(false)
{}

PosixShm::PosixShm ( const std::string& name, bool create ):
shm(-1),
mapped(nullptr),
_mappedSize(0),
_name("/" + name),
owns(create)
{
    if(!owns)
        shm = shm_open(_name.c_str(), O_RDWR, 0644);
    else
        shm = shm_open(_name.c_str(), O_RDWR | O_CREAT | O_EXCL, 0644);
    
    if(shm < 0)
    {
        if(errno == EEXIST)
            owns = false;
        else
            throw PosixException("shm_open", errno);
    }
}

PosixShm::~PosixShm() noexcept
{
    destroy();
}

PosixShm::PosixShm ( PosixShm&& shm ) noexcept:
shm(shm.shm),
mapped(shm.mapped),
_mappedSize(shm._mappedSize),
_name(shm._name),
owns(shm.owns)
{
    shm.unsafeClear();
}

PosixShm& PosixShm::operator= ( PosixShm&& shm ) noexcept
{
    if(this == &shm)
        return *this;
    
    this->destroy();
    
    this->shm = shm.shm;
    this->mapped = shm.mapped;
    this->_mappedSize = shm._mappedSize;
    this->_name = shm._name;
    this->owns = shm.owns;
    
    shm.unsafeClear();
    
    return *this;
}

size_type PosixShm::size() const
{
    struct stat fd_stat;
    
    int err = fstat(shm, &fd_stat);
    if(err)
        throw PosixException("fstat", errno);
    
    return fd_stat.st_size;
}

void* PosixShm::resize ( size_type size, bool remap )
{
    unmap();
    
    int err;
    do
    {
        err = ftruncate(shm, size);
    } while(err && errno == EINTR);
    
    if(err)
        throw PosixException("ftruncate", errno);
    
    if(remap)
        return map(size);
    else
        return nullptr;
}

void* PosixShm::map ( size_type size )
{
    if(mapped != nullptr)
        return mapped;
    
    if(size == 0)
        size = this->size();
    
    mapped = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, shm, 0);
    if(mapped == MAP_FAILED)
    {
        mapped = nullptr;
        throw PosixException("mmap", errno);
    }
    
    _mappedSize = size;
    
    return mapped;
}

void PosixShm::unmap() noexcept
{
    if(mapped == nullptr)
        return;
    
    munmap(mapped, _mappedSize);
    mapped = nullptr;
    _mappedSize = 0;
}

void PosixShm::destroy() noexcept
{
    unmap();
    
    if(shm >= 0)
    {
        int err;
        do
        {
            err = close(shm);
        } while(err && errno == EINTR);
        shm = -1;
    }
    
    // TODO: Add error logging here.
    if(owns)
    {
        shm_unlink(_name.c_str());
        owns = false;
    }
}

void PosixShm::unsafeClear() noexcept
{
    shm = -1;
    mapped = nullptr;
    _mappedSize = 0;
    _name.clear();
    owns = false;
}
