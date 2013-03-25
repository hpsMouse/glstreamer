#include <cstdio>
#include <cstdlib>

#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

#include <sys/types.h>
#include <sys/socket.h>

#include <netdb.h>

#include "../defaultspecs.h"
#include "../fdfiller.h"
#include "../linkbuffer.h"
#include "../MemBufferOStream.h"
#include "../typemgr.h"
#include "../typespec.h"

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
    LinkBuffer linkBufffer(2);
    TypeSpec *typeSpec = TypeManager::getTypeSpec<std::vector<std::string>>();
    FdFiller filler(addrs->ai_addr, addrs->ai_addrlen, 0x11223344, &linkBufffer, typeSpec);
    freeaddrinfo(addrs);
    for(int i = 0; i < 5; ++i)
    {
        std::string line;
        std::getline(std::cin, line);
        std::istringstream words(line);
        std::vector<std::string> wordvec(std::istream_iterator<std::string>(words), (std::istream_iterator<std::string>()));
        auto& buffer = linkBufffer.getEmpty();
        buffer.clear();
        MemBufferOStream os(buffer);
        typeSpec->serialize_auto(&wordvec, os);
        linkBufffer.putFull();
    }
}
