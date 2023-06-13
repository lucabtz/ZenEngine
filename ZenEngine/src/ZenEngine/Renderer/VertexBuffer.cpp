#include "VertexBuffer.h"


#include "RendererAPI.h"
#include "Renderer.h"
#include "ZenEngine/Core/Macros.h"
#include "Platform/OpenGL/OpenGLVertexBuffer.h"

namespace ZenEngine
{
    std::shared_ptr<VertexBuffer> VertexBuffer::Create(uint32_t inSize)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:    ZE_ASSERT_CORE_MSG(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return std::make_shared<OpenGLVertexBuffer>(inSize);
		}

		ZE_ASSERT_CORE_MSG(false, "Unknown RendererAPI!");
		return nullptr;
	}

	std::shared_ptr<VertexBuffer> VertexBuffer::Create(const float *inVertices, uint32_t inSize)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:    ZE_ASSERT_CORE_MSG(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return std::make_shared<OpenGLVertexBuffer>(inVertices, inSize);
		}

		ZE_ASSERT_CORE_MSG(false, "Unknown RendererAPI!");
		return nullptr;
	}

    IMPLEMENT_METHOD(VertexBuffer_Create1, VertexBuffer)
    {
		mResource->Resource = VertexBuffer::Create(Size);
    }

    IMPLEMENT_METHOD(VertexBuffer_Create2, VertexBuffer)
    {
		mResource->Resource = VertexBuffer::Create(Vertices.data(), Size);
    }

    IMPLEMENT_METHOD(VertexBuffer_Bind, VertexBuffer)
    {
		mResource->Resource->Bind();
    }

    IMPLEMENT_METHOD(VertexBuffer_Unbind, VertexBuffer)
    {
		mResource->Resource->Unbind();
    }

    IMPLEMENT_METHOD(VertexBuffer_SetData, VertexBuffer)
    {
		mResource->Resource->SetData(Data, Size);
    }

	IMPLEMENT_METHOD(VertexBuffer_SetLayout, VertexBuffer)
    {
		mResource->Resource->SetLayout(Layout);
    }

}