#include <cstdio>
#include <cstdlib>

#include <memory>

#include "../glstreamer.h"
#include "../processlink.h"
#include "../processor.h"

#include "case2.h"

int main(int argc, char *argv[])
{
    using namespace std;
    using namespace glstreamer;
    
    init();
    registerTypes();
    
    if(argc != 2)
    {
        fprintf(stderr, "Usage: %s reader | caster | processor | printer\n", argv[0]);
        return EXIT_FAILURE;
    }
    
    unique_ptr<Processor> processor;
    vector<unique_ptr<Link>> links;
    string role(argv[1]);
    if(role == "reader")
    {
        processor.reset(makeReader());
        links.resize(1);
        links[0].reset(new ProcessLink(processor->outputArg(0), "test.link0", 2));
    }
    else if(role == "caster")
    {
        processor.reset(makeCaster());
        links.resize(3);
        links[0].reset(new ProcessLink("test.link0", processor->inputArg(0), 2));
        links[1].reset(new ProcessLink(processor->outputArg(0), "test.link1", 2));
        links[2].reset(new ProcessLink(processor->outputArg(1), "test.link2", 2));
    }
    else if(role == "processor")
    {
        processor.reset(makeProcessor());
        links.resize(2);
        links[0].reset(new ProcessLink("test.link1", processor->inputArg(0), 2));
        links[1].reset(new ProcessLink(processor->outputArg(0), "test.link3", 2));
    }
    else if(role == "printer")
    {
        processor.reset(makePrinter());
        links.resize(2);
        links[0].reset(new ProcessLink("test.link3", processor->inputArg(0), 2));
        links[1].reset(new ProcessLink("test.link2", processor->inputArg(1), 2));
    }
    else
    {
        fprintf(stderr, "Unrecognized role \"%s\"\n", argv[1]);
        return EXIT_FAILURE;
    }
    
    for(int i = 0; i < 5; ++i)
        processor->execute();
}
