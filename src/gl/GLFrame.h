#ifndef _C324C41E_B26C_11E2_8F86_206A8A22A96A
#define _C324C41E_B26C_11E2_8F86_206A8A22A96A

#include <vector>

#include "gl.inc.h"

#include "GLObject.h"

namespace glstreamer_gl
{
    typedef GLenum GLDataType;
    typedef GLenum GLExternalFormat;
    typedef GLint  GLInternalFormat;
    
    template <typename _Pixel, GLDataType _DataType, GLExternalFormat _ExternalFormat, GLInternalFormat _InternalFormat>
    struct PixelTypePack
    {
        typedef _Pixel Pixel;
        static constexpr auto DataType = _DataType;
        static constexpr auto ExternalFormat = _ExternalFormat;
        static constexpr auto InternalFormat = _InternalFormat;
    };
    
    template <typename PixelTypePack>
    class GLFrameData
    {
    public:
        typedef typename PixelTypePack::Pixel Pixel;
        
        GLFrameData():
        pixelData(),
        width_(0),
        height_(0)
        {}
        
        Pixel *data() { return pixelData.data(); }
        Pixel const* data() const { return pixelData.data(); }
        
        GLsizei width()  const { return width_; }
        GLsizei height() const { return height_; }
        
        void clear()
        {
            pixelData.clear();
            width_ = 0;
            height_ = 0;
        }
        
        void resize(GLsizei width, GLsizei height)
        {
            if(width == width_ && height == height_)
                return;
            
            width_ = width;
            height_ = height;
            pixelData.resize(width * height);
        }
        
    private:
        std::vector<Pixel> pixelData;
        
        GLsizei width_, height_;
    };
    
    template <typename PixelTypePack>
    class GLTextureData
    {
        typedef typename PixelTypePack::Pixel Pixel;
        static constexpr GLDataType DataType = PixelTypePack::DataType;
        static constexpr GLExternalFormat ExternalFormat = PixelTypePack::ExternalFormat;
        static constexpr GLInternalFormat InternalFormat = PixelTypePack::InternalFormat;
    public:
        GLTextureData():
        texture_(),
        width_(0),
        height_(0)
        {
            bind();
        }
        
        TextureObject& obj() { return texture_; }
        
        GLsizei width()  const { return width_; }
        GLsizei height() const { return height_; }
        
        void resize(GLsizei width, GLsizei height)
        {
            if(width == width_ && height == height_)
                return;
            
            bind();
            gl_Call(glTexImage2D(target,
                                 0,
                                 PixelTypePack::InternalFormat,
                                 width,
                                 height,
                                 0,
                                 PixelTypePack::ExternalFormat,
                                 PixelTypePack::DataType,
                                 nullptr
            ));
            width_ = width;
            height_ = height;
        }
        
        void readData(Pixel *data)
        {
            bind();
            gl_Call(glGetTexImage(target,
                                  0,
                                  ExternalFormat,
                                  DataType,
                                  data
            ));
        }
        
        void storeData(GLsizei width, GLsizei height, Pixel const* data)
        {
            bind();
            gl_Call(glTexImage2D(target,
                                 0,
                                 InternalFormat,
                                 width,
                                 height,
                                 0,
                                 ExternalFormat,
                                 DataType,
                                 data
            ));
            width_ = width;
            height_ = height;
        }
        
        void bind()
        {
            gl_Call(glBindTexture(target, texture_));
        }
        
        static constexpr GLenum target = GL_TEXTURE_2D;
        
    private:
        TextureObject texture_;
        
        GLsizei width_, height_;
    };
    
    struct RGBA
    {
        GLubyte r, g, b, a;
    };
    
    typedef PixelTypePack<RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, GL_RGBA, GL_RGBA> RGBAFrame;
    typedef PixelTypePack<GLfloat, GL_FLOAT, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT> DepthFrame;
}

#endif
