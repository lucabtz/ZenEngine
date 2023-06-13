#pragma once

#include <memory>
#include <vector>

#include "RenderCommand.h"

namespace ZenEngine
{
    class VertexArray
	{
	public:
		virtual ~VertexArray() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void AddVertexBuffer(const std::shared_ptr<class VertexBuffer>& inVertexBuffer) = 0;
		virtual void SetIndexBuffer(const std::shared_ptr<class IndexBuffer>& inIndexBuffer) = 0;

		virtual const std::vector<std::shared_ptr<class VertexBuffer>>& GetVertexBuffers() const = 0;
		virtual const std::shared_ptr<class IndexBuffer>& GetIndexBuffer() const = 0;

		static std::shared_ptr<VertexArray> Create();
	};

	PROXY(VertexArray)
	{
		SETUP_PROXY(VertexArray)
	public:
		CREATE_METHOD_COMMAND_NO_PARAM(VertexArray_Create)
		static Proxy Create()
		{
			Proxy p(new Resource);
			Renderer::Get().Submit<VertexArray_Create>(p.mHandle);
			return p;
		}

		CREATE_METHOD_COMMAND_NO_PARAM(VertexArray_Bind)
		void Bind()
		{
			Renderer::Get().Submit<VertexArray_Bind>(mHandle);
		}
	
		CREATE_METHOD_COMMAND_NO_PARAM(VertexArray_Unbind)
		void Unbind()
		{
			Renderer::Get().Submit<VertexArray_Bind>(mHandle);
		}

		CREATE_METHOD_COMMAND_ONE_PARAM_NO_REF(VertexArray_AddVertexBuffer, RenderResourceContainer<class VertexBuffer> *, VBResource)
		void AddVertexBuffer(const RenderHandle<class VertexBuffer> &inVBHandle)
		{
			Renderer::Get().Submit<VertexArray_AddVertexBuffer>(mHandle, inVBHandle.GetResourceContainer());
		}

		CREATE_METHOD_COMMAND_ONE_PARAM_NO_REF(VertexArray_SetIndexBuffer, RenderResourceContainer<class IndexBuffer> *, IBResource)
		void SetIndexBuffer(const RenderHandle<class IndexBuffer> &inIBHandle)
		{
			Renderer::Get().Submit<VertexArray_SetIndexBuffer>(mHandle, inIBHandle.GetResourceContainer());
		}

	};
}