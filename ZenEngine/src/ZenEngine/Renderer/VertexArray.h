#pragma once

#include <memory>
#include <vector>

#include "Renderer.h"

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

}