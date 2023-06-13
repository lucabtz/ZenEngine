#include "IndexBuffer.h"

#include "RendererAPI.h"
#include "ZenEngine/Core/Macros.h"
#include "Platform/OpenGL/OpenGLIndexBuffer.h"

namespace ZenEngine
{
    std::shared_ptr<IndexBuffer> IndexBuffer::Create(const uint32_t *inIndices, uint32_t inCount)
    {
        switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:    ZE_ASSERT_CORE_MSG(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return std::make_shared<OpenGLIndexBuffer>(inIndices, inCount);
		}

		ZE_ASSERT_CORE_MSG(false, "Unknown RendererAPI!");
		return nullptr;
    }

	IMPLEMENT_METHOD(IndexBuffer_Create, IndexBuffer)
	{
		mResource->Resource = IndexBuffer::Create(Indices.data(), Count);
	}

	IMPLEMENT_METHOD(IndexBuffer_Bind, IndexBuffer)
	{
		mResource->Resource->Bind();
	}

	IMPLEMENT_METHOD(IndexBuffer_Unbind, IndexBuffer)
	{
		mResource->Resource->Unbind();
	}

}