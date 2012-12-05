#ifndef _9FBA6ACC_3C94_11E2_8AAB_206A8A22A96A
#define _9FBA6ACC_3C94_11E2_8AAB_206A8A22A96A

#include <string>

#include "types.h"

namespace glstreamer
{
    class PosixShm
    {
    public:
        PosixShm() noexcept;
        
        /**
         * \brief Open a POSIX shared memory object by name
         * 
         * The name mustn't contain any slashes, including the initial one,
         * which will be added automatically by this constructor.
         * 
         * If \c create is \c true and the creation fails because of an existing
         * shared memory with the same name, no exception will be thrown,
         * instead hasOwnership() will return \c false. In such a case you may
         * want to reopen the shared memory object with a new PosixShm or report
         * an error to the upper layer.
         * 
         * \param name The shared memory object name without the initial
         *             slash.
         * \param create If set to \c true, a new shared memory object is
         *               created.
         */
        explicit PosixShm(std::string const& name, bool create = false);
        
        ~PosixShm() noexcept;
        
        PosixShm(PosixShm&& shm) noexcept;
        PosixShm& operator = (PosixShm&& shm) noexcept;
        
        /**
         * \brief Get real size of the object
         * 
         * Since the size can be changed by any process with permission,
         * it's never cached by this function. Thus this function might not be
         * as fast as expected and is not suitable for heavy use.
         */
        size_type size() const;
        
        size_type mappedSize() const { return _mappedSize; }
        
        /**
         * \brief Resize the shared memory object
         * 
         * By default the resize operation will unmap any previously mapped
         * area. To remap the object, pass \c true to \c remap parameter.
         * 
         * \return The newly mapped address if <tt>remap == true</tt>, or
         * \c nullptr if <tt>remap == false</tt>.
         */
        void* resize(size_type size, bool remap = false);
        
        /**
         * \brief Map the object into the process' address space
         * 
         * Attempting to map an already mapped object will retrun the
         * previously mapped address without a second mapping, and the
         * \c size parameter will also be ignored.
         * 
         * \param size The size to be mapped. If this parameter is zero the size
         * returned by size() is used instead.
         * 
         * \see unmap()
         */
        void* map(size_type size = 0);
        
        /**
         * \brief Unmap the object
         * 
         * An object can be unmaped more than once, and any extra
         * unmapping has no effects.
         * 
         * \see map()
         */
        void unmap() noexcept;
        
        bool hasOwnership() const { return owns; }
        
    private:
        PosixShm(PosixShm const&) = delete;
        PosixShm& operator = (PosixShm const&) = delete;
        
        void destroy() noexcept;
        void unsafeClear() noexcept;
        
        int shm;
        void *mapped;
        size_type _mappedSize;
        std::string _name;
        bool owns;
    };
}

#endif
