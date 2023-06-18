#pragma once

#include "ZenEngine/Renderer/UniformBuffer.h"

namespace ZenEngine
{

    class OpenGLUniformBuffer : public UniformBuffer
    {
    public:
        OpenGLUniformBuffer(uint32_t inSize, uint32_t inBinding);
        ~OpenGLUniformBuffer();
		
        virtual void Bind() override;
        virtual void Bind(uint32_t inBinding) override;
        virtual void SetData(const void* inData, uint32_t inSize, uint32_t inOffset = 0) override;
    private:
        uint32_t mRendererId;
        uint32_t mBinding;
    };
}