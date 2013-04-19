#include <cerrno>

#include <sys/types.h>
#include <unistd.h>

#include "prettyio.h"

#include "CancellableThread.h"

#include "exceptions.h"
#include "posixexception.h"

using namespace glstreamer;

void glstreamer::readFd ( int fd, void* buf, size_type size )
{
    char *p = static_cast<char*>(buf);
    
    while(size > 0)
    {
        ssize_t nread = cancel_point(read(fd, p, size), negerrno);
        if(nread > 0)
        {
            size -= nread;
            p += nread;
        }
        else if(nread == 0)
            throw EOFError();
        else
            throw_posix(read);
    }
}

void glstreamer::writeFd( int fd, const void* buf, size_type size )
{
    char const* p = static_cast<char const*>(buf);
    
    while(size > 0)
    {
        ssize_t nwrite = cancel_point(write(fd, p, size), negerrno);
        if(nwrite >= 0)
        {
            size -= nwrite;
            p += nwrite;
        }
        else
            throw_posix(write);
    }
}
