#include <cstdio>
#include <cstdlib>

#include <sys/types.h>
#include <sys/socket.h>

#include <netdb.h>

#include "../defaultspecs.h"
#include "../fdfiller.h"
#include "../linkbuffer.h"
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
    TypeSpec *typeSpec = TypeManager::getTypeSpec<unsigned>();
    FdFiller filler(addrs->ai_addr, addrs->ai_addrlen, 0x11223344, &linkBufffer, typeSpec);
    freeaddrinfo(addrs);
    for(int i = 0; i < 5; ++i)
    {
        unsigned u;
        std::scanf("%u", &u);
        auto& buffer = linkBufffer.getEmpty();
        buffer.resize(typeSpec->serialize_size());
        typeSpec->serialize_fixed(&u, buffer.data());
        linkBufffer.putFull();
    }
}
