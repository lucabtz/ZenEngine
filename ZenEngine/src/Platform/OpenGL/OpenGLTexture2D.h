#pragma once

#include "ZenEngine/Renderer/Texture2D.h"

#include <glad/glad.h>

namespace ZenEngine
{
    class OpenGLTexture2D : public Texture2D
    {
    public:
        OpenGLTexture2D(const Texture2D::Properties &inProperties);
        virtual ~OpenGLTexture2D();

        virtual const Texture2D::Properties& GetProperties() const override { return mProperties; }

        virtual uint32_t GetWidth() const override { return mProperties.Width; }
        virtual uint32_t GetHeight() const override { return mProperties.Height; }
        virtual uint32_t GetRendererID() const override { return mRendererId; }

        virtual void SetData(void* inData, uint32_t inSize) override;

        virtual void Bind(uint32_t inSlot = 0) const override;
    private:
        Texture2D::Properties mProperties;
        uint32_t mRendererId;
        GLenum mInternalFormat;
    };
}