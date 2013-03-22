#ifndef _C43F3862_88AE_11E2_A65C_206A8A22A96A
#define _C43F3862_88AE_11E2_A65C_206A8A22A96A

#include <cerrno>

#include <exception>
#include <thread>

namespace posixpp
{
    class CancelException : public std::exception
    {
    public:
        CancelException();
        
        virtual const char* what() const noexcept;
    };
    
    void enable_cancellation();
    // NOTE: This is also a cancellation point. However the cancellation ability
    // is always disabled, even if a CancelException is thrown.
    void disable_cancellation();
    
    void enter_syscall();
    void check_cancel();
    void leave_syscall();
    
    bool cancel_thread(std::thread &thread);
    
    #define cancel_point(call, testerr)\
    ({\
        ::posixpp::enter_syscall();\
        decltype(call) result;\
        do\
        {\
            result = call;\
            if(testerr(result) == EINTR)\
                ::posixpp::check_cancel();\
            else\
                break;\
        } while(true);\
        ::posixpp::leave_syscall();\
        result;\
    })
    
    template <typename T>
    inline constexpr int negerrno(T const& result)
    {
        return result < T() ? errno : 0;
    }
    
    template <typename T>
    inline constexpr int nonzeroerrno(T const& result)
    {
        return result != T() ? errno : 0;
    }
    
    template <typename T>
    inline constexpr int zeroerrno(T const& result)
    {
        return result == T() ? errno : 0;
    }
}

#endif
