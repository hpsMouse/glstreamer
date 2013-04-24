#include <cstdio>
#include <cstdlib>

#include <iostream>
#include <iterator>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include <sys/types.h>
#include <sys/socket.h>

#include <netdb.h>

#include "../glstreamer.h"
#include "../processor.h"
#include "../SocketLinkOutput.h"

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
    init();
    addrinfo* addrs = nullptr;
    int err = getaddrinfo(argv[1], argv[2], nullptr, &addrs);
    if(err)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(err));
        return EXIT_FAILURE;
    }
    unique_ptr<Processor> reader(makeStringVectorReader());
    SocketLinkOutput link(reader->outputArg(0), addrs->ai_addr, addrs->ai_addrlen, 0x11223344);
    freeaddrinfo(addrs);
    for(int i = 0; i < 5; ++i)
    {
        reader->execute();
    }
}
