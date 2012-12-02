#ifndef _9748D502_3B8D_11E2_95C2_206A8A22A96A
#define _9748D502_3B8D_11E2_95C2_206A8A22A96A

#include <string>

#include <semaphore.h>

namespace glstreamer
{
    class PosixSem
    {
    public:
        PosixSem();
        
        /**
         * \brief Open a POSIX semaphore by name.
         * 
         * Note that the name mustn't contain any slashes, including the
         * initial one, which will be automatically added by this constructor.
         * 
         * If \c create is \c true and the creation fails because of an existing
         * semaphore, no exception will be thrown, instead hasOwnership()
         * will return \c false. In such a case you may want to reopen the
         * semaphore with a new PosixSem object or report an error to the upper
         * level.
         * 
         * \param name   the semaphore name without the initial slash
         * \param create if set to true, then a new semaphore is created
         * \param value  initial value when creating a new semaphore
         */
        explicit PosixSem(std::string const& name, bool create = false, unsigned int value = 0);
        
        ~PosixSem() noexcept;
        
        PosixSem(PosixSem&& sem) noexcept;
        PosixSem& operator = (PosixSem&& sem) noexcept;
        
        void post();
        void wait();
        
        void lock()   { wait(); }
        void unlock() { post(); }
        
        bool hasOwnership() const { return owns; }
        
    private:
        PosixSem(PosixSem const&) = delete;
        PosixSem& operator = (PosixSem const&) = delete;
        
        void destroy() noexcept;
        void unsafeClear() noexcept;
        
        std::string _name;
        sem_t *sem;
        bool owns;
    };
}

#endif
