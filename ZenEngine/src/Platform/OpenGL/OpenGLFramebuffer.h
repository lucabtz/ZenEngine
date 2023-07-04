#pragma once

#include <mutex>

#include "ZenEngine/Renderer/Framebuffer.h"
#include "ZenEngine/Core/Macros.h"

namespace ZenEngine
{
    class OpenGLFramebuffer : public Framebuffer
    {
    public:
        OpenGLFramebuffer(const Framebuffer::Properties& inProperties);
        virtual ~OpenGLFramebuffer();

        void Invalidate();

        virtual void Bind() override;
        virtual void Unbind() override;

        virtual void Resize(uint32_t width, uint32_t height) override;

        virtual uint32_t GetColorAttachmentRendererId(uint32_t inIndex = 0) const override 
        { 
            ZE_ASSERT_CORE_MSG(inIndex < mColorAttachmentsIds.size(), "Invalid index given!"); 
            return mColorAttachmentsIds[inIndex]; 
        }

        virtual void BindColorAttachmentTexture(uint32_t inIndex = 0, uint32_t inSlot = 0) const override;
        virtual void BindDepthAttachmentTexture(uint32_t inSlot = 0) const override;
        virtual void BindAllAttachments(uint32_t inStartingSlot = 0) const override;

        virtual const Properties &GetProperties() const override
        { 
            return mProperties;
        }
    private:
        uint32_t mRendererId = 0;
        Properties mProperties;

        std::vector<TextureProperties> mColorAttachmentsProperties;
        TextureProperties mDepthAttachmentProperties;

        std::vector<uint32_t> mColorAttachmentsIds;
        uint32_t mDepthAttachmentId = 0;
    };
}