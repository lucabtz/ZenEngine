#pragma once

#include <memory>
#include <vector>

namespace ZenEngine
{


    class Framebuffer
    {
    public:
        enum class TextureFormat
        {
            None = 0,

            // Color
            RGBA8,
            RedInteger,

            // Depth/stencil
            Depth24Stencil8,

            // Defaults
            Depth = Depth24Stencil8
        };

        struct TextureProperties
        {
            TextureProperties() = default;
            TextureProperties(TextureFormat inFormat)
                : Format(inFormat) {}

            TextureFormat Format  = TextureFormat::None;
            // TODO: filtering/wrap
        };

        struct AttachmentProperties
        {
            AttachmentProperties() = default;
            AttachmentProperties(std::initializer_list<TextureProperties> attachments)
                : Attachments(attachments) {}

            std::vector<TextureProperties> Attachments;
        };

        struct Properties
        {
            uint32_t Width = 0;
            uint32_t Height = 0;
            AttachmentProperties AttachmentProps;
            uint32_t Samples = 1;

            bool SwapChainTarget = false;
        };


        virtual ~Framebuffer() = default;

        virtual void Bind() = 0;
        virtual void Unbind() = 0;

        virtual void Resize(uint32_t inWidth, uint32_t inHeight) = 0;

        virtual uint32_t GetColorAttachmentRendererId(uint32_t inIndex = 0) const = 0;

        virtual void BindColorAttachmentTexture(uint32_t inIndex = 0, uint32_t inSlot = 0) const = 0;
        virtual void BindDepthAttachmentTexture(uint32_t inSlot = 0) const = 0;
        virtual void BindAllAttachments(uint32_t inStartingSlot = 0) const = 0;

        virtual const Properties &GetProperties() const = 0;

        static std::shared_ptr<Framebuffer> Create(const Properties& inProperties);
    };

}