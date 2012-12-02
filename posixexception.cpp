#include <unordered_map>
#include <sstream>

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

PosixException::PosixException ( const string& funcName, int errnoValue ):
runtime_error("Error calling " + funcName + "(): " + lookupErrnoName(errnoValue)),
call(funcName),
errnoValue(errnoValue)
{}
