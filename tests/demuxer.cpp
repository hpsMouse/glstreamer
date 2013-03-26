#include <cstdio>
#include <cstdlib>

#include <iostream>
#include <string>
#include <vector>

#include <sys/types.h>
#include <sys/socket.h>

#include <netdb.h>

#include "../defaultspecs.h"
#include "../demuxer.h"
#include "../processor.h"
#include "../SocketLinkInput.h"

#include "testfactory.h"

using namespace std;
using namespace glstreamer;

int main(int argc, char *argv[])
{
    if(argc < 3)
    {
        fprintf(stderr, "Usage: %s addr port\n", argv[0]);
        return EXIT_FAILURE;
    }
    registerDefaultTypes();
    addrinfo* addrs = nullptr;
    int err = getaddrinfo(argv[1], argv[2], nullptr, &addrs);
    if(err)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(err));
        return EXIT_FAILURE;
    }
    Demuxer demuxer(addrs->ai_addr, addrs->ai_addrlen);
    freeaddrinfo(addrs);
    
    unique_ptr<Processor> printer(makeStringVectorPrinter());
    SocketLinkInput link(&demuxer, 0x11223344, printer->inputArg(0));
    for(int i = 0; i < 5; ++i)
    {
        printer->execute();
    }
}
