#pragma once

#include "ZenEngine/Renderer/VertexBuffer.h"
#include <stdint.h>

namespace ZenEngine
{
    
    class OpenGLVertexBuffer : public VertexBuffer
	{
	public:
        OpenGLVertexBuffer(uint32_t inSize);
        OpenGLVertexBuffer(const float* inVertices, uint32_t inSize);

		virtual ~OpenGLVertexBuffer();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void SetData(const void* inData, uint32_t inSize) override;

		virtual const BufferLayout& GetLayout() const override { return mLayout; }
		virtual void SetLayout(const BufferLayout& inLayout) override { mLayout = inLayout; }
    private:
        uint32_t mRendererId;
        BufferLayout mLayout;
    };
}