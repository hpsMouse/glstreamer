#include <iostream>

#include "argblock.h"
#include "defaultspecs.h"
#include "internallink.h"
#include "processor.h"
#include "typespec.h"

template struct glstreamer::DefaultTypeSpec<int>;

using namespace glstreamer;

class Provider : public FixedProcessor<TypeList<>, TypeList<int, float, char>>
{
public:
    template <typename ... Types>
    Provider(Types const& ... names):
    FixedProcessor<TypeList<>, TypeList<int, float, char>>(names ...),
    counter(0)
    {}
    
    virtual void do_run(int& i, float& f, char& c) override
    {
        i = counter;
        f = counter * 2;
        c = counter * 3;
        ++counter;
    }
    
private:
    int counter;
};

class P : public FixedProcessor<TypeList<int, float, char>, TypeList<long, double, short>>
{
public:
    template <typename ... Types>
    P(Types const& ... names):
    FixedProcessor<TypeList<int, float, char>, TypeList<long, double, short>>(names...)
    {}
    
    virtual void do_run(int& ii, float& fi, char& ci, long& lo, double& _do, short& so) override
    {
        lo = ii * 2;
        _do = fi * 2;
        so = ci * 2;
    }
};

class Printer : public FixedProcessor<TypeList<long, double, short>, TypeList<>>
{
public:
    template <typename ... Types>
    Printer(Types const& ... names):
    FixedProcessor<TypeList<long, double, short>, TypeList<>>(names ...)
    {}
    
    virtual void do_run(long& l, double& d, short& s) override
    {
        using namespace std;
        cout << l << ' ' << d << ' ' << s << endl;
    }
};

int main()
{
    using namespace std;
    using namespace glstreamer;
    registerDefaultTypes();
    ArgBlock argBlock;
    argBlock.addSlots<int, float, char>("ivalue", "fvalue", "cvalue");
    for(ArgBlock::size_type i = 0; i < argBlock.argCount(); ++i)
    {
        Slot const& slot(argBlock.arg(i));
        cout << slot.name << " :: " << slot.typeSpec->id()->name() << endl;
    }
    Provider provider("a", "b", "c");
    P p("d", "e", "f", "g", "h", "i");
    Printer printer("j", "k", "l");
    InternalLink l1(&provider, 0, &p, 0), l2(&provider, 1, &p, 1), l3(&provider, 2, &p, 2);
    InternalLink l4(&p, 0, &printer, 0), l5(&p, 1, &printer, 1), l6(&p, 2, &printer, 2);
    provider.run();
    p.run();
    printer.run();
    provider.run();
    p.run();
    printer.run();
    provider.run();
    p.run();
    printer.run();
    return 0;
}
