#include <iostream>

#include "argblock.h"
#include "defaultspecs.h"
#include "internalsinglelink.h"
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
    InternalSingleLink l0(provider.outputArg(0), p.inputArg(0));
    InternalSingleLink l1(provider.outputArg(1), p.inputArg(1));
    InternalSingleLink l2(provider.outputArg(2), p.inputArg(2));
    InternalSingleLink l3(p.outputArg(0), printer.inputArg(0));
    InternalSingleLink l4(p.outputArg(1), printer.inputArg(1));
    InternalSingleLink l5(p.outputArg(2), printer.inputArg(2));
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
