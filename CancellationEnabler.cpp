#include "CancellationEnabler.h"

#include "cancel.h"

using namespace posixpp;

CancellationEnabler::CancellationEnabler()
{
    enable_cancellation();
}

CancellationEnabler::~CancellationEnabler() noexcept
{
    try
    {
        disable_cancellation();
    }
    catch(CancelException const&)
    {}
}
