#ifndef _17C9431C_979E_11E2_A9BC_206A8A22A96A
#define _17C9431C_979E_11E2_A9BC_206A8A22A96A

#include <cerrno>

#include <condition_variable>
#include <mutex>
#include <thread>

namespace glstreamer
{
    class CancellableThread
    {
    public:
        CancellableThread();
        virtual ~CancellableThread() = default;
        
        void start();
        void waitForFinish();
        
        void cancel();
        
        static CancellableThread* this_thread();
        
        static void enable_cancellation();
        static void enter_syscall();
        static void check_cancel();
        static void leave_syscall();
        static void disable_cancellation();
        
    protected:
        virtual void run() = 0;
        
    private:
        void launch();
        
        void _enable_cancellation();
        void _enter_syscall();
        void _check_cancel();
        void _leave_syscall();
        void _disable_cancellation();
        
        std::thread stdHandle;
        
        bool cancellable;
        
        int cancel_state;
        int cancel_inprogress;
        std::condition_variable cond_handled;
        std::mutex mutex_inprogress;
        std::mutex cancelling;
    };
    
    class CancelException : public std::exception
    {
    public:
        CancelException():
        std::exception()
        {}
        
        virtual const char* what() const noexcept
        {
            return "Thread cancelled.";
        }
    };
    
    // NOTE: Ignores CancelException thrown by disable_cancellation.
    class CancellationEnabler
    {
    public:
        CancellationEnabler();
        ~CancellationEnabler() noexcept;
        
    private:
        CancellationEnabler(CancellationEnabler const&) = delete;
        CancellationEnabler& operator = (CancellationEnabler const&) = delete;
    };
    
    #define cancel_point(call, testerr)\
    ({\
        ::glstreamer::CancellableThread::enter_syscall();\
        decltype(call) result;\
        do\
        {\
            result = call;\
            if(testerr(result) == EINTR)\
                ::glstreamer::CancellableThread::check_cancel();\
            else\
                break;\
        } while(true);\
        ::glstreamer::CancellableThread::leave_syscall();\
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
