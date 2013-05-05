#ifndef _0ABF477A_B468_11E2_BFE0_206A8A22A96A
#define _0ABF477A_B468_11E2_BFE0_206A8A22A96A

#include "GLOps.h"

namespace glstreamer_gl
{
    template <typename Ops>
    class GLObject : private Ops
    {
    public:
        typedef typename Ops::ID ID;
        
        GLObject():
        id(Ops::generateOne())
        {}
        
        ~GLObject() noexcept
        {
            Ops::deleteOne(id);
        }
        
        ID get() const
        {
            return id;
        }
        
        operator ID() const
        {
            return get();
        }
        
    private:
        ID id;
    };
    
#define GLSTREAMER_GL_DEF_OBJECT(ObjName, OpName)\
    typedef GLObject<OpName> ObjName;
    
    GLSTREAMER_GL_DEF_OBJECT(TextureObject, TextureOp);
    GLSTREAMER_GL_DEF_OBJECT(FramebufferObject, FramebufferOp);
    GLSTREAMER_GL_DEF_OBJECT(BufferObject, BufferOp);
}

#endif
