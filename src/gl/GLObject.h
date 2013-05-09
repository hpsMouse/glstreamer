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
        
        GLObject(GLObject<Ops>&& obj):
        id(obj.id)
        {
            obj.id = ID();
        }
        
        GLObject<Ops>& operator = (GLObject<Ops>&& obj)
        {
            Ops::deleteOne(id);
            id = obj.id;
            obj.id = ID();
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
        GLObject(GLObject<Ops> const&) = delete;
        GLObject<Ops>& operator = (GLObject<Ops> const&) = delete;
        
        ID id;
    };
    
#define GLSTREAMER_GL_DEF_OBJECT(ObjName, OpName)\
    typedef GLObject<OpName> ObjName;
    
    GLSTREAMER_GL_DEF_OBJECT(TextureObject, TextureOp);
    GLSTREAMER_GL_DEF_OBJECT(FramebufferObject, FramebufferOp);
    GLSTREAMER_GL_DEF_OBJECT(BufferObject, BufferOp);
    GLSTREAMER_GL_DEF_OBJECT(VertexArrayObject, VertexArrayOp);
    GLSTREAMER_GL_DEF_OBJECT(RenderbufferObject, RenderbufferOp);
}

#endif
