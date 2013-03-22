#ifndef _997DABD0_908E_11E2_A983_206A8A22A96A
#define _997DABD0_908E_11E2_A983_206A8A22A96A

namespace posixpp
{
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
}

#endif
