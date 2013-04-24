#include <fcntl.h>
#include <sys/stat.h>

#include "posixsem.h"

#include "posixexception.h"

using namespace glstreamer;

PosixSem::PosixSem() noexcept:
sem(nullptr),
_name(),
owns(false)
{}

PosixSem::PosixSem ( const std::string& name, bool create, unsigned int value ):
sem(nullptr),
_name("/" + name),
owns(create)
{
    if(!owns)
        sem = sem_open(_name.c_str(), O_RDWR);
    else
        sem = sem_open(_name.c_str(), O_RDWR | O_CREAT | O_EXCL, 0644, value);
    
    if(sem == SEM_FAILED)
    {
        sem = nullptr;
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
sem(sem.sem),
_name(sem._name),
owns(sem.owns)
{
    sem.unsafeClear();
}

PosixSem& PosixSem::operator= ( PosixSem&& sem ) noexcept
{
    if(this == &sem)
        return *this;
    
    this->destroy();
    
    this->sem = sem.sem;
    this->_name = sem._name;
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
    sem = nullptr;
    _name.clear();
    owns = false;
}
