#include <iostream>

#include "testfactory.h"

#include "../fixedprocessor.h"
#include "../internalsinglelink.h"
#include "../threadedlink.h"

using namespace std;
using namespace glstreamer;

class Provider : public FixedProcessor<TypeList<>, TypeList<int, char, float>>
{
public:
    template <typename ... Args>
    Provider(Args&& ... args):
    FixedProcessor<TypeList<>, TypeList<int, char, float>>(std::forward<Args>(args)...),
    counter(0)
    {}
    
    virtual void do_run(int& i, char& c, float& f) override
    {
        ++counter;
        i = counter;
        c = counter * 2;
        f = counter * 3;
    }
    
private:
    int counter;
};

class P : public FixedProcessor<TypeList<int, char, float>, TypeList<long, short, double>>
{
public:
    template <typename ... Args>
    P(Args&&...args):
    FixedProcessor<TypeList<int, char, float>, TypeList<long, short, double>>(std::forward<Args>(args)...)
    {}
    
    virtual void do_run(int& i, char& c, float& f, long& l, short& s, double& d) override
    {
        l = i + 1;
        s = c + 1;
        d = f + 1;
    }
};

class Printer : public FixedProcessor<TypeList<long, short, double>, TypeList<>>
{
public:
    template <typename ... Args>
    Printer(Args&& ... args):
    FixedProcessor<TypeList<long, short, double>, TypeList<>>(std::forward<Args>(args)...)
    {}
    
    virtual void do_run(long& l, short& s, double& d) override
    {
        cout << l << endl << s << endl << d << endl;
    }
};

Processor* makeProvider()
{
    return new Provider("intsrc", "charsrc", "floatsrc");
}

Processor* makeP()
{
    return new P("intsink", "charsink", "floatsink", "longsrc", "shortsrc", "doublesrc");
}

Processor* makePrinter()
{
    return new Printer("longsink", "shortsink", "doublesink");
}

Link* makeInternalSingleLink(const OutputSlot& src, const InputSlot& dst)
{
    return new InternalSingleLink(src, dst);
}

Link* makeThreadedLink(const OutputSlot& src, const InputSlot& dst)
{
    return new ThreadedLink(src, dst, 3);
}
