#ifndef _600D4C20_B276_11E2_8977_206A8A22A96A
#define _600D4C20_B276_11E2_8977_206A8A22A96A

#include "../defaulttypespec.h"

#include "GLFrame.h"

namespace glstreamer_gl
{
    template <typename PixelTypePack>
    class GLFrameTypeSpec : public glstreamer::TypeSpecBasic<GLFrameData<PixelTypePack>>, public glstreamer::TypeSpecNoFixedSerialize
    {
        typedef typename PixelTypePack::Pixel Pixel;
        typedef GLFrameData<PixelTypePack> Frame;
        typedef GLTextureData<PixelTypePack> Texture;
    public:
        virtual glstreamer::LocalArgBase* createLocal() const override final;
        
        virtual void context_out ( void* obj, glstreamer::LocalArgBase* localArg ) const override final;
        
        virtual void context_in ( void* obj, glstreamer::LocalArgBase* localArg ) const override final;
        
        virtual void serialize_variable ( const void*, glstreamer::LocalArgBase* localArg, glstreamer::OStream& os ) const override final;
        
        virtual void deserialize_varialbe ( void*, glstreamer::LocalArgBase* localArg, glstreamer::IStream& is ) const override final;
    };
}

#endif
