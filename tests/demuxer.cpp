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
#include "../linkbuffer.h"
#include "../MemBufferIStream.h"
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
    
    typedef std::vector<std::string> value_type;
    
    TypeSpec *typeSpec = TypeManager::getTypeSpec<value_type>();
    LinkBuffer* buffer = demuxer.getLink(0x11223344, 2, typeSpec);
    for(int i = 0; i < 5; ++i)
    {
        std::vector<char>& data = buffer->getFull();
        printf("size: %zu\n", data.size());
        MemBufferIStream is(data.data(), data.size());
        value_type value;
        typeSpec->deserialize_auto(&value, is);
        for(auto const& str : value)
            std::cout << str << ' ';
        std::cout << std::endl;
        buffer->putEmpty();
    }
}
