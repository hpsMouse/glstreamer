#ifndef _89B765EE_484D_11E2_B82F_206A8A22A96A
#define _89B765EE_484D_11E2_B82F_206A8A22A96A

namespace glstreamer
{
    class PosixFd
    {
    public:
        explicit PosixFd(int fd = -1);
        ~PosixFd();
        
        PosixFd(PosixFd&& fd) noexcept;
        PosixFd& operator = (PosixFd&& fd);
        
        int fd() const
        { return _fd; }
        
        int release() noexcept
        {
            int result = _fd;
            unsafeClear();
            return result;
        }
        
    private:
        PosixFd(PosixFd const&) = delete;
        PosixFd& operator = (PosixFd const&) = delete;
        
        void clear();
        void unsafeClear() noexcept;
        
        int _fd;
    };
}

#endif
