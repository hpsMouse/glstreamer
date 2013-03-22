#include <condition_variable>
#include <map>
#include <mutex>

#include <signal.h>
#include <unistd.h>

#include "cancel.h"

using namespace posixpp;

CancelException::CancelException():
std::exception()
{}

const char* CancelException::what() const noexcept
{
    return "Thread cancelled.";
}

namespace
{
    enum { ENTERING_SYSCALL = (1 << 0), CANCEL_MARKED = (1 << 1) };
    
    struct CancellationInfo
    {
        CancellationInfo():
        cancel_state(0),
        cancel_inprogress(0),
        cond_handled(),
        mutex_inprogress()
        {}
        int cancel_state;
        int cancel_inprogress;
        std::condition_variable cond_handled;
        std::mutex mutex_inprogress;
        
        CancellationInfo(CancellationInfo const&) = delete;
        CancellationInfo& operator = (CancellationInfo const&) = delete;
    };
}

static __thread CancellationInfo *cancellation_info = nullptr;
static std::map<std::thread::id, CancellationInfo*> cancellation_map;
static std::mutex map_lock;

static void empty_handler(int)
{
    ::write(2, "Async cancellation received.\n", sizeof("Async cancellation received.\n"));
}

void posixpp::enable_cancellation()
{
    if(cancellation_info != nullptr)
        return;
    cancellation_info = new CancellationInfo;
    
    struct sigaction act;
    act.sa_handler = empty_handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGUSR1, &act, nullptr);
    
    std::unique_lock<std::mutex> map_locker(map_lock);
    cancellation_map.insert(std::make_pair(std::this_thread::get_id(), cancellation_info));
}

void posixpp::disable_cancellation()
{
    if(cancellation_info == nullptr)
        return;
    
    std::unique_lock<std::mutex> map_locker(map_lock);
    cancellation_map.erase(std::this_thread::get_id());
    map_locker.unlock();
    
    int old_state = __sync_fetch_and_and(&cancellation_info->cancel_state, ~CANCEL_MARKED);
    
    delete cancellation_info;
    cancellation_info = nullptr;
    
    if((old_state & CANCEL_MARKED))
        throw CancelException();
}

static bool cancellationEnabled()
{
    return cancellation_info != nullptr;
}

void posixpp::enter_syscall()
{
    if(!cancellationEnabled())
        return;
    
    int old_state = __sync_fetch_and_or(&cancellation_info->cancel_state, ENTERING_SYSCALL);
    if((old_state & CANCEL_MARKED))
    {
        __sync_fetch_and_and(&cancellation_info->cancel_state, ~(ENTERING_SYSCALL | CANCEL_MARKED));
        throw CancelException();
    }
}

void posixpp::check_cancel()
{
    if(!cancellationEnabled())
        return;
    
    __sync_synchronize();
    int old_state = cancellation_info->cancel_state;
    if((old_state & CANCEL_MARKED))
    {
        std::unique_lock<std::mutex> locker(cancellation_info->mutex_inprogress);
        cancellation_info->cancel_inprogress = 0;
        cancellation_info->cond_handled.notify_one();
        __sync_fetch_and_and(&cancellation_info->cancel_state, ~(ENTERING_SYSCALL | CANCEL_MARKED));
        throw CancelException();
    }
}

void posixpp::leave_syscall()
{
    if(!cancellationEnabled())
        return;
    
    int old_state = __sync_fetch_and_and(&cancellation_info->cancel_state, ~ENTERING_SYSCALL);
    if((old_state & CANCEL_MARKED))
    {
        std::unique_lock<std::mutex> locker(cancellation_info->mutex_inprogress);
        cancellation_info->cancel_inprogress = 0;
        cancellation_info->cond_handled.notify_one();
    }
}

bool posixpp::cancel_thread ( std::thread& thread )
{
    std::thread::id thread_id = thread.get_id();
    if(thread_id == std::this_thread::get_id())
        throw CancelException();
    
    std::unique_lock<std::mutex> map_locker(map_lock);
    
    auto iter_cancellation_info = cancellation_map.find(thread_id);
    if(iter_cancellation_info == cancellation_map.end())
        return false;
    
    CancellationInfo* info = iter_cancellation_info->second;
    
    int old_state = __sync_fetch_and_or(&info->cancel_state, CANCEL_MARKED);
    if((old_state & CANCEL_MARKED) || !(old_state & ENTERING_SYSCALL))
        return true;
    
    map_locker.unlock();
    
    std::unique_lock<std::mutex> locker(info->mutex_inprogress);
    info->cancel_inprogress = 1;
    do
        pthread_kill(thread.native_handle(), SIGUSR1);
    while(info->cond_handled.wait_for(locker, std::chrono::seconds(1)) == std::cv_status::timeout);
    return true;
}
