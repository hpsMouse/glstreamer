#include <unistd.h>

#include "posixfd.h"

#include "posixexception.h"

using namespace glstreamer;

PosixFd::PosixFd ( int fd ):
_fd(fd)
{}

PosixFd::~PosixFd()
{
    clear();
}

PosixFd::PosixFd ( PosixFd&& fd ) noexcept:
_fd(fd._fd)
{
    fd.unsafeClear();
}

PosixFd& PosixFd::operator= ( PosixFd&& fd )
{
    if(&fd != this)
    {
        clear();
        this->_fd = fd._fd;
        fd.unsafeClear();
    }
    return *this;
}

void PosixFd::clear()
{
    if(_fd < 0)
        return;
    
    int err;
    do
    {
        err = close(_fd);
    } while(err && errno == EINTR);
    if(err)
        throw_posix(close);
}

void PosixFd::unsafeClear() noexcept
{
    _fd = -1;
}
