#pragma once

#include <memory>

namespace ZenEngine
{
    class UniformBuffer
	{
	public:
		virtual ~UniformBuffer() {}

        virtual void Bind() = 0;
        virtual void Bind(uint32_t inBinding) = 0;
		virtual void SetData(const void* inData, uint32_t inSize, uint32_t inOffset = 0) = 0;
		
		static std::shared_ptr<UniformBuffer> Create(uint32_t inSize, uint32_t inBinding);
	};
}