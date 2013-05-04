#ifndef _170B019C_B2E0_11E2_88C7_206A8A22A96A
#define _170B019C_B2E0_11E2_88C7_206A8A22A96A

#include "../fixedprocessor.h"
#include "../typelist.h"

#include "GLFrame.h"

namespace glstreamer_gl
{
    template <typename NonPixelInputList, typename NonPixelOutputList, typename InputPixelTypeList, typename OutputPixelTypeList>
    class GLFixedProcessor;
    
    template <typename ... NonPixelInputs, typename ... NonPixelOutputs, typename ... InputPixelTypes, typename ... OutputPixelTypes>
    class GLFixedProcessor<
    glstreamer::TypeList<NonPixelInputs...>,
    glstreamer::TypeList<NonPixelOutputs...>,
    glstreamer::TypeList<InputPixelTypes...>,
    glstreamer::TypeList<OutputPixelTypes...>
    > : public glstreamer::FixedProcessor<
    glstreamer::TypeList<NonPixelInputs..., GLFrameData<InputPixelTypes>...>,
    glstreamer::TypeList<NonPixelOutputs..., GLFrameData<OutputPixelTypes>...>
    >
    {
    public:
        GLFixedProcessor(typename glstreamer::Const<std::string const&, NonPixelInputs>::type ... nonPixelInputNames,
                         typename glstreamer::Const<std::string const&, InputPixelTypes>::type ... pixelInputNames,
                         typename glstreamer::Const<std::string const&, NonPixelOutputs>::type ... nonPixelOutputNames,
                         typename glstreamer::Const<std::string const&, OutputPixelTypes>::type ... pixelOutputNames
        ):
        glstreamer::FixedProcessor<
        glstreamer::TypeList<NonPixelInputs..., GLFrameData<InputPixelTypes>...>,
        glstreamer::TypeList<NonPixelOutputs..., GLFrameData<OutputPixelTypes>...>
        >(nonPixelInputNames ...,
          pixelInputNames ...,
          nonPixelOutputNames ...,
          pixelOutputNames ...
        )
        {}
        
    protected:
        static constexpr std::size_t inputDataCount = sizeof...(NonPixelInputs);
        static constexpr std::size_t inputFramesCount = sizeof...(InputPixelTypes);
        static constexpr std::size_t outputDataCount = sizeof...(NonPixelOutputs);
        static constexpr std::size_t outputFramesCount = sizeof...(OutputPixelTypes);
        
        static constexpr std::size_t inputFramesStart = inputDataCount;
        static constexpr std::size_t inputFramesEnd = inputFramesStart + inputFramesCount;
        static constexpr std::size_t outputFramesStart = inputDataCount + inputFramesCount + outputDataCount;
        static constexpr std::size_t outputFramesEnd = outputFramesStart + outputFramesCount;
        
        virtual void gl_run(NonPixelInputs& ... nonPixelInputs,
                            NonPixelOutputs& ... nonPixelOutputs,
                            GLTextureData<InputPixelTypes>& ...textureInputs,
                            GLTextureData<OutputPixelTypes>& ...textureOutputs
                           ) = 0;
        
    private:
        virtual void do_run(NonPixelInputs& ... nonPixelInputs,
                            GLFrameData<InputPixelTypes>& ...,
                            NonPixelOutputs& ... nonPixelOutputs,
                            GLFrameData<OutputPixelTypes>& ...
                           ) override
        {
            bindInputTextureArguments<inputFramesStart, 0, InputPixelTypes...>(nonPixelInputs ..., nonPixelOutputs ...);
        }
        
        template <std::size_t argumentIndex, std::size_t textureIndex, typename PixelType, typename ... PixelTypes, typename ... ParsedPixelTypes>
        void bindInputTextureArguments(NonPixelInputs& ... nonPixelInputs,
                                       NonPixelOutputs& ... nonPixelOutputs,
                                       GLTextureData<ParsedPixelTypes>& ... textures)
        {
            using namespace glstreamer;
            FullSlot& slot = this->inputArgs.fullSlot(argumentIndex);
            GLTextureData<PixelType>& texture = slot.localArg->getArg<GLTextureData<PixelType>>();
            gl_Call(glActiveTexture(GL_TEXTURE0 + textureIndex));
            texture.bind();
            gl_Call(glGenerateMipmap(texture.target));
            bindInputTextureArguments<argumentIndex + 1, textureIndex + 1, PixelTypes...>
            (nonPixelInputs ...,
             nonPixelOutputs ...,
             textures ...,
             texture
            );
        }
        
        template <std::size_t argumentIndex, std::size_t textureIndex, typename ... PixelTypes, typename ... ParsedPixelTypes>
        typename std::enable_if<sizeof...(PixelTypes) == 0>::type
        bindInputTextureArguments(NonPixelInputs& ... nonPixelInputs,
                                  NonPixelOutputs& ... nonPixelOutputs,
                                  GLTextureData<ParsedPixelTypes>& ... textures
                                 )
        {
            bindOutputTextureArguments<outputFramesStart, inputFramesCount, OutputPixelTypes ...>
            (nonPixelInputs ...,
             nonPixelOutputs ...,
             textures ...
            );
        }
        
        template <std::size_t argumentIndex, std::size_t textureIndex, typename PixelType, typename ... PixelTypes, typename ... ParsedPixelTypes>
        void bindOutputTextureArguments(NonPixelInputs& ... nonPixelInputs,
                                        NonPixelOutputs& ... nonPixelOutputs,
                                        GLTextureData<ParsedPixelTypes>& ... textures
                                       )
        {
            using namespace glstreamer;
            FullSlot& slot = this->outputArgs.fullSlot(argumentIndex);
            GLTextureData<PixelType>& texture = slot.localArg->getArg<GLTextureData<PixelType>>();
//             Texture::Active(textureIndex);
//             texture.bind();
            bindOutputTextureArguments<argumentIndex + 1, textureIndex + 1, PixelTypes...>
            (nonPixelInputs ...,
             nonPixelOutputs ...,
             textures ...,
             texture
            );
        }
        
        template <std::size_t argumentIndex, std::size_t textureIndex, typename ... PixelTypes, typename ... ParsedPixelTypes>
        typename std::enable_if<sizeof...(PixelTypes) == 0>::type
        bindOutputTextureArguments(NonPixelInputs& ... nonPixelInputs,
                                   NonPixelOutputs& ... nonPixelOutputs,
                                   GLTextureData<ParsedPixelTypes>& ... textures
                                  )
        {
            gl_run(nonPixelInputs ...,
                   nonPixelOutputs ...,
                   textures ...
            );
        }
    };
}

#endif
