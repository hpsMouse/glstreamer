#include <thread>

#include "../defaultspecs.h"
#include "../processlink.h"
#include "../processor.h"

#include "testfactory.h"

using namespace std;
using namespace glstreamer;

template <typename T>
void ignore_var(T const&)
{}

int main()
{
    registerDefaultTypes();
    
    unique_ptr<Processor> provider(makeProvider());
    unique_ptr<Link> links[3] = {
        unique_ptr<Link>(new ProcessLink(provider->outputArg(0), "test.link0", 3)),
        unique_ptr<Link>(new ProcessLink(provider->outputArg(1), "test.link1", 3)),
        unique_ptr<Link>(new ProcessLink(provider->outputArg(2), "test.link2", 3))
    };
    ignore_var(links);
    
    for(int i = 0; i < 5; ++i)
    {
        this_thread::sleep_for(chrono::duration<long>(2));
        provider->execute();
    }
}
