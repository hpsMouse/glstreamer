#include <cstring>

#include <algorithm>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

#include "case2.h"

#include "../defaulttypespec.h"
#include "../fixedprocessor.h"
#include "../istream.h"
#include "../ostream.h"
#include "../typemgr.h"

using namespace std;
using namespace glstreamer;

namespace
{
    template <typename T>
    struct VectorSpec : public TypeSpecBasic<vector<T>>, public TypeSpecNoContext, public TypeSpecNoFixedSerialize
    {
        VectorSpec():
        typeSpec(TypeManager::getTypeSpec<T>()),
        sSize(typeSpec->serialize_size())
        {}
        
        virtual void serialize_variable ( const void* obj, LocalArgBase*, OStream& os ) const override
        {
            vector<T> const& vec = *static_cast<vector<T> const*>(obj);
            
            os << uint32_t(vec.size());
            
            if(sSize == 0)
            {
                for(T const& v : vec)
                    typeSpec->serialize_variable(&v, nullptr, os);
            }
            else
            {
                for(T const& v : vec)
                {
                    char *buffer = static_cast<char*>(os.requireInternalBuffer(sSize));
                    typeSpec->serialize_fixed(&v, nullptr, buffer);
                    os.pushInternalBuffer(buffer, sSize);
                }
            }
        }
        
        virtual void deserialize_varialbe ( void* obj, LocalArgBase*, IStream& is ) const override
        {
            vector<T>& vec = *static_cast<vector<T>*>(obj);
            
            uint32_t size;
            is >> size;
            
            vec.resize(size);
            
            if(sSize == 0)
            {
                for(uint32_t i = 0; i < size; ++i)
                {
                    typeSpec->deserialize_varialbe(&vec[i], nullptr, is);
                }
            }
            else
            {
                vector<char> buffer(sSize);
                for(uint32_t i = 0; i < size; ++i)
                {
                    is.readData(buffer.data(), buffer.size());
                    typeSpec->deserialize_fixed(&vec[i], nullptr, buffer.data());
                }
            }
        }
        
    private:
        VectorSpec(VectorSpec<T> const&) = delete;
        VectorSpec<T>& operator = (VectorSpec<T> const&) = delete;
        
        TypeSpec *typeSpec;
        size_type sSize;
    };
    
    struct StringSpec : public TypeSpecBasic<string>, public TypeSpecNoContext, public TypeSpecNoFixedSerialize
    {
        virtual void serialize_variable ( const void* obj, LocalArgBase*, OStream& os ) const override
        {
            string const& str = *static_cast<string const*>(obj);
            
            uint32_t size = str.length();
            
            os << size;
            
            void* buffer = os.requireInternalBuffer(size);
            memcpy(buffer, str.data(), size);
            os.pushInternalBuffer(buffer, size);
        }
        
        virtual void deserialize_varialbe ( void* obj, LocalArgBase*, IStream& is ) const override
        {
            string& str = *static_cast<string*>(obj);
            
            uint32_t size;
            is >> size;
            
            vector<char> buffer(size);
            is.readData(buffer.data(), size);
            
            str.assign(buffer.begin(), buffer.end());
        }
    };
}

template <typename T>
static void registerVectorType()
{
    static VectorSpec<T> spec;
    TypeManager::registerTypeSpec<vector<T>>(&spec);
}

void registerTypes()
{
    static StringSpec spec;
    TypeManager::registerTypeSpec<string>(&spec);
    registerVectorType<int>();
    registerVectorType<double>();
    registerVectorType<string>();
    registerVectorType<float>();
}

using ReaderPrototype = FixedProcessor<TypeList<>, TypeList<vector<int>>>;

class Reader : public ReaderPrototype
{
public:
    Reader():
    ReaderPrototype("reader-output")
    {}
    
    virtual void do_run(vector<int>& vec) override
    {
        string line;
        getline(cin, line);
        istringstream is(line);
        copy(istream_iterator<int>(is), istream_iterator<int>(), back_inserter(vec));
    }
};

using CasterPrototype = FixedProcessor<TypeList<vector<int>>, TypeList<vector<double>, vector<string>>>;

class Caster : public CasterPrototype
{
public:
    Caster():
    CasterPrototype("caster-input", "caster-dblout", "caster-strout")
    {}
    
    virtual void do_run(vector<int>& in, vector<double>& dblout, vector<string>& strout) override
    {
        for(int i : in)
        {
            ostringstream os;
            os << i;
            dblout.push_back(i);
            strout.push_back(os.str());
        }
    }
};

using ProcessorPrototype = FixedProcessor<TypeList<vector<double>>, TypeList<vector<float>>>;

class VProcessor : public ProcessorPrototype
{
public:
    VProcessor():
    ProcessorPrototype("processor-input", "processor-output")
    {}
    
    virtual void do_run(vector<double>& input, vector<float>& output) override
    {
        for(double d : input)
            output.push_back(d * 2);
    }
};

using PrinterPrototype = FixedProcessor<TypeList<vector<float>, vector<string>>, TypeList<>>;

class Printer : public PrinterPrototype
{
public:
    Printer():
    PrinterPrototype("printer-fltin", "printer-strin")
    {}
    
    virtual void do_run(vector<float>& fltin, vector<string>& strin) override
    {
        cout << "Floats: ";
        copy(fltin.begin(), fltin.end(), ostream_iterator<float>(cout, " "));
        cout << endl;
        cout << "String: ";
        copy(strin.begin(), strin.end(), ostream_iterator<string>(cout, " "));
        cout << endl;
    }
};

Processor* makeReader()
{
    return new Reader;
}

Processor* makeCaster()
{
    return new Caster;
}

Processor* makeProcessor()
{
    return new VProcessor;
}

Processor* makePrinter()
{
    return new Printer;
}
