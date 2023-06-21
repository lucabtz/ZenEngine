#pragma once

#include <memory>
#include <vector>

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
        static std::shared_ptr<IndexBuffer> Create(const std::vector<uint32_t> &inIndices);
    };

}