#include "OpenGLFramebuffer.h"

#include <glad/glad.h>

namespace ZenEngine
{
    static const uint32_t MaxFramebufferSize = 8192;

    static bool IsDepthFormat(Framebuffer::TextureFormat inFormat)
    {
        switch (inFormat)
        {
        case Framebuffer::TextureFormat::Depth24Stencil8:  return true;
        }
        return false;
    }

    static void AttachColorTexture(uint32_t inId, int inSamples, GLenum inInternalFormat, GLenum inFormat, uint32_t inWidth, uint32_t inHeight, int inIndex)
    {
        bool multisampled = inSamples > 1;
        if (multisampled)
        {
            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, inSamples, inInternalFormat, inWidth, inHeight, GL_FALSE);
        }
        else
        {
            glTexImage2D(GL_TEXTURE_2D, 0, inInternalFormat, inWidth, inHeight, 0, inFormat, GL_UNSIGNED_BYTE, nullptr);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + inIndex, multisampled? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D, inId, 0);
    }

    static void AttachDepthTexture(uint32_t inId, int inSamples, GLenum inFormat, GLenum inAttachmentType, uint32_t inWidth, uint32_t inHeight)
    {
        bool multisampled = inSamples > 1;
        if (multisampled)
        {
            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, inSamples, inFormat, inWidth, inHeight, GL_FALSE);
        }
        else
        {
            glTexStorage2D(GL_TEXTURE_2D, 1, inFormat, inWidth, inHeight);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }

        glFramebufferTexture2D(GL_FRAMEBUFFER, inAttachmentType, multisampled? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D, inId, 0);
    }

    static GLenum FBTextureFormatToGL(Framebuffer::TextureFormat inFormat)
    {
        switch (inFormat)
        {
        case Framebuffer::TextureFormat::RGBA8:       return GL_RGBA8;
        case Framebuffer::TextureFormat::RedInteger:  return GL_RED_INTEGER;
        }

        ZE_ASSERT(false);
        return 0;
    }

    OpenGLFramebuffer::OpenGLFramebuffer(const Framebuffer::Properties &inProperties)
        : mProperties(inProperties)
    {
        for (auto textureProps : inProperties.AttachmentProps.Attachments)
        {
            if (IsDepthFormat(textureProps.Format))
                mDepthAttachmentProperties = textureProps;
            else
                mColorAttachmentsProperties.push_back(textureProps);
        }

        Invalidate();
    }


    OpenGLFramebuffer::~OpenGLFramebuffer()
    {
        glDeleteFramebuffers(1, &mRendererId);
        glDeleteTextures(mColorAttachmentsIds.size(), mColorAttachmentsIds.data());
        glDeleteTextures(1, &mDepthAttachmentId);
    }

    void OpenGLFramebuffer::Invalidate()
    {
        if (mRendererId != 0)
        {
            glDeleteFramebuffers(1, &mRendererId);
            glDeleteTextures(mColorAttachmentsIds.size(), mColorAttachmentsIds.data());
            glDeleteTextures(1, &mDepthAttachmentId);
            
            mColorAttachmentsIds.clear();
            mDepthAttachmentId = 0;
        }

        glCreateFramebuffers(1, &mRendererId);
        glBindFramebuffer(GL_FRAMEBUFFER, mRendererId);

        bool multisample = mProperties.Samples > 1;

        // Attachments
        if (mColorAttachmentsProperties.size() > 0)
        {
            mColorAttachmentsIds.resize(mColorAttachmentsProperties.size());
            glCreateTextures(multisample?  GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D, mColorAttachmentsProperties.size(), mColorAttachmentsIds.data());

            for (size_t i = 0; i < mColorAttachmentsIds.size(); ++i)
            {
                glBindTexture(multisample?  GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D, mColorAttachmentsIds[i]);
                switch (mColorAttachmentsProperties[i].Format)
                {
                case TextureFormat::RGBA8:
                    AttachColorTexture(mColorAttachmentsIds[i], mProperties.Samples, GL_RGBA8, GL_RGBA, mProperties.Width, mProperties.Height, i);
                    break;
                case TextureFormat::RedInteger:
                    AttachColorTexture(mColorAttachmentsIds[i], mProperties.Samples, GL_R32I, GL_RED_INTEGER, mProperties.Width, mProperties.Height, i);
                    break;
                }
            }
        }

        if (mDepthAttachmentProperties.Format != TextureFormat::None)
        {
            glCreateTextures(multisample?  GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D, 1, &mDepthAttachmentId);
            glBindTexture(multisample?  GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D, mDepthAttachmentId);

            switch (mDepthAttachmentProperties.Format)
            {
                case TextureFormat::Depth24Stencil8:
                    AttachDepthTexture(mDepthAttachmentId, mProperties.Samples, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT, mProperties.Width, mProperties.Height);
                    break;
            }
        }

        
        if (mColorAttachmentsIds.size() > 1)
        {
            ZE_ASSERT_CORE_MSG(mColorAttachmentsIds.size() <= 4, "Too many attachments!");
            GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
            glDrawBuffers(mColorAttachmentsIds.size(), buffers);
        }
        else if (mColorAttachmentsIds.empty())
        {
            // Only depth-pass
            glDrawBuffer(GL_NONE);
        }
        

        ZE_ASSERT_CORE_MSG(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!");

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    
    void OpenGLFramebuffer::Bind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, mRendererId);
        glViewport(0, 0, mProperties.Width, mProperties.Height);
    }

    void OpenGLFramebuffer::Unbind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void OpenGLFramebuffer::Resize(uint32_t inWidth, uint32_t inHeight)
    {
        if (inWidth == 0 || inHeight == 0 || inWidth > MaxFramebufferSize || inHeight > MaxFramebufferSize)
        {
            ZE_CORE_WARN("Attempted to rezize framebuffer to {}, {}", inWidth, inHeight);
            return;
        }
        mProperties.Width = inWidth;
        mProperties.Height = inHeight;
        Invalidate();
    }

}