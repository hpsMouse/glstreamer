#include "GLFrameTypeSpec.h"

#include "gl.inc.h"

#include "../istream.h"
#include "../LocalArg.h"
#include "../nettypes.h"
#include "../ostream.h"

using namespace glstreamer_gl;

template <typename PixelTypePack>
glstreamer::LocalArgBase* GLFrameTypeSpec<PixelTypePack>::createLocal() const
{
    return new glstreamer::LocalArg<Texture>;
}

template <typename PixelTypePack>
void GLFrameTypeSpec<PixelTypePack>::context_out ( void* obj, glstreamer::LocalArgBase* localArg ) const
{
    Frame& frame = *static_cast<Frame*>(obj);
    Texture& texture = localArg->getArg<Texture>();
    frame.resize(texture.width(), texture.height());
    texture.readData(frame.data());
}

template <typename PixelTypePack>
void GLFrameTypeSpec<PixelTypePack>::context_in ( void* obj, glstreamer::LocalArgBase* localArg ) const
{
    Frame& frame = *static_cast<Frame*>(obj);
    Texture& texture = localArg->getArg<Texture>();
    texture.storeData(frame.width(), frame.height(), frame.data());
}

template <typename PixelTypePack>
void GLFrameTypeSpec<PixelTypePack>::serialize_variable ( const void* , glstreamer::LocalArgBase* localArg, glstreamer::OStream& os ) const
{
    using glstreamer::NetUInt32;
    
    Texture& texture = localArg->getArg<Texture>();
    GLsizei width = texture.width(), height = texture.height();
    
    os << NetUInt32(width).rawValue() << NetUInt32(height).rawValue();
    
    glstreamer::size_type size = width * height * sizeof(typename PixelTypePack::Pixel);
    void *buffer = os.requireInternalBuffer(size);
    texture.readData(static_cast<Pixel*>(buffer));
    os.pushInternalBuffer(buffer, size);
}

template <typename PixelTypePack>
void GLFrameTypeSpec<PixelTypePack>::deserialize_varialbe ( void* , glstreamer::LocalArgBase* localArg, glstreamer::IStream& is ) const
{
    using glstreamer::NetUInt32;
    
    Texture& texture = localArg->getArg<Texture>();
    
    NetUInt32 width, height;
    is >> width.rawValue() >> height.rawValue();
    
    glstreamer::size_type size = width * height * sizeof(typename PixelTypePack::Pixel);
    void const* buffer = is.requireInternalBuffer(size);
    texture.storeData(width, height, static_cast<Pixel const*>(buffer));
    is.releaseInternalBuffer(buffer, size);
}

template class GLFrameTypeSpec<RGBAFrame>;
template class GLFrameTypeSpec<DepthFrame>;
