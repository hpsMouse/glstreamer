#ifndef _CCDDDC34_45ED_11E2_959D_206A8A22A96A
#define _CCDDDC34_45ED_11E2_959D_206A8A22A96A

#include <semaphore.h>

namespace glstreamer
{
    class PosixUnnamedSem
    {
    public:
        PosixUnnamedSem(unsigned initValue = 0);
        ~PosixUnnamedSem() noexcept;
        
        void post();
        void wait();
        
        void lock()
        {
            wait();
        }
        
        void unlock()
        {
            post();
        }
        
    private:
        PosixUnnamedSem(PosixUnnamedSem const&) = delete;
        PosixUnnamedSem& operator = (PosixUnnamedSem const&) = delete;
        
        sem_t sem;
    };
}

#endif
