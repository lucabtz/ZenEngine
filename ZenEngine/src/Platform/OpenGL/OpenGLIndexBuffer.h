#pragma once

#include "ZenEngine/Renderer/IndexBuffer.h"

namespace ZenEngine
{

    class OpenGLIndexBuffer : public IndexBuffer
    {
    public:
		OpenGLIndexBuffer(const uint32_t* inIndices, uint32_t inCount);
		virtual ~OpenGLIndexBuffer();

		virtual void Bind() const;
		virtual void Unbind() const;

		virtual uint32_t GetCount() const { return mCount; }
	private:
		uint32_t mRendererId;
		uint32_t mCount;
    };

}