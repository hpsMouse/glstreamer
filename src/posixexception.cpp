#include <cstdlib>

#include <unordered_map>
#include <sstream>

#include <execinfo.h>
#include <string.h>

#include "posixexception.h"

using namespace std;
using namespace glstreamer;

static string lookupErrnoName(int errnoValue)
{
    char name[1024];
    char *errStr = strerror_r(errnoValue, name, 1024);
    return std::string(errStr);
}

static string getBacktrace()
{
    string result;
    void *addrs[16];
    int n = ::backtrace(addrs, 16);
    char **symbols = backtrace_symbols(addrs, n);
    for(int i = 0; i < n; ++i)
    {
        result += symbols[i];
        result += '\n';
    }
    free(symbols);
    return result;
}

PosixException::PosixException ( const string& funcName, int errnoValue, bool backtrace ):
runtime_error(string()),
message(),
call(funcName),
errnoValue(errnoValue),
backtrace(backtrace ? getBacktrace() : string())
{
    message = "Error calling " + funcName + "(): " + lookupErrnoName(errnoValue);
    if(backtrace)
    {
        message.append("\nbacktrace:\n");
        message.append(this->backtrace);
    }
}

const char* PosixException::what() const noexcept
{
    return message.c_str();
}
