#include "posixunnamedsem.h"

#include "CancellableThread.h"

#include "posixexception.h"

using namespace glstreamer;

PosixUnnamedSem::PosixUnnamedSem ( unsigned int initValue ):
sem()
{
    int err = sem_init(&sem, 0, initValue);
    if(err)
        throw_posix(sem_init);
}

PosixUnnamedSem::~PosixUnnamedSem() noexcept
{
    sem_destroy(&sem);
}

void PosixUnnamedSem::post()
{
    int err = sem_post(&sem);
    if(err)
        throw_posix(sem_post);
}

void PosixUnnamedSem::wait()
{
    int err = cancel_point(sem_wait(&sem), nonzeroerrno);
    if(err)
        throw_posix(sem_wait);
}
