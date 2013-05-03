#ifndef _C324C41E_B26C_11E2_8F86_206A8A22A96A
#define _C324C41E_B26C_11E2_8F86_206A8A22A96A

#include <vector>

#include "ogl.inc.h"

namespace glstreamer_gl
{
    template <typename _Pixel, oglplus::PixelDataType _DataType, oglplus::PixelDataFormat _ExternalFormat, oglplus::PixelDataInternalFormat _InternalFormat>
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
        static constexpr oglplus::PixelDataType DataType = PixelTypePack::DataType;
        static constexpr oglplus::PixelDataFormat ExternalFormat = PixelTypePack::ExternalFormat;
        static constexpr oglplus::PixelDataInternalFormat InternalFormat = PixelTypePack::InternalFormat;
    public:
        GLTextureData():
        texture_(),
        width_(0),
        height_(0)
        {
            bind();
        }
        
        oglplus::Texture& obj() { return texture_; }
        
        GLsizei width()  const { return width_; }
        GLsizei height() const { return height_; }
        
        void resize(GLsizei width, GLsizei height)
        {
            if(width == width_ && height == height_)
                return;
            
            bind();
            oglplus::Texture::Image2D(target,
                                      0,
                                      PixelTypePack::InternalFormat,
                                      width,
                                      height,
                                      0,
                                      PixelTypePack::ExternalFormat,
                                      PixelTypePack::DataType,
                                      nullptr
            );
            width_ = width;
            height_ = height;
        }
        
        void readData(Pixel *data)
        {
            bind();
            using namespace oglplus;
            OGLPLUS_GLFUNC(GetTexImage)(GLenum(target),
                                        0,
                                        GLenum(ExternalFormat),
                                        GLenum(DataType),
                                        data
            );
        }
        
        void storeData(GLsizei width, GLsizei height, Pixel const* data)
        {
            bind();
            oglplus::Texture::Image2D(target,
                                      0,
                                      InternalFormat,
                                      width,
                                      height,
                                      0,
                                      ExternalFormat,
                                      DataType,
                                      data
            );
            width_ = width;
            height_ = height;
        }
        
        void bind()
        {
            texture_.Bind(target);
        }
        
        static constexpr oglplus::TextureTarget target = oglplus::TextureTarget::_2D;
        
    private:
        oglplus::Texture texture_;
        
        GLsizei width_, height_;
    };
    
    struct RGBA
    {
        GLubyte r, g, b, a;
    };
    
    typedef PixelTypePack<RGBA, oglplus::PixelDataType::UnsignedInt_8_8_8_8_Rev, oglplus::PixelDataFormat::RGBA, oglplus::PixelDataInternalFormat::RGBA> RGBAFrame;
}

#endif
