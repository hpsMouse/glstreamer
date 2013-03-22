#include <cstdio>
#include <cstdlib>

#include <sys/types.h>
#include <sys/socket.h>

#include <netdb.h>

#include "../defaultspecs.h"
#include "../demuxer.h"
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
    Demuxer demuxer(addrs->ai_addr, addrs->ai_addrlen);
    freeaddrinfo(addrs);
    TypeSpec *typeSpec = TypeManager::getTypeSpec<unsigned>();
    LinkBuffer* buffer = demuxer.getLink(0x11223344, 2, typeSpec);
    for(int i = 0; i < 5; ++i)
    {
        std::vector<char>& data = buffer->getFull();
        printf("size: %zu\n", data.size());
        unsigned value;
        typeSpec->deserialize_fixed(&value, data.data());
        printf("%u\n", value);
        buffer->putEmpty();
    }
}
