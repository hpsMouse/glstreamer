#ifndef _D869E78A_9770_11E2_9499_206A8A22A96A
#define _D869E78A_9770_11E2_9499_206A8A22A96A

#include <memory>
#include <string>

#include "../defaulttypespec.h"
#include "../istream.h"
#include "../ostream.h"
#include "../types.h"

namespace glstreamer_core
{
    using namespace glstreamer;
    struct StringSpec : public TypeSpecBasic<std::string>, public TypeSpecNoContext, public TypeSpecNoFixedSerialize
    {
        virtual void serialize_variable ( const void* obj, LocalArgBase*, OStream& os ) const override
        {
            std::string const* str = static_cast<std::string const*>(obj);
            os << Word64(str->size());
            os.writeData(str->data(), str->size());
        }
        
        virtual void deserialize_varialbe ( void* obj, LocalArgBase*, IStream& is ) const override
        {
            std::string* str = static_cast<std::string*>(obj);
            Word64 size;
            is >> size;
            std::unique_ptr<char[]> buffer(new char[size]);
            is.readData(buffer.get(), size);
            str->assign(buffer.get(), buffer.get() + size);
        }
    };
}

#endif
