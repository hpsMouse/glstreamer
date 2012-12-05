#include "../defaultspecs.h"
#include "../processlink.h"
#include "../processor.h"

#include "testfactory.h"

using namespace std;
using namespace glstreamer;

int main()
{
    registerDefaultTypes();
    
    unique_ptr<Processor> p(makeP()), printer(makePrinter());
    unique_ptr<Link> links[6] = {
        unique_ptr<Link>(new ProcessLink("test.link0", p->inputArg(0), 3)),
        unique_ptr<Link>(new ProcessLink("test.link1", p->inputArg(1), 3)),
        unique_ptr<Link>(new ProcessLink("test.link2", p->inputArg(2), 3)),
        unique_ptr<Link>(makeInternalSingleLink(p->outputArg(0), printer->inputArg(0))),
        unique_ptr<Link>(makeInternalSingleLink(p->outputArg(1), printer->inputArg(1))),
        unique_ptr<Link>(makeInternalSingleLink(p->outputArg(2), printer->inputArg(2)))
    };
    
    for(int i = 0; i < 5; ++i)
    {
        p->execute();
        printer->execute();
    }
}
