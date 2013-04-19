#include <signal.h>

#include "CancellableThread.h"

using namespace glstreamer;

namespace
{
    enum { ENTERING_SYSCALL = (1 << 0), CANCEL_MARKED = (1 << 1) };
}

static __thread CancellableThread* current_thread = nullptr;

CancellableThread::CancellableThread():
stdHandle(),
cancellable(false),
cancel_state(0),
cancel_inprogress(0),
cond_handled(),
mutex_inprogress(),
cancelling()
{}

void CancellableThread::start()
{
    stdHandle = std::thread([this]{this->launch();});
}

void CancellableThread::waitForFinish()
{
    stdHandle.join();
}

void CancellableThread::cancel()
{
    if(this == current_thread)
        throw CancelException();
    
    int old_state = __sync_fetch_and_or(&cancel_state, CANCEL_MARKED);
    if((old_state & CANCEL_MARKED) || !(old_state & ENTERING_SYSCALL))
        return;
    
    std::lock_guard<std::mutex> canceling_locker(cancelling);
    std::unique_lock<std::mutex> locker(mutex_inprogress);
    cancel_inprogress = 1;
    do
        pthread_kill(stdHandle.native_handle(), SIGUSR1);
    while(cond_handled.wait_for(locker, std::chrono::seconds(1)) == std::cv_status::timeout);
}

CancellableThread* CancellableThread::this_thread()
{
    return current_thread;
}

void CancellableThread::enable_cancellation()
{
    if(current_thread == nullptr)
        return;
    else
        current_thread->_enable_cancellation();
}

void CancellableThread::enter_syscall()
{
    if(current_thread == nullptr)
        return;
    else
        current_thread->_enter_syscall();
}

void CancellableThread::check_cancel()
{
    if(current_thread == nullptr)
        return;
    else
        current_thread->_check_cancel();
}

void CancellableThread::leave_syscall()
{
    if(current_thread == nullptr)
        return;
    else
        current_thread->_leave_syscall();
}

void CancellableThread::disable_cancellation()
{
    if(current_thread == nullptr)
        return;
    else
        current_thread->_disable_cancellation();
}

void CancellableThread::launch()
{
    current_thread = this;
    this->run();
}

static void empty_handler(int)
{}

void CancellableThread::_enable_cancellation()
{
    if(cancellable)
        return;
    
    struct sigaction act;
    act.sa_handler = empty_handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGUSR1, &act, nullptr);
    
    cancellable = true;
}

void CancellableThread::_enter_syscall()
{
    if(!cancellable)
        return;
    
    int old_state = __sync_fetch_and_or(&cancel_state, ENTERING_SYSCALL);
    if((old_state & CANCEL_MARKED))
    {
        __sync_fetch_and_and(&cancel_state, ~(ENTERING_SYSCALL | CANCEL_MARKED));
        throw CancelException();
    }
}

void CancellableThread::_check_cancel()
{
    if(!cancellable)
        return;
    
    __sync_synchronize();
    int old_state = cancel_state;
    if((old_state & CANCEL_MARKED))
    {
        std::unique_lock<std::mutex> locker(mutex_inprogress);
        cancel_inprogress = 0;
        cond_handled.notify_one();
        __sync_fetch_and_and(&cancel_state, ~(ENTERING_SYSCALL | CANCEL_MARKED));
        throw CancelException();
    }
}

void CancellableThread::_leave_syscall()
{
    if(!cancellable)
        return;
    
    int old_state = __sync_fetch_and_and(&cancel_state, ~ENTERING_SYSCALL);
    if((old_state & CANCEL_MARKED))
    {
        std::unique_lock<std::mutex> locker(mutex_inprogress);
        cancel_inprogress = 0;
        cond_handled.notify_one();
    }
}

void CancellableThread::_disable_cancellation()
{
    if(!cancellable)
        return;
    
    cancellable = false;
    
    int old_state = __sync_fetch_and_and(&cancel_state, ~CANCEL_MARKED);
    
    if((old_state & CANCEL_MARKED))
        throw CancelException();
}

CancellationEnabler::CancellationEnabler()
{
    CancellableThread::enable_cancellation();
}

CancellationEnabler::~CancellationEnabler() noexcept
{
    try
    {
        CancellableThread::disable_cancellation();
    }
    catch(CancelException const&)
    {}
}
