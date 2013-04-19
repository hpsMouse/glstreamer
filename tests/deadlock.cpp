#include <cstdio>
#include <cstdlib>

#include <memory>
#include <string>

#include "../fixedprocessor.h"
#include "../glstreamer.h"
#include "../processlink.h"

using namespace std;
using namespace glstreamer;

using PAType = FixedProcessor<TypeList<>, TypeList<int, int>>;
class PA : public PAType
{
public:
    PA():
    PAType("slot1", "slot2")
    {}
    
    virtual void do_run(int&, int&) override
    {}
};

using PBType = FixedProcessor<TypeList<int, int>, TypeList<>>;
class PB : public PBType
{
public:
    PB():
    PBType("slot1", "slot2")
    {}
    
    virtual void do_run(int&, int&) override
    {}
};

int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        fprintf(stderr, "Usage: %s a | b | c\n", argv[0]);
        return EXIT_FAILURE;
    }
    
    init();
    
    string role = argv[1];
    unique_ptr<Processor> p;
    unique_ptr<Link> link1, link2;
    if(role == "a")
    {
        p.reset(new PA);
        link1.reset(new ProcessLink(p->outputArg(0), "test.link0", 2));
        link2.reset(new ProcessLink(p->outputArg(1), "test.link1", 2));
    }
    else if(role == "b")
    {
        p.reset(new PB);
        link1.reset(new ProcessLink("test.link1", p->inputArg(0), 2));
        link2.reset(new ProcessLink("test.link0", p->inputArg(1), 2));
    }
    else
    {
        fprintf(stderr, "Unknown role %s\n", role.c_str());
        return EXIT_FAILURE;
    }
}
