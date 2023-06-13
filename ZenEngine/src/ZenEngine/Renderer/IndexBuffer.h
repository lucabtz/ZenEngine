#pragma once

#include <memory>
#include "RenderCommand.h"

namespace ZenEngine
{

	class IndexBuffer
	{
	public:
		virtual ~IndexBuffer() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual uint32_t GetCount() const = 0;

		static std::shared_ptr<IndexBuffer> Create(const uint32_t* inIndices, uint32_t inCount);
	};

	PROXY(IndexBuffer)
	{
		SETUP_PROXY(IndexBuffer)
	public:
		CREATE_METHOD_COMMAND_TWO_PARAM(IndexBuffer_Create, const std::vector<uint32_t>, Indices, uint32_t, Count)
		static Proxy Create(const std::vector<uint32_t> &inIndices, uint32_t inCount)
		{
			Proxy p(new Resource);
			Renderer::Get().Submit<IndexBuffer_Create>(p.mHandle, inIndices, inCount);
			return p;
		}

		CREATE_METHOD_COMMAND_NO_PARAM(IndexBuffer_Bind)
		void Bind()
		{
			Renderer::Get().Submit<IndexBuffer_Bind>(mHandle);
		}

		CREATE_METHOD_COMMAND_NO_PARAM(IndexBuffer_Unbind)
		void Unbind()
		{
			Renderer::Get().Submit<IndexBuffer_Unbind>(mHandle);
		}
	};
}