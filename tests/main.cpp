#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

#include <sys/types.h>
#include <unistd.h>

#include "../defaultspecs.h"
#include "../posixsem.h"
#include "../processor.h"

#include "testfactory.h"

using namespace std;
using namespace chrono;
using namespace glstreamer;

int main()
{
    pid_t pid = getpid();
    
    registerDefaultTypes();
    
#if 0
    Processor *provider = makeProvider();
    Processor *p = makeP();
    Processor *printer = makePrinter();
#endif
    
#if 0
    // Semaphore test
    PosixSem semaphore("semtest", true);
    if(semaphore.hasOwnership())
    {
        clog << pid << " got ownership" << endl;
        for(int i = 0; i < 5; ++i)
        {
            this_thread::sleep_for(duration<long>(2));
            semaphore.post();
        }
    }
    else
    {
        clog << pid << " didn't get ownership" << endl;
        semaphore = PosixSem("semtest");
        semaphore.wait();
        clog << pid << " got semaphore" << endl;
    }
#endif
    
#if 0
    // Internal link test
    vector<Link*> links = {
        makeInternalSingleLink(provider->outputArg(0), p->inputArg(0)),
        makeInternalSingleLink(provider->outputArg(1), p->inputArg(1)),
        makeInternalSingleLink(provider->outputArg(2), p->inputArg(2)),
        makeInternalSingleLink(p->outputArg(0), printer->inputArg(0)),
        makeInternalSingleLink(p->outputArg(1), printer->inputArg(1)),
        makeInternalSingleLink(p->outputArg(2), printer->inputArg(2))
    };
    
    for(int i = 0; i < 100000000; ++i)
    {
        provider->execute();
        p->execute();
        printer->execute();
    }
#endif
    
#if 0
    // Threaded link test
    vector<Link*> links = {
        makeThreadedLink(provider->outputArg(0), p->inputArg(0)),
        makeThreadedLink(provider->outputArg(1), p->inputArg(1)),
        makeThreadedLink(provider->outputArg(2), p->inputArg(2)),
        makeThreadedLink(p->outputArg(0), printer->inputArg(0)),
        makeThreadedLink(p->outputArg(1), printer->inputArg(1)),
        makeThreadedLink(p->outputArg(2), printer->inputArg(2))
    };
    
    thread t1([provider]()
    {
        for(int i = 0; i < 7; ++i)
        {
            provider->execute();
        }
        
    });
    thread t2([p]()
    {
        this_thread::sleep_for(duration<long>(5));
        for(int i = 0; i < 7; ++i)
        {
            p->execute();
            this_thread::sleep_for(duration<long>(1));
        }
    });
    thread t3([printer]()
    {
        for(int i = 0; i < 7; ++i)
            printer->execute();
    });
    
    t1.join();
    t2.join();
    t3.join();
#endif
    
#if 0
    // ArgBlock test
    std::string ccc("ccc"), dddd("dddd");
    P p("a", (char const*)"bb", ccc, std::move(dddd), "eeeee", "ffffff");
    Processor &pr = static_cast<Processor&>(p);
    cout << pr.inputArgCount() << " slots." << endl;
    for(size_type i = 0; i < pr.inputArgCount(); ++i)
    {
        FullSlot const& slot(pr.inputArg(i).toSlot());
        cout << "Slot " << i << ":" << endl;
        cout << slot.simpleSlot << endl;
        cout << slot.simpleSlot->arg << endl;
        cout << slot.simpleSlot->link << endl;
        cout << slot.simpleSlot->typeSpec << endl;
        cout << slot.simpleSlot->typeSpec->id().name() << endl;
        cout << slot.typeSpec << endl;
        cout << slot.name << endl;
        cout << slot.processor << endl;
        cout << slot.direction << endl;
    }
    cout << pr.outputArgCount() << " slots." << endl;
    for(size_type i = 0; i < pr.outputArgCount(); ++i)
    {
        FullSlot const& slot(pr.outputArg(i).toSlot());
        cout << "Slot " << i << ":" << endl;
        cout << slot.simpleSlot << endl;
        cout << slot.simpleSlot->arg << endl;
        cout << slot.simpleSlot->link << endl;
        cout << slot.simpleSlot->typeSpec << endl;
        cout << slot.simpleSlot->typeSpec->id().name() << endl;
        cout << slot.typeSpec << endl;
        cout << slot.name << endl;
        cout << slot.processor << endl;
        cout << slot.direction << endl;
    }
#endif
}
