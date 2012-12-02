#include <fcntl.h>
#include <sys/stat.h>

#include "posixsem.h"

#include "posixexception.h"

using namespace glstreamer;

PosixSem::PosixSem():
_name(),
sem(nullptr),
owns(false)
{}

PosixSem::PosixSem ( const std::string& name, bool create, unsigned int value ):
_name("/" + name),
sem(nullptr),
owns(create)
{
    if(!owns)
        sem = sem_open(_name.c_str(), O_RDWR);
    else
        sem = sem_open(_name.c_str(), O_RDWR | O_CREAT | O_EXCL, 0644, value);
    
    if(sem == SEM_FAILED)
    {
        if(errno == EEXIST)
            owns = false;
        else
            throw PosixException("sem_open", errno);
    }
}

PosixSem::~PosixSem() noexcept
{
    destroy();
}

PosixSem::PosixSem ( PosixSem&& sem ) noexcept:
_name(sem._name),
sem(sem.sem),
owns(sem.owns)
{
    sem.unsafeClear();
}

PosixSem& PosixSem::operator= ( PosixSem&& sem ) noexcept
{
    if(this == &sem)
        return *this;
    
    this->destroy();
    
    this->_name = sem._name;
    this->sem = sem.sem;
    this->owns = sem.owns;
    
    sem.unsafeClear();
    
    return *this;
}

void PosixSem::post()
{
    int err = sem_post(sem);
    if(err)
        throw PosixException("sem_post", errno);
}

void PosixSem::wait()
{
    int err;
    do
    {
        err = sem_wait(sem);
    } while (err && errno == EINTR);
}

void PosixSem::destroy() noexcept
{
    // Not testing for SEM_FAILED because in such a fault the PosixSem object
    // cannot be constructed, thus cannot be destroyed.
    if(sem != nullptr)
    {
        sem_close(sem);
        sem = nullptr;
    }
    
    // TODO: Add error logging here (cannot throw an exception in destructor).
    if(owns)
    {
        sem_unlink(_name.c_str());
        owns = false;
    }
}

void PosixSem::unsafeClear() noexcept
{
    _name.clear();
    sem = nullptr;
    owns = false;
}
